/*
 * 86Box	A hypervisor and IBM PC system emulator that specializes in
 *		running old operating systems and software designed for IBM
 *		PC systems and compatibles from 1981 through fairly recent
 *		system designs based on the PCI bus.
 *
 *		This file is part of the 86Box distribution.
 *
 *		Implementation of the EPSON E01243NC (T9898B) Gate Array.
 *      NOTE: As no documentation is available (yet), this chipset 
 *      has been reverese-engineered. Thus, its behavior may not
 *      be fully accurate.
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

#define ENABLE_EPSON_E01243NC_LOG 1

typedef struct
{
    uint8_t	reg_065;
    uint8_t	reg_067;
    uint8_t reg_069;
} epson_e01243nc_t;

#ifdef ENABLE_EPSON_E01243NC_LOG
int epson_e01243nc_do_log = ENABLE_EPSON_E01243NC_LOG;
static void
epson_e01243nc_log(const char *fmt, ...)
{
    va_list ap;

    if (epson_e01243nc_do_log) {
        va_start(ap, fmt);
        pclog_ex(fmt, ap);
    	va_end(ap);
    }
}
#else
#define epson_e01243nc_log(fmt, ...)
#endif

static void
epson_e01243nc_write(uint16_t addr, uint8_t val, void *priv)
{
    epson_e01243nc_t *dev = (epson_e01243nc_t *) priv;
    epson_e01243nc_log("EPSON E01243NC Gate Array: Write %02x at %02x\n", val, addr);
    switch (addr) {
        case 0x065:
            dev->reg_065 = val;
            break;
        case 0x067:
            dev->reg_067 = val;
            break;
        case 0x069:
            dev->reg_069 = val;
            break;
    }
}

static uint8_t
epson_e01243nc_read(uint16_t addr, void *priv)
{
    epson_e01243nc_t *dev = (epson_e01243nc_t *) priv;
    uint8_t ret = 0xff;
    switch (addr) {
        case 0x065:
            ret = dev->reg_065;
            break;
        case 0x067:
            ret = dev->reg_067;
            break;
        // base/onboard memory
        case 0x069:
            ret = dev->reg_069;
            break;
    }
    epson_e01243nc_log("EPSON E01243NC Gate Array: Read %02x at %02x\n", ret, addr);
    return ret;
}


static void
epson_e01243nc_close(void *priv)
{
    epson_e01243nc_t *dev = (epson_e01243nc_t *) priv;

    free(dev);
}

static void *
epson_e01243nc_init(const device_t *info)
{
    epson_e01243nc_t *dev = (epson_e01243nc_t *) malloc(sizeof(epson_e01243nc_t));
    memset(dev, 0, sizeof(epson_e01243nc_t));

    
    /* GA98 registers */
    dev->reg_065 = 0xff;
    /* RAM page registers */
    dev->reg_067 = 0xff;
    /* RAM enable registers */
    dev->reg_069 = 0x00;
    
    io_sethandler(0x00e5, 0x0003, epson_e01243nc_read, NULL, NULL, epson_e01243nc_write, NULL, NULL, dev);

    return dev;
}

const device_t epson_e01243nc_device = {
    "EPSON E01243NC Gate Array",
    0,
    0,
    epson_e01243nc_init, epson_e01243nc_close, NULL,
    { NULL }, NULL, NULL,
    NULL
};
