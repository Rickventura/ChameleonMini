/*
 * ISO15693.c
 *
 *  Created on: 01-03-2017
 *      Author: Phillip Nash
 * 
 *  TODO:
 *    - ISO15693AddressedLegacy should be replaced with ISO15693Addressed and appropriate check
 *      should be performed (see TITagitstandard.c) - ceres-c
 */


#include "Sl2s2002.h"
#include "../Codec/ISO15693.h"
#include "../Memory.h"
#include "Crypto1.h"
#include "../Random.h"
#include "ISO15693-A.h"

/* Any tag shall include the general ISO15693 state machine */
#include "ISO15693_sm_definitions.h"
#include "ISO15693_state_machine.h"


// tag specific functions
void Sl2s2002SetUid(ConfigurationUidType Uid);
void Sl2s2002GetUid(ConfigurationUidType Uid);
uint16_t Sl2s2002_readmultiple(uint8_t *FrameBuf, struct ISO15693_parameters *request);
uint16_t Sl2s2002_readsingle(uint8_t *FrameBuf, struct ISO15693_parameters *request);
uint16_t Sl2s2002_getSysInfo(uint8_t *FrameBuf, struct ISO15693_parameters *request);
uint16_t Sl2s2002_getmultBlockSec (uint8_t *FrameBuf, struct ISO15693_parameters *request);

// dereferenced functions used in ISO15693_state_machine.h     
extern void (*TagSetUid)(ConfigurationUidType Uid);
extern void (*TagGetUid)(ConfigurationUidType Uid);
extern uint16_t (*readmultiple) (uint8_t *FrameBuf, struct ISO15693_parameters *request); 
extern uint16_t (*readsingle) (uint8_t *FrameBuf, struct ISO15693_parameters *request); 
extern uint16_t (*getsysInfo) (uint8_t *FrameBuf, struct ISO15693_parameters *request); 
extern uint16_t (*getmultblocksec) (uint8_t *FrameBuf, struct ISO15693_parameters *request); 


void Sl2s2002AppInit(void)
{
    State = STATE_READY;

	// initialize dereferenced pointers
    TagGetUid		= Sl2s2002GetUid;
    TagSetUid		= Sl2s2002SetUid;
	readsingle		= Sl2s2002_readsingle;
	readmultiple	= Sl2s2002_readmultiple;
	getsysInfo		= Sl2s2002_getSysInfo;
	getmultblocksec = Sl2s2002_getmultBlockSec;

	// initialize TagDef Structure with tag's #defines

	TagDef.UID_SIZE			= SL2S_UID_SIZE;
    TagDef.MEM_SIZE			= SL2S_MEM_SIZE; 
	TagDef.BYTES_PER_PAGE	= SL2S_BYTES_PER_PAGE;
	TagDef.NUMBER_OF_SECTORS= SL2S_NUMBER_OF_SECTORS;   
	TagDef.MEM_UID_ADDRESS	= SL2S_MEM_UID_ADDRESS;     
}

void Sl2s2002AppReset(void)
{
    State = STATE_READY;
}


void Sl2s2002AppTask(void)
{
    
}

void Sl2s2002AppTick(void)
{

    
}

uint16_t Sl2s2002AppProcess(uint8_t* FrameBuf, uint16_t FrameBytes)
{
 return(IS015693AppProcess(FrameBuf,FrameBytes));
}

