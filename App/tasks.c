/************************************************************************************

Copyright (c) 2001-2016  University of Washington Extension.

Module Name:

    tasks.c

Module Description:

    The tasks that are executed by the test application.

2016/2 Nick Strathy adapted it for NUCLEO-F401RE 

************************************************************************************/
#include <stdarg.h>

#include "bsp.h"
#include "print.h"
#include "mp3Util.h"
#include <SD.h>

#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ILI9341.h>
#include <Adafruit_FT6206.h>
#include "starwars.h" //mp3 file
#include "starwarslogo.c" //bmp file
#include "rocky.c" //bmp file
#include "pinkpanther.c" //bmp file



Adafruit_ILI9341 lcdCtrl = Adafruit_ILI9341(); // The LCD controller

Adafruit_FT6206 touchCtrl = Adafruit_FT6206(); // The touch controller



Adafruit_GFX_Button btnPlayObj = Adafruit_GFX_Button(); //Class for Play ButtonUI element
Adafruit_GFX_Button btnNextObj = Adafruit_GFX_Button(); //Class for Next ButtonUI element
Adafruit_GFX_Button btnPrevObj = Adafruit_GFX_Button(); //Class for Prev ButtonUI element
Adafruit_GFX_Button btnListObj = Adafruit_GFX_Button(); //Class for Prev ButtonUI element


#define BUFSIZE 256
#define PENRADIUS 3
#define PLAYBTN "Play"
#define STOPBTN "Stop"
#define NEXTBTN " Next>>"
#define PREVBTN " <<Prev"
#define LISTBTN " List"
#define WIDTHBTN 85
#define HEIGHTBTN 35
#define BOXSIZE 30
#define PAGE1   1
#define PAGE2   2
#define PAGE3   3

#define BTN_OUTLINE     ILI9341_BLACK
#define BTN_FILL        ILI9341_BLUE
#define BTN_TEXT_COLOR  ILI9341_WHITE
#define SCR_FILL_COLOR  ILI9341_NAVY



/************************************************************************************

   Allocate the stacks for each task.
   The maximum number of tasks the application can have is defined by OS_MAX_TASKS in os_cfg.h

************************************************************************************/

static OS_STK   LcdTouchDemoTaskStk[APP_CFG_TASK_START_STK_SIZE];
static OS_STK   Mp3DemoTaskStk[APP_CFG_TASK_START_STK_SIZE];
static OS_STK   SystemInitTaskStk[APP_CFG_TASK_START_STK_SIZE];
     
// Task prototypes
void LcdTouchDemoTask(void* pdata);
void Mp3DemoTask(void* pdata);
void SystemInitTask(void* pdata);


// Useful functions
long MapTouchToScreen(long x, long in_min, long in_max, long out_min, long out_max);
void PrintToLcdWithBuf(char *buf, int size, char *format, ...);
static void DrawPlayButton();
static boolean PlayMp3File(char * mp3FileName);
static boolean ExecutePlayer(TS_Point touchPoint);
static boolean ExcuteListView(TS_Point touchPoint);
static boolean DrawLcdContents(char* filename);
boolean IsMP3(char* filename);
static boolean GetSDFileList();
static boolean MP3Controller(TS_Point touchPoint);

// Globals
File SDFileList[6];

Adafruit_GFX_Button btnListArray[6];

BOOLEAN nextSong = OS_FALSE;
BOOLEAN playState = false; 
BOOLEAN mp3TaskStarted = false; 
int currentPage = PAGE2;
int nextPage = PAGE3;
char* currfilename;
char* prevfilename;
char* nextfilename;

HANDLE hMp3 = 0;
HANDLE hSPI = 0;

//Maps the touch point to screen
long MapTouchToScreen(long x, long in_min, long in_max, long out_min, long out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}


//Controls the play and stop functionality
static boolean MP3Controller(TS_Point touchPoint){
    
  switch(currentPage){
    case PAGE1:
      
      break;
    case PAGE2:
      ExcuteListView(touchPoint);
      break;
    case PAGE3:
      ExecutePlayer(touchPoint);
      break;
  }
  
  return true;
}

