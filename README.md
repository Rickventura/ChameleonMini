Chameleon-Mini rev G ISO15693 STATE MACHINE VERSION II(NO USE OF POINTERS TO FUNCTIONS)
===========================================
This is a repository forked from ChameleonMini rev G, an attempt made to build a core ISO15693 state machine to be exploited by 
any ISO 15693 compliant tag applications.
For instance it is assumed that the state machine calls a wrapper function "TagGetUid2, defined in the tag source file, which wraps a specific tag function like  TITagitstandardGetUid as follows.

	void TagGetUid(ConfigurationUidType Uid)
	{
    		TITagitstandardGetUid(Uid);	
	}	

When the state machine calls the wrapper TagGetUid(Uid) it indirectly calls TITagitstandardGetUid(Uid). 
As other tags will wrap up their own TagGetUid(Uid) function, the state machine always gets the correct Uid. 
Same is true for any other function needed by the state machine.
Although wrapping a function into another, being like 2 calls instead of one, isn't as 
efficient as the use of pointers to functions, it is assumed to be more palatable to those who have little or no experience with 
pointers. Overall the method is inferior to using pointers to functions for more that a reason, though, in this limited 
context,to all extent, it is practically equivalent. 
	
TI Tag-it HF-I STANDARD TRANSPONDER EMULATOR: A WORKING EXAMPLE
============================================
    Here explained are the steps to follow to make the C sources for an emulator based on the presented core
    ISO15693 state machine.    

STEP 1: ISO15693_state_machine.h wrapper functions    
=================================================
Since the state machine is included as a header in all tag sources, functions wrapper must be declared as static in the state machine or in tag files.
All wrapper functions, commonly used by any tag, and necessary to operate the state machine, are already declared in ISO15693_state_machine.h.
For instance the state machine calls a wrapper function  "TagGetUid", in the TI Tag-it case TITagitstandardGetUid flips the UID head to tail which may not be the case with other tags. In ISO15693_state_machine.h. we'll have a declaration like 

static void TagGetUid(ConfigurationUidType Uid) ;

While in the tag file TITagitstandard.c, the above function is defined as a wrapper which calls the real tag function:

void TagGetUid(ConfigurationUidType Uid) {
  TITagitstandardGetUid(Uid) ;
}
  
STEP 2: in TITagitstandard.c include the necessary files:
===============================================================
    #include "../Codec/ISO15693.h"
    #include "../Memory.h"
    #include "Crypto1.h"
    #include "../Random.h"
    #include "TITagitstandard.h"

/* Any tag shall include the general ISO15693 state machine */
    
    #include "ISO15693_sm_definitions.h"
    #include "ISO15693_state_machine.h"

STEP 3: in TITagitstandard.c define the custom tag Approcess to return the function IS015693AppProcess
=================================================

    uint16_t TITagitstandardAppProcess  (uint8_t* FrameBuf, uint16_t FrameBytes){
        return(IS015693AppProcess(FrameBuf,FrameBytes))
    }
    
STEP 4: In TITagitstandard.c define external mandatory functions TITagitstandardSetUid,TITagitstandardGetUid
=================================================
The following two functions must be declared external as they are used in file ../Firmware/Chameleon-Mini/Configuration.c

	void TITagitstandardGetUid(ConfigurationUidType Uid);
	void TITagitstandardSetUid(ConfigurationUidType Uid);

	void TITagitstandardGetUid(ConfigurationUidType Uid)
    	{	
        	MemoryReadBlock(&Uid[0], TAG_MEM_UID_ADDRESS, ActiveConfiguration.UidSize);        
        	TITagitstandardFlipUid(Uid);
    	}	

    	void TITagitstandardSetUid(ConfigurationUidType Uid)
    	{
        	TITagitstandardFlipUid(Uid);    
        	MemoryWriteBlock(Uid, TAG_MEM_UID_ADDRESS, ActiveConfiguration.UidSize);
	}
    
STEP 5: in TITagitstandard.c define the wrap mandatory functions used by the State machine
=================================================

    void TagGetUid(ConfigurationUidType Uid)
    {
        TITagitstandardGetUid(Uid);	
    }

STEP 6: in TITagitstandard.c declare and define a tag specific switch as follows
=================================================
Mandatory for all tags is a tag specific switch.
A tag specific switch, called from the state machine, is assumed to direct specific command calls.
In the simple TI Tag-it STANDARD case, only a specific command function is included: Tag_readsingle.
The default response is the same for all tags.

static uint16_t Tag_readsingle( uint8_t *FrameBuf, struct ISO15693_parameters *request);

uint16_t Tag_SWITCH_COMMANDS(uint8_t *FrameBuf, struct ISO15693_parameters *request)
{

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


STEP 7: in TITagitstandard.c define the tag specific command functions as follows
=================================================

    uint16_t Tag_readsingle( uint8_t *FrameBuf, struct ISO15693_parameters *request)
    {
     
    	function's detail 
     
    }
 
 
 
