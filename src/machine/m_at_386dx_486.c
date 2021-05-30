/*
 * 86Box	A hypervisor and IBM PC system emulator that specializes in
 *		running old operating systems and software designed for IBM
 *		PC systems and compatibles from 1981 through fairly recent
 *		system designs based on the PCI bus.
 *
 *		This file is part of the 86Box distribution.
 *
 *		Implementation of 386DX and 486 machines.
 *
 *
 *
 * Authors:	Sarah Walker, <http://pcem-emulator.co.uk/>
 *		Miran Grca, <mgrca8@gmail.com>
 *
 *		Copyright 2010-2020 Sarah Walker.
 *		Copyright 2016-2020 Miran Grca.
 */
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <wchar.h>
#define HAVE_STDARG_H
#include <86box/86box.h>
#include "cpu.h"
#include <86box/timer.h>
#include <86box/io.h>
#include <86box/device.h>
#include <86box/chipset.h>
#include <86box/keyboard.h>
#include <86box/mem.h>
#include <86box/nvr.h>
#include <86box/pci.h>
#include <86box/fdd.h>
#include <86box/fdc.h>
#include <86box/fdc_ext.h>
#include <86box/rom.h>
#include <86box/sio.h>
#include <86box/hdc.h>
#include <86box/video.h>
#include <86box/flash.h>
#include <86box/scsi_ncr53c8xx.h>
#include <86box/hwm.h>
#include <86box/machine.h>

int
machine_at_acc386_init(const machine_t *model)
{
    int ret;

   ret = bios_load_linear(L"roms/machines/acc386/acc386.BIN",
			  0x000f0000, 65536, 0);

    if (bios_only || !ret)
	return ret;

    machine_at_common_init(model);
    device_add(&acc2168_device);
    device_add(&keyboard_at_ami_device);

    if (fdc_type == FDC_INTERNAL)
    device_add(&fdc_at_device);

    return ret;
}


int
machine_at_asus386_init(const machine_t *model)
{
    int ret;

	ret = bios_load_linear(L"roms/machines/asus386/ASUS_ISA-386C_BIOS.bin",
				0x000f0000, 65536, 0);

    if (bios_only || !ret)
	return ret;

    machine_at_common_init(model);
    device_add(&rabbit_device);
    device_add(&keyboard_at_ami_device);

    if (fdc_type == FDC_INTERNAL)
    device_add(&fdc_at_device);

    return ret;
}


int
machine_at_sis401_init(const machine_t *model)
{
    int ret;

    ret = bios_load_linear(L"roms/machines/sis401/SIS401-2.AMI",
			   0x000f0000, 65536, 0);

    if (bios_only || !ret)
	return ret;

    machine_at_common_ide_init(model);
    device_add(&sis_85c401_device);
    device_add(&keyboard_at_ami_device);

    if (fdc_type == FDC_INTERNAL)
    device_add(&fdc_at_device);

    return ret;
}


int
machine_at_av4_init(const machine_t *model)
{
    int ret;

    ret = bios_load_linear(L"roms/machines/av4/amibios_486dx_isa_bios_aa4025963.bin",
			   0x000f0000, 65536, 0);

    if (bios_only || !ret)
	return ret;

    machine_at_common_ide_init(model);
    device_add(&sis_85c460_device);
    device_add(&keyboard_at_ami_device);

    if (fdc_type == FDC_INTERNAL)
    device_add(&fdc_at_device);

    return ret;
}


int
machine_at_valuepoint433_init(const machine_t *model)	// hangs without the PS/2 mouse
{
    int ret;

    ret = bios_load_linear(L"roms/machines/valuepoint433/$IMAGEP.FLH",
			   0x000e0000, 131072, 0);

    if (bios_only || !ret)
	return ret;

    machine_at_common_ide_init(model);
    device_add(&sis_85c461_device);
    device_add(&keyboard_ps2_device);

    if (fdc_type == FDC_INTERNAL)
    device_add(&fdc_at_device);

    return ret;
}


int
machine_at_ecs386_init(const machine_t *model)
{
    int ret;

    ret = bios_load_interleaved(L"roms/machines/ecs386/AMI BIOS for ECS-386_32 motherboard - L chip.bin",
				L"roms/machines/ecs386/AMI BIOS for ECS-386_32 motherboard - H chip.bin",
				0x000f0000, 65536, 0);

    if (bios_only || !ret)
	return ret;

    machine_at_common_init(model);
    device_add(&cs8230_device);

    if (fdc_type == FDC_INTERNAL)
    device_add(&fdc_at_device);

    device_add(&keyboard_at_ami_device);

    return ret;
}