static boolean ExcuteListView(TS_Point touchPoint)
{
    
    char* fileName;
    //Finding which button is clicked
    for(int i =0; i<6 ; i++)
    {
       if( btnListArray[i].contains(touchPoint.x, touchPoint.y))
       {
            
            currfilename = btnListArray[i].mp3FileName;
            currfilename[12] = 0;
            
            
            //currfilename[13] = 0;
            //OSTimeDly(2);
            currentPage = PAGE3;
            nextPage = PAGE2;
            DrawLcdContents(currfilename);
       }
         
    }
    
   return true; 

}

// Checks if a file is MP3 depending upon extention (MP3)
boolean IsMP3(char* filename){
  
  char *ext= "MP3";
  
  for(int i=0; filename[i] != '\0';i++)
  {
    
      if(filename[i] == '.')
      {
        i++;
        int count = 0;
        while(filename[i] == ext[count++])
        {
            i++;
            //ext++;
            if(ext[count] == '\0')
              return true;
        }
              
      }
        
  }
  return false;
}




static boolean ExecutePlayer(TS_Point touchPoint)
{
  
  //Play button
  if(btnPlayObj.contains(touchPoint.x, touchPoint.y))
  {
    btnPlayObj.drawButton(true);
    if(btnPlayObj.isPressed()){
      if(!mp3TaskStarted){
        //Added code to start mp3 task
        OSTaskCreate(Mp3DemoTask, (void*)0, &Mp3DemoTaskStk[APP_CFG_TASK_START_STK_SIZE-1], APP_TASK_TEST1_PRIO);
        mp3TaskStarted = true;
        
      }
      
      btnPlayObj.changeLabelButton(&lcdCtrl , STOPBTN);
      btnPlayObj.drawButton(false);
      btnPlayObj.press(false);
      playState = true;
      
    }else{
      btnPlayObj.changeLabelButton(&lcdCtrl , PLAYBTN);
      btnPlayObj.drawButton(false);
      btnPlayObj.press(true);
      playState = false;
      
    }
  }
  
  if(btnListObj.contains(touchPoint.x, touchPoint.y))
  {
    playState = false;
    btnListObj.press(true);
    currentPage = PAGE2;
      GetSDFileList();
  }
  
  if(btnNextObj.contains(touchPoint.x, touchPoint.y))
  {
    playState = false;
    btnNextObj.press(true);
    
    for(int i =0 ; i<6 ; i++ )
    {
      if(strcmp(currfilename , SDFileList[i].name()) == 0)
      {
        int nextTrack = i+1;
        if(nextTrack == 6)
          nextTrack = 0;
        currfilename = SDFileList[nextTrack].name();
        
        DrawLcdContents(currfilename);
        playState = true;
        btnPlayObj.press(false);
        OSTimeDly(100);
        break;
      }
    }
  }
  
  if(btnPrevObj.contains(touchPoint.x, touchPoint.y))
  {
    playState = false;
    btnPrevObj.press(true);
    
    for(int i =0 ; i<6 ; i++ )
    {
      if(strcmp(currfilename , SDFileList[i].name()) == 0)
      {
        int nextTrack = i-1;
        if(nextTrack == -1)
          nextTrack = 5;
        
        currfilename = SDFileList[nextTrack].name();
        
        DrawLcdContents(currfilename);
        
        playState = true;
        btnPlayObj.press(false);
        OSTimeDly(100);
        break;
      }
    }
  }
  
  return true;

}


//Drawing the play button
static void DrawPlayButton(){
  
  btnPlayObj.initButton(&lcdCtrl, 160, 218, WIDTHBTN, HEIGHTBTN,BTN_OUTLINE, BTN_FILL, BTN_TEXT_COLOR,PLAYBTN, 2);
  btnPlayObj.drawButton(false);
  btnPlayObj.press(true);

}


