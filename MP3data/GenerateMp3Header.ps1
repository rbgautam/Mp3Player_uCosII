<#
.SYNOPSIS
    Powershell script to read an input file and dump it to standard output  
    in the form of a C byte array constant where each byte 
    in the input is printed in hexadecimal form like 0x##

    Note: In Powershell, to get the output in ASCII format you have to pipe it to Out-File like this:
    & c:\MyPath1\GenerateMp3Header c:\MyPath2\InFile.mp3 -MaxBytes 40000 | Out-File -Encoding ascii MyFile.h
#>

param(
    [parameter(mandatory=$true)][string]$InFile,
    [parameter(mandatory=$false)][int]$MaxBytes = -1
)

$startedWriting = $false

#$bytesPerLine = 16
$bytesPerLine = 32
$buffer = new-object Byte[] $bytesPerLine

$stream = [System.IO.File]::OpenRead($InFile)
if (!$?)
{
    throw "Cannot open file $InFile"
}

$bytesToRead = $stream.Length
if ($bytesToRead -eq 0)
{
    throw "Empty input file: $InFile"
}

if ($MaxBytes -gt 0 -and $MaxBytes -lt $bytesToRead)
{
    $bytesToRead = $MaxBytes
}

while ($stream.Position -lt $bytesToRead)
{
    if ($bytesToRead - $stream.Position -lt $bytesPerLine)
    {
        # handle case where last line is not full
        $bytesPerLine = $bytesToRead - $stream.Position
    }
    
    $byteCount = $stream.Read($buffer, 0, $bytesPerLine)
    if (!$startedWriting -and $byteCount -gt 0)
    {
        echo "const unsigned char Wave[] ="
        echo "{"
        $startedWriting = $true
    }
    $line = new-object System.Text.StringBuilder
    for ($i=0; $i -lt $byteCount; $i++)
    {
        $tmp = $line.Append([String]::Format("0x{0:X2},", $buffer[$i]))
    }
    echo $line.ToString()
}

if ($startedWriting)
{
    echo "};"
}