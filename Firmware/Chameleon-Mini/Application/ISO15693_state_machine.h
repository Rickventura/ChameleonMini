/*
 * ISO15693_state_machine.h
 *
 *  Created by rickventura on: Nov-16-2018 
 *        
 *      
 *  A more general 15693 state machine. 
 *  Dereferenced functions (pointer to functions) are used to support tags specific functions.
 *  Reuse of this code is supposed to speedup and generalize code making for different 15693 tags.
 *      
 */


/* Dereferenced Tag specific functions: 
 Function uint16_t IS015693AppProcess(uint8_t* FrameBuf, uint16_t FrameBytes). 
 is supposed to be a general 15693 state machine including responses to all possible commands.
 A TAG C file named something like myTAG.c is necessary as well and ISO15693_state_machine.h shall be included into myTAG.c.
 The decision to include an .h file, instead of compiling a .c file and linking the corresponding object file, was made to 
 leave everything else unchanged, the make file as well.
 See TITagitstandard.c for a practical example of myTAG.c. Any new tag shall be defined in its own  c file, as it was before 
 the recent splitting of the state_machine from the tag. The function myTAGAppProcess(uint8_t* FrameBuf, uint16_t FrameBytes),
 defined in any tag sorce file like myTAG.c, shall define myTAGAppProcess(uint8_t* FrameBuf, uint16_t FrameBytes) which encapsulates a 
 call to IS015693AppProcess(uint8_t* FrameBuf, uint16_t FrameBytes).
 In myTAG.c it should look like
  
 uint16_t myTAGAppProcess(uint8_t* FrameBuf, uint16_t FrameBytes) 
 { 
   IS015693AppProcess( FrameBuf,  FrameBytes);
 }
 
 In the state machine file, defining a command, is about designing and defining a response function for that command.
 Some commands trigger responses which are the same for any TAG. Other commands trigger TAG specific responses.
 We call ISO15693 responses those which are independent of any specific TAG.
 The following commands and their corresponding functions are tag independent (there are certainly more).
 They are already included in this core part of the code, the ISO15693_state_machine.

 STAY_QUIET     response function is ISO15693_stay_quiet(enum status *State , uint8_t *FrameBuf, struct ISO15693_parameters *request) ;
 SELECT         response function is ISO15693_select(enum status *State , uint8_t *FrameBuf, struct ISO15693_parameters *request);
 RESET_TO_READY response function is ISO15693_reset_to_ready(enum status *State , uint8_t *FrameBuf, struct ISO15693_parameters *request);
 WRITE_SINGLE   response function is ISO15693_writesingle(uint8_t *FrameBuf, struct ISO15693_parameters *request);

 Code for other commands, those requiring specific responses (TAG dependent), shall be written in their specific files.
 An example of those specific responses is the READ_SINGLE of a TI TAG-IT STANDARD.
 We define the fuction Tagit_readsingle() in TITagitstandard.c.
 The call to a generic readsingle() function is triggered by a "commands switch" defined in the state machine, as it was before the split.
 The association between the tag specific myTag_readsingle() with the generic readsingle() function is accomplished in TITagitstandard.c. 
 As a function pointer is just a variable, we simply assigning our Tagit_readsingle() to readsingle(). 
  
 uint16_t (*readsingle) (uint8_t *FrameBuf, struct ISO15693_parameters *request) = Tagit_readsingle
  
 To the benefict of those who aren't familiar with pointers to functions, commonly used in c,
 a call like (*readsingle)(FrameBuf,&request), readsingle being the pointer to the function (*readsingle), 
 simply becames a call to Tagit_readsingle(FrameBuf,&request).
 By so doing any assignment in specific tag files like 
 
 (*read_single) = Tag1_readsingle. 
 (*read_single) = Tag2_readsingle. 
 
 will make a call (*read_single)(FrameBuf,&request) trigger its assigned function.
 We have achieved what is called a "dereferenced call".
 
 More technically , in TITagitstandard.c we shall write:

 uint16_t Tagit_readsingle(uint8_t *FrameBuf, struct ISO15693_parameters *request);  // the usual function declaration
 uint16_t (*readsingle) (uint8_t *FrameBuf, struct ISO15693_parameters *request) = Tagit_readsingle; 

 Of course we still must code Tagit_readsingle as we would do anyway.

 In ISO15693_state_machine.h
 we shall add the following line to let the code know that readsingle pointer is defined elsewhere. 

 extern uint16_t (*readsingle) (uint8_t *FrameBuf, struct ISO15693_parameters *request);

 Finally we shall have the command code added to the switch statment.

 switch (Command){

   case ISO15693_CMD_READ_SINGLE:        
       ResponseByteCount = (*readsingle)(FrameBuf, &request);  dereferenced call to Tagit_readsingle.              
       break;         
  
 }      
*/
//Dereferenced Tag specific functions 
extern void (*TagGetUid)(ConfigurationUidType Uid) ;
extern void (*TagSetUid)(ConfigurationUidType Uid) ;
extern uint16_t (*readsingle) (uint8_t *FrameBuf, struct ISO15693_parameters *request);

// shared machine state functions
static struct ISO15693_parameters ISO15693_extract_par (uint8_t *FrameBuf);
static uint8_t ISO15693_status_check ( enum status *State , struct ISO15693_parameters *request , uint16_t *ResponseByteCount )	;
static uint16_t IS015693AppProcess(uint8_t* FrameBuf, uint16_t FrameBytes);
	
// shared ISO15693 commands functions
static uint16_t  ISO15693_inventory ( uint8_t *FrameBuf , struct ISO15693_parameters *request );
static uint16_t ISO15693_stay_quiet(enum status *State , uint8_t *FrameBuf, struct ISO15693_parameters *request) ;
static uint16_t ISO15693_select(enum status *State , uint8_t *FrameBuf, struct ISO15693_parameters *request);
static uint16_t ISO15693_reset_to_ready(enum status *State , uint8_t *FrameBuf, struct ISO15693_parameters *request);
static uint16_t ISO15693_writesingle(uint8_t *FrameBuf, struct ISO15693_parameters *request);

// pointers to functions included in the standard ISO15693 machine
static uint16_t (*stay_quiet)(enum status *State , uint8_t *FrameBuf, struct ISO15693_parameters *request) =  ISO15693_stay_quiet;
static uint16_t (*select)(enum status *State , uint8_t *FrameBuf, struct ISO15693_parameters *request) = ISO15693_select;
static uint16_t (*reset_to_ready) (enum status *State , uint8_t *FrameBuf, struct ISO15693_parameters *request) = ISO15693_reset_to_ready;
static uint16_t (*writesingle)(uint8_t *FrameBuf, struct ISO15693_parameters *request) = ISO15693_writesingle;

