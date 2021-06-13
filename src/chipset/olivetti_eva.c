/*
 * 86Box	A hypervisor and IBM PC system emulator that specializes in
 *		running old operating systems and software designed for IBM
 *		PC systems and compatibles from 1981 through fairly recent
 *		system designs based on the PCI bus.
 *
 *		This file is part of the 86Box distribution.
 *
 *		Implementation of the Olivetti EVA (98/86) Gate Array.
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

// #define ENABLE_OLIVETTI_EVA_LOG 1

typedef struct
{
    uint8_t	reg_065;
    uint8_t	reg_067;
    uint8_t reg_069;
    // uint8_t mem_check;
} olivetti_eva_t;

#ifdef ENABLE_OLIVETTI_EVA_LOG
int olivetti_eva_do_log = ENABLE_OLIVETTI_EVA_LOG;
static void
olivetti_eva_log(const char *fmt, ...)
{
    va_list ap;

    if (olivetti_eva_do_log) {
        va_start(ap, fmt);
        pclog_ex(fmt, ap);
    	va_end(ap);
    }
}
#else
#define olivetti_eva_log(fmt, ...)
#endif

static void
olivetti_eva_write(uint16_t addr, uint8_t val, void *priv)
{
    olivetti_eva_t *dev = (olivetti_eva_t *) priv;
    olivetti_eva_log("Olivetti EVA Gate Array: Write %02x at %02x\n", val, addr);

    switch (addr) {
        case 0x065:
            dev->reg_065 = val;
            // if (val & 0x80 && !(dev->mem_check)){
            //     dev->reg_069 = 0x17;
            // }
            
            // if (val & 0x80)
            //     dev->mem_check = 1;
            // else
            //     dev->reg_069 = 0x9;
            break;
        case 0x067:
            dev->reg_067 = val;
            break;
        case 0x069:
            dev->reg_069 = val;
            /*
             * Unfortunately, if triggered, the BIOS remapping function fails causing 
             * a fatal error. Therefore, this code section is currently commented.
             */
            // if (val & 1){
            //     /* 
            //      * Set the register to 7 or above for the BIOS to trigger the 
            //      * memory remapping function if shadowing is active.
            //      */
            //     dev->reg_069 = 0x7;
            // }
            // if (val & 8) {
            //     /*
            //      * Activate shadowing for region e0000-fffff
            //      */
            //     mem_remap_top(256);
            //     mem_set_mem_state_both(0xa0000, 0x60000, MEM_READ_INTERNAL | MEM_WRITE_INTERNAL);
            // }
            
            // if (val == 0 && !(dev->mem_check)){
            //     dev->reg_069 = 0x8;
            // }
            // else if (val & 8) {
            //         mem_set_mem_state_both(0xe0000, 0x20000, MEM_READ_INTERNAL | MEM_WRITE_INTERNAL);
            //         // mem_set_mem_state_both(0xe0000, 0x20000, MEM_READ_EXTANY | MEM_WRITE_EXTANY);
            //         dev->reg_069 = 0x1;
            //         dev->mem_check = 1;
            //     }
                // else if (val & 8 & dev->mem_check)
                //     mem_set_mem_state_both(0xe0000, 0x20000, MEM_READ_EXTANY | MEM_WRITE_EXTANY);
                // dev->mem_check = 1;
            // if (val & 0x40)
            //     mem_set_mem_state_both(0xe0000, 0x20000, MEM_READ_INTERNAL | MEM_WRITE_DISABLED);
            break;
    }
}

static uint8_t
olivetti_eva_read(uint16_t addr, void *priv)
{
    olivetti_eva_t *dev = (olivetti_eva_t *) priv;
    uint8_t ret = 0xff;
    switch (addr) {
        case 0x065:
            ret = dev->reg_065;
            break;
        case 0x067:
            /* never happens */
            ret = dev->reg_067;
            break;
        case 0x069:
            ret = dev->reg_069;
            break;
    }
    olivetti_eva_log("Olivetti EVA Gate Array: Read %02x at %02x\n", ret, addr);
    return ret;
}


static void
olivetti_eva_close(void *priv)
{
    olivetti_eva_t *dev = (olivetti_eva_t *) priv;

    free(dev);
}

static void *
olivetti_eva_init(const device_t *info)
{
    olivetti_eva_t *dev = (olivetti_eva_t *) malloc(sizeof(olivetti_eva_t));
    memset(dev, 0, sizeof(olivetti_eva_t));

    /* operations
     write 0 67h, write 1 69h
     write 1 69h, read 69h, add 2, write 69h, read 69h, remove 2, write 69h
     read 69h, add 4, write 69h, read 69h, remove 4, write 69h
     //no --> read 69h, cmp 7, if > add 8, write 69h
     read 69h, remove 8, write 69h
     read 69h, remove 40, write 69h
     read 69h, add 48, write 69h
     read 65h, remove 80, write 65h
     read 65h, add 80, write 65h
     //no --> read 69h, if 8 add 40, write 69h, read 65h, add 80, write 65h
     read 69h, add 1, write 69h
     read 69h, add 10, writr 69h
     //no --> read 69h, cmp 11, if not 0 write 1 to 69h
     read 69h, add 20, write 69h
     read 69h, remove 80, add 40, write 69h
    */
    
    // dev->mem_check = 0;
    
    /* GA98 registers */
    dev->reg_065 = 0x00;
    
    /* RAM page registers: never read, only set */
    dev->reg_067 = 0x00;
    /* 
     * if > 0 ram test is skipped (set during warm-boot)
     * if bit 3 is set, machine hangs -> if set, shadow ram is reported, should be set programmatically
     * bios code can set bit 4, 1 or 2, unclear when it should happen
     * bios code can set bit 6 and 3, unclear when it should happen
     * bit 6 is set if bit 3 is high
     * bit 5 is set when remapping occurs
     */
    
    /* RAM enable registers */
    dev->reg_069 = 0x0;
    
    io_sethandler(0x0065, 0x0001, olivetti_eva_read, NULL, NULL, olivetti_eva_write, NULL, NULL, dev);
    io_sethandler(0x0067, 0x0001, olivetti_eva_read, NULL, NULL, olivetti_eva_write, NULL, NULL, dev);
    io_sethandler(0x0069, 0x0001, olivetti_eva_read, NULL, NULL, olivetti_eva_write, NULL, NULL, dev);
    
    /* When shadowing is not enabled in BIOS, all upper memory is available as XMS */
    mem_remap_top(384);
    
    /* 
     * Default settings when NVRAM is cleared activate shadowing.
     * Thus, to avoid boot errors, remap only 256k from UMB to XMS.
     * Remove this block once BIOS memory remapping works.
     */
    mem_remap_top(256);

    return dev;
}

const device_t olivetti_eva_device = {
    "Olivetti EVA Gate Array",
    0,
    0,
    olivetti_eva_init, olivetti_eva_close, NULL,
    { NULL }, NULL, NULL,
    NULL
};
