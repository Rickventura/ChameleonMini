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


STEP 2: functions called in the state machine(dereferenced functions) must be declared as extern to the tag specific file
===============================================================
    Assuming TITagitstandardGetUid is tag specific and (*TagGetUid)() is the function called in the state machine

    void TITagitstandardGetUid(ConfigurationUidType Uid); this is usual function declaration
    extern void (*TagGetUid)(ConfigurationUidType Uid);   this is the dereferenced function declared in the state machine
        
    void TITagitstandardSetUid(ConfigurationUidType Uid);    
    extern void (*TagSetUid)(ConfigurationUidType Uid); 
    
    uint16_t TITagitstandard_readsingle(uint8_t *FrameBuf, struct ISO15693_parameters *request);   
    extern uint16_t (*readsingle) (uint8_t *FrameBuf, struct ISO15693_parameters *request);  

STEP 3: Initialize the dereferenced functions 
=================================================
    One way to initialize the dereferenced fuctions is to assign them int the init function
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
    
 STEP 5: In ISO15693_state_machine.h declare the dereferenced functions    
 =================================================
    Functions which calls are in the state machine must be decleared in order to be used by TITagitstandard.c
    void (*TagGetUid)(ConfigurationUidType Uid) ;
    void (*TagSetUid)(ConfigurationUidType Uid) ;
    uint16_t (*readsingle) (uint8_t *FrameBuf, struct ISO15693_parameters *request)
 
 
 
 
