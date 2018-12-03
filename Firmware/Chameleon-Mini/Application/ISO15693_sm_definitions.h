static enum status{
    STATE_READY,
    STATE_SELECTED,
    STATE_QUIET
} State;

static enum request_addressed{
    OUR_ADDR = 1 ,
    NOT_ADDRESSED  = 0 ,
    TO_OTHERS = -1
};


static struct ISO15693_parameters {

 uint8_t Flags;
 uint8_t cmd;
 uint8_t *Frame_Uid;
 uint8_t *Frame_params;
 enum request_addressed isaddressed;
 ConfigurationUidType tagUid;
 uint8_t Bytes_per_Page  ;

 unsigned int subcarrier_flg:1;
 unsigned int data_rate_flg :1;
 unsigned int inventory_flg :1;
 unsigned int prot_ext_flg  :1;
 unsigned int address_flg   :1;
 unsigned int select_flg    :1;
 unsigned int option_flg    :1;
 unsigned int AFI_flg       :1;
 unsigned int Nb_slot_flg   :1; 
 unsigned int RFU_flg       :1;

};

static struct TagDefines{
	uint16_t BYTES_PER_PAGE ;
	uint16_t UID_SIZE;
	uint16_t MEM_SIZE;     
	uint16_t NUMBER_OF_SECTORS;
	uint16_t MEM_UID_ADDRESS;
} TagDef;
