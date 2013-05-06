/* reloc_x86_64.c - position independent x86_64 ELF shared object relocator
   Copyright (C) 1999 Hewlett-Packard Co.
	Contributed by David Mosberger <davidm@hpl.hp.com>.
   Copyright (C) 2005 Intel Co.
	Contributed by Fenghua Yu <fenghua.yu@intel.com>.

    All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions
    are met:

    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above
      copyright notice, this list of conditions and the following
      disclaimer in the documentation and/or other materials
      provided with the distribution.
    * Neither the name of Hewlett-Packard Co. nor the names of its
      contributors may be used to endorse or promote products derived
      from this software without specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND
    CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
    INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
    MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
    DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
    BE LIABLE FOR ANYDIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY,
    OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
    PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
    PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
    THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
    TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
    THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
    SUCH DAMAGE.
*/

#include <elf.h>
#include <link.h>	/* get _DYNAMIC decl and ElfW and ELFW macros */


#undef NULL
#define uint64_t	efi_uint64_t
#define int64_t		efi_int64_t
#define uint32_t	efi_uint32_t
#define int32_t		efi_int32_t
#define uint16_t	efi_uint16_t
#define int16_t		efi_int16_t
#define uint8_t		efi_uint8_t
#define int8_t		efi_int8_t

#undef NULL
#define uint64_t	efi_uint64_t
#define int64_t		efi_int64_t
#define uint32_t	efi_uint32_t
#define int32_t		efi_int32_t
#define uint16_t	efi_uint16_t
#define int16_t		efi_int16_t
#define uint8_t		efi_uint8_t
#define int8_t		efi_int8_t

#include <efi.h>


EFI_STATUS _relocate (long ldbase, ElfW(Dyn) *dyn, EFI_HANDLE image, EFI_SYSTEM_TABLE *systab)
{
	long relsz = 0, relent = 0;
	ElfW(Rel) *rel = 0;
	unsigned long *addr;
	int i;

	for (i = 0; dyn[i].d_tag != DT_NULL; ++i) {
		switch (dyn[i].d_tag) {
			case DT_RELA:
				rel = (ElfW(Rel)*)
					((unsigned long)dyn[i].d_un.d_ptr
					 + ldbase);
				break;

			case DT_RELASZ:
				relsz = dyn[i].d_un.d_val;
				break;

			case DT_RELAENT:
				relent = dyn[i].d_un.d_val;
				break;

			default:
				break;
		}
	}

        if (!rel && relent == 0)
                return EFI_SUCCESS;

 	if (!rel || relent == 0)
 		return EFI_LOAD_ERROR;

	while (relsz > 0) {
		/* apply the relocs */
		switch (ELF64_R_TYPE (rel->r_info)) {
			case R_X86_64_NONE:
				break;

			case R_X86_64_RELATIVE:
				addr = (unsigned long *)
					(ldbase + rel->r_offset);
				*addr += ldbase;
				break;

			default:
				break;
		}
		rel = (ElfW(Rel)*) ((char *) rel + relent);
		relsz -= relent;
	}
	return EFI_SUCCESS;
}