static void DrawNextButton(){
  btnNextObj.initButton(&lcdCtrl, 270, 218, WIDTHBTN, HEIGHTBTN,BTN_OUTLINE, BTN_FILL, BTN_TEXT_COLOR,NEXTBTN, 2);
  btnNextObj.drawButton(false);
  //btnObj.press(true);

}

static void DrawPrevButton(){
  btnPrevObj.initButton(&lcdCtrl, 50, 218, WIDTHBTN, HEIGHTBTN,BTN_OUTLINE, BTN_FILL, BTN_TEXT_COLOR,PREVBTN, 2);
  btnPrevObj.drawButton(false);
  //btnObj.press(true);

}

static void DrawListButton(){
  btnListObj.initButton(&lcdCtrl, 270, 20, WIDTHBTN, HEIGHTBTN,BTN_OUTLINE, BTN_FILL, BTN_TEXT_COLOR,LISTBTN, 2);
  btnListObj.drawButton(false);
  //btnObj.press(true);

}



static boolean DrawLcdContents(char* filename)
{
    char buf[BUFSIZE];
    
    lcdCtrl.fillScreen(SCR_FILL_COLOR);
    
    lcdCtrl.setRotation(1);
    
    if(strcmp("CANTINA.MP3",filename) ==  0 ||strcmp("STARWARS.MP3",filename) ==  0 ||strcmp("IMPERIAL.MP3",filename) ==  0  )
      lcdCtrl.drawXBitmap(10,5,(INT8U*) starwarlogo_bits, starwarlogo_width, starwarlogo_height,  ILI9341_BLACK);
    
    if(strcmp("PINKPAN.MP3",filename) ==  0 )
      lcdCtrl.drawXBitmap(10,5,(INT8U*) pinkpanther_bits, 154, 169,  ILI9341_PINK);
      
    if(strcmp("GONNAFLY.MP3",filename) ==  0 || strcmp("EYEOTIGE.MP3",filename) ==  0)      
      lcdCtrl.drawXBitmap(10,5,(INT8U*) rocky_bits, 222, 129,  ILI9341_WHITE);
    
    // Print a message on the LCD
    lcdCtrl.setCursor(10,starwarlogo_height + 10);
    lcdCtrl.setTextColor(ILI9341_WHITE);  
    lcdCtrl.setTextSize(2);
    PrintToLcdWithBuf(buf, BUFSIZE, filename);
    
    lcdCtrl.setRotation(1);
    DrawPlayButton();
    DrawNextButton();
    DrawPrevButton();
    DrawListButton();
    
    return true;

}

static boolean GetSDFileList(){
    char buf[BUFSIZE];
    lcdCtrl.fillScreen(SCR_FILL_COLOR);
    // Displaying the header
    lcdCtrl.setRotation(1);

    lcdCtrl.fillRect(0, 0, ILI9341_TFTHEIGHT, BOXSIZE, ILI9341_BLACK);
    lcdCtrl.setCursor(20, 3);
    lcdCtrl.setTextColor(ILI9341_WHITE);  
    lcdCtrl.setTextSize(2);
    PrintToLcdWithBuf(buf, BUFSIZE, "List of Songs");
     //opening file
    File dir = SD.open("/");
    dir.seek(0);
    //File entry = dir.openNextFile();
    //while(entry.available)
    
    int yPos =50;
    int fileCount =0;
    while (1)
    {
        File entry = dir.openNextFile();
        if (!entry)
        {
            break;
        }
        if (entry.isDirectory())  // skip directories
        {
            entry.close();
            continue;
        }
        
        char* entryname = entry.name();
        if(IsMP3(entryname)){
          SDFileList[fileCount] = entry;
          btnListArray[fileCount] = Adafruit_GFX_Button(); 
          //OSTimeDly(5);
          //btnListArray[fileCount].mp3FileName = entryname;
          
          
          
         
          
          lcdCtrl.setRotation(1);
          // Print filename on the LCD
          OSTimeDly(50);     // One reason for the delay here is 
                              // to allow the screen to be repainted by lower pri task.
                              // A semaphore might be a better way of controlling lcd access.
          btnListArray[fileCount].initButton(&lcdCtrl, 80, yPos, WIDTHBTN+80, HEIGHTBTN-5,BTN_OUTLINE, BTN_FILL, BTN_TEXT_COLOR,entry.name(), 2);
          btnListArray[fileCount].drawButton(false);

          yPos = yPos + 32;
          fileCount++;
        }
        
        entry.close();
    }
    dir.seek(0); // reset directory file to read again;  
    return true;

}



