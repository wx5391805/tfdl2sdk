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

void mver_reg_init7(void)
{
    spin_lock_init(&reg_bank_lock);
}

void mver_reg_write327(volatile uint32_t *addr, uint32_t value)
{
    if (true == mve_rsrc_data7.hw_interaction)
    {
        writel(value, addr);
    }
}

uint32_t mver_reg_read327(volatile uint32_t *addr)
{
    if (true == mve_rsrc_data7.hw_interaction)
    {
        return readl(addr);
    }
    else
    {
        return 0;
    }
}

tCS *mver_reg_get_coresched_bank7(void)
{
    spin_lock(&reg_bank_lock);
    return (tCS *)mve_rsrc_data7.regs;
}

void mver_reg_put_coresched_bank7(tCS **regs)
{
    *regs = NULL;
    spin_unlock(&reg_bank_lock);
}

tCS *mver_reg_get_coresched_bank_irq7(void)
{
    return (tCS *)mve_rsrc_data7.regs;
}

uint32_t mver_reg_get_fuse7(void)
{
    return mve_rsrc_data7.fuse;
}

uint32_t mver_reg_get_version7(void)
{
    return mve_rsrc_data7.hw_version;
}

#ifdef UNIT

void mver_driver_set_hw_interaction7(bool enable)
{
    mve_rsrc_data7.hw_interaction = enable;
}
EXPORT_SYMBOL(mver_driver_set_hw_interaction7);

#endif

EXPORT_SYMBOL(mver_reg_write327);
EXPORT_SYMBOL(mver_reg_read327);
EXPORT_SYMBOL(mver_reg_get_coresched_bank7);
EXPORT_SYMBOL(mver_reg_put_coresched_bank7);
EXPORT_SYMBOL(mver_reg_get_fuse7);
EXPORT_SYMBOL(mver_reg_get_version7);
