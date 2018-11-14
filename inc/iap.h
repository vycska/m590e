#ifndef __IAP_H__
#define __IAP_H__

#define IAP_LOCATION                      (0x1fff1ff1) //1 at the end because the IAP routine is a thumb code
#define IAP_EXECUTE(a,b)                  ((void(*)(unsigned int*,unsigned int*))(IAP_LOCATION))(a,b)

#define IAP_PREPARE_SECTORS               50
#define IAP_COPY_RAM_TO_FLASH             51
#define IAP_ERASE_SECTORS                 52
#define IAP_BLANK_CHECK_SECTORS           53
#define IAP_READ_PART_ID                  54
#define IAP_READ_BOOT_CODE_VERSION        55
#define IAP_COMPARE                       56
#define IAP_REINVOKE_ISP                  57
#define IAP_READ_UID                      58
#define IAP_ERASE_PAGE                    59

#define IAP_CMD_SUCCESS               0
#define IAP_INVALID_COMMAND           1
#define IAP_SRC_ADDR_ERROR            2
#define IAP_DST_ADDR_ERROR            3
#define IAP_SRC_ADDR_NOT_MAPPED       4
#define IAP_DST_ADDR_NOT_MAPPED       5
#define IAP_COUNT_ERROR               6
#define IAP_INVALID_SECTOR            7
#define IAP_SECTOR_NOT_BLANK          8
#define IAP_SECTOR_NOT_PREPARED       9
#define IAP_COMPARE_ERROR             10
#define IAP_BUSY                      11
#define IAP_ERR_ISP_IRC_NO_POWER      17
#define IAP_ERR_ISP_FLASH_NO_POWER    18
#define IAP_ERR_ISP_FLASH_NO_CLOCK    1b

int sector(unsigned int);
int iap_prepare_sectors(unsigned int, unsigned int);
int iap_copy_ram_to_flash(unsigned int, unsigned char *, unsigned int);
int iap_erase_sectors(int, int);
int iap_blank_check_sectors(unsigned int, unsigned int);
unsigned int iap_read_part_id(void);
unsigned short iap_read_boot_code_version(void);
int iap_compare(unsigned int, unsigned int, int);
void iap_reinvoke_isp(void);
unsigned int *iap_read_uid(void);
int iap_erase_page(int,int);

#endif

/*
   - the size of the sector is 1 KB
   - the size of the page is 64 B
   - one sector contains 16 pages
   - sectors 0 .. 31
   - pages 0 .. 511
   - CRP is invoked by programming a specific pattern in flash location at 0x000002fc
   - IAP commands are not affected by CRP
   - some IAP commands use the top 32 B of the on-chip RAM
   - command parameter array: [0] command code, [1] param 0, [2] param 1, ...
   - status result array: [0] status code, [1] result 0, [2] result 1, ...
   - the maximum stack usage in the user allocated stack space is 148 bytes and grows downwards
 */