static boolean PlayMp3File(char * mp3FileName)
{
  
  //TODO: implement SDCard or Flash Mode
    char buf[BUFSIZE];
    int count = 0;
    
    if(playState && mp3TaskStarted){
      PrintWithBuf(buf, BUFSIZE, "Begin streaming sound file  count=%d\n", ++count);
      //Mp3Stream(hMp3, (INT8U*)StarWars, sizeof(StarWars));
      //Mp3Stream(hMp3, (INT8U*)PinkPanther, sizeof(PinkPanther));
      btnPlayObj.changeLabelButton(&lcdCtrl , STOPBTN);
      btnPlayObj.drawButton(false);
      if(IsMP3(mp3FileName))  //entry.name()
        Mp3StreamSDFile(hMp3,mp3FileName); ////entry.name()
      PrintWithBuf(buf, BUFSIZE, "Done streaming sound file  count=%d\n", count);
    }
    
    return true;
}

/************************************************************************************

   This task is the initial task running, started by main(). It starts
   the system tick timer and creates all the other tasks. Then it deletes itself.

************************************************************************************/
void StartupTask(void* pdata)
{
    char buf[BUFSIZE];

    PjdfErrCode pjdfErr;
    INT32U length;
    static HANDLE hSD = 0;
    static HANDLE hSPI = 0;

	PrintWithBuf(buf, BUFSIZE, "StartupTask: Begin\n");
	PrintWithBuf(buf, BUFSIZE, "StartupTask: Starting timer tick\n");

    // Start the system tick
//    OS_CPU_SysTickInit(OS_TICKS_PER_SEC);
    SysTick_Config(CLOCK_HSI / OS_TICKS_PER_SEC);
    
    // Initialize SD card
    PrintWithBuf(buf, PRINTBUFMAX, "Opening handle to SD driver: %s\n", PJDF_DEVICE_ID_SD_ADAFRUIT);
    hSD = Open(PJDF_DEVICE_ID_SD_ADAFRUIT, 0);
    if (!PJDF_IS_VALID_HANDLE(hSD)) while(1);


    PrintWithBuf(buf, PRINTBUFMAX, "Opening SD SPI driver: %s\n", SD_SPI_DEVICE_ID);
    // We talk to the SD controller over a SPI interface therefore
    // open an instance of that SPI driver and pass the handle to 
    // the SD driver.
    hSPI = Open(SD_SPI_DEVICE_ID, 0);
    if (!PJDF_IS_VALID_HANDLE(hSPI)) while(1);
    
   
    
    length = sizeof(HANDLE);
    pjdfErr = Ioctl(hSD, PJDF_CTRL_SD_SET_SPI_HANDLE, &hSPI, &length);
    if(PJDF_IS_ERROR(pjdfErr)) while(1);

     //Starting sd card reader
    if(!SD.begin(hSD) ){
      PrintWithBuf(buf, BUFSIZE, "Attempt to initialize SD card failed.\n");
    }
    // Create the test tasks
    PrintWithBuf(buf, BUFSIZE, "StartupTask: Creating the application tasks\n");
     
    
    // The maximum number of tasks the application can have is defined by OS_MAX_TASKS in os_cfg.h
    OSTaskCreate(LcdTouchDemoTask, (void*)0, &LcdTouchDemoTaskStk[APP_CFG_TASK_START_STK_SIZE-1], APP_TASK_TEST2_PRIO);
    OSTaskCreate(SystemInitTask, (void*)0, &SystemInitTaskStk[APP_CFG_TASK_START_STK_SIZE-1], APP_TASK_TEST3_PRIO);
    
             
     
     // Delete ourselves, letting the work be done in the new tasks.
    PrintWithBuf(buf, BUFSIZE, "StartupTask: deleting self\n");
	OSTaskDel(OS_PRIO_SELF);
}