int
machine_at_spc6000a_init(const machine_t *model)
{
    int ret;

    ret = bios_load_interleaved(L"roms/machines/spc6000a/3c80.u27",
				L"roms/machines/spc6000a/9f80.u26",
				0x000f8000, 32768, 0);

    if (bios_only || !ret)
	return ret;

    machine_at_common_init_ex(model, 1);
    device_add(&cs8230_device);

    if (fdc_type == FDC_INTERNAL)
    device_add(&fdc_at_device);

    device_add(&keyboard_at_samsung_device);

    return ret;
}


int
machine_at_rycleopardlx_init(const machine_t *model)
{
    int ret;

    ret = bios_load_linear(L"roms/machines/rycleopardlx/486-RYC-Leopard-LX.BIN",
			   0x000f0000, 65536, 0);

    if (bios_only || !ret)
	return ret;

    machine_at_common_ide_init(model);

    device_add(&opti283_device);
    device_add(&keyboard_at_ami_device);

    if (fdc_type == FDC_INTERNAL)
    device_add(&fdc_at_device);

    return ret;
}


int
machine_at_486vchd_init(const machine_t *model)
{
    int ret;

    ret = bios_load_linear(L"roms/machines/486vchd/486-4386-VC-HD.BIN",
			   0x000f0000, 65536, 0);

    if (bios_only || !ret)
	return ret;

    machine_at_common_init(model);

    device_add(&via_vt82c49x_device);
    device_add(&keyboard_at_device);

    if (fdc_type == FDC_INTERNAL)
    device_add(&fdc_at_device);

    return ret;
}


int
machine_at_cs4031_init(const machine_t *model)
{
    int ret;

	ret = bios_load_linear(L"roms/machines/cs4031/CHIPS_1.AMI",
				0x000f0000, 65536, 0);

    if (bios_only || !ret)
	return ret;

    machine_at_common_init(model);
    device_add(&cs4031_device);
    device_add(&keyboard_at_ami_device);

    if (fdc_type == FDC_INTERNAL)
    device_add(&fdc_at_device);

    return ret;
}


int
machine_at_pb410a_init(const machine_t *model)
{
    int ret;

    ret = bios_load_linear(L"roms/machines/pb410a/pb410a.080337.4abf.u25.bin",
			   0x000e0000, 131072, 0);

    if (bios_only || !ret)
	return ret;

    machine_at_ibm_common_ide_init(model);

    device_add(&keyboard_ps2_device);

    device_add(&acc3221_device);
    device_add(&acc2168_device);

    device_add(&phoenix_486_jumper_device);

    if (gfxcard == VID_INTERNAL)
	device_add(&ht216_32_pb410a_device);

    return ret;
}


#if defined(DEV_BRANCH) && defined(USE_VECT486VL)
int
machine_at_vect486vl_init(const machine_t *model)	// has HDC problems
{
    int ret;

   ret = bios_load_linear(L"roms/machines/vect486vl/aa0500.ami",
			  0x000e0000, 131072, 0);

    if (bios_only || !ret)
	return ret;

    machine_at_common_init(model);

    device_add(&vl82c480_device);
    device_add(&keyboard_ps2_ami_device);
    device_add(&fdc37c661_device); // presumably SMC FDC37C651
    device_add(&ide_isa_2ch_device);

    if (gfxcard == VID_INTERNAL)
	device_add(&gd5428_onboard_device);

    return ret;
}


const device_t *
at_vect486vl_get_device(void)
{
    return &gd5428_onboard_device;
}
#endif


int
machine_at_acera1g_init(const machine_t *model)
{
    int ret;

   ret = bios_load_linear(L"roms/machines/acera1g/4alo001.bin",
			  0x000e0000, 131072, 0);

    if (bios_only || !ret)
	return ret;

    machine_at_common_init(model);

    if (gfxcard == VID_INTERNAL)
	device_add(&gd5428_onboard_device);

    device_add(&ali1429_device);
    device_add(&keyboard_ps2_acer_pci_device);
    device_add(&ide_isa_2ch_device);

    if (fdc_type == FDC_INTERNAL)
    device_add(&fdc_at_device);

    return ret;
}


