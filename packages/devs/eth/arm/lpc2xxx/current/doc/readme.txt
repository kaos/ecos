Important note:
--------------

The LPC2xxx EMAC driver was tested with the LPC2468 OEM board from Embedded
Artists. The driver was tested with the lwIP TCP/IP stack and with Redboot.

While the driver works fine for the lwIP TCP/IP stack and in Redboot, it
is not possible to debug eCos applications via GDB ethernet connection or 
run eCos applications from Redboot command line if you are connected to 
Redboot via TCP/IP. You can test this, if you run or debug the tm_basic
test. The tm_basic test crashes, as soon as cyg_thread_delay() is executed
from the main thread (line 1678 in tm_basic.cxx). This call would cause a 
context switch.

I did a lot of debugging to identify the reason for this strange behaviour
but could not get a final result. The EMAC driver contains a lot of debug
messages. From reading the debug messages I'm quite sure that there is
no memory or stack violation from the ethernet driver. The multi-threaded
lwIP TCP echo test works without any poblems and also the programm upload
via GDB or the Redboot communication vi TCP/IP works fine.

It is no problem to start the tm_basic test from the Redboot command line
if you are connected via serial line. If you are connected via TCP/IP then
the tm_basic test crashes as soon as it comes to the cyg_thread_delay()
in line 1687. If the tm_basic test is started from Redboot (connected
via TCP/IP) with the -n option that forces Redboot to stop the ethernet
driver, then the test does not crash. So there seems to be a problem if
the TCP/IP stack is active if Redboot starts a RAM application. I did a
lot of JTAG debugging to find the reason for this behaviour but was not
succesful.

I decided to release the driver because I'm quite sure that the driver
itself works well. Maybe this is a board related issue or a startup code
related issue. Maybe someone will find the reason.
 
