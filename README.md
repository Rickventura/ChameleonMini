Chameleon-Mini rev G ISO15693 STATE MACHINE VERSION II(NO USE OF POINTERS TO FUNCTIONS)
===========================================
This is a repository forked from ChameleonMini rev G, an attempt made to build a core ISO15693 state machine to be exploited by any ISO 15693 compliant tag applications.
It is assumed that a function like TagGetUid( a function called by the state machine), in the tag source file, wraps a specific tag function like  TITagitstandardGetUid.

By so doing, when the state machine calls (*TagGetUid)(Uid) it will
call TITagitstandardGetUid(Uid) instead. As any tags will assign its own function, like TagGetUid = MyTag_GetUid, the state machine always
gets the correct Uid. Same is true for any function which may depend on a specific tag.


TI Tag-it HF-I STANDARD TRANSPONDER EMULATOR: A WORKING EXAMPLE
============================================
    Here explained are the steps to follow to make the C sources for an emulator based on the presented core
    ISO15693 state machine.    

STEP 1: ISO15693_state_machine.h shared functions    
=================================================
    Since the state machine is included in all tag sources,shared functions must be declared as static in the state machine.
    
    static void TagGetUid(ConfigurationUidType Uid) ;// mandatory
    static void TagSetUid(ConfigurationUidType Uid) ;// mandatory
    static uint16_t Tag_readsingle (uint8_t *FrameBuf, struct ISO15693_parameters *request);// custom function
  
STEP 2: Add the tag's custom command in the switch statement
  =================================================
    case ISO15693_CMD_READ_SINGLE:        
       ResponseByteCount = Tag_readsingle(FrameBuf, &request);         
      break;

STEP 3: Include the necessary files in TITagitstandard.c:
===============================================================
    #include "../Codec/ISO15693.h"
    #include "../Memory.h"
    #include "Crypto1.h"
    #include "../Random.h"
    #include "TITagitstandard.h"

/* Any tag shall include the general ISO15693 state machine */
    
    #include "ISO15693_sm_definitions.h"
    #include "ISO15693_state_machine.h"

STEP 4: Define the custom tag Approcess to return the function IS015693AppProcess as follows
=================================================
    uint16_t TITagitstandardAppProcess  (uint8_t* FrameBuf, uint16_t FrameBytes){
        return(IS015693AppProcess(FrameBuf,FrameBytes))
    }
    
STEP 5: Define the mandatory shared functions to encapsulate the TITagitstandardSetUid,TITagitstandardGetUid
=================================================
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
    void TagGetUid(ConfigurationUidType Uid)
    {
        TITagitstandardGetUid(Uid);	
    }

    void TagSetUid(ConfigurationUidType Uid)
    {
        // Reverse UID before writing it
	    TITagitstandardSetUid(Uid);
    }

STEP 6: Define the custom functions
=================================================

    uint16_t Tag_readsingle( uint8_t *FrameBuf, struct ISO15693_parameters *request)
    {
     
     
     
    }
 
 
 