uint16_t IS015693AppProcess(uint8_t* FrameBuf, uint16_t FrameBytes)
{   
     
   
    if (FrameBytes >= ISO15693_MIN_FRAME_SIZE) {
        if(ISO15693CheckCRC(FrameBuf, FrameBytes - ISO15693_CRC16_SIZE)) {
         
            // At this point, we have a valid ISO15693 frame            
            uint16_t ResponseByteCount = ISO15693_APP_NO_RESPONSE;            
            uint8_t mayExecute = 0;
	    struct ISO15693_parameters request;            
            
            
            ResponseByteCount = 0;
            request    = ISO15693_extract_par (FrameBuf);	
            mayExecute = ISO15693_status_check( &State , &request , &ResponseByteCount );

            if (mayExecute){ 
	      switch ( request.cmd ) {

	        case ISO15693_CMD_STAY_QUIET:         
                  ResponseByteCount = ISO15693_stay_quiet(&State , FrameBuf, &request);
                  break;

                case ISO15693_CMD_SELECT:             
                  ResponseByteCount = ISO15693_select (&State , FrameBuf, &request);
                  break;

	        case ISO15693_CMD_RESET_TO_READY:     
                  ResponseByteCount = ISO15693_reset_to_ready(&State , FrameBuf, &request); 
                  break;

                case ISO15693_CMD_INVENTORY:  
		  ResponseByteCount = ISO15693_inventory(FrameBuf , &request);         
                  break;       

   	       case ISO15693_CMD_WRITE_SINGLE:       
	          ResponseByteCount = ISO15693_writesingle(FrameBuf, &request);         
	          break;

	      case ISO15693_CMD_READ_SINGLE:        
	          ResponseByteCount = (*readsingle)(FrameBuf, &request);         
                  break;
              default:
                ResponseByteCount = 0;
                break;
            }// end switch

           } 

           if (ResponseByteCount > 0) {
                /* There is data to be sent. Append CRC */
                ISO15693AppendCRC(FrameBuf, ResponseByteCount);
                ResponseByteCount += ISO15693_CRC16_SIZE;
            }

            return ResponseByteCount;

        } else { // Invalid CRC
            return ISO15693_APP_NO_RESPONSE;
        }
    } else { // Min frame size not met
        return ISO15693_APP_NO_RESPONSE;
    }

}      


struct ISO15693_parameters ISO15693_extract_par (uint8_t *FrameBuf)
{
  /* ISO 15693 general request: 
    [FLAGS(1BYTE)][CMD(1BYTE)][OPTIONAL UID(8BYTES)][DATA SIZE AND MEANING DEPENDS ON CMD]
                               ^ Frame_Uid          ^ Frame_params
  */
  struct ISO15693_parameters request;
  
  
  request.Flags = FrameBuf[ISO15693_ADDR_FLAGS];     // FrameBuf[0x00]
  request.cmd   = FrameBuf[ISO15693_ADDR_FLAGS + 1]; // FrameBuf[0x01] 
  request.Frame_Uid =(uint8_t *) NULL;
  request.Frame_params = (uint8_t *)NULL;
  request.Bytes_per_Page = TAG_BYTES_PER_PAGE;

 /* set flags according to 15693-3 2009_A2_2015.pdf manual Tables 3,4,5 page 9 */
  request.subcarrier_flg= FrameBuf[ISO15693_ADDR_FLAGS] & ISO15693_REQ_FLAG_SUBCARRIER ? 1 : 0;
  request.data_rate_flg = FrameBuf[ISO15693_ADDR_FLAGS] & ISO15693_REQ_FLAG_DATARATE   ? 1 : 0;
  request.inventory_flg = FrameBuf[ISO15693_ADDR_FLAGS] & ISO15693_REQ_FLAG_INVENTORY  ? 1 : 0;
  request.prot_ext_flg  = FrameBuf[ISO15693_ADDR_FLAGS] & ISO15693_REQ_FLAG_PROT_EXT   ? 1 : 0;
  request.address_flg   = 0;
  request.select_flg    = 0;
  request.option_flg    = FrameBuf[ISO15693_ADDR_FLAGS] & ISO15693_REQ_FLAG_OPTION     ? 1 : 0;
  request.RFU_flg       = FrameBuf[ISO15693_ADDR_FLAGS] & ISO15693_REQ_FLAG_RFU        ? 1 : 0;
  request.AFI_flg      = 0;
  request.Nb_slot_flg   = 0;
 
   if (request.inventory_flg ) { // when inventory flag is set flags set according to table 5
   
    request.AFI_flg       = FrameBuf[ISO15693_ADDR_FLAGS] & ISO15693_REQ_FLAG_AFI ?      1 : 0;
    request.Nb_slot_flg   = FrameBuf[ISO15693_ADDR_FLAGS] & ISO15693_REQ_FLAG_NB_SLOTS ? 1 : 0;    

  } else { // sets according to table 4
    request.address_flg  = FrameBuf[ISO15693_ADDR_FLAGS] & ISO15693_REQ_FLAG_ADDRESS   ? 1 : 0;
    request.select_flg   = FrameBuf[ISO15693_ADDR_FLAGS] & ISO15693_REQ_FLAG_SELECT    ? 1 : 0;    
  }

