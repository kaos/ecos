
#define FLASH_Read_ID      0x90
#define FLASH_Read_Query   0x98
#define FLASH_Read_Status  0x70
#define FLASH_Clear_Status 0x50
#define FLASH_Status_Ready 0x80
#define FLASH_Write        0xE8
#define FLASH_Program      0x10
#define FLASH_Block_Erase  0x20
#define FLASH_Clear_Locks  0x60
#define FLASH_Confirm      0xD0
#define FLASH_Configure    0xB8
#define FLASH_Configure_ReadyWait      0x00
#define FLASH_Configure_PulseOnErase   0x01
#define FLASH_Configure_PulseOnProgram 0x02
#define FLASH_Configure_PulseOnBoth    0x03
#define FLASH_Reset        0xFF

#define FLASH_BLOCK_SIZE   0x10000
