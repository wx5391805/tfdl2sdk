/*
 * (C) COPYRIGHT ARM Limited. All rights reserved.
 *
 * This program is free software and is provided to you under the terms of the
 * GNU General Public License version 2 as published by the Free Software
 * Foundation, and any use by you of this program is subject to the terms
 * of such GNU licence.
 *
 * A copy of the licence is included with the program, and can also be obtained
 * from Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA  02110-1301, USA.
 *
 */

#include "mve_rsrc_register.h"
#include "mve_rsrc_driver.h"

#ifdef EMULATOR
#include "emulator_userspace.h"
#else
#include <asm/io.h>
#include <linux/spinlock.h>
#include <linux/export.h>
#endif

static spinlock_t reg_bank_lock;

void mver_reg_init8(void)
{
    spin_lock_init(&reg_bank_lock);
}

void mver_reg_write328(volatile uint32_t *addr, uint32_t value)
{
    if (true == mve_rsrc_data8.hw_interaction)
    {
        writel(value, addr);
    }
}

uint32_t mver_reg_read328(volatile uint32_t *addr)
{
    if (true == mve_rsrc_data8.hw_interaction)
    {
        return readl(addr);
    }
    else
    {
        return 0;
    }
}

tCS *mver_reg_get_coresched_bank8(void)
{
    spin_lock(&reg_bank_lock);
    return (tCS *)mve_rsrc_data8.regs;
}

void mver_reg_put_coresched_bank8(tCS **regs)
{
    *regs = NULL;
    spin_unlock(&reg_bank_lock);
}

tCS *mver_reg_get_coresched_bank_irq8(void)
{
    return (tCS *)mve_rsrc_data8.regs;
}

uint32_t mver_reg_get_fuse8(void)
{
    return mve_rsrc_data8.fuse;
}

uint32_t mver_reg_get_version8(void)
{
    return mve_rsrc_data8.hw_version;
}

#ifdef UNIT

void mver_driver_set_hw_interaction8(bool enable)
{
    mve_rsrc_data8.hw_interaction = enable;
}
EXPORT_SYMBOL(mver_driver_set_hw_interaction8);

#endif

EXPORT_SYMBOL(mver_reg_write328);
EXPORT_SYMBOL(mver_reg_read328);
EXPORT_SYMBOL(mver_reg_get_coresched_bank8);
EXPORT_SYMBOL(mver_reg_put_coresched_bank8);
EXPORT_SYMBOL(mver_reg_get_fuse8);
EXPORT_SYMBOL(mver_reg_get_version8);