void SystemInitTask(void* pdata)
{
    char buf[BUFSIZE];
    OSTimeDly(500);
    PrintWithBuf(buf, BUFSIZE, "InitSystemTask: starting\n");
    //Setting startpage as PAGE2
    currentPage = PAGE2;
    
    if(currentPage == PAGE3 ){
      DrawLcdContents("PINKPAN.MP3");
      currfilename = "PINKPAN.MP3";
    }
    
    if(currentPage == PAGE2 )
      GetSDFileList();
     PrintWithBuf(buf, BUFSIZE, "InitSystemTask: deleting self\n");
     
     OSTaskDel(APP_TASK_TEST3_PRIO);
}



/************************************************************************************

   Runs LCD/Touch demo code

************************************************************************************/
void LcdTouchDemoTask(void* pdata)
{
    
    PjdfErrCode pjdfErr;
    INT32U length;

	char buf[BUFSIZE];
	PrintWithBuf(buf, BUFSIZE, "LcdTouchDemoTask: starting\n");

	PrintWithBuf(buf, BUFSIZE, "Opening LCD driver: %s\n", PJDF_DEVICE_ID_LCD_ILI9341);
    // Open handle to the LCD driver
    HANDLE hLcd = Open(PJDF_DEVICE_ID_LCD_ILI9341, 0);
    if (!PJDF_IS_VALID_HANDLE(hLcd)) while(1);

	PrintWithBuf(buf, BUFSIZE, "Opening LCD SPI driver: %s\n", LCD_SPI_DEVICE_ID);
    // We talk to the LCD controller over a SPI interface therefore
    // open an instance of that SPI driver and pass the handle to 
    // the LCD driver.
    HANDLE hSPI = Open(LCD_SPI_DEVICE_ID, 0);
    if (!PJDF_IS_VALID_HANDLE(hSPI)) while(1);

    length = sizeof(HANDLE);
    pjdfErr = Ioctl(hLcd, PJDF_CTRL_LCD_SET_SPI_HANDLE, &hSPI, &length);
    if(PJDF_IS_ERROR(pjdfErr)) while(1);

	PrintWithBuf(buf, BUFSIZE, "Initializing LCD controller\n");
    lcdCtrl.setPjdfHandle(hLcd);
    lcdCtrl.begin();

    
    
     PrintWithBuf(buf, BUFSIZE, "Initializing FT6206 touchscreen controller\n");
    // Open handle to the Touch driver
    HANDLE hTouch = Open(PJDF_DEVICE_ID_TOUCH_FT6206, 0);
    if (!PJDF_IS_VALID_HANDLE(hTouch)) while(1);

    
    touchCtrl.setPjdfHandle(hTouch);
    //touchCtrl.begin();
    
    if (! touchCtrl.begin(40)) {  // pass in 'sensitivity' coefficient
        PrintWithBuf(buf, BUFSIZE, "Couldn't start FT6206 touchscreen controller\n");
        while (1);
    }
    
      
    PrintWithBuf(buf, BUFSIZE, "LCDDemoTask : Done\n");
    
    int currentcolor = ILI9341_RED;
    OSTimeDly(5);
    
    #if OS_CRITICAL_METHOD == 3u                                   /* Storage for CPU status register      */
      OS_CPU_SR  cpu_sr = 0u;
    #endif
    //OS_CPU_SR  cpu_sr;

     while (1) { 
        boolean touched = false;
        OSTimeDly(20);
        OS_ENTER_CRITICAL();
        // TODO: Poll for a touch on the touch panel
        // <Your code here>
        // <hint: Call a function provided by touchCtrl
        if(touchCtrl.touched()){
          
          touched = true;
        }
        
        OS_EXIT_CRITICAL();
 
        if (! touched) {
            //btnPlayObj.drawButton(false);
            OSTimeDly(1);
            continue;
        }
        
        TS_Point rawPoint;
       
        // TODO: Retrieve a point  
        
        TS_Point p = TS_Point();
        //p.x = MapTouchToScreen(rawPoint.x, 0, ILI9341_TFTWIDTH, ILI9341_TFTWIDTH, 0);
        //p.y = MapTouchToScreen(rawPoint.y, 0, ILI9341_TFTHEIGHT, ILI9341_TFTHEIGHT, 0);
        //Modified due to the orientation change
        //ILI9341_TFTHEIGHT = 320
        //ILI9341_TFTWIDTH = 240
       
        
        
        // <Your code here>
        if(touched){
          rawPoint =  touchCtrl.getPoint();
          p.x = MapTouchToScreen(rawPoint.y, ILI9341_TFTHEIGHT, 0, 0, ILI9341_TFTHEIGHT);
          p.y = MapTouchToScreen(rawPoint.x, 0, ILI9341_TFTWIDTH, 0, ILI9341_TFTWIDTH);
          PrintWithBuf(buf, BUFSIZE, "X: = %d, Y: %d, p.x: %d, p.y: %d\n", rawPoint.x, rawPoint.y, p.x,p.y );
          MP3Controller(p);
        }
        
        
        
        if (rawPoint.x == 0 && rawPoint.y == 0)
        {
            continue; // usually spurious, so ignore
        }
        
        lcdCtrl.setRotation(1);
        // transform touch orientation to screen orientation.
               
    	
        lcdCtrl.fillCircle((p.x),( p.y), PENRADIUS, currentcolor);
        
    }
    
    
   
}
/************************************************************************************

   Runs MP3 demo code

************************************************************************************/
void Mp3DemoTask(void* pdata)
{
    PjdfErrCode pjdfErr;
    INT32U length;
    
    //char filename[50];
    
    
    //OSTimeDly(5000); // Allow other task to initialize LCD before we use it.
    
	char buf[BUFSIZE];
	PrintWithBuf(buf, BUFSIZE, "Mp3DemoTask: starting\n");

	PrintWithBuf(buf, BUFSIZE, "Opening MP3 driver: %s\n", PJDF_DEVICE_ID_MP3_VS1053);
    // Open handle to the MP3 decoder driver
    hMp3 = Open(PJDF_DEVICE_ID_MP3_VS1053, 0);
    if (!PJDF_IS_VALID_HANDLE(hMp3)) while(1);

	PrintWithBuf(buf, BUFSIZE, "Opening MP3 SPI driver: %s\n", MP3_SPI_DEVICE_ID);
    // We talk to the MP3 decoder over a SPI interface therefore
    // open an instance of that SPI driver and pass the handle to 
    // the MP3 driver.
    hSPI = Open(MP3_SPI_DEVICE_ID, 0);
    if (!PJDF_IS_VALID_HANDLE(hSPI)) while(1);

    length = sizeof(HANDLE);
    pjdfErr = Ioctl(hMp3, PJDF_CTRL_MP3_SET_SPI_HANDLE, &hSPI, &length);
    if(PJDF_IS_ERROR(pjdfErr)) while(1);

    // Send initialization data to the MP3 decoder and run a test
    PrintWithBuf(buf, BUFSIZE, "Starting MP3 device test\n");
    Mp3Init(hMp3);
    
    while(1){
      OSTimeDly(500);
      if(playState && mp3TaskStarted)
        PlayMp3File(currfilename);
    }
    
}




// Renders a character at the current cursor position on the LCD
static void PrintCharToLcd(char c)
{
    lcdCtrl.write(c);
}

/************************************************************************************

   Print a formated string with the given buffer to LCD.
   Each task should use its own buffer to prevent data corruption.

************************************************************************************/
void PrintToLcdWithBuf(char *buf, int size, char *format, ...)
{
    va_list args;
    va_start(args, format);
    PrintToDeviceWithBuf(PrintCharToLcd, buf, size, format, args);
    va_end(args);
}




