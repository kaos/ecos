//####COPYRIGHTBEGIN####
//                                                                          
// ----------------------------------------------------------------------------
// Copyright (C) 1998, 1999, 2000 Red Hat, Inc.
//
// This program is part of the eCos host tools.
//
// This program is free software; you can redistribute it and/or modify it 
// under the terms of the GNU General Public License as published by the Free 
// Software Foundation; either version 2 of the License, or (at your option) 
// any later version.
// 
// This program is distributed in the hope that it will be useful, but WITHOUT 
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or 
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for 
// more details.
// 
// You should have received a copy of the GNU General Public License along with
// this program; if not, write to the Free Software Foundation, Inc., 
// 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
//
// ----------------------------------------------------------------------------
//                                                                          
//####COPYRIGHTEND####
//=================================================================
//
//        ser_filter.cxx
//
//        Serial test filter
//
//=================================================================
//=================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):     jskov
// Contributors:  jskov
// Date:          1999-03-01
// Description:   This program acts as a filter between GDB and the test
//                running on the target, allowing testing of the serial
//                driver without confusing GDB.
// Usage:         Run the program with one argument, the serial port
//                on with the target is connected.
//                Run serial test in GDB, connecting to the target with
//                'target remote localhost:5678'.
//
// To Do:
//  o Add timeout setup and handling for recovery, can rely on testing
//    agent to control global timeout.
//  o Saving chunks that caused transfer failure?
//     - In SEND with echo, do CRC on 32-byte sub-packets
//  o Additional To Do items under each sub-protocol function.
//  o Option to get all serial IO written (in hex, > and < prepends 
//    input/output lines) to a file.
//  o Clean up the mess in this file....
//  o Make main() listen on two ports - use one for ser filter, the other for
//    null filter connections.
//  o Maybe use environment variable to find X10 reset command.
//####DESCRIPTIONEND####

#include "eCosTestSerialFilter.h"
#include "eCosTestDownloadFilter.h"
#include "eCosTestUtils.h"
#include "eCosTrace.h"

#define __USE_DL_FILTER

bool opt_ser_debug = false;
bool opt_null_filter = false;
bool opt_console_output = false;
bool opt_X10_reset = false;
bool opt_filter_trace = false;
char opt_X10_port[2];

void
no_gdb(const char* pszPort, int nBaud, 
       CeCosTestSocket::FilterFunc *pSerialToSocketFilterFunc,
       void *pParam, bool *pbStop)
{
  fprintf(stderr, "no_gdb, listening on %s\n",pszPort);
  
  CeCosTestSocket dummy_socket;
  CeCosTestSerial serial;
  serial.SetBlockingReads(false);
  bool rc=false;
  
  // Open serial device.
  if (!serial.Open(pszPort,nBaud)){
    ERROR("Couldn't open port %s\n",pszPort);
  } else {
    // Flush the serial buffer.
    serial.Flush();
    
    serial.ClearError();
    enum {BUFSIZE=8192};
    void *pBuf=malloc(BUFSIZE);
    rc=true;
    while(rc && (0==pbStop || !(*pbStop))){
      unsigned int nRead=0;
      
      for(;;) {
        if(serial.Read(pBuf,BUFSIZE,nRead)){
          if(nRead>0){
            break;
          }
          Sleep(1);
        } else {
          fprintf(stderr, "Serial read failed (%d)\n", errno);
        }
      }
      
      if(pSerialToSocketFilterFunc){
        rc=pSerialToSocketFilterFunc(pBuf,nRead,serial,dummy_socket,
          pParam);
      }
    }
    free(pBuf);
  }
}