const device_t *
at_acera1g_get_device(void)
{
    return &gd5428_onboard_device;
}


static void
machine_at_ali1429_common_init(const machine_t *model)
{
    machine_at_common_ide_init(model);

    device_add(&ali1429_device);

    device_add(&keyboard_at_ami_device);

    if (fdc_type == FDC_INTERNAL)
    device_add(&fdc_at_device);
}


int
machine_at_ali1429_init(const machine_t *model)
{
    int ret;

   ret = bios_load_linear(L"roms/machines/ami486/ami486.bin",
			  0x000f0000, 65536, 0);

    if (bios_only || !ret)
	return ret;

    machine_at_ali1429_common_init(model);

    return ret;
}


int
machine_at_winbios1429_init(const machine_t *model)
{
    int ret;

    ret = bios_load_linear(L"roms/machines/win486/ali1429g.amw",
			   0x000f0000, 65536, 0);

    if (bios_only || !ret)
	return ret;

    machine_at_ali1429_common_init(model);

    return ret;
}


int
machine_at_opti495_init(const machine_t *model)
{
    int ret;

    ret = bios_load_linear(L"roms/machines/award495/opt495s.awa",
			   0x000f0000, 65536, 0);

    if (bios_only || !ret)
	return ret;

    machine_at_common_ide_init(model);

    device_add(&opti495_device);

    device_add(&keyboard_at_device);

    if (fdc_type == FDC_INTERNAL)
    device_add(&fdc_at_device);

    return ret;
}


static void
machine_at_opti495_ami_common_init(const machine_t *model)
{
    machine_at_common_ide_init(model);

    device_add(&opti495_device);

    device_add(&keyboard_at_ami_device);

    if (fdc_type == FDC_INTERNAL)
    device_add(&fdc_at_device);
}


int
machine_at_opti495_ami_init(const machine_t *model)
{
    int ret;

    ret = bios_load_linear(L"roms/machines/ami495/opt495sx.ami",
			   0x000f0000, 65536, 0);

    if (bios_only || !ret)
	return ret;

    machine_at_opti495_ami_common_init(model);

    return ret;
}


int
machine_at_opti495_mr_init(const machine_t *model)
{
    int ret;

    ret = bios_load_linear(L"roms/machines/mr495/opt495sx.mr",
			   0x000f0000, 65536, 0);

    if (bios_only || !ret)
	return ret;

    machine_at_opti495_ami_common_init(model);

    return ret;
}

int
machine_at_403tg_init(const machine_t *model)
{
    int ret;

    ret = bios_load_linear(L"roms/machines/403tg/403TG.BIN",
			   0x000f0000, 65536, 0);

    if (bios_only || !ret)
	return ret;

    machine_at_common_init(model);

    device_add(&opti895_device);

    device_add(&keyboard_at_device);

    if (fdc_type == FDC_INTERNAL)
    device_add(&fdc_at_device);

    return ret;
}


int
machine_at_pc330_6571_init(const machine_t *model)	// doesn't like every CPU other than the iDX4 and the Intel OverDrive, hangs without a PS/2 mouse
{
    int ret;

    ret = bios_load_linear(L"roms/machines/pc330_6571/$IMAGES.USF",
			   0x000e0000, 131072, 0);

    if (bios_only || !ret)
	return ret;

    machine_at_common_init(model);

    device_add(&opti802g_device);
    device_add(&keyboard_ps2_device);
    device_add(&fdc37c665_device);
    device_add(&ide_opti611_vlb_device);
    device_add(&intel_flash_bxt_device);

    return ret;
}

int
machine_at_mvi486_init(const machine_t *model)
{
    int ret;

    ret = bios_load_linear(L"roms/machines/mvi486/MVI627.BIN",
			   0x000f0000, 65536, 0);

    if (bios_only || !ret)
	return ret;

    machine_at_common_init(model);

    device_add(&opti895_device);

    device_add(&keyboard_at_device);
    device_add(&pc87311_ide_device);

    return ret;
}

static void
machine_at_sis_85c471_common_init(const machine_t *model)
{
    machine_at_common_init(model);

    if (fdc_type == FDC_INTERNAL)
    device_add(&fdc_at_device);

    device_add(&sis_85c471_device);
}


