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

#ifdef EMULATOR
#include "emulator_userspace.h"
#else
#include "linux/export.h"
#include "linux/pm_runtime.h"
#include "linux/slab.h"
#include "linux/device.h"
#include <linux/stat.h>
#endif

#include "mve_rsrc_driver.h"
#include "mve_rsrc_pm.h"
#include "mve_rsrc_scheduler.h"
#include "mve_rsrc_log.h"
#include "mve_rsrc_irq.h"

#include "machine/mve_power_management.h"
#include "machine/mve_dvfs.h"
#include "machine/mve_config.h"

#ifdef EMULATOR
static uint32_t vpu_freq = 20;
#endif

void mver_pm_request_frequency8(uint32_t freq)
{
#ifdef EMULATOR
    vpu_freq = freq;
#else
    struct mve_dvfs_callback_conf *dvfs_conf;
    dvfs_conf = (struct mve_dvfs_callback_conf *)mve_config_get_value8(mve_rsrc_data8.config,
                                                                      MVE_CONFIG_DEVICE_ATTR_DVFS_CALLBACKS);
    if (NULL == dvfs_conf || NULL == dvfs_conf->set_rate)
    {
        MVE_LOG_PRINT(&mve_rsrc_log8, MVE_LOG_ERROR, "Unable to change the frequency.");
    }
    else
    {
        dvfs_conf->set_rate(freq);
    }
#endif
}

uint32_t mver_pm_read_frequency8(void)
{
#ifdef EMULATOR
    return vpu_freq;
#else
    uint32_t freq = 0;
    struct mve_dvfs_callback_conf *dvfs_conf;

    dvfs_conf = (struct mve_dvfs_callback_conf *)mve_config_get_value8(mve_rsrc_data8.config,
                                                                      MVE_CONFIG_DEVICE_ATTR_DVFS_CALLBACKS);
    if (NULL == dvfs_conf || NULL == dvfs_conf->get_rate)
    {
        MVE_LOG_PRINT(&mve_rsrc_log8, MVE_LOG_ERROR, "Unable to read the frequency.");
        freq = 0;
    }
    else
    {
        freq = dvfs_conf->get_rate();
    }

    return freq;
#endif
}

uint32_t mver_pm_read_max_frequency8(void)
{
#ifdef EMULATOR
    return 100;
#else
    uint32_t freq = 0;
    struct mve_dvfs_callback_conf *dvfs_conf;

    dvfs_conf = (struct mve_dvfs_callback_conf *)mve_config_get_value8(mve_rsrc_data8.config,
                                                                      MVE_CONFIG_DEVICE_ATTR_DVFS_CALLBACKS);
    if (NULL == dvfs_conf || NULL == dvfs_conf->get_max_rate)
    {
        printk(KERN_ERR "[MVE] Unable to read max frequency\n");
        freq = 0;
    }
    else
    {
        freq = dvfs_conf->get_max_rate();
    }

    return freq;
#endif
}

#if defined(CONFIG_SYSFS) && defined(_DEBUG)

static ssize_t sysfs_read_vpu_freq(struct device *dev,
                                   struct device_attribute *attr,
                                   char *buf)
{
    uint32_t freq;

    freq = mver_pm_read_frequency8();
    return snprintf(buf, PAGE_SIZE, "%d\n", (int)freq);
}

static ssize_t sysfs_write_vpu_freq(struct device *dev,
                                    struct device_attribute *attr,
                                    const char *buf,
                                    size_t count)
{
    int freq;

    freq = simple_strtol(buf, NULL, 10);
    if (0 != freq)
    {
        mver_pm_request_frequency8(freq);
    }

    return count;
}

static struct device_attribute sysfs_files[] =
{
    __ATTR(vpu_freq, S_IRUGO, sysfs_read_vpu_freq, sysfs_write_vpu_freq)
};

#endif /* CONFIG_SYSFS && _DEBUG */

void mver_pm_init8(struct device *dev)
{
#if defined(CONFIG_SYSFS) && defined(_DEBUG)
    device_create_file(dev, &sysfs_files[0]);
#endif
}

