Chameleon-Mini rev G ISO15693 STATE MACHINE
===========================================
This is a repository forked from ChameleonMini rev G, an attempt made to build a core ISO15693 state machine to be exploited by any ISO 15693 compliant tag applications.

TI Tag-it HF-I STANDARD TRANSPONDER EMULATOR: A WORKING EXAMPLE
============================================
    Here explained are the steps to follow to make the C sources for an emulator based on the presented core
    ISO15693 state machine.    

STEP 1: Include the necessary files in TITagitstanadrd.c:
===============================================================
    #include "../Codec/ISO15693.h"
    #include "../Memory.h"
    #include "Crypto1.h"
    #include "../Random.h"
    #include "TITagitstandard.h"

/* Any tag shall include the general ISO15693 state machine */
    
    #include "ISO15693_sm_definitions.h"
    #include "ISO15693_state_machine.h"


STEP 2: functions called in the state machine(dereferenced functions) must be declared as extern in the tag specific file
===============================================================
    Assuming TITagitstandardGetUid is tag specific and (*TagGetUid)() is the function called in the state machine

    void TITagitstandardGetUid(ConfigurationUidType Uid); this is the usual function declaration
    extern void (*TagGetUid)(ConfigurationUidType Uid);   this is the dereferenced function declared in the state machine
        
    void TITagitstandardSetUid(ConfigurationUidType Uid);    
    extern void (*TagSetUid)(ConfigurationUidType Uid); 
    
    uint16_t TITagitstandard_readsingle(uint8_t *FrameBuf, struct ISO15693_parameters *request);   
    extern uint16_t (*readsingle) (uint8_t *FrameBuf, struct ISO15693_parameters *request);  

STEP 3: Initialize the dereferenced functions 
=================================================
    One way to initialize the dereferenced fuctions is to assign them in TITagitstandardAppInit.
    void TITagitstandardAppInit(void)
    {
        State = STATE_READY;
        TagGetUid = TITagitstandardGetUid;
        TagGetUid = TITagitstandardGetUid;	
        readsingle = TITagitstandard_readsingle;	
    }


STEP 4: Define the tag Approcess that returns the function IS015693AppProcess as follows
=================================================
    uint16_t TITagitstandardAppProcess  (uint8_t* FrameBuf, uint16_t FrameBytes){
        return(IS015693AppProcess(FrameBuf,FrameBytes))
    }
    
STEP 5:Define all the functions declared in STEP2
=================================================
    void TITagitstandardGetUid(ConfigurationUidType Uid)
    {
    }

        
    void TITagitstandardSetUid(ConfigurationUidType Uid)
    {
    }

   
    uint16_t Tagit_readsingle(uint8_t *FrameBuf, struct ISO15693_parameters *request);   
    {
    }
    
 STEP 6: In ISO15693_state_machine.h declare the dereferenced functions    
 =================================================
    Functions which calls are in the state machine must be decleared in order to be used by TITagitstandard.c
    void (*TagGetUid)(ConfigurationUidType Uid) ;
    void (*TagSetUid)(ConfigurationUidType Uid) ;
    uint16_t (*readsingle) (uint8_t *FrameBuf, struct ISO15693_parameters *request)
 
 STEP 7: In ISO15693_state_machine.h add the custom command to the command switch:
 =================================================
 
    case ISO15693_CMD_READ_SINGLE:        
	          ResponseByteCount = Tag_readsingle(FrameBuf, &request);         
    break;
 