int
machine_at_ami471_init(const machine_t *model)
{
    int ret;

    ret = bios_load_linear(L"roms/machines/ami471/SIS471BE.AMI",
			   0x000f0000, 65536, 0);

    if (bios_only || !ret)
	return ret;

    machine_at_sis_85c471_common_init(model);
    device_add(&ide_vlb_device);
    device_add(&keyboard_at_ami_device);

    return ret;
}


int
machine_at_vli486sv2g_init(const machine_t *model)
{
    int ret;

    ret = bios_load_linear(L"roms/machines/vli486sv2g/0402.001",
			   0x000f0000, 65536, 0);

    if (bios_only || !ret)
	return ret;

    machine_at_sis_85c471_common_init(model);
    device_add(&ide_vlb_2ch_device);
    device_add(&keyboard_at_device);

    return ret;
}


int
machine_at_dtk486_init(const machine_t *model)
{
    int ret;

    ret = bios_load_linear(L"roms/machines/dtk486/4siw005.bin",
			   0x000f0000, 65536, 0);

    if (bios_only || !ret)
	return ret;

    machine_at_sis_85c471_common_init(model);
    device_add(&ide_vlb_device);
    device_add(&keyboard_at_device);

    return ret;
}


int
machine_at_px471_init(const machine_t *model)
{
    int ret;

    ret = bios_load_linear(L"roms/machines/px471/SIS471A1.PHO",
			   0x000f0000, 65536, 0);

    if (bios_only || !ret)
	return ret;

    machine_at_sis_85c471_common_init(model);
    device_add(&ide_vlb_device);
    device_add(&keyboard_at_device);

    return ret;
}


int
machine_at_win471_init(const machine_t *model)
{
    int ret;

    ret = bios_load_linear(L"roms/machines/win471/486-SiS_AC0360136.BIN",
			   0x000f0000, 65536, 0);

    if (bios_only || !ret)
	return ret;

    machine_at_sis_85c471_common_init(model);
    device_add(&ide_vlb_device);
    device_add(&keyboard_at_ami_device);

    return ret;
}


int
machine_at_vi15g_init(const machine_t *model)
{
    int ret;

    ret = bios_load_linear(L"roms/machines/vi15g/vi15gr23.rom",
			   0x000f0000, 65536, 0);

    if (bios_only || !ret)
	return ret;

    machine_at_sis_85c471_common_init(model);
    device_add(&ide_vlb_device);
    device_add(&keyboard_at_ami_device);

    return ret;
}


static void
machine_at_sis_85c496_common_init(const machine_t *model)
{
    device_add(&ide_pci_2ch_device);

    pci_init(PCI_CONFIG_TYPE_1);
    pci_register_slot(0x05, PCI_CARD_NORTHBRIDGE, 0, 0, 0, 0);

    pci_set_irq_routing(PCI_INTA, PCI_IRQ_DISABLED);
    pci_set_irq_routing(PCI_INTB, PCI_IRQ_DISABLED);
    pci_set_irq_routing(PCI_INTC, PCI_IRQ_DISABLED);
    pci_set_irq_routing(PCI_INTD, PCI_IRQ_DISABLED);
}


int
machine_at_r418_init(const machine_t *model)
{
    int ret;

    ret = bios_load_linear(L"roms/machines/r418/r418i.bin",
			   0x000e0000, 131072, 0);

    if (bios_only || !ret)
	return ret;

    machine_at_common_init_ex(model, 2);

    machine_at_sis_85c496_common_init(model);
    device_add(&sis_85c496_device);
    pci_register_slot(0x0B, PCI_CARD_NORMAL, 1, 2, 3, 4);
    pci_register_slot(0x0D, PCI_CARD_NORMAL, 2, 3, 4, 1);
    pci_register_slot(0x0F, PCI_CARD_NORMAL, 3, 4, 1, 2);
    pci_register_slot(0x07, PCI_CARD_NORMAL, 4, 1, 2, 3);

    device_add(&fdc37c665_device);
    device_add(&keyboard_ps2_pci_device);

    return ret;
}


int
machine_at_m4li_init(const machine_t *model)
{
    int ret;

    ret = bios_load_linear(L"roms/machines/m4li/M4LI.04S",
			   0x000e0000, 131072, 0);

    if (bios_only || !ret)
	return ret;

    machine_at_common_init_ex(model, 2);

    machine_at_sis_85c496_common_init(model);
    device_add(&sis_85c496_device);
    pci_register_slot(0x0B, PCI_CARD_NORMAL, 1, 2, 3, 4);
    pci_register_slot(0x0D, PCI_CARD_NORMAL, 2, 3, 4, 1);
    pci_register_slot(0x07, PCI_CARD_NORMAL, 4, 1, 2, 3);
    pci_register_slot(0x0F, PCI_CARD_NORMAL, 3, 4, 1, 2);

    device_add(&fdc37c665_device);
    device_add(&keyboard_ps2_pci_device);

    return ret;
}


