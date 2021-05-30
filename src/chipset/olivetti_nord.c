/*
 * 86Box	A hypervisor and IBM PC system emulator that specializes in
 *		running old operating systems and software designed for IBM
 *		PC systems and compatibles from 1981 through fairly recent
 *		system designs based on the PCI bus.
 *
 *		This file is part of the 86Box distribution.
 *
 *		Implementation of the Olivetti NORD Gate Array.
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
#include <86box/mem.h>

#define ENABLE_OLIVETTI_NORD_LOG 1

typedef struct
{
    uint8_t	reg[0x9];
    uint8_t mem_remap;
} olivetti_nord_t;

#ifdef ENABLE_OLIVETTI_NORD_LOG
int olivetti_nord_do_log = ENABLE_OLIVETTI_NORD_LOG;
static void
olivetti_nord_log(const char *fmt, ...)
{
    va_list ap;

    if (olivetti_nord_do_log) {
        va_start(ap, fmt);
        pclog_ex(fmt, ap);
    	va_end(ap);
    }
}
#else
#define olivetti_nord_log(fmt, ...)
#endif

static void
olivetti_nord_write(uint16_t port, uint8_t val, void *priv)
{
    olivetti_nord_t *dev = (olivetti_nord_t *) priv;

    uint16_t addr = port & 0xf;

    dev->reg[addr] = val & 0xdf;

    switch (addr) {
        //2x 61h shadow
        //3x 61h no shadow
        // case 8:
        //     if(val == 0x80)
        //         dev->mem_remap = 1;
        //         break;
        // case 1:
        //     if(dev->mem_remap){
        //         if(val & 0x10)
        //             mem_remap_top(256);
        //         else
        //             mem_remap_top(384);
        //         dev->mem_remap = 0;
        //     }
        //     break;
    //     case 4:
    //         if(val == 0xae)
    //             dev->mem_remap = 1;
    //         else if(val == 0x52) {
    //             if (dev->mem_remap == 1)
    //                 dev->mem_remap = 2;
    //             else
    //                 dev->mem_remap = 0;
    //         }
    //         else if(val == 0x50) {
    //             if (dev->mem_remap == 2)
    //                 mem_remap_top(256);
    //             dev->mem_remap = 0;
    //         } 
    //         else
    //             dev->mem_remap = 0;
    //         break;
    }

    olivetti_nord_log("Olivetti NORD Gate Array: Write %02x at %02x\n", val, port);
    
}

static uint8_t
olivetti_nord_read(uint16_t port, void *priv)
{
    olivetti_nord_t *dev = (olivetti_nord_t *) priv;
    uint8_t ret = 0xff;
    
    uint16_t addr = port & 0xf;

    ret = dev->reg[addr];
    
    switch (addr) {
        case 3:
            ret = dev->reg[addr] & 0xdf;
            if(hasfpu)
                ret |= 0x20;
            break;
    }

    olivetti_nord_log("Olivetti NORD Gate Array: Read %02x at %02x\n", ret, port);
    return ret;
}


static void
olivetti_nord_close(void *priv)
{
    olivetti_nord_t *dev = (olivetti_nord_t *) priv;

    free(dev);
}

static void *
olivetti_nord_init(const device_t *info)
{
    olivetti_nord_t *dev = (olivetti_nord_t *) malloc(sizeof(olivetti_nord_t));
    memset(dev, 0, sizeof(olivetti_nord_t));

    // dev->mem_remap = 0;

    // io_sethandler(0x0061, 0x0001, NULL, NULL, NULL, olivetti_nord_write, NULL, NULL, dev);
    
    io_sethandler(0x0010, 0x0004, olivetti_nord_read, NULL, NULL, olivetti_nord_write, NULL, NULL, dev);
    io_sethandler(0x0094, 0x0001, olivetti_nord_read, NULL, NULL, olivetti_nord_write, NULL, NULL, dev);
    io_sethandler(0x0098, 0x0001, olivetti_nord_read, NULL, NULL, olivetti_nord_write, NULL, NULL, dev);

    mem_remap_top(384);

    return dev;
}

//gives cache controller error when fpu is installed (why?)
const device_t olivetti_nord_device = {
    "Olivetti NORD Gate Array",
    0,
    0,
    olivetti_nord_init, olivetti_nord_close, NULL,
    { NULL }, NULL, NULL,
    NULL
};
