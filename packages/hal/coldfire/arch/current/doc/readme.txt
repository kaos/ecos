SOME NOTES ON THE USAGE OF THE HAL FOR COLDFIRE

This HAL was obtained by rewriting the original port of eCos to ColdFire done
by Wade Jensen. The following guidelines give you hints about using this
software.

* Use at least GCC version 3.4.1. This version has an improved support for
  ColdFire processors. However, this compiler might not be able to build the
  "cxxsupp.cxx" test in the "infra" package.

* The version of the binutils I used is 2.15.90.0.1.1.

* The version of newlib I used is 1.13.0.

* The version of the BDM tools I used is 1.3.0.

* If you want to debug code by means of the serial connection to eCos, don't
use a version of GDB with BDM support compiled into. There is some kind of
interaction between BDM and serial targets that prevents the latter from
functioning correctly. Do not even use a clean GDB 6.3 distribution to debug
through the serial cable. The downloading of code to the target is broken in
that version. Instead, use a version of GDB grabbed from CVS repository. I
used a version downloaded after June 13, 2005, and the bug had been corrected.

* Currently (version 6.3), GDB doesn't support the ColdFire MAC unit. Thus, it
is not possible to show the MAC registers, via the "info registers" command,
when debugging an eCos application through a serial connection. The BDM addon
to GDB doesn't suffer from a similar limitation. However, there is a little
hack in order to show and modify the MAC registers even with a serial
connection. When a breakpoint is hit, the GDB module of eCos stores the
current register values in a structure of type HAL_SavedRegisters, pointed to
by the _hal_registers variable. If you included the GDB stub in your
application, it is then possible to display the MAC registers (and all of the
others) by issuing at the GDB prompt the following command:

print *(struct HAL_SavedRegisters *)_hal_registers

In case you are using a GDB stub burned in ROM to debug an application in RAM,
you first have to determine where the GDB stub stores the _hal_registers
variable in its own private region of RAM: use the tool m68k-elf-objdump to
find that. For example, if the address of the _hal_registers variable used by
the GDB stub is 0x1e88, then you should use the following command to display
the registers of the application being debugged:

print *(struct HAL_SavedRegisters *)*0x1e88

It is also possible to modify the value of the MAC registers, by updating the
relative field of the HAL_SavedRegisters structure. Don't try to modify the
other registers because the modifications will be discarded when the
application is restarted: use the usual GDB commands instead. Finally, the
correct value of the PC register is the one shown by the "info registers"
command.

* I added at the architecture level only the features that I could directly
test. When I developed the architecture HAL, I had a ColdFire MCF5272 at
hands, which has a MAC unit. That's why in the architecture HAL there is
currently only support for the MAC unit, and no support for the EMAC and
floating point units. However, I tried to write the HAL in the most generic
fashion I could imagine, in order to make it easy to add new architectural
features and new processor variants.

* If you want to burn an eCos image into the flash ROM, you cannot rely on the
ROM monitor provided with the board, but you have to use the BDM interface.
If you work under Windows, you can use the free CFFlasher utility, available
on the Freescale Semiconductor web site. The BDM tools also contain a
text-mode utility to do that, but I have not tested it. 


Enrico Piria (epiria AT fastwebnet DOT it)
November 16, 2005