  (*TagGetUid)(request.tagUid); // get the actual tag uid using a pointer to a tag specific function whatever it might be

  if (request.address_flg){ // addressed request       
    
    request.Frame_Uid = &FrameBuf[ISO15693_ADDR_FLAGS + 2]; // set pointer to request Uid    
    
    if (ISO15693CompareUid(request.Frame_Uid, request.tagUid)) {            
     //request addressed to this tag
      request.isaddressed = OUR_ADDR; /*request is addressed to this tag.  isaddressed tells more tan the related address_flag: */
      request.Frame_params = request.Frame_Uid + ActiveConfiguration.UidSize; // Frame_params pointer set to the first parameter in the request   	    
     }
     else   
       request.isaddressed = TO_OTHERS; // request addressed to other tags            
      
  }
  else { // request not addressed
      request.isaddressed  = NOT_ADDRESSED; /* request not addressed*/ 
      request.Frame_params = &FrameBuf[ISO15693_ADDR_FLAGS + 2];   	        
  }
  
 return request;   
 
}



uint16_t  ISO15693_inventory ( uint8_t *FrameBuf , struct ISO15693_parameters *request )
{

   uint16_t ResponseByteCount = 0;

   if (request->inventory_flg) {//The Inventory_flag shall be set to 1 according to 15693-3 2009_A2_2015.pdf manual 10.3.1
   	FrameBuf[ISO15693_ADDR_FLAGS] = ISO15693_RES_FLAG_NO_ERROR;
   	FrameBuf[ISO15693_RES_ADDR_PARAM] = ISO15693_RES_INVENTORY_DSFID;
   	ISO15693CopyUid(&FrameBuf[ISO15693_RES_ADDR_PARAM + 0x01], request->tagUid);
   	ResponseByteCount = 10;
   } else ResponseByteCount = 0; //If the VICC detects an error, it shall remain silent 15693-3 2009_A2_2015.pdf manual 10.3.1.
     
   return ResponseByteCount;
  
}



uint16_t ISO15693_writesingle(uint8_t *FrameBuf , struct ISO15693_parameters *request)
{ // returns responsByteCount
 
  uint16_t  ResponseByteCount = 0;
  uint16_t PageAddress , MemLocation ;

 
   /* ISO 15693 READ SINGLE request: 
      addressed    [FLAGS(1BYTE)][CMD(1BYTE)][UID(8BYTES)][BLOCK #][4 bytes(to be written)]
                                                          ^  
                                                         request->Frame_params

       NOT_ADDRESSED  [FLAGS(1BYTE)][CMD(1BYTE)][BLOCK #][4 bytes(to be written)]
                                                ^  
                                               request->Frame_params
   */

  // request->Frame_params is assumed to be correctly assigned by the extract_param function to point to the block # parameter(PageAddress) 
  PageAddress = *request->Frame_params++; // Assign PageAddres and make request->Frame_params to point to the data to be written  

  MemLocation = PageAddress * request->Bytes_per_Page;
  MemoryWriteBlock( request->Frame_params, MemLocation, request->Bytes_per_Page);
  FrameBuf[ISO15693_ADDR_FLAGS] = ISO15693_RES_FLAG_NO_ERROR;
  ResponseByteCount = 1;
 

 return ResponseByteCount;  
}