int
machine_at_ls486e_init(const machine_t *model)
{
    int ret;

    ret = bios_load_linear(L"roms/machines/ls486e/LS486E RevC.BIN",
			   0x000e0000, 131072, 0);

    if (bios_only || !ret)
	return ret;

    machine_at_common_init_ex(model, 2);

    machine_at_sis_85c496_common_init(model);
    device_add(&sis_85c496_ls486e_device);
    pci_register_slot(0x0B, PCI_CARD_NORMAL, 1, 2, 3, 4);
    pci_register_slot(0x0D, PCI_CARD_NORMAL, 2, 3, 4, 1);
    pci_register_slot(0x0F, PCI_CARD_NORMAL, 3, 4, 1, 2);
    pci_register_slot(0x06, PCI_CARD_NORMAL, 4, 1, 2, 3);

    device_add(&fdc37c665_device);
    device_add(&keyboard_ps2_ami_pci_device);

    return ret;
}


int
machine_at_4dps_init(const machine_t *model)
{
    int ret;

    ret = bios_load_linear(L"roms/machines/4dps/4DPS172G.BIN",
			   0x000e0000, 131072, 0);

    if (bios_only || !ret)
	return ret;

    machine_at_common_init_ex(model, 2);

    machine_at_sis_85c496_common_init(model);
    device_add(&sis_85c496_device);
    pci_register_slot(0x0B, PCI_CARD_NORMAL, 1, 2, 3, 4);
    pci_register_slot(0x0D, PCI_CARD_NORMAL, 2, 3, 4, 1);
    pci_register_slot(0x0E, PCI_CARD_NORMAL, 3, 4, 1, 2);
    pci_register_slot(0x07, PCI_CARD_NORMAL, 4, 1, 2, 3);

    device_add(&w83787f_device);
    device_add(&keyboard_ps2_pci_device);

    device_add(&intel_flash_bxt_device);

    return ret;
}


int
machine_at_4sa2_init(const machine_t *model)
{
    int ret;

    ret = bios_load_linear(L"roms/machines/4sa2/4saw0911.bin",
			   0x000e0000, 131072, 0);

    if (bios_only || !ret)
	return ret;

    machine_at_common_init_ex(model, 2);

    machine_at_sis_85c496_common_init(model);
    device_add(&sis_85c496_device);
    pci_register_slot(0x0B, PCI_CARD_NORMAL, 1, 2, 3, 4);
    pci_register_slot(0x0D, PCI_CARD_NORMAL, 2, 3, 4, 1);
    pci_register_slot(0x0F, PCI_CARD_NORMAL, 3, 4, 1, 2);
    pci_register_slot(0x11, PCI_CARD_NORMAL, 4, 1, 2, 3);

    device_add(&pc87332_device);
    device_add(&keyboard_ps2_pci_device);

    device_add(&intel_flash_bxt_device);

    return ret;
}


int
machine_at_alfredo_init(const machine_t *model)
{
    int ret;

    ret = bios_load_linear_combined(L"roms/machines/alfredo/1010AQ0_.BIO",
				    L"roms/machines/alfredo/1010AQ0_.BI1", 0x1c000, 128);

    if (bios_only || !ret)
	return ret;

    machine_at_common_init(model);
    device_add(&ide_pci_2ch_device);

    pci_init(PCI_CONFIG_TYPE_2 | PCI_NO_IRQ_STEERING);
    pci_register_slot(0x00, PCI_CARD_NORTHBRIDGE, 0, 0, 0, 0);
    pci_register_slot(0x01, PCI_CARD_IDE, 0, 0, 0, 0);
    pci_register_slot(0x06, PCI_CARD_NORMAL, 3, 2, 1, 4);
    pci_register_slot(0x0E, PCI_CARD_NORMAL, 2, 1, 3, 4);
    pci_register_slot(0x0C, PCI_CARD_NORMAL, 1, 3, 2, 4);
    pci_register_slot(0x02, PCI_CARD_SOUTHBRIDGE, 0, 0, 0, 0);
    device_add(&keyboard_ps2_ami_pci_device);
    device_add(&sio_device);
    device_add(&fdc37c663_device);
    device_add(&intel_flash_bxt_ami_device);

    device_add(&i420tx_device);

    return ret;
}


