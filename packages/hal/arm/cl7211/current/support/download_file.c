//****************************************************************************
//
// download_file.c - Send a raw data file to the EB72xx board.
// 
// Adapted from the Cirrus Logic 'DOWNLOAD.C' program
//
// Copyright (c) 1999 Cirrus Logic, Inc.
//
// Adapted for Linux by Cygnus Solutions, Inc.
//
//****************************************************************************
#include <stdio.h>
#include "bootcode.h"

#define DEFAULT_PORT "/dev/ttyS0"
extern char ReceiveChar(long);
extern void SendChar(long, char);
extern void SetBaud(long, long);
extern int  CharRead(long port);
extern void WaitForOutputReady(long port);
extern long OpenPort(char *);

//****************************************************************************
//
// WaitFor waits until a specific character is read from the comm port.
//
//****************************************************************************
void
WaitFor(long lPort, char cWaitChar)
{
    char cChar;

    //
    // Wait until we read a specific character from the comm port.
    //
    while(1)
    {
        //
        // Read a character.
        //
        cChar = ReceiveChar(lPort);

        //
        // Stop waiting if we received the character.
        //
        if(cChar == cWaitChar)
        {
            break;
        }
    }
}

//****************************************************************************
//
// This program waits for the '<' character from the boot ROM, sends the boot
// code, waits for the '>' from the boot ROM, waits for the '?' from the boot
// code, changes the serial port rate (preferably to 115200), downloads the
// user data file, and then prints out progress status as the boot code writes
// the user data file to the flash.
//
//****************************************************************************
void
main(int argc, char *argv[])
{
    long lPort;
    long lRate = 38400;
    long lFileSize, lIdx;
    char cChar, cFirstChar, *pcFile, cRateChar;
    FILE *pFile;

    //
    // Make sure that a filename was specified.
    //
    if(argc < 2)
    {
        fprintf(stderr, "Usage: download <filename> {<baud rate> {<comm port>}}\n");
        return;
    }

    //
    // If a baud rate was specified, then read it and make sure it is valid.
    //
    if(argc > 2)
    {
        lRate = atoi(argv[2]);
        if((lRate != 9600) && (lRate != 19200) && (lRate != 28800) &&
           (lRate != 38400) && (lRate != 57600) && (lRate != 115200))
        {
            fprintf(stderr, "Invalid baud rate.\n");
            return;
        }
    }

    //
    // If a comm port was specified, then read it and make sure it is valid.
    //
    if(argc > 3)
    {
        lPort = OpenPort(argv[3]);
    } else 
    {
        lPort = OpenPort(DEFAULT_PORT);
    }
    if (lPort < 0) 
    {
        fprintf(stderr, "Can't open port\n");
        return;
    }

    //
    // Open the file to be downloaded.
    //
    pFile = fopen(argv[1], "rb");
    if(!pFile)
    {
        fprintf(stderr, "Could not open file '%s'.\n", argv[1]);
        return;
    }

    //
    // Get the size of the file.
    //
    fseek(pFile, 0, SEEK_END);
    lFileSize = ftell(pFile);
    fseek(pFile, 0, SEEK_SET);

    //
    // Allocate memory to hold the file contents.
    //
    pcFile = (char *)malloc(lFileSize);
    if(!pcFile)
    {
        fprintf(stderr, "Failed to allocate memory for the file.\n");
        return;
    }

    //
    // Read the contents of the file into memory.
    //
    if(fread(pcFile, 1, lFileSize, pFile) != lFileSize)
    {
        fprintf(stderr, "Failed to read file '%s'.\n", argv[1]);
        return;
    }

    //
    // Close the file.
    //
    fclose(pFile);

    //
    // Open the port to the desired baud rate.
    //
    SetBaud(lPort, lRate);

    //
    // Empty out the input queue.
    //
    while(CharReady(lPort))
    {
        cChar = ReceiveChar(lPort);
    }

    //
    // Tell the user to reset the board.
    //
    fprintf(stderr, "Waiting for the board to wakeup...");

    //
    // Send a '-' character until we receive back a '?' character.
    //
    while(1)
    {
        //
        // Send a '-' character.
        //
        SendChar(lPort, '-');

        //
        // Wait a little bit.
        //
        for(lIdx = 0; lIdx < 1024 * 1024; lIdx++)
        {
        }

        //
        // See if there is a character waiting to be read.
        //
        if(CharReady(lPort))
        {
            //
            // Read the character.
            //
            cChar = ReceiveChar(lPort);

            //
            // Quit waiting if this is a '?'.
            //
            if(cChar == '?')
            {
                break;
            }
        }
    }

    //
    // Empty out the input queue.
    //
    while(CharReady(lPort))
    {
        cChar = ReceiveChar(lPort);
    }

    //
    // Send the download file "command"
    //
    SendChar(lPort, 'D');

    //
    // Send the length of the data file.
    //
    SendChar(lPort, (char)(lFileSize & 0xFF));
    SendChar(lPort, (char)((lFileSize >> 8) & 0xFF));
    SendChar(lPort, (char)((lFileSize >> 16) & 0xFF));
    SendChar(lPort, (char)((lFileSize >> 24) & 0xFF));

    //
    // Tell the user that we are downloading the file data.
    //
    fprintf(stderr, "\nDownloading file data...(  0%%)");

    //
    // Send the actual data in the file.
    //
    for(lIdx = 0; lIdx < lFileSize; lIdx++)
    {
        //
        // Send this byte.
        //
        SendChar(lPort, pcFile[lIdx]);

        //
        // Periodically print out our progress.
        //
        if((lIdx & 127) == 127)
        {
            fprintf(stderr, "\b\b\b\b\b%3d%%)", ((lIdx + 1) * 100) / lFileSize);
            //
            // Read a character from the boot code.
            //
            cChar = ReceiveChar(lPort);
        }
    }

    //
    // Tell the user we are done.
    //
    fprintf(stderr, "\nSuccessfully downloaded '%s'.\n", argv[1]);
}

