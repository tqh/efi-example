EFI-example
===========

Self-contained minimal example of building an EFI application (under 64 bit Linux atm) without external build dependencies.

TODO: Much more info...

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