uint16_t ISO15693_reset_to_ready(enum status *State , uint8_t *FrameBuf, struct ISO15693_parameters *request){

  uint16_t ResponseByteCount = 0;
   
   if (request->address_flg){
        if ( request->isaddressed == OUR_ADDR) {
          FrameBuf[ISO15693_ADDR_FLAGS] = ISO15693_RES_FLAG_NO_ERROR;
          ResponseByteCount = 1;
          *State = STATE_READY;
        } else ResponseByteCount = 0; // not addressed to us

   } else if ( *State == STATE_SELECTED && request->select_flg) {
        // 15693-3 2009_A2_2015.pdf manual 10.3.2 page 13 state picture
        FrameBuf[ISO15693_ADDR_FLAGS] = ISO15693_RES_FLAG_NO_ERROR;
        ResponseByteCount = 1;
        *State = STATE_READY;
   }
  return ResponseByteCount ;  
}

uint16_t ISO15693_stay_quiet(enum status *State , uint8_t *FrameBuf, struct ISO15693_parameters *request)
{
 /*15693-3 2009_A2_2015.pdf manual 10.3.2
   When receiving the Stay quiet command, the VICC shall enter the quiet state and shall NOT send back a
   response. There is NO response to the Stay quiet command*/
 uint16_t  ResponseByteCount = 0;

 // The stay quiet command command  shall  always be be executed executed in in Addressed Addressed mode

 if (request->isaddressed == OUR_ADDR)
    *State = STATE_QUIET;
 else if (*State == STATE_SELECTED)
 	 ResponseByteCount = 0;

 return ResponseByteCount;

}

uint16_t  ISO15693_select (enum status *State , uint8_t *FrameBuf, struct ISO15693_parameters *request)
{
 //15693-3 2009_A2_2015.pdf manual 10.4.6:
  uint16_t  ResponseByteCount = 0;

  
  if (request->isaddressed == OUR_ADDR && !request->select_flg) {// for a select command the Select_flag is set to 0. The Address_flag is set to 1.
    //if the UID is equal to its own UID, the VICC shall enter the selected state and shall send a response
       *State = STATE_SELECTED;                     
       FrameBuf[ISO15693_ADDR_FLAGS] = ISO15693_RES_FLAG_NO_ERROR;
       ResponseByteCount = 1;         
    	
  } else if (request->isaddressed == TO_OTHERS ){
     if ( *State == STATE_SELECTED ){ 
	     /* if in selected state and different UID it shall return to the Ready state and 
	        shall not send a response.15693-3 2009_A2_2015.pdf manual 10.4.6: */
  	  *State = STATE_READY;
           ResponseByteCount = 0;


     } else 
	//not in select state and UID different from its own, the VICC shall remain in its state and shall not return a response.
        ResponseByteCount = 0;

  } else { // not addressed 
    /* The Select command shall always be executed in Addressed mode */
    ResponseByteCount = 0;
  }

  return ResponseByteCount;

}
        

uint8_t ISO15693_status_check ( enum status *State , struct ISO15693_parameters *request , uint16_t *ResponseByteCount )
                               
{
 uint8_t mayExecute = 1 ;

 switch ( *State ){

  case  STATE_READY:
      if (request->select_flg) 
           mayExecute = 0;
      break;

  case STATE_QUIET:

      if (!request->address_flg || 
          request->inventory_flg) 
          mayExecute = 0;         

    break;   

  case  STATE_SELECTED:

     if (request->select_flg){
	mayExecute = 1;
    
     } else if(request->cmd == ISO15693_CMD_SELECT && request->isaddressed == TO_OTHERS){
	/* 
 	   if the UID is different to its own and in selected state, the VICC shall return to the Ready state and
	   shall not send a response. The Select command shall always be executed in Addressed mode. (The
	   Select_flag is set to 0. The Address_flag is set to 1. see ISO_IEC_15693-3-2009_A2_2015.pdf page 29	)
	*/
          *State = STATE_READY;
          *ResponseByteCount = 0;    
           mayExecute = 0;    
     }   
     break;

 
  
 }//end switch
 

 return mayExecute; // returns whether a command may be executed

}