int
machine_at_486sp3g_init(const machine_t *model)
{
    int ret;

    ret = bios_load_linear(L"roms/machines/486sp3g/PCI-I-486SP3G_0306.001 (Beta).bin",
			   0x000e0000, 131072, 0);

    if (bios_only || !ret)
	return ret;

    machine_at_common_init(model);
    device_add(&ide_pci_2ch_device);

    pci_init(PCI_CONFIG_TYPE_2 | PCI_NO_IRQ_STEERING);
    pci_register_slot(0x00, PCI_CARD_NORTHBRIDGE, 0, 0, 0, 0);
    pci_register_slot(0x01, PCI_CARD_IDE, 0, 0, 0, 0);
    pci_register_slot(0x03, PCI_CARD_NORMAL, 1, 2, 3, 4);	/* 03 = Slot 1 */
    pci_register_slot(0x04, PCI_CARD_NORMAL, 2, 3, 4, 1);	/* 04 = Slot 2 */
    pci_register_slot(0x05, PCI_CARD_NORMAL, 3, 4, 1, 2);	/* 05 = Slot 3 */
    pci_register_slot(0x06, PCI_CARD_NORMAL, 4, 1, 2, 3);	/* 06 = Slot 4 */
    pci_register_slot(0x07, PCI_CARD_SCSI, 1, 2, 3, 4);		/* 07 = SCSI */
    pci_register_slot(0x02, PCI_CARD_SOUTHBRIDGE, 0, 0, 0, 0);
    device_add(&keyboard_ps2_ami_pci_device); /* Uses the AMIKEY KBC */
    device_add(&sio_device);	/* Site says it has a ZB, but the BIOS is designed for an IB. */
    device_add(&pc87332_device);
    device_add(&sst_flash_29ee010_device);

    device_add(&i420zx_device);
    device_add(&ncr53c810_onboard_pci_device);

    return ret;
}


int
machine_at_486ap4_init(const machine_t *model)
{
    int ret;

    ret = bios_load_linear(L"roms/machines/486ap4/0205.002",
			   0x000e0000, 131072, 0);

    if (bios_only || !ret)
	return ret;

    machine_at_common_init(model);

    pci_init(PCI_CONFIG_TYPE_1 | PCI_NO_IRQ_STEERING);
    /* Excluded: 5, 6, 7, 8 */
    pci_register_slot(0x05, PCI_CARD_NORTHBRIDGE, 0, 0, 0, 0);
    pci_register_slot(0x09, PCI_CARD_NORMAL, 1, 2, 3, 4);	/* 09 = Slot 1 */
    pci_register_slot(0x0a, PCI_CARD_NORMAL, 2, 3, 4, 1);	/* 0a = Slot 2 */
    pci_register_slot(0x0b, PCI_CARD_NORMAL, 3, 4, 1, 2);	/* 0b = Slot 3 */
    pci_register_slot(0x0c, PCI_CARD_NORMAL, 4, 1, 2, 3);	/* 0c = Slot 4 */
    device_add(&keyboard_ps2_ami_pci_device); /* Uses the AMIKEY KBC */

    if (fdc_type == FDC_INTERNAL)
    device_add(&fdc_at_device);

    device_add(&i420ex_device);

    return ret;
}


#if defined(DEV_BRANCH) && defined(NO_SIO)
int
machine_at_486vipio2_init(const machine_t *model)
{
    int ret;

    ret = bios_load_linear(L"roms/machines/486vipio2/1175G701.BIN",
			   0x000e0000, 131072, 0);

    if (bios_only || !ret)
	return ret;

    machine_at_common_init(model);

    pci_init(PCI_CONFIG_TYPE_1);
    pci_register_slot(0x00, PCI_CARD_NORTHBRIDGE, 0, 0, 0, 0);
    pci_register_slot(0x08, PCI_CARD_NORMAL, 1, 2, 3, 4);
    pci_register_slot(0x09, PCI_CARD_NORMAL, 2, 3, 4, 1);
    pci_register_slot(0x0A, PCI_CARD_NORMAL, 3, 4, 1, 2);
    pci_register_slot(0x0B, PCI_CARD_NORMAL, 4, 1, 2, 3);

    device_add(&via_vt82c49x_device);
    device_add(&via_vt82c505_device);
    device_add(&ide_vlb_2ch_device);
    device_add(&w83787f_device);
    device_add(&keyboard_at_device);

    return ret;
}
#endif


