/*
 * Copyright (c) 2011, Intel Corporation
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 *    * Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *    * Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer
 *      in the documentation and/or other materials provided with the
 *      distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 */
 
#include "efibind.h"
#include "efidef.h"
#include "efidevp.h"
#include "eficon.h"
#include "efiapi.h"
#include "efierr.h"
#include "efiprot.h"

//static EFI_GUID GraphicsOutputProtocolGUID = EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID;
static EFI_GUID BlockIoProtocolGUID = BLOCK_IO_PROTOCOL;
static EFI_GUID DevicePathGUID = DEVICE_PATH_PROTOCOL;
//static CHAR16 *haikuGUID = L"42465331-3BA3-10F1-802A-4861696B7521";

void printInt(SIMPLE_TEXT_OUTPUT_INTERFACE *conOut, int value) {
	CHAR16 out[32];
	CHAR16 *ptr = out;
	if (value == 0) {
		conOut->OutputString(conOut, L"0");
		return;
	}

	ptr += 31;
	*--ptr = 0;
	int tmp = value;// >= 0 ? value : -value; 
	
	while (tmp) {
		*--ptr = '0' + tmp % 10;
		tmp /= 10;
	}
	if (value < 0) *--ptr = '-';
	conOut->OutputString(conOut, ptr);
}

void printLabeledInt(SIMPLE_TEXT_OUTPUT_INTERFACE *conOut, CHAR16 *label, int value) {
  conOut->OutputString(conOut, label);
  printInt(conOut, value);
  conOut->OutputString(conOut, L"\r\n");
}

CHAR16 asChar(UINT8 nibble) {
  return nibble + (nibble < 10 ? '0' : '7');
}

void printUUID(SIMPLE_TEXT_OUTPUT_INTERFACE *conOut, UINT8 uuid[16]) {
  int charPos = 0;
  int i;
  CHAR16 *uuidStr= L"00000000-0000-0000-0000-000000000000";
  for(i = 3; i >= 0; i--) {
    uuidStr[charPos++] = asChar(uuid[i] >> 4);
    uuidStr[charPos++] = asChar(uuid[i] & 0xF);
  }
  charPos++;
  for(i = 5; i >= 4; i--) {
    uuidStr[charPos++] = asChar(uuid[i] >> 4);
    uuidStr[charPos++] = asChar(uuid[i] & 0xF);
  }
  charPos++;
  for(i = 7; i >= 6; i--) {
    uuidStr[charPos++] = asChar(uuid[i] >> 4);
    uuidStr[charPos++] = asChar(uuid[i] & 0xF);
  }
  charPos++;
  for(i = 8; i <= 9; i++) {
    uuidStr[charPos++] = asChar(uuid[i] >> 4);
    uuidStr[charPos++] = asChar(uuid[i] & 0xF);
  }
    
  for(i = 10; i < 16; i++) {
    if(i == 4 || i == 6 || i == 8 || i == 10) charPos++;
    uuidStr[charPos++] = asChar(uuid[i] >> 4);
    uuidStr[charPos++] = asChar(uuid[i] & 0xF);
  }
conOut->OutputString(conOut, L"\r\n");
  conOut->OutputString(conOut, uuidStr);
}


void printDevicePath(SIMPLE_TEXT_OUTPUT_INTERFACE *conOut, EFI_DEVICE_PATH *devicePath) {
  EFI_DEVICE_PATH *node = devicePath;
  // https://github.com/vathpela/gnu-efi/blob/master/lib/dpath.c for printing device paths.
  //TODO: Create a file image with gpt partion and a haiku image.
  conOut->OutputString(conOut, L"Device type: ");
  conOut->OutputString(conOut, L"  ");

  for (; !IsDevicePathEnd(node); node = NextDevicePathNode(node)) {
    conOut->OutputString(conOut, L" \\ ");
    printInt(conOut, node->Type);
    conOut->OutputString(conOut, L".");
    printInt(conOut, node->SubType);
    
    if( node->Type == MEDIA_DEVICE_PATH && node->SubType == MEDIA_HARDDRIVE_DP ) {
      conOut->OutputString(conOut, L"\r\n");

      HARDDRIVE_DEVICE_PATH *hdPath = (HARDDRIVE_DEVICE_PATH *) node;
      printLabeledInt(conOut, L"  [HARDDRIVE_DEVICE_PATH] Partition nr: ",  hdPath->PartitionNumber);
      printLabeledInt(conOut, L"  [HARDDRIVE_DEVICE_PATH] Partition start: ",  hdPath->PartitionStart);
      printLabeledInt(conOut, L"  [HARDDRIVE_DEVICE_PATH] Partition size: ",  hdPath->PartitionSize);
      printLabeledInt(conOut, L"  [HARDDRIVE_DEVICE_PATH] MBR type: ",  hdPath->MBRType);
      printLabeledInt(conOut, L"  [HARDDRIVE_DEVICE_PATH] Signature type: ",  hdPath->SignatureType);
      
      printUUID(conOut, hdPath->Signature);
      conOut->OutputString(conOut, L"\r\n\r\n");
    }
  }
  conOut->OutputString(conOut, L"\r\n");
}

