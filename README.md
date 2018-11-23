Chameleon-Mini rev G ISO15693 STATE MACHINE
===========================================
This is a repository forked from ChameleonMini rev G, and attempt made to build a core ISO15693 state machine to be exploited by any ISO 15693 compliant tag applications. The Application directory consists of source files written in C language, the files ISO15693_state_machine.h and ISO15963_sm_definitions.h are assumed to be included as header into a tag specific file in order to be exploited.

A WORKING EXAMPLE: TI Tag-it HF-I STANDARD TRANSPONDER EMULATOR
===============================================================
The file TITagitstandard.c in this application directory is a working example of how a source file should be written in order to exploit the core ISO15693 state machine. 

STEP 1: include the necessary files as shown below:

include "../Codec/ISO15693.h"
#include "../Memory.h"
#include "Crypto1.h"
#include "../Random.h"
#include "TITagitstandard.h"
/* Any tag shall include the general ISO15693 state machine */
#include "ISO15693_sm_definitions.h"
#include "ISO15693_state_machine.h"

STEP 2: declare the functions that must be associated with some function call into the state machine

void TITagitstandardGetUid(ConfigurationUidType Uid); //this function gets the correct UID from a Tag-it standard tag
//Assuming (*TagGetUid) is the function call in the state machine 
void (*TagGetUid)(ConfigurationUidType Uid) = TITagitstandardGetUid; // dereferenced pointer TagGetUid used in       

ISO15693_state_machine.h void TITagitstandardSetUid(ConfigurationUidType Uid);
//Assuming (*TagSetUid) is the function call in the state machine 
void (*TagSetUid)(ConfigurationUidType Uid) = TITagitstandardSetUid; // association of TagSetUid with TITagitstandardSetUid 

// Assuming (*readsingle) is the function call in the state machine
uint16_t Tagit_readsingle(uint8_t *FrameBuf, struct ISO15693_parameters *request);   
uint16_t (*readsingle) (uint8_t *FrameBuf, struct ISO15693_parameters *request) = Tagit_readsingle;  

STEP 3:
//Assuming TITagitstandardAppProcess is the tag application process function, write the call to the state machine application process as follow, where return(IS015693AppProcess(FrameBuf,FrameBytes)); will be the same for all tags.
uint16_t TITagitstandardAppProcess  (uint8_t* FrameBuf, uint16_t FrameBytes){
    return(IS015693AppProcess(FrameBuf,FrameBytes));
}

Repository Structure
--------------------
The code repository contains
* Doc: A doxygen documentation 
* Drivers: Chameleon drivers for Windows and Linux
* Dumps: Dumps of different smartcards
* Hardware: The layout and schematics of the PCB
* Firmware: The complete firmware including a modified Atmel DFU bootloader and LUFA
* Software: Contains a python tool for an easy configuration (and more) of the ChameleonMini, Note that this is currently under construction
* RevE: Contains the whole contents of the discontinued RevE repository.
* RevE-light: Contains our development files for the RevE-light - **WARNING:** currently not supported / not functional
