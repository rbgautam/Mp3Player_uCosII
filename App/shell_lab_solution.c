/*
    shell.c
    A simple command line interface shell.

    Developed for University of Washington embedded systems programming certificate
    
    2016/3 Nick Strathy wrote/arranged it
*/



// TODO: shouldn't know about the hardware in this module:

#include "bsp.h"
#include "print.h"

#define BUFSIZE 256
#define ARRAYCOUNT(array) (sizeof(array)/sizeof(*array))

static void PJShellcd(char *dir);
static void PJShellgetticks(void);
static void PJShellls(void);


// Define command strings here
static const char *CmdList[] =
{
	"cd",
    "getticks",
	"ls",
};

static int cmdLen[ARRAYCOUNT(CmdList)];

typedef enum
{
	CommandEnumcd,
	CommandEnumgetticks,
	CommandEnumls,
	CommandEnumInvalid
}CommandEnum_t;

/*
 NAME:
   PJShellInit
 PURPOSE:
   Initialize the command line interpreter.
 PARAMETERS:
   none
 RETURN:
   none
 EXAMPLE:
   PJShellInit()
 OTHER:
 */
static void PJShellInit(void)
{
	// Initialize the array of command string lengths
	int iCmdList;
	for (iCmdList = 0; iCmdList < ARRAYCOUNT(CmdList); iCmdList += 1)
	{
		cmdLen[iCmdList] = strlen(CmdList[iCmdList]);
	}
    //curDir = "/";
}


/*
 NAME:
   PJShellEntry
 PURPOSE:
   Entry point for the command line interpreter thread.
 PARAMETERS:
   pArg: not used
 RETURN:
   none
 EXAMPLE:
   PJShellEntry()
 OTHER:
 */
void PJShellEntry(void *pArg)
{
	PJShellInit();
    OSTimeDly(2000); // wait for other tasks to initialize

    while(1)
    {
    	PrintString("Shell>");
    	char cmdLine[81];
    	int iCmdLine = 0;
    	char ch = 0;

    	while (iCmdLine < ARRAYCOUNT(cmdLine) - 1)
    	{
			ch = ReadByte();
			PrintByte(ch);

			// Break when 'Enter' is pressed
			if ((ch == '\r') || (ch == '\n'))
			{
				PrintString("\r\n");
				break;
			}

			cmdLine[iCmdLine] = ch;
			iCmdLine += 1;
    	}

    	cmdLine[iCmdLine] = 0; // ensure that command line is null terminated

    	CommandEnum_t cmdEnum;
    	for (cmdEnum = CommandEnumcd; cmdEnum < CommandEnumInvalid; cmdEnum  = (CommandEnum_t) ((int)cmdEnum + 1))
    	{
    		if (!strncmp(cmdLine, CmdList[cmdEnum], cmdLen[cmdEnum]))
    		{
    			break;
    		}
    	}

		switch (cmdEnum)
		{
		case CommandEnumcd:
			PJShellcd(&cmdLine[cmdLen[CommandEnumcd]] + 1);
			break;
		case CommandEnumgetticks:
			PJShellgetticks();
			break;
		case CommandEnumls:
			PJShellls();
			break;
		default:
			PrintString("  invalid command\r\n");
			break;
		}

    }
}


static void PJShellcd(char *dir)
{
    PrintString("Hello from cd command\n");
}


static void PJShellgetticks()
{
    INT32U ticks = OSTimeGet();
    Print_uint32(ticks);
    PrintString("\n");
}


static void PJShellls()
{
    PrintString("Hello from ls command\n");
}