/**
 * efi_main - The entry point for the EFI application
 * @image: firmware-allocated handle that identifies the image
 * @SystemTable: EFI system table
 */
EFI_STATUS
efi_main(EFI_HANDLE image, EFI_SYSTEM_TABLE *systemTable)
{
	EFI_BOOT_SERVICES *bs = systemTable->BootServices;

	SIMPLE_TEXT_OUTPUT_INTERFACE *conOut = systemTable->ConOut;
	EFI_EVENT event = systemTable->ConIn->WaitForKey;
	UINTN index;
  EFI_HANDLE handles[100];
  EFI_DEVICE_PATH *devicePath;
  EFI_BLOCK_IO *blockIOProtocol;
  UINTN bufferSize = 100 * sizeof(EFI_HANDLE);
  int i, noOfHandles;

  EFI_STATUS status = bs->LocateHandle( 
    ByProtocol, 
    &BlockIoProtocolGUID, 
    NULL, /* Ignored for AllHandles or ByProtocol */
    &bufferSize, handles);

  noOfHandles = bufferSize == 0 ? 0 : bufferSize / sizeof(EFI_HANDLE);
  printLabeledInt(conOut, L"No of handles reported: ", noOfHandles);
  if (EFI_ERROR(status)) {
    conOut->OutputString(conOut, L"Failed to LocateHandles!\r\n");
    return status;
  }
  for (i = 0; i < noOfHandles; i++) {
    status = bs->HandleProtocol(handles[i], &DevicePathGUID, (void *) &devicePath);
    if (EFI_ERROR(status) || devicePath == NULL) {
      conOut->OutputString(conOut, L"Skipped handle, device path error!\r\n");
      continue;
    }
    status = bs->HandleProtocol(handles[i], &BlockIoProtocolGUID, (void *) &blockIOProtocol);
    if (EFI_ERROR(status) || blockIOProtocol == NULL) {
      conOut->OutputString(conOut, L"Skipped handle, block io protocol error!\r\n");
      continue;
    }
    printDevicePath(conOut, devicePath);
    printLabeledInt(conOut, L"Media ID: ", blockIOProtocol->Media->MediaId );
    
/*  
    printLabeledInt(conOut, L"Device type: ",  DevicePathType(devicePath));
    switch (DevicePathType(devicePath)) {
      case MEDIA_HARDDRIVE_DP: {
        HARDDRIVE_DEVICE_PATH *hdPath = (HARDDRIVE_DEVICE_PATH *) devicePath;
        printLabeledInt(conOut, L"  [HARDDRIVE_DEVICE_PATH] Partition nr: ",  hdPath->PartitionNumber);
        printLabeledInt(conOut, L"  [HARDDRIVE_DEVICE_PATH] Partition start: ",  hdPath->PartitionStart);
        printLabeledInt(conOut, L"  [HARDDRIVE_DEVICE_PATH] Partition size: ",  hdPath->PartitionSize);
        printLabeledInt(conOut, L"  [HARDDRIVE_DEVICE_PATH] MBR type: ",  hdPath->MBRType);
        printLabeledInt(conOut, L"  [HARDDRIVE_DEVICE_PATH] Signature type: ",  hdPath->SignatureType);
      } break;
      case MEDIA_CDROM_DP: {
        conOut->OutputString(conOut, L"  [MEDIA_CDROM_DP]: \r\n");
        
      } break;
      default: {
        if( IsDevicePathUnpacked(devicePath))
          conOut->OutputString(conOut, L"  unpacked device path, whatever that means.\r\n");
        printLabeledInt(conOut, L"Raw type: ",  devicePath->Type);
      }
    }
*/
    if ((i%2)==0 && i != 0) {
      bs->WaitForEvent(1, &event, &index);
    }
  }
	bs->WaitForEvent(1, &event, &index);

	return EFI_SUCCESS;
}
