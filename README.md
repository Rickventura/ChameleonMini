Chameleon-Mini rev G ISO15693 STATE MACHINE
===========================================
This is a repository forked from ChameleonMini rev G, an attempt made to build a core ISO15693 state machine to be exploited by any ISO 15693 compliant tag applications.

A WORKING EXAMPLE:
TI Tag-it HF-I STANDARD TRANSPONDER EMULATOR
============================================
The file TITagitstandard.c in this application directory is a working example of how a source file should be written in order to exploit the core ISO15693 state machine. 

STEP 1: include the necessary files as shown below:
===============================================================
    #include "../Codec/ISO15693.h"
    #include "../Memory.h"
    #include "Crypto1.h"
    #include "../Random.h"
    #include "TITagitstandard.h"

/* Any tag shall include the general ISO15693 state machine */
    
    #include "ISO15693_sm_definitions.h"
    #include "ISO15693_state_machine.h"


STEP 2: functions to be associated with some function call into the state machine must be declared
===============================================================
    Assuming TITagitstandardGetUid is tag specific and (*TagGetUid) is the function call in the state machine
    void TITagitstandardGetUid(ConfigurationUidType Uid);
    void (*TagGetUid)(ConfigurationUidType Uid) = TITagitstandardGetUid; 
        
    void TITagitstandardSetUid(ConfigurationUidType Uid);    
    void (*TagSetUid)(ConfigurationUidType Uid) = TITagitstandardSetUid; 
    
    uint16_t Tagit_readsingle(uint8_t *FrameBuf, struct ISO15693_parameters *request);   
    uint16_t (*readsingle) (uint8_t *FrameBuf, struct ISO15693_parameters *request) = Tagit_readsingle;  

STEP 3: Define the tag Appprocess function that returns the function IS015693AppProcess aas follows
=================================================
    uint16_t TITagitstandardAppProcess  (uint8_t* FrameBuf, uint16_t FrameBytes){
        return(IS015693AppProcess(FrameBuf,FrameBytes))
    }
    
STEP 4:Define all the functions declared in STEP2
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
    
 STEP 5: Back to ISO15693_state_machine.h 
    extern declaration
 =================================================
    extern void (*TagGetUid)(ConfigurationUidType Uid) ;
    extern void (*TagSetUid)(ConfigurationUidType Uid) ;
    extern uint16_t (*readsingle) (uint8_t *FrameBuf, struct ISO15693_parameters *request)
 
 
 
 