uint16_t Sl2s2002_readsingle(uint8_t *FrameBuf, struct ISO15693_parameters *request)
{
	/* ISO 15693 READ SINGLE request: 
	      addressed    [FLAGS(1BYTE)][CMD(1BYTE)][UID(8BYTES)][BLOCK #]
	                                                             ^  
	                                                         Frame_params

	       NOT_ADDRESSED  [FLAGS(1BYTE)][CMD(1BYTE)][BLOCK #]
	                                                 ^  
	                                             Frame_params
	*/
	uint16_t ResponseByteCount = 0;	
	uint16_t PageAddress , MemLocation; 
  	uint8_t *FramePtr;


    
	FrameBuf[0] = ISO15693_RES_FLAG_NO_ERROR; /* Flags */
	if (request->option_flg){

		FrameBuf[1] = 0x00; /* security dummy to 0 */
		FramePtr = FrameBuf + 2;
		ResponseByteCount = 6;

	} else {

		FramePtr = FrameBuf + 1;
		ResponseByteCount = 5;
	}

	PageAddress = *(request->Frame_params); // request->Frame_params always points to the first parameter's
	MemLocation = PageAddress * request->Bytes_per_Page;   
	MemoryReadBlock(FramePtr, MemLocation , request->Bytes_per_Page); 
	return ResponseByteCount;
}

uint16_t Sl2s2002_getSysInfo(uint8_t *FrameBuf, struct ISO15693_parameters *request)
{

	uint16_t ResponseByteCount = 0;
	
	FrameBuf[0] = ISO15693_RES_FLAG_NO_ERROR; /* Flags */
	FrameBuf[1] = 0x0F; /* InfoFlags */
	ISO15693CopyUid(&FrameBuf[2], request->tagUid);		
	FrameBuf[10] = 0x00;
	FrameBuf[11] = 0xC2;
	FrameBuf[12] = 0x03;
	FrameBuf[13] = 0x03;
	FrameBuf[14] = 0x01;
	ResponseByteCount = 15;

	
	return ResponseByteCount ;
}

uint16_t Sl2s2002_readmultiple(uint8_t *FrameBuf, struct ISO15693_parameters *request)
{
	uint16_t PageAddress , blocks;
	uint8_t  count;
	uint8_t *FrameBufPtr; 
	uint16_t ResponseByteCount = 0;
	
	PageAddress	= *(request->Frame_params + 0); // request->parameters always points to the first parameter wheter or not is addressed
	blocks		= *(request->Frame_params + 1); // 2nd request parameter is the actual number of blocks to read		
	FrameBuf[0] = ISO15693_RES_FLAG_NO_ERROR;
	ResponseByteCount = 1;
	FrameBufPtr = &FrameBuf[1]; 

	for (count = 0; count < blocks ; count++){
	
	    /* [block security status(optional)] [data(page size)] */
		/* repeat the above for the requested number see 15693-3 2009_A2_2015.pdf page 27 */

		if (request->option_flg) {  
			*FrameBufPtr++ = 0x00;// if option flag set add the block security status
			ResponseByteCount += 1;
		}

		MemoryReadBlock(FrameBufPtr , PageAddress , request->Bytes_per_Page);
		PageAddress += request->Bytes_per_Page; // next page to read from
		FrameBufPtr += request->Bytes_per_Page; // next frameBuff position to write to.
		ResponseByteCount += request->Bytes_per_Page;
	}

	return ResponseByteCount;
}

uint16_t Sl2s2002_getmultBlockSec(uint8_t *FrameBuf, struct ISO15693_parameters *request)
{
	uint16_t ResponseByteCount = 0;
	uint8_t PageAddressCount = FrameBuf[11] + 1;

	if (request.isaddressed) {
		FrameBuf[0] = 0; /* Flags */
		for (uint8_t i = 0; i < PageAddressCount; i++) 
		FrameBuf[i] = 0x00;
	}

	ResponseByteCount = 1 + PageAddressCount;
    return ResponseByteCount;
}

void Sl2s2002GetUid(ConfigurationUidType Uid)
{
    MemoryReadBlock(&Uid[0], SL2S_MEM_UID_ADDRESS, ActiveConfiguration.UidSize);
}

void Sl2s2002SetUid(ConfigurationUidType Uid)
{
    MemoryWriteBlock(Uid, SL2S_MEM_UID_ADDRESS, ActiveConfiguration.UidSize);
}

