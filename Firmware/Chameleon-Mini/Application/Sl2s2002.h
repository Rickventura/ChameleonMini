/*
 * Sl2s2002.h
 *
 *  Created on: Nov-20-2018
 *      Author: @Rickventura
 */

#ifndef SL2S2002_H_
#define SL2S2002_H_

#include "Application.h"
#include "ISO15693-A.h"
/* any tag shall define the following constants */
#define ISO15693_GENERIC_UID_SIZE    8 //ISO15693_UID_SIZE
#define ISO15693_GENERIC_MEM_SIZE    8192 //ISO15693_MAX_MEM_SIZE

/*
NXP Semiconductors SL2S2002_SL2S2102 ICODE SLIX

The 1024 bit EEPROM memory is divided into 32 blocks. A block is the smallest access
unit. Each block consists of 4 bytes (1 block = 32 bits). Bit 0 in each byte represents the
least significant bit (LSB) and bit 7 the most significant bit (MSB), respectively.
The memory is divided into 2 parts:
•
Configuration area
Within this part of the memory all required information is stored, such as UID, write
protection, access control information, passwords, AFI and EAS. This memory area
cannot be directly accessed.
•
User memory
Within the 896 bit memory area the user data are stored. Direct read/write access to
this part of the memory is possible depending on the related write protection condition
*/

#define SL2S_UID_SIZE        ISO15693_GENERIC_UID_SIZE  // UID CODING: E0 04 01 xx xx xx xx xx 
#define SL2S_MEM_SIZE        128  // 1024 bits tag Sl2s2002 ICODE SLIX
#define SL2S_BYTES_PER_PAGE      4
#define SL2S_NUMBER_OF_SECTORS   ( SL2S_MEM_SIZE / SL2S_BYTES_PER_PAGE )
#define SL2S_MEM_UID_ADDRESS     0x00

void Sl2s2002AppInit(void);
void Sl2s2002AppReset(void);
void Sl2s2002AppTask(void);
void Sl2s2002AppTick(void);
uint16_t Sl2s2002AppProcess(uint8_t* FrameBuf, uint16_t FrameBytes);
void Sl2s2002GetUid(ConfigurationUidType Uid);
void Sl2s2002SetUid(ConfigurationUidType Uid);



#endif /* VICINITY_H_ */
