EFI-example
===========

Self-contained minimal example of building an EFI application (under 64 bit Linux atm) without external build dependencies.

This project was created to research the base for an EFI bootloader for the Haiku Operating System, http://www.haiku-os.org/


Background
----------
So I've been researching EFI applications a lot for Haiku and I think I've finally made some sense of it. EFI applications use the PE image format and use Microsoft register calling conventions which means it is a pain in the ass to build from non Microsoft OS'es. Thankfully GNU-EFI provides a way to easily build EFI applications, but it is a bit complicated for our needs.

We don't need the complexity as we don't care about different platforms and compilers or extra functionality that GNU-EFI provides.

Therefore I stripped it down to the bare basics: the assembly start function, the relocation function and the EFI headers. Also setup all the specific compile time flags needed for gcc (>=4.7) and the linker script to create the .efi file. This gives us an EFI application that is 4.7KB, prints a message and waits for a key before exiting.

And this is that project:
https://github.com/tqh/efi-example

Building
--------
Make sure you have gcc 4.7.0 or newer. Hopefully your objcopy will be able to produce EFI applications, you probably need to update binutils if not.

In a terminal:

`make` will produce the EFI application *example.efi*. 

And also *gfx_example.efi* which is a more advanced example showing how to load the
gfx protocol and iterating thru the modes.

`make clean` will remove build objects.

Testing in QEMU
---------------
You will need to install QEMU and have the Tianocore EFI firmware to test the EFI application.

You can download the x64 firmware from here: http://sourceforge.net/apps/mediawiki/tianocore/index.php?title=OVMF

Follow their guide on how to run: http://sourceforge.net/apps/mediawiki/tianocore/index.php?title=How_to_run_OVMF

You need to do the following for setup:

1. Download OVMF file.
2. Unpack.
3. Rename *OVMF.fd* to *bios.bin*.
4. Rename *CirrusLogic5446.rom* to *vgabios-cirrus.bin*.
5. Create a *hd* directory that will be used as a FAT harddrive in Qemu.

And this is the actual test procedure:

6. Copy *example.efi* to this directory.
7. Launch QEMU with `qemu-system-x86_64 -L . -hda fat:hd` where *hd* is the directory acting as harddrive.
8. QEMU will launch showing the Tianocore logo and will print some text and give you a `shell>`-prompt.
9. Switch to *fs0* by typing `fs0:`.
10. You can use `ls` to show the files on *fs0*. `help` is another useful command.
11. Execute *example.efi* by typing `example.efi`.
12. The application will run, print the message and wait for a key.
13. Press a key to exit and return to shell.

Testing or Running on Real Hardware
------------------------
WARNING: I take no absolutly NO responsablitiy for this, make sure you know what you are doing. This is just a quick summary.

At the moment my XUbuntu don't boot if *example.efi* boots before, so DO NOT DO THIS UNLESS YOU KNOW WHAT YOU ARE DOING.
YOU SHOULD FAMILARIZE YOURSELF WITH HOW TO EDIT AND REMOVE BOOT OPTIONS IN THE FIRMWARE BEFORE EXPEREMENTING WITH THIS!

Usually the UEFI boot partition is mounted at */boot/efi*. It should have a *EFI* sub-directory that contains
the different EFI applications. These apps probably have their own directories as well:
```
ls /boot/efi/EFI
ASUS  Boot  efilinux  haiku  Microsoft  ubuntu
```

1. Create a directory for our application */boot/efi/EFI/example* will do.
2. Copy *example.efi* to the */boot/efi/EFI/example* directory.
3. You need to use, and probably install the *efibootmgr* application that can add and install boot options. You can also add and remove options in your firmware but I will not explain this here.
All the calls to *efibootmgr* below are done with `sudo`
4. Running `efibootmgr` will list the boot order and in a cryptic way:

```
BootCurrent: 0002
Timeout: 0 seconds
BootOrder: 0001,0002,0000,0003
Boot0000* Windows Boot Manager
Boot0001* Haiku
Boot0002* ubuntu
Boot0003* ubuntu
```

NOTE: Always make sure to note the boot order, so that you can always restore it.

In this example it will try to run 0001 first, and then when that exits or fails go on to boot 0002, which is Ubuntu.

If I wanted to change the bootorder I could execute `efibootmgr --bootorder 0,1,2,3` (these are hex numbers) and it would change to:

```
BootCurrent: 0002
Timeout: 0 seconds
BootOrder: 0000,0001,0002,0003
Boot0000* Windows Boot Manager
Boot0001* Haiku
Boot0002* ubuntu
Boot0003* ubuntu
```
*BootCurrent* is just telling me that at the moment Ubuntu is booted.

To add */boot/efi/EFI/example/example.efi* application you need to add it to the boot order.
This will only be needed to be done once, after that you can just replace example.efi when you test a newer version.

`efibootmgr -c -L "Example" -l 'EFI\example\example.efi'` creates an new entry *Example* that points to *example.efi*.

It will be added first to the boot order and run when you start. It will print the message, wait for key and exit, which will cause the next boot item to launch.


Removing the boot option *Boot0004* can be done with:
`efibootmgr -B -b 4` where *B* is for *Delete* and *b* points to the bootnumber *Boot0004* 

Useful Links
------------
http://wiki.phoenix.com/wiki/index.php/Category:UEFI_2.1

Files
-----
All files except this README.MD, example.c and Makefile come from GNU-EFI: http://sourceforge.net/projects/gnu-efi/  
The example.c and Makefile was originally from efilinux, but have been modified extensivly: https://github.com/mfleming/efilinux

License
-------
Most of the files contain appropriate licensing information.
The headers subdirectory come from gnu-efi's inc dir and their README.efilib states:
```
The files in the "lib" and "inc" subdirectories are using the EFI Application 
Toolkit distributed by Intel at http://developer.intel.com/technology/efi

This code is covered by the following agreement:

Copyright (c) 1998-2000 Intel Corporation

Redistribution and use in source and binary forms, with or without modification, are permitted
provided that the following conditions are met:

Redistributions of source code must retain the above copyright notice, this list of conditions and
the following disclaimer.

Redistributions in binary form must reproduce the above copyright notice, this list of conditions
and the following disclaimer in the documentation and/or other materials provided with the
distribution.

THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES,
INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL INTEL BE
LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGE. THE EFI SPECIFICATION AND ALL OTHER INFORMATION
ON THIS WEB SITE ARE PROVIDED "AS IS" WITH NO WARRANTIES, AND ARE SUBJECT
TO CHANGE WITHOUT NOTICE.
```

The files in the glue sub-directory comes from gnu-efi's gnuefi sub directories. Most of the files contain
appropriate licensing but for the files lacking README.gnuefi states:
```
[...]
 gnuefi: This directory contains the glue necessary to convert ELF64
  binaries to EFI binaries.  Various runtime code bits, such as
	a self-relocator are included as well.  This code has been
	contributed by the Hewlett-Packard Company and is distributed
	under the GNU GPL.
[...]
```
The Makefile and example.c contains their license and should be the same as the Intel one above.

