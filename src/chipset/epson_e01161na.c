/*
 * 86Box	A hypervisor and IBM PC system emulator that specializes in
 *		running old operating systems and software designed for IBM
 *		PC systems and compatibles from 1981 through fairly recent
 *		system designs based on the PCI bus.
 *
 *		This file is part of the 86Box distribution.
 *
 *		Implementation of the EPSON E01161NA (SE2020) Gate Array.
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
#include <86box/lpt.h>
#include <86box/serial.h>
#include <86box/mem.h>

#define ENABLE_EPSON_E01161NA_LOG 1

typedef struct
{
    uint8_t	reg[0x10];
    serial_t *uart;
} epson_e01161na_t;

#ifdef ENABLE_EPSON_E01161NA_LOG
int epson_e01161na_do_log = ENABLE_EPSON_E01161NA_LOG;
static void
epson_e01161na_log(const char *fmt, ...)
{
    va_list ap;

    if (epson_e01161na_do_log) {
        va_start(ap, fmt);
        pclog_ex(fmt, ap);
    	va_end(ap);
    }
}
#else
#define epson_e01161na_log(fmt, ...)
#endif

static void
lpt1_handler(epson_e01161na_t *dev)
{
    int temp;
    uint16_t lpt_port = 0x378;
    uint8_t lpt_irq = 7;

    /* bits 4-5:
     * 11 378h
     * 10 278h
     * 0? disabled
     */
    temp = (dev->reg[0] >> 4) & 3;

    switch (temp) {
	case 2:
		lpt_port = 0x278;
        epson_e01161na_log("EPSON E01161NA Gate Array: Parallel port configured as LPT2\n");
		break;
	case 3:
		lpt_port = 0x378;
		epson_e01161na_log("EPSON E01161NA Gate Array: Parallel port configured as LPT1\n");
		break;
	default:
		lpt_port = 0x000;
		lpt_irq = 0xff;
		break;
    }

    if (lpt_port)
	lpt1_init(lpt_port);

    lpt1_irq(lpt_irq);
}


static void
serial_handler(epson_e01161na_t *dev)
{
    int temp;
    /* bits 6-7:
     * 11 com1
     * 10 com2
     * 0? disabled
     */
    temp = (dev->reg[0] >> 6) & 3;

    switch (temp) {
	case 2:
		serial_setup(dev->uart, 0x2f8, 3);
		epson_e01161na_log("EPSON E01161NA Gate Array: Serial port configured as COM2\n");
		break;
	case 3:
		serial_setup(dev->uart, 0x3f8, 4);
		epson_e01161na_log("EPSON E01161NA Gate Array: Serial port configured as COM1\n");
		break;
	default:
		break;
    }
}

static void
epson_e01161na_write(uint16_t port, uint8_t val, void *priv)
{
    epson_e01161na_t *dev = (epson_e01161na_t *) priv;

    uint16_t addr = port & 0xf;
    uint8_t valxor;

    valxor = val ^ dev->reg[addr];
    dev->reg[addr] = val;

    epson_e01161na_log("EPSON E01161NA Gate Array: Write %02x at %02x\n", val, port);


    switch (addr) {
        /*
         * 1b0: on-board ports
         * bit 7: enable serial port
         * bit 6: serial port primary
         * bit 5: enable parallel port
         * bit 4: parallel port primary
         * ax2e/ax3s: -3
         * others: -0
         */
        case 0:
            /* reconfigure serial port */
            if(valxor & 0xc0){
                serial_remove(dev->uart);
                epson_e01161na_log("EPSON E01161NA Gate Array: Serial port removed\n");
                if (val & 0x80)
                    serial_handler(dev);
            }
            /* reconfigure parallel port */
            if(valxor & 0x30){
                lpt1_remove();
                epson_e01161na_log("EPSON E01161NA Gate Array: Parallel port removed\n");
                if (val & 0x20)
                    lpt1_handler(dev);
                }
            // dev->reg[addr] = 0x0;
            break;
        /*
         * 1b1: write-only
         * always 04
         */
        case 1:
            break;
        /*
         * 1b2: auto speed??
         * ax2e: 03
         * el2: 2a 0a 2a 2b 3b
         * l2: 0a
         * ax3s: 00 03
         * ax4s portable: 24 26
         * el3s: 26 27
         * l3s/ax3 portable: 26
         * el3: 16 17
         */
        case 2:
            break;
        /*
         * 1b3: memory management
         * bit 7: 1024+ kb memory as ems
         * bit 6: always 1?
         * bit 5: 640-1024 kb memory as ems 
         * bit 4: use 640-1024 kb memory
         * bits 1-3: on-board memory:
         *  110: 2 MB
         *  101: 3 MB
         *  111: 5 MB
         *  0??: 1 MB (soldered)
         * bit 0: set to 1 by ax3s
         * ax2e: 4e
         * el2: ce
         * l2: 4a
         * ax3s: 07 00
         * ax3s portable/el3s: 10 17 10
         * l3s: 13
         * ax3 portable/el3: b0 b6 b2
         */
        case 3:
            //ax3s: always remapped
            //el3s: never remapped
            //el3: not remapped or 256k remapped (128k shadow bios)
            if (valxor & 0x30 && !is386) {
                if ((val & 0x10) && !(val & 0x20)) {
                    epson_e01161na_log("EPSON E01161NA Gate Array: UMB remapped\n");
                    mem_remap_top(384);
                }
                else {
                    epson_e01161na_log("EPSON E01161NA Gate Array: UMB not remapped\n");
                    mem_remap_top(0);
                }
            }
            break;
        /*
         * 1b4: ??
         * el2/l2: 00 04 00
         */
        case 4:
            break;
        /*
         * 1b5: shadow management?
         * ax2e f7
         * ax3s 03 01
         * ax3s portable/el3s/l3s 00
         * ax3 portable/el3 80 00 10 90 00 0c 00 30
         */
        case 5:
            break;
        /*
         * 1b6: diskette drive/laptop display?
         * l2/l3s: 20 30
         * ax3s portable/ax3 portable: 08 28 38 18
         */
        case 6:
            break;
    }
    
}

