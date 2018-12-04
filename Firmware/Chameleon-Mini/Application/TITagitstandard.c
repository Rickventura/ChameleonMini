/*
 * TITagitstandard.c
 *
 *  Created by rickventura on: Nov-16-2018 
 *  Modified by ceres-c to finish things up    
 *
 * This File defines only those special functions necessary to emulate a TI™ HF-I TI STANDARD.
 * While a previous file with the same name was inclusive of the iso 15693 state machine this was rebuild and parted from 
 * the ISO 15693 state machine to achieve tag independece. A file named ISO15693_state_machine.h contains a new ISO15693 state machine.
 * 
 * Dereferenced functions are used in ISO15693_state_machine.h to support tag independece.
 * Reuse of this code is supposed to speedup and generalize code making for different 15693 tags.
 *      
 */

#include "../Codec/ISO15693.h"
#include "../Memory.h"
#include "Crypto1.h"
#include "../Random.h"
#include "TITagitstandard.h"
/* Any tag shall include the general ISO15693 state machine */
#include "ISO15693_sm_definitions.h"
#include "ISO15693_state_machine.h"


/* Tag's specific functions necessary to the ISO15693 state machine shall be declared
and assigned to a dereferenced pointer to function used by the state machine*/
static void TagSetUid(ConfigurationUidType Uid) ;
uint16_t Tag_readsingle( uint8_t *FrameBuf, struct ISO15693_parameters *request);


uint16_t Tag_SWITCH_COMMANDS(uint8_t *FrameBuf, struct ISO15693_parameters *request)
{
	uint16_t ResponseByteCount = 0;
	//#define ISO15693_CMD_GET_BLOCK_SEC      0x2C		
	//#define ISO15693_CMD_WRITE_MULTIPLE     0x24
	//#define ISO15693_CMD_SELECT             0x25
	//#define ISO15693_CMD_RESET_TO_READY     0x26
	//#define ISO15693_CMD_WRITE_AFI          0x27
	//#define ISO15693_CMD_LOCK_AFI           0x28
	//#define ISO15693_CMD_WRITE_DSFID        0x29
	//#define ISO15693_CMD_LOCK_DSFID         0x2A

	switch (request->cmd){
		case ISO15693_CMD_READ_SINGLE:
			ResponseByteCount = Tag_readsingle(FrameBuf, request);
			break;

		default:
			ResponseByteCount = IS015693_CMDNotSuported(FrameBuf);
			break;

	}
	return  ResponseByteCount;
}

void TITagitstandardAppInit(void)
{
    State = STATE_READY;
	// initialize TagDef Structure with tag's #defines

	TagDef.UID_SIZE			= TAG_STD_UID_SIZE;
	TagDef.MEM_SIZE			= TAG_STD_MEM_SIZE; 
	TagDef.BYTES_PER_PAGE	= TAG_BYTES_PER_PAGE;
	TagDef.NUMBER_OF_SECTORS= TAG_NUMBER_OF_SECTORS;   
	TagDef.MEM_UID_ADDRESS	= TAG_MEM_UID_ADDRESS; 

	// initialize Dereferenced pointers to functions   
}

void TITagitstandardAppReset(void)
{
    State = STATE_READY;
	
    
}


void TITagitstandardAppTask(void)
{

}
 
void TITagitstandardAppTick(void)
{

}

uint16_t TITagitstandardAppProcess  (uint8_t* FrameBuf, uint16_t FrameBytes){
	    
	
	return(IS015693AppProcess(FrameBuf,FrameBytes));

}

uint16_t Tag_readsingle( uint8_t *FrameBuf, struct ISO15693_parameters *request)
{
  

  uint16_t ResponseByteCount = 0;
  uint16_t PageAddress , MemLocation; 
  uint8_t *FramePtr;
  int errflag = 0;

  /* ISO 15693 READ SINGLE request: 
      addressed    [FLAGS(1BYTE)][CMD(1BYTE)][UID(8BYTES)][BLOCK #]
                                                             ^  
                                                         Frame_params

       NOT_ADDRESSED  [FLAGS(1BYTE)][CMD(1BYTE)][BLOCK #]
                                                 ^  
                                             Frame_params
   */

  // request->Frame_params is assumed to be correctly assigned by the extract_param function to point to the block # parameter(PageAddress) 
	PageAddress = *(request->Frame_params); 
   

	if ( PageAddress >= TAG_NUMBER_OF_SECTORS) { /* the reader is requesting a sector out of bound */
		errflag = 1;
		FrameBuf[ISO15693_ADDR_FLAGS]     = ISO15693_RES_FLAG_ERROR;
		FrameBuf[ISO15693_RES_ADDR_PARAM] = ISO15693_RES_ERR_BLK_NOT_AVL; /* real TiTag standard reply with this error */
		ResponseByteCount = 2;        
	}

	else if (request->option_flg) { /* request with option flag set */

		FrameBuf[ISO15693_ADDR_FLAGS] = ISO15693_RES_FLAG_NO_ERROR;
		/*Tagit standard UID is stored in blocks 8 and 9 which are blocked */
		FrameBuf[1] = ( PageAddress == 8 || PageAddress == 9) ? 0x02 : 0x00; /* block security status: when option flag set */
		FramePtr = FrameBuf + 2;
		ResponseByteCount = 6;
	} 
 
	else { /* request with option flag not set*/
		FrameBuf[ISO15693_ADDR_FLAGS] = ISO15693_RES_FLAG_NO_ERROR; /* Flags */
		FramePtr = FrameBuf + 1;
		ResponseByteCount = 5;
	}

 	if (!errflag) {
		MemLocation = PageAddress * request->Bytes_per_Page;   
		MemoryReadBlock(FramePtr, MemLocation , request->Bytes_per_Page); 
	 }

	 return ResponseByteCount;

}

void TITagitstandardGetUid(ConfigurationUidType Uid)
{
    MemoryReadBlock(&Uid[0], TAG_MEM_UID_ADDRESS, ActiveConfiguration.UidSize);
    // Reverse UID after reading it
    TITagitstandardFlipUid(Uid);
}

void TITagitstandardSetUid(ConfigurationUidType Uid)
{
    // Reverse UID before writing it
    TITagitstandardFlipUid(Uid);
    
    MemoryWriteBlock(Uid, TAG_MEM_UID_ADDRESS, ActiveConfiguration.UidSize);
}

void TagGetUid(ConfigurationUidType Uid)
{
	// wraps TITagitstandardGetUid;
	TITagitstandardGetUid(Uid);
}

void TagSetUid(ConfigurationUidType Uid)
{
	// wraps TITagitstandardSetUid;
	TITagitstandardSetUid(Uid);
}


void TITagitstandardFlipUid(ConfigurationUidType Uid)
{

  uint8_t tmp , *tail ;
  tail = Uid + ActiveConfiguration.UidSize - 1; 
  while ( Uid < tail ){
    tmp = *Uid;
    *Uid++ = *tail ;
    *tail-- = tmp;	     	
  }

}