#if defined(DEV_BRANCH) && defined(USE_M1489)
int
machine_at_abpb4_init(const machine_t *model)
{
    int ret;

    ret = bios_load_linear(L"roms/machines/abpb4/486-AB-PB4.BIN",
			   0x000e0000, 131072, 0);

    if (bios_only || !ret)
	return ret;

    machine_at_common_init(model);
    
    pci_init(PCI_CONFIG_TYPE_1);
    pci_register_slot(0x00, PCI_CARD_NORTHBRIDGE, 0, 0, 0, 0);
    pci_register_slot(0x03, PCI_CARD_NORMAL, 1, 2, 3, 4);
    pci_register_slot(0x04, PCI_CARD_NORMAL, 2, 3, 4, 1);
    pci_register_slot(0x05, PCI_CARD_NORMAL, 3, 4, 1, 2);

    device_add(&ali1489_device);
    device_add(&w83787f_device);
    device_add(&keyboard_at_device);

    return ret;
}

int
machine_at_win486pci_init(const machine_t *model)
{
    int ret;

    ret = bios_load_linear(L"roms/machines/win486pci/v1hj3.BIN",
			   0x000e0000, 131072, 0);

    if (bios_only || !ret)
	return ret;

    machine_at_common_init(model);
    
    pci_init(PCI_CONFIG_TYPE_1);
    pci_register_slot(0x00, PCI_CARD_NORTHBRIDGE, 0, 0, 0, 0);
    pci_register_slot(0x03, PCI_CARD_NORMAL, 1, 2, 3, 4);
    pci_register_slot(0x04, PCI_CARD_NORMAL, 2, 3, 4, 1);
    pci_register_slot(0x05, PCI_CARD_NORMAL, 3, 4, 1, 2);

    device_add(&ali1489_device);
    device_add(&prime3b_device);
    device_add(&keyboard_at_ami_device);

    return ret;
}
#endif


int
machine_at_itoxstar_init(const machine_t *model)
{
    int ret;

    ret = bios_load_linear(L"roms/machines/itoxstar/stara.rom",
			   0x000c0000, 262144, 0);

    if (bios_only || !ret)
	return ret;

    machine_at_common_init(model);

    pci_init(PCI_CONFIG_TYPE_1);
    pci_register_slot(0x0B, PCI_CARD_NORTHBRIDGE, 0, 0, 0, 0);
    pci_register_slot(0x0C, PCI_CARD_SOUTHBRIDGE, 0, 0, 0, 0);
    pci_register_slot(0x1F, PCI_CARD_NORMAL, 1, 2, 3, 4);
    device_add(&w83977f_device);
    device_add(&keyboard_ps2_ami_pci_device);
    device_add(&stpc_client_device);
    device_add(&sst_flash_29ee020_device);
    device_add(&w83781d_device); /* fans: Chassis, CPU, unused; temperatures: Chassis, CPU, unused */
    hwm_values.fans[2] = 0; /* unused */
    hwm_values.temperatures[2] = 0; /* unused */
    hwm_values.voltages[0] = 0; /* Vcore unused */

    return ret;
}


int
machine_at_arb1479_init(const machine_t *model)
{
    int ret;

    ret = bios_load_linear(L"roms/machines/arb1479/1479a.rom",
			   0x000c0000, 262144, 0);

    if (bios_only || !ret)
	return ret;

    machine_at_common_init(model);

    pci_init(PCI_CONFIG_TYPE_1);
    pci_register_slot(0x0B, PCI_CARD_NORTHBRIDGE, 0, 0, 0, 0);
    pci_register_slot(0x0C, PCI_CARD_SOUTHBRIDGE, 0, 0, 0, 0);
    pci_register_slot(0x1F, PCI_CARD_NORMAL, 1, 0, 0, 0);
    pci_register_slot(0x1E, PCI_CARD_NORMAL, 2, 3, 4, 1);
    pci_register_slot(0x1D, PCI_CARD_NORMAL, 3, 4, 1, 2);
    device_add(&w83977f_device);
    device_add(&keyboard_ps2_ami_pci_device);
    device_add(&stpc_consumer2_device);
    device_add(&sst_flash_29ee020_device);

    return ret;
}


