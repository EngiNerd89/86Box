/*
 * 86Box	A hypervisor and IBM PC system emulator that specializes in
 *		running old operating systems and software designed for IBM
 *		PC systems and compatibles from 1981 through fairly recent
 *		system designs based on the PCI bus.
 *
 *		This file is part of the 86Box distribution.
 *
 *		Implementation of the Olivetti IOC02 I/O Controller.
 *
 *      Note: This chipset has no datasheet, everything were done via
 *      reverse engineering the BIOS of various machines using it.
 *
 * Authors: EngiNerd <webmaster.crrc@yahoo.it>
 *
 *		Copyright 2020-2021 EngiNerd
 */


#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#define HAVE_STDARG_H
#include <86box/86box.h>
#include "cpu.h"
#include <86box/timer.h>
#include <86box/io.h>
#include <86box/device.h>
#include <86box/chipset.h>
#include <86box/video.h>
#include <86box/mem.h>

#define ENABLE_OLIVETTI_ioc02_LOG 1

typedef struct
{
    uint8_t	reg_068;
    uint8_t	reg_06a;
    uint8_t reg_06c;
    // uint8_t mem_check;
} olivetti_ioc02_t;

#ifdef ENABLE_OLIVETTI_ioc02_LOG
int olivetti_ioc02_do_log = ENABLE_OLIVETTI_ioc02_LOG;
static void
olivetti_ioc02_log(const char *fmt, ...)
{
    va_list ap;

    if (olivetti_ioc02_do_log) {
        va_start(ap, fmt);
        pclog_ex(fmt, ap);
    	va_end(ap);
    }
}
#else
#define olivetti_ioc02_log(fmt, ...)
#endif

static void
olivetti_ioc02_write(uint16_t addr, uint8_t val, void *priv)
{
    olivetti_ioc02_t *dev = (olivetti_ioc02_t *) priv;
    olivetti_ioc02_log("Olivetti ioc02 Gate Array: Write %02x at %02x\n", val, addr);

    switch (addr) {
        case 0x068:
            dev->reg_068 = val;
            break;
        case 0x06a:
            dev->reg_06a = val;
            break;
        case 0x06c:
            dev->reg_06c = val;
            break;
    }
}

static uint8_t
olivetti_ioc02_read(uint16_t addr, void *priv)
{
    olivetti_ioc02_t *dev = (olivetti_ioc02_t *) priv;
    uint8_t ret = 0xff;
    switch (addr) {
        case 0x068:
            ret = dev->reg_068;
            // ret = 0x04;
            break;
        case 0x06a:
            // bit 2 = 1, otherwise io error
            ret = dev->reg_06a;
            // ret = 0x04;
            break;
        case 0x06c:
            ret = dev->reg_06c;
            break;
    }
    olivetti_ioc02_log("Olivetti ioc02 Gate Array: Read %02x at %02x\n", ret, addr);
    return ret;
}


static void
olivetti_ioc02_close(void *priv)
{
    olivetti_ioc02_t *dev = (olivetti_ioc02_t *) priv;

    free(dev);
}

static void *
olivetti_ioc02_init(const device_t *info)
{
    olivetti_ioc02_t *dev = (olivetti_ioc02_t *) malloc(sizeof(olivetti_ioc02_t));
    memset(dev, 0, sizeof(olivetti_ioc02_t));

    /* GA98 registers */
    dev->reg_068 = 0x04;
    
    /* RAM page registers: never read, only set */
    dev->reg_06a = 0x04;
    /* 
     * if > 0 ram test is skipped (set during warm-boot)
     * if bit 3 is set, machine hangs -> if set, shadow ram is reported, should be set programmatically
     * bios code can set bit 4, 1 or 2, unclear when it should happen
     * bios code can set bit 6 and 3, unclear when it should happen
     * bit 6 is set if bit 3 is high
     * bit 5 is set when remapping occurs
     */
    
    /* RAM enable registers */
    dev->reg_06c = 0xff;
    
    io_sethandler(0x0068, 0x0001, olivetti_ioc02_read, NULL, NULL, olivetti_ioc02_write, NULL, NULL, dev);
    io_sethandler(0x006a, 0x0001, olivetti_ioc02_read, NULL, NULL, olivetti_ioc02_write, NULL, NULL, dev);
    io_sethandler(0x006c, 0x0001, olivetti_ioc02_read, NULL, NULL, olivetti_ioc02_write, NULL, NULL, dev);
    
    return dev;
}

const device_t olivetti_ioc02_device = {
    "Olivetti ioc02 Gate Array",
    0,
    0,
    olivetti_ioc02_init, olivetti_ioc02_close, NULL,
    { NULL }, NULL, NULL,
    NULL
};
