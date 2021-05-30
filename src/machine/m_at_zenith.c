//complains about slushware, i.e. shadow ram
//slushware is 128k at address 0e0000-0effff, 0f1000-0ffff
//scratchpad is at address 0f0000-0f0fff
//non-slushed bios is 128k at address fe0000-feffff, ff0000-ff0fff
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <wchar.h>
#include <86box/86box.h>
#include "cpu.h"
#include <86box/timer.h>
#include <86box/dma.h>
#include <86box/nmi.h>
#include <86box/pic.h>
#include <86box/pit.h>
#include <86box/mem.h>
#include <86box/rom.h>
#include <86box/device.h>
#include <86box/fdd.h>
#include <86box/fdc.h>
#include <86box/fdc_ext.h>
#include <86box/gameport.h>
#include <86box/keyboard.h>
#include <86box/lpt.h>
#include <86box/serial.h>
#include <86box/machine.h>
#include <86box/io.h>
#include <86box/vid_cga.h>
#include <86box/chipset.h>


typedef struct {
    mem_mapping_t scratchpad_mapping;
    uint8_t *scratchpad_ram;
} zenith_t;


static uint8_t
zenith_scratchpad_read(uint32_t addr, void *p)
{
    zenith_t *dev = (zenith_t *)p;
    return dev->scratchpad_ram[addr & 0x1ffff];
}


static void
zenith_scratchpad_write(uint32_t addr, uint8_t val, void *p)
{
    zenith_t *dev = (zenith_t *)p;
    dev->scratchpad_ram[addr & 0x1ffff] = val;
}


static void *
zenith_scratchpad_init(const device_t *info)
{
    zenith_t *dev;

    dev = (zenith_t *)malloc(sizeof(zenith_t));
    memset(dev, 0x00, sizeof(zenith_t));	
	
    dev->scratchpad_ram = malloc(0x20000);
    
    mem_mapping_add(&dev->scratchpad_mapping, 0xfe0000, 0x20000,
			zenith_scratchpad_read, NULL, NULL,
			zenith_scratchpad_write, NULL, NULL,
			dev->scratchpad_ram,  MEM_MAPPING_EXTERNAL, dev);
			
    return dev;
}


static void 
zenith_scratchpad_close(void *p)
{
    zenith_t *dev = (zenith_t *)p;

    free(dev->scratchpad_ram);
    free(dev);
}


static const device_t zenith_scratchpad_device = {
    "Zenith scratchpad RAM",
    0, 0,
    zenith_scratchpad_init, zenith_scratchpad_close, NULL,
    { NULL },
    NULL,
    NULL
};


int
machine_at_z200_init(const machine_t *model)
{
    int ret;
    ret = bios_load_linear(L"roms/machines/zdsz200/zenith_z-248.bin",
			   0x000f0000, 65536, 0);
    
    if (bios_only || !ret)
	    return ret;

    machine_at_common_init(model);

    //213-f2-fb-e6-258
    device_add(&addr_debugger_device);
    
    if (fdc_type == FDC_INTERNAL)
	    device_add(&fdc_at_device);

    device_add(&zenith_scratchpad_device);

    device_add(&keyboard_at_device);

    nmi_init();
    
    return ret;
}

int
machine_at_z386_init(const machine_t *model)
{
    int ret;
    ret = bios_load_linear(L"roms/machines/zdsz386/ZENITH.BIO",
			   0x000f0000, 65536, 0);
    
    if (bios_only || !ret)
	    return ret;

    machine_at_common_init(model);

    //213-f2-fb-e6-258
    device_add(&addr_debugger_device);
    
    if (fdc_type == FDC_INTERNAL)
	    device_add(&fdc_at_device);

    device_add(&zenith_scratchpad_device);
    
    device_add(&keyboard_at_device);

    nmi_init();
    
    return ret;
}