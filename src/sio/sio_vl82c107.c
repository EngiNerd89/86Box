/*
 * 86Box	A hypervisor and IBM PC system emulator that specializes in
 *		running old operating systems and software designed for IBM
 *		PC systems and compatibles from 1981 through fairly recent
 *		system designs based on the PCI bus.
 *
 *		This file is part of the 86Box distribution.
 *
 *		Emulation of the VLSI VL82C107 SCAMP I/O Combo.
 *
 *
 *
 * Author:	EngiNerd <webmaster.crrc@yahoo.it>
 * 
 *      Copyright 2021 EngiNerd.
 */
#include <stdarg.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#define HAVE_STDARG_H
#include <86box/86box.h>
#include <86box/io.h>
#include <86box/timer.h>
#include <86box/device.h>
#include <86box/lpt.h>
#include <86box/mem.h>
#include <86box/nvr.h>
#include <86box/pci.h>
#include <86box/rom.h>
#include <86box/serial.h>
#include <86box/hdc.h>
#include <86box/hdc_ide.h>
#include <86box/fdd.h>
#include <86box/fdc.h>
#include <86box/sio.h>

// #define ENABLE_VL82C107_LOG 1

#define HAS_FDC_FUNCTIONALITY dev->fdc_function

#ifdef ENABLE_VL82C107_LOG
int vl82c107_do_log = ENABLE_VL82C107_LOG;
static void
vl82c107_log(const char *fmt, ...)
{
    va_list ap;

    if (vl82c107_do_log)
    {
        va_start(ap, fmt);
        pclog_ex(fmt, ap);
        va_end(ap);
    }
}
#else
#define vl82c107_log(fmt, ...)
#endif

typedef struct {
    uint8_t index, fdc_function,
        regs[256];
    fdc_t *fdc;
    serial_t *uart[2];
} vl82c107_t;


static void
lpt1_handler(vl82c107_t *dev)
{
    int temp;
    uint16_t lpt_port = 0x378;
    uint8_t lpt_irq = 7;

    /* bits 4-5:
     * 00 3bch
     * 01 378h
     * 10 278h
     * 11 disabled
     */
    temp = (dev->regs[dev->index] >> 4) & 3;

    switch (temp) {
	case 0:
		lpt_port = 0x3bc;
		break;
	case 1:
		lpt_port = 0x378;
		break;
	case 2:
		lpt_port = 0x278;
		break;
	case 3:
		lpt_port = 0x000;
		lpt_irq = 0xff;
		break;
    }

    if (lpt_port)
	lpt1_init(lpt_port);

    lpt1_irq(lpt_irq);
}


static void
serial_handler(vl82c107_t *dev, int uart)
{
    int temp;
    /* 
     * bit 1: serial ports as COM1 COM2
     * bit 2: disable serial port 1
     * bit 3: disable serial port 2
     */
    temp = (dev->regs[dev->index] >> (2 + uart)) & 1;

    /* current serial port is enabled */
    if (!temp){
        /* serial port 2 */
        if (uart) {
            /* COM2 */
            if((dev->regs[dev->index] >> 1) & 1)
                serial_setup(dev->uart[uart], 0x2f8, 3);
            /* COM4 */
            else
                serial_setup(dev->uart[uart], 0x2e8, 3);
        } else {
            /* COM1 */
            if((dev->regs[dev->index] >> 1) & 1)
                serial_setup(dev->uart[uart], 0x3f8, 4);
            /* COM3 */
            else
                serial_setup(dev->uart[uart], 0x3e8, 4);
        }    
    }
}