static uint8_t
epson_e01161na_read(uint16_t port, void *priv)
{
    epson_e01161na_t *dev = (epson_e01161na_t *) priv;
    uint8_t ret = 0xff;
    
    uint16_t addr = port & 0xf;

    ret = dev->reg[addr];
    
    switch (addr) {
        case 2:
            // add 2
            // hangs el2
            // ret = 0x02;
            break;
        case 3:
            if (is386){
                /* 386-based systems */
                ret = ret & 0xf8;
                switch (mem_size)
                {
                /* 1 MB */
                case 0x400:
                    break;
                /* 2 MB or 3 MB */
                case 0x800:
                case 0xc00:
                    ret |= 1;
                    break;
                /* 4 MB or 5 MB */
                case 0x1000:
                case 0x1400:
                    ret |= 2;
                    break;
                /* 6 MB or more */
                default:
                    ret |= 3;
                    break;
                }
            } else {
                /* 286-based systems */
                ret = ret & 0xf1;
                switch (mem_size)
                {
                /* 1 MB */
                case 0x400:
                    break;
                /* 2 MB */
                case 0x800:
                    ret |= 0xc;
                    break;
                /* 3 MB or 4 MB*/
                case 0xc00:
                case 0x1000:
                    ret |= 0xa;
                    break;
                /* 5 MB or more */
                default:
                    ret |= 0xe;
                    break;
                }
            }
            break;
        case 6:
            // test 4
            // test 20
            // test 10?
            ret = 0x10;
            break;
    }

    epson_e01161na_log("EPSON E01161NA Gate Array: Read %02x at %02x\n", ret, port);
    return ret;
}

void
epson_e01161na_reset(epson_e01161na_t *dev)
{
    lpt1_remove();
    lpt1_handler(dev);
    serial_remove(dev->uart);
    serial_handler(dev);
    
}


static void
epson_e01161na_close(void *priv)
{
    epson_e01161na_t *dev = (epson_e01161na_t *) priv;

    free(dev);
}

static void *
epson_e01161na_init(const device_t *info)
{
    epson_e01161na_t *dev = (epson_e01161na_t *) malloc(sizeof(epson_e01161na_t));
    memset(dev, 0, sizeof(epson_e01161na_t));

    dev->uart = device_add_inst(&ns16550_device, 1);

    // mem_remap_top(384);

    epson_e01161na_reset(dev);

    io_sethandler(0x01b0, 0x0010, epson_e01161na_read, NULL, NULL, epson_e01161na_write, NULL, NULL, dev);

    return dev;
}

const device_t epson_e01161na_device = {
    "EPSON E01161NA Gate Array",
    0,
    0,
    epson_e01161na_init, epson_e01161na_close, NULL,
    { NULL }, NULL, NULL,
    NULL
};