int
main(int argc, char** argv)
{
  int nSock = 0;
  int baud_rate, nTcpPort;
  
  bool opt_no_gdb = false;
  char* ser_port;
  int i=1;
  if(!CeCosTestUtils::CommandLine(argc,argv)){
    goto Usage;
  }
  if(!CeCosTest::Init()){
    goto Usage;
  }
  
  while(i<argc){
    if(argv[i][0]=='-'){
      switch(argv[i][1]){
      case 't':
        CeCosTrace ::EnableTracing(true);
        break;
      case 'f':
        opt_filter_trace = true;
        break;
      case 'S':
        opt_ser_debug = true;
        break;
      case 'n':
        opt_no_gdb = true;
        // fall through! Output on console when no GDB.
      case 'c':
        opt_console_output = true;
        break;
      case '0':
        opt_null_filter = true;
        break;
      case 'X':
        // X-10 port to reset when connection drops
        opt_X10_reset = true;
        opt_X10_port[0] = argv[i][2];
        opt_X10_port[1] = argv[i][3];
        break;
      default:
        fprintf(stderr,"Unrecognized switch %s\n",argv[i]);
        goto Usage;
        break;
      }
      for(int j=i;j<argc;j++){
        argv[j]=argv[j+1];
      }
      argc--;
      argv[argc]=0;
    } else {
      i++;
    }
  }
  
  if(!((3==argc && opt_no_gdb) || (4==argc && !opt_no_gdb)))
  {
    goto Usage;
  }
  
  if (opt_no_gdb) {
    ser_port = argv[1];
    baud_rate=atoi(argv[2]);
  } else {
    nTcpPort=atoi(argv[1]);
    if(0==nTcpPort){
      fprintf(stderr,"Invalid port %s\n",argv[1]);
      return main(0,argv); // Provoke usage message
    }
    
    ser_port = argv[2];
    baud_rate=atoi(argv[3]);
    
    nSock = CeCosTestSocket::Listen(nTcpPort);
    if (-1 == nSock) {
      fprintf(stderr, "Couldn't access socket.\n");
      throw "listen failed";
    }
  }
  
  
  
  for(;;) {
    CeCosTestSerialFilter* serial_filter = 
      new CeCosTestSerialFilter();
#ifdef __USE_DL_FILTER
    CeCosTestDownloadFilter* download_filter = 
      new CeCosTestDownloadFilter();
#else // UNIX
    void* download_filter = NULL;
#endif
    
    // Set filter configuration
    serial_filter->SetFilterTrace(opt_filter_trace);
    serial_filter->SetSerialDebug(opt_ser_debug);
    serial_filter->SetConsoleOutput(opt_console_output);
    
#ifdef __USE_DL_FILTER
    // Set download filter configuration
    download_filter->SetFilterTrace(opt_filter_trace);
    download_filter->SetSerialDebug(opt_ser_debug);
#endif
    
    // Set serial side filter
    CeCosTestSocket::FilterFunc *ser_filter_function = 
      &SerialFilterFunction;
    if (opt_null_filter)
      ser_filter_function = NULL;
    
#ifdef __USE_DL_FILTER
    // Set socket side filter
    CeCosTestSocket::FilterFunc *sock_filter_function =
      &DownloadFilterFunction;
#else // UNIX
    CeCosTestSocket::FilterFunc *sock_filter_function = NULL;
    
#endif
    
    try {
      if (opt_no_gdb)
        no_gdb(ser_port, baud_rate, ser_filter_function, 
        (void*)serial_filter, NULL);
      else
        CeCosTestSocket::ConnectSocketToSerial(nSock, ser_port, 
        baud_rate, 
        ser_filter_function, 
        (void*)serial_filter, 
        sock_filter_function, 
        (void*)download_filter,
        NULL);
    } 
    catch (const char* p) {
      fprintf(stderr, "Caught filter crash: %s\n", p);
    }
    
    if (opt_X10_reset) {
      char X10_cmd[80];
      sprintf(X10_cmd, "/usr/unsupported/bin/x10cli x10 5000 %c %c", 
        opt_X10_port[0], opt_X10_port[1]);
      system(X10_cmd);
    }
    
    delete serial_filter;
    delete download_filter;
  }
  
  CeCosTest::Term();
  return 0;
  
Usage:
  const char *pszMe="ser_filter";
  fprintf(stderr,"Usage: %s [-t -c -S -0 -Xab] TcpIPport SerialPort BaudRate\n",
    //                                              1         2          3
    pszMe);
  fprintf(stderr," or:   %s -n [-t -c -S -0 -Xab] SerialPort BaudRate\n",
    //                                              1          2
    pszMe);
  fprintf(stderr, " -t: Enable tracing.\n");
  fprintf(stderr, " -f: Enable filter output tracing.\n");
  fprintf(stderr, " -S: Output data read from serial line.\n");
  fprintf(stderr, " -c: Output data on console instead of via GDB.\n");
  fprintf(stderr, " -n: No GDB.\n");
  fprintf(stderr, " -0: Use null filter.\n");
  fprintf(stderr, " -Xab: Reset X-10 Port 'a b' when TCP connection breaks\n");
  CeCosTest::Term();
  return 1;
}

