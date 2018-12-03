/*
 * tcpClient.h
 *
 *  Created on: 8.01.2017 ã.
 *      Author: admin
 */

#ifndef INCLUDE_DRIVER_TCPSERVER_H_
#define INCLUDE_DRIVER_TCPSERVER_H_
#include "pgmspace.h"

#define LOCAL_PORT 80
#define DISCON_TIME 2

#define SECTOR_SIZE 0x1000
#define BOOT_CONFIG_SECTOR 1
#define START_ADDRESS_ROM_1 0x02000
#define START_ADDRESS_ROM_2 0x82000
typedef struct {
	uint32 start_addr;
	uint32 start_sector;
	int32 last_sector_erased;
	uint8 extra_count;
	uint8 extra_bytes[4];
} rboot_write_status;
#define MAX_ROMS 4
typedef struct {
	uint8 magic;           ///< Our magic, identifies rBoot configuration - should be BOOT_CONFIG_MAGIC
	uint8 version;         ///< Version of configuration structure - should be BOOT_CONFIG_VERSION
	uint8 mode;            ///< Boot loader mode (MODE_STANDARD | MODE_GPIO_ROM | MODE_GPIO_SKIP)
	uint8 current_rom;     ///< Currently selected ROM (will be used for next standard boot)
	uint8 gpio_rom;        ///< ROM to use for GPIO boot (hardware switch) with mode set to MODE_GPIO_ROM
	uint8 count;           ///< Quantity of ROMs available to boot
	uint8 unused[2];       ///< Padding (not used)
	uint32 roms[MAX_ROMS]; ///< Flash addresses of each ROM
#ifdef BOOT_CONFIG_CHKSUM
	uint8 chksum;          ///< Checksum of this configuration structure (if BOOT_CONFIG_CHKSUM defined)
#endif
	uint32 len[MAX_ROMS];  ///Length rom
} rboot_config;
rboot_config rboot_read_config(void);
void TcpInit();
static const char html_program[] PROGMEM={
#include "program.h"
};

static const char html_info[] PROGMEM={
#include "info.h"
};

 static const char html_lan[] PROGMEM ={
#include "lan.h"
 };


#endif /* INCLUDE_DRIVER_TCPSERVER_H_ */