int
machine_at_pcm9340_init(const machine_t *model)
{
    int ret;

    ret = bios_load_linear(L"roms/machines/pcm9340/9340v110.bin",
			   0x000c0000, 262144, 0);

    if (bios_only || !ret)
	return ret;

    machine_at_common_init(model);

    pci_init(PCI_CONFIG_TYPE_1);
    pci_register_slot(0x0B, PCI_CARD_NORTHBRIDGE, 0, 0, 0, 0);
    pci_register_slot(0x0C, PCI_CARD_SOUTHBRIDGE, 0, 0, 0, 0);
    pci_register_slot(0x1D, PCI_CARD_NORMAL, 4, 1, 2, 3);
    pci_register_slot(0x1E, PCI_CARD_NORMAL, 3, 4, 1, 2);
    pci_register_slot(0x1F, PCI_CARD_NORMAL, 2, 3, 4, 1);
    device_add_inst(&w83977f_device, 1);
    device_add_inst(&w83977f_device, 2);
    device_add(&keyboard_ps2_ami_pci_device);
    device_add(&stpc_elite_device);
    device_add(&sst_flash_29ee020_device);

    return ret;
}


int
machine_at_pcm5330_init(const machine_t *model)
{
    int ret;

    ret = bios_load_linear(L"roms/machines/pcm5330/5330_13b.bin",
			   0x000c0000, 262144, 0);

    if (bios_only || !ret)
	return ret;

    machine_at_common_init(model);

    pci_init(PCI_CONFIG_TYPE_1);
    pci_register_slot(0x0B, PCI_CARD_NORTHBRIDGE, 0, 0, 0, 0);
    pci_register_slot(0x0C, PCI_CARD_SOUTHBRIDGE, 0, 0, 0, 0);
    pci_register_slot(0x0D, PCI_CARD_SOUTHBRIDGE, 0, 0, 0, 0);
    pci_register_slot(0x0E, PCI_CARD_SOUTHBRIDGE, 1, 2, 3, 4);
    pci_register_slot(0x13, PCI_CARD_NORMAL, 1, 2, 3, 4);
    device_add(&stpc_serial_device);
    device_add(&w83977f_370_device);
    device_add(&keyboard_ps2_ami_pci_device);
    device_add(&stpc_atlas_device);
    device_add(&sst_flash_29ee020_device);

    return ret;
}

/*
 * Current bugs: 
 * - RLL/MFM/ESDI hard disk controllers do not work
 * - BIOS shadowing not implemented
 */
int
machine_at_epson_ax3_init(const machine_t *model)
{
    int ret;

    ret = bios_load_interleaved(L"roms/machines/epson_ax3/EVAX3",
				L"roms/machines/epson_ax3/ODAX3",
				0x000f0000, 65536, 0);

    if (bios_only || !ret)
	    return ret;

    machine_at_common_init(model);

    /* replace with correct chipset implementation */
    //mem_remap_top(384);

    //found 22-23
    device_add(&addr_debugger_device);

    device_add(&epson_e01161na_device);

    device_add(&keyboard_at_device);
    
    if (fdc_type == FDC_INTERNAL)	
	    device_add(&fdc_at_device);
    
    return ret;
}


//not working (post error 1d 1c) --> works with debugger
int
machine_at_epson_ax3_33port_init(const machine_t *model)
{
    int ret;

    ret = bios_load_linear(L"roms/machines/epson_ax3_33port/AX33POR",
			   0x000e0000, 131072, 0);

    if (bios_only || !ret)
	    return ret;

    machine_at_common_ide_init(model);
    device_add(&keyboard_ps2_device);
    device_add(&fdc_at_device);

    //found 1b0-1bf
    device_add(&epson_e01161na_device);

    device_add(&vga_device);
    
    return ret;
}


//unknown specs, epson bios
//returns xms error
int
machine_at_epson_el3_33_init(const machine_t *model)
{
    int ret;

    ret = bios_load_linear(L"roms/machines/epson_el3_33/M4.107",
			   0x000e0000, 131072, 0);

    if (bios_only || !ret)
	    return ret;

    machine_at_common_ide_init(model);
    mem_remap_top(384);

    device_add(&keyboard_ps2_device);
    device_add(&fdc_at_nsc_device);

    //1b0-1bf
    device_add(&epson_e01161na_device);

    //video may be wd90c10??
    
    return ret;
}
