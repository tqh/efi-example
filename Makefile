#
# Copyright (c) 2011, Intel Corporation
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
#
#    * Redistributions of source code must retain the above copyright
#      notice, this list of conditions and the following disclaimer.
#    * Redistributions in binary form must reproduce the above
#      copyright notice, this list of conditions and the following
#      disclaimer in the documentation and/or other materials provided
#      with the distribution.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
# "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
# LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
# FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
# COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
# INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
# BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
# LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
# CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
# LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
# ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.
#

%.efi: %.so
	objcopy -j .text -j .sdata -j .data -j .dynamic -j .dynsym -j .rel \
		-j .rela -j .reloc -S --target=$(FORMAT) $*.so $@

%.S: %.c
	$(CC) $(INCDIR) $(CFLAGS) $(CPPFLAGS) -S $< -o $@

#TODO: At the moment only x86_64 is tested.
ARCH := $(shell $(CC) -dumpmachine | sed "s/\(-\).*$$//")

ifneq ($(ARCH),x86_64)
	ARCH=ia32
endif

INCDIR := headers


CFLAGS=-I. -I$(INCDIR) -I$(INCDIR)/$(ARCH) \
		-DGNU_EFI_USE_MS_ABI -fPIC -fshort-wchar -ffreestanding \
		-maccumulate-outgoing-args \
		-Wall -D$(ARCH) -Werror

ifeq ($(ARCH),ia32)
	CFLAGS += -m32
	FORMAT=efi-app-x86-64
endif
ifeq ($(ARCH),x86_64)
	CFLAGS += -m64 -mno-red-zone
	FORMAT=efi-app-$(ARCH)
endif

LDFLAGS=-T glue/$(ARCH)/elf_efi.lds -Bsymbolic -shared -nostdlib -znocombreloc 

EXAMPLES=example.efi gfx_example.efi
COMMON = glue/$(ARCH)/relocation_func.o glue/$(ARCH)/start_func.o


all: $(EXAMPLES)


example.efi: example.so

example.so: $(COMMON) example.o
	$(LD) $(LDFLAGS) -o $@ $^ $(shell $(CC) $(CFLAGS) -print-libgcc-file-name)


gfx_example.efi: gfx_example.so

gfx_example.so: $(COMMON) gfx_example.o
	$(LD) $(LDFLAGS) -o $@ $^ $(shell $(CC) $(CFLAGS) -print-libgcc-file-name)


clean:
	rm -f $(EXAMPLES) *.so $(COMMON) example.o gfx_example.o