static void
vl82c107_write(uint16_t port, uint8_t val, void *priv)
{
    vl82c107_t *dev = (vl82c107_t *) priv;
    uint8_t valxor;

    vl82c107_log("SIO: Write %02x at %02x\n", val, port);
    
    switch (port)
    {
    case 0xec:
        dev->index = val;
        break;
    
    case 0xed:
        if ((dev->index >= 0x1b) && (dev->index <= 0x1f)) {
            valxor = val ^ dev->regs[dev->index];
            dev->regs[dev->index] = val;
            switch (dev->index)
            {
            /* 
            * CSCTRL
            * bit 7: (FDCEN) enable FDC
            * bit 6: (LPTEN) enable parallel port
            * bits 4-5: (LPT1) parallel port address
            * bit 3: (COMB) enable second serial port
            * bit 2: (COMA) enable first serial port
            * bit 1: (COMS) serial ports as COM1 COM2
            * bit 0: (IDEN) enable IDE
            */
            case 0x1e:
                
                /* reconfigure IDE controller */
                if (valxor & 0x1) {
                    vl82c107_log("SIO: HDC disabled\n");
                    ide_pri_disable();
                    if (val & 0x1) {
                        vl82c107_log("SIO: HDC enabled\n");
                        ide_set_base(0, 0x1f0);
                        ide_set_side(0, 0x3f6);
                        ide_pri_enable();
                    }
                }
                /* reconfigure serial ports */
                if (valxor & 0xe) {
                    vl82c107_log("SIO: serial port 1 disabled\n");
                    serial_remove(dev->uart[0]);
                    vl82c107_log("SIO: serial port 2 disabled\n");
                    serial_remove(dev->uart[1]);
                    /* first serial port */
                    if (val & 4) {
                        vl82c107_log("SIO: serial port 1 enabled\n");
                        serial_handler(dev, 0);
                    /* second serial port */
                    } if (val & 8) {
                        vl82c107_log("SIO: serial port 2 enabled\n");
                        serial_handler(dev, 1);
                    }
                }
                /* reconfigure parallel port */
                if (valxor & 0x70) {
                    vl82c107_log("SIO: parallel port disabled\n");
                    lpt1_remove();
                    if ((val & 0x40) && !((val & 0x20) && (val & 0x10))) {
                        vl82c107_log("SIO: parallel port enabled\n");
                        lpt1_handler(dev);
                    }
                }
                /* reconfigure floppy disk controller */
                if ((valxor & 0x80) && HAS_FDC_FUNCTIONALITY) {
                    vl82c107_log("SIO: FDC disabled\n");
                    fdc_remove(dev->fdc);
                    if (val & 0x80) {
                        vl82c107_log("SIO: FDC enabled\n");
                        fdc_set_base(dev->fdc, 0x3f0);
                    }
                }
                
                break;
            
            default:
                break;
            }
        }
        break;
    }
    
    return;
}


uint8_t
vl82c107_read(uint16_t port, void *priv)
{
    vl82c107_t *dev = (vl82c107_t *) priv;
    uint8_t ret = 0xff;

    switch (port)
    {
    case 0xec:
        ret = dev->index;
        break;
    
    case 0xed:
        if ((dev->index >= 0x1b) && (dev->index <= 0x1f))
            ret = dev->regs[dev->index];
        break;
    }
    
    vl82c107_log("SIO: Read %02x at %02x\n", ret, port);
    
    return ret;
}


void
vl82c107_reset(vl82c107_t *dev)
{
    /* CSCTRL */
    dev->regs[0x1e] = 0xcf;
    /* REVID */
    dev->regs[0x1f] = 0x70;

    /*
	0 = 360 rpm @ 500 kbps for 3.5"
	1 = Default, 300 rpm @ 500,300,250,1000 kbps for 3.5"
    */
    lpt1_remove();
    lpt1_handler(dev);
    serial_remove(dev->uart[0]);
    serial_remove(dev->uart[1]);
    serial_handler(dev, 0);
    serial_handler(dev, 1);
    if (HAS_FDC_FUNCTIONALITY)
        fdc_reset(dev->fdc);
    // ide_pri_enable();
}


static void
vl82c107_close(void *priv)
{
    vl82c107_t *dev = (vl82c107_t *) priv;

    free(dev);
}


static void *
vl82c107_init(const device_t *info)
{
    vl82c107_t *dev = (vl82c107_t *) malloc(sizeof(vl82c107_t));
    memset(dev, 0, sizeof(vl82c107_t));

    /* Avoid conflicting with machines that make no use of the VL82C107 FDC control */
    HAS_FDC_FUNCTIONALITY = info->local;

    if (HAS_FDC_FUNCTIONALITY)
        dev->fdc = device_add(&fdc_at_nsc_device);
    
    dev->uart[0] = device_add_inst(&ns16550_device, 1);
    dev->uart[1] = device_add_inst(&ns16550_device, 2);

    device_add(&ide_isa_device);

    vl82c107_reset(dev);

    io_sethandler(0xec, 0x0002,
		      vl82c107_read, NULL, NULL, vl82c107_write, NULL, NULL, dev);
     

    return dev;
}


const device_t vl82c107_device = {
    "VLSI VL82C107 SCAMP I/O Combo",
    0,
    0,
    vl82c107_init, vl82c107_close, NULL,
    { NULL }, NULL, NULL,
    NULL
};

const device_t vl82c107_fdc_device = {
    "VLSI VL82C107 SCAMP I/O Combo with FDC functionality",
    0,
    1,
    vl82c107_init, vl82c107_close, NULL,
    { NULL }, NULL, NULL,
    NULL
};
