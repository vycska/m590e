#include "iap.h"
#include "main.h"
#include "utils-asm.h"
#include "utils.h"
#include "lpc824.h"
#include <string.h>

extern char _flash_start, _flash_end;

static unsigned int command[5] = { 0 }, result[5] = {0};

int sector(unsigned int adr) {
   return adr/0x400;
}

//this command must be executed before executing "copy ram to flash" or "erase sectors" command
int iap_prepare_sectors(unsigned int start_sector, unsigned int end_sector) {
   command[0] = IAP_PREPARE_SECTORS;
   command[1] = start_sector;
   command[2] = end_sector;
   IAP_EXECUTE(command, result);
   return result[0]; //CMD_SUCCESS | BUSY | INVALID_SECTOR
}

//this command is used to program the flash memory
//"dest_address" must be 64 bytes boundary; "source_address" must be word boundary; "len" must be 64 | 128 | 256 | 512 | 1024
int iap_copy_ram_to_flash(unsigned int dest_address, unsigned char *source_address, unsigned int len) {
   unsigned int primask;
   _dsb();
   primask = _start_critical();
   iap_prepare_sectors(sector(dest_address), sector(dest_address + len - 1));
   command[0] = IAP_COPY_RAM_TO_FLASH;
   command[1] = dest_address;
   command[2] = (unsigned int)source_address;
   command[3] = len;
   IAP_EXECUTE(command, result);
   _end_critical(primask);
   return result[0]; //CMD_SUCCESS | SRC_ADDR_ERROR | DST_ADDR_ERROR | SRC_ADDR_NOT_MAPPED | DST_ADDR_NOT_MAPPED | COUNT_ERROR | SECTOR_NOT_PREPARED
}

//this command is used to erase a sector or multiple sectors of on-chip flash memory
int iap_erase_sectors(int start_sector, int end_sector) {
   unsigned int primask;
   _dsb();
   primask = _start_critical();
   iap_prepare_sectors(start_sector, end_sector);
   command[0] = IAP_ERASE_SECTORS;
   command[1] = start_sector;
   command[2] = end_sector;
   IAP_EXECUTE(command, result);
   _end_critical(primask);
   return result[0]; //CMD_SUCCESS | BUSY | SECTOR_NOT_PREPARED | INVALID_SECTOR
}

//this command is used to blank check a sector or multiple sectors of on-chip flash memory
int iap_blank_check_sectors(unsigned int start_sector, unsigned int end_sector) {
   command[0] = IAP_BLANK_CHECK_SECTORS;
   command[1] = start_sector;
   command[2] = end_sector;
   IAP_EXECUTE(command, result);
   return result[0]; //CMD_SUCCESS | BUSY | SECTOR_NOT_BLANK | INVALID_SECTOR
}

//this command is used to read the part identification number
unsigned int iap_read_part_id(void) {
   command[0] = IAP_READ_PART_ID;
   IAP_EXECUTE(command, result);
   return result[1];
}

//this command is used to read boot the boot code version number
unsigned short iap_read_boot_code_version(void) {
   command[0] = IAP_READ_BOOT_CODE_VERSION;
   IAP_EXECUTE(command, result);
   return (unsigned short)result[1]; //byte1.byte0
}

//this command is used to compare the memory contents at two locations
//addresses must be at word boundary; len must be multiple of 4
int iap_compare(unsigned int address1, unsigned int address2, int len) {
   command[0] = IAP_COMPARE;
   command[1] = address1;
   command[2] = address2;
   command[3] = len;
   IAP_EXECUTE(command, result);
   return result[0]; //CMD_SUCCESS | COMPARE_ERROR | COUNT_ERROR | ADDR_ERROR | ADDR_NOT_MAPPED
}

//this command is used to invoke the bootloader in ISP mode
void iap_reinvoke_isp(void) {
   command[0] = IAP_REINVOKE_ISP;
   IAP_EXECUTE(command, result);
}

//this command is used to read the unique ID
unsigned int *iap_read_uid(void) {
   command[0] = IAP_READ_UID;
   IAP_EXECUTE(command, result);
   return &result[1];
}

//this command is used to erase a page or multiple pages of on-chip flash memory
int iap_erase_page(int start_page, int end_page) {
   unsigned int primask;
   _dsb();
   primask = _start_critical();
   iap_prepare_sectors(sector(start_page*64), sector(end_page*64));
   command[0] = IAP_ERASE_PAGE;
   command[1] = start_page;
   command[2] = end_page;
   IAP_EXECUTE(command, result);
   _end_critical(primask);
   return result[0]; //CMD_SUCCESS | BUSY | SECTOR_NOT_PREPARED | INVALID_SECTOR
}

int iflash_write(unsigned int addr, unsigned char *buf, int len) {
   unsigned char interim_page[64];
   int l, p, res;

   for(res=1; len>0 && addr>=(unsigned int)&_flash_start && addr<(unsigned int)&_flash_end; addr+=l, buf+=l, len-=l) {
      p = addr / 64;
      l = MIN2(len, 64-addr%64);
      memcpy(interim_page, (void*)(p*64), 64);
      memcpy(interim_page + addr%64, buf, l);
      res = res && iap_erase_page(p, p)==IAP_CMD_SUCCESS;
      res = res && iap_copy_ram_to_flash(p*64, interim_page, 64)==IAP_CMD_SUCCESS;
   }
   return res;
}