void mver_pm_deinit8(struct device *dev)
{
#ifndef EMULATOR
    struct mve_dvfs_callback_conf *dvfs_conf;
    dvfs_conf = (struct mve_dvfs_callback_conf *)mve_config_get_value8(mve_rsrc_data8.config,
                                                                      MVE_CONFIG_DEVICE_ATTR_DVFS_CALLBACKS);
    if (NULL == dvfs_conf || NULL == dvfs_conf->stop)
    {
        MVE_LOG_PRINT(&mve_rsrc_log8, MVE_LOG_ERROR, "no dvfs_conf or stop.");
    }
    else
    {
        dvfs_conf->stop();
    }
#endif
#if defined(CONFIG_SYSFS) && defined(_DEBUG)
    device_remove_file(dev, &sysfs_files[0]);
#endif
}

int mver_pm_request_suspend8(void)
{
    mver_irq_disable8();
    return pm_runtime_put_sync(&mve_rsrc_data8.pdev->dev);
}

int mver_pm_request_resume8(void)
{
    int ret, val;

    ret = pm_runtime_get_sync(&mve_rsrc_data8.pdev->dev);
    val = mver_irq_enable8();
    if (0 != val)
    {
        MVE_LOG_PRINT(&mve_rsrc_log8, MVE_LOG_ERROR, "Unable to register interrupt handler: \'%d\'", val);
    }

    return ret;
}

EXPORT_SYMBOL(mver_pm_request_frequency8);
EXPORT_SYMBOL(mver_pm_read_frequency8);
EXPORT_SYMBOL(mver_pm_request_suspend8);
EXPORT_SYMBOL(mver_pm_request_resume8);

int mver_pm_poweron8(void)
{
#ifdef EMULATOR
#else
    struct mve_pm_callback_conf *pm_conf;
    struct mve_dvfs_callback_conf *dvfs_conf;

    /* Enable clock and power to the hardware */
    pm_conf = (struct mve_pm_callback_conf *)mve_config_get_value8(mve_rsrc_data8.config,
                                                                  MVE_CONFIG_DEVICE_ATTR_POWER_CALLBACKS);
    dvfs_conf = (struct mve_dvfs_callback_conf *)mve_config_get_value8(mve_rsrc_data8.config,
                                                                      MVE_CONFIG_DEVICE_ATTR_DVFS_CALLBACKS);
    if (NULL == pm_conf || NULL == dvfs_conf)
    {
        return -ENXIO;
    }

    pm_conf->power_on_callback();
    dvfs_conf->enable_clock();

    /* Resume suspended sessions */
    mver_scheduler_resume8();
#endif
    return 0;
}

int mver_pm_poweroff8(void)
{
#ifdef EMULATOR
#else
    struct mve_pm_callback_conf *pm_conf;
    struct mve_dvfs_callback_conf *dvfs_conf;

    /* Suspend all running sessions */
    mver_scheduler_suspend8();

    /* Shut off clock and power to the hardware */
    pm_conf = (struct mve_pm_callback_conf *)mve_config_get_value8(mve_rsrc_data8.config,
                                                                  MVE_CONFIG_DEVICE_ATTR_POWER_CALLBACKS);
    dvfs_conf = (struct mve_dvfs_callback_conf *)mve_config_get_value8(mve_rsrc_data8.config,
                                                                      MVE_CONFIG_DEVICE_ATTR_DVFS_CALLBACKS);
    if (NULL == pm_conf || NULL == dvfs_conf)
    {
        return -ENXIO;
    }

    dvfs_conf->disable_clock();
    pm_conf->power_off_callback();
#endif
    return 0;
}

#ifdef CONFIG_PM
int mver_pm_suspend8(struct device *dev)
{
    mver_pm_poweroff8();

    return 0;
}

int mver_pm_resume8(struct device *dev)
{
    mver_pm_poweron8();

    return 0;
}

int mver_pm_runtime_suspend8(struct device *dev)
{
    return mver_pm_suspend8(dev);
}

int mver_pm_runtime_resume8(struct device *dev)
{
    return mver_pm_resume8(dev);
}

int mver_pm_runtime_idle8(struct device *dev)
{
    pm_runtime_suspend(dev);
    return 1;
}
#endif
