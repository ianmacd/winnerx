/* Copyright (c) 2012-2015, 2017-2018, The Linux Foundation.
 * All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef QPNP_PON_H
#define QPNP_PON_H

#include <dt-bindings/input/qcom,qpnp-power-on.h>
#include <linux/errno.h>

/**
 * enum pon_trigger_source: List of PON trigger sources
 * %PON_SMPL:		PON triggered by Sudden Momentary Power Loss (SMPL)
 * %PON_RTC:		PON triggered by real-time clock (RTC) alarm
 * %PON_DC_CHG:		PON triggered by insertion of DC charger
 * %PON_USB_CHG:	PON triggered by insertion of USB
 * %PON_PON1:		PON triggered by other PMIC (multi-PMIC option)
 * %PON_CBLPWR_N:	PON triggered by power-cable insertion
 * %PON_KPDPWR_N:	PON triggered by long press of the power-key
 */
enum pon_trigger_source {
	PON_SMPL = 1,
	PON_RTC,
	PON_DC_CHG,
	PON_USB_CHG,
	PON_PON1,
	PON_CBLPWR_N,
	PON_KPDPWR_N,
};

/**
 * enum pon_power_off_type: Possible power off actions to perform
 * %PON_POWER_OFF_RESERVED:          Reserved, not used
 * %PON_POWER_OFF_WARM_RESET:        Reset the MSM but not all PMIC peripherals
 * %PON_POWER_OFF_SHUTDOWN:          Shutdown the MSM and PMIC completely
 * %PON_POWER_OFF_HARD_RESET:        Reset the MSM and all PMIC peripherals
 * %PON_POWER_OFF_MAX_TYPE:          Reserved, not used
 */
enum pon_power_off_type {
	PON_POWER_OFF_RESERVED		= 0x00,
	PON_POWER_OFF_WARM_RESET	= PON_POWER_OFF_TYPE_WARM_RESET,
	PON_POWER_OFF_SHUTDOWN		= PON_POWER_OFF_TYPE_SHUTDOWN,
	PON_POWER_OFF_HARD_RESET	= PON_POWER_OFF_TYPE_HARD_RESET,
	PON_POWER_OFF_MAX_TYPE		= 0x10,
};

enum pon_restart_reason {
	PON_RESTART_REASON_UNKNOWN		= 0x00,
	PON_RESTART_REASON_RECOVERY		= 0x01,
	PON_RESTART_REASON_BOOTLOADER		= 0x02,
	PON_RESTART_REASON_RTC			= 0x03,
	PON_RESTART_REASON_DMVERITY_CORRUPTED	= 0x04,
	PON_RESTART_REASON_DMVERITY_ENFORCE	= 0x05,
	PON_RESTART_REASON_KEYS_CLEAR		= 0x06,

	/* 32 ~ 63 for OEMs/ODMs secific features */
	PON_RESTART_REASON_OEM_MIN		= 0x20,
	PON_RESTART_REASON_OEM_MAX		= 0x3f,
/* CONFIG_SEC_BSP */

	PON_RESTART_REASON_CP_CRASH		= 0x12,
	PON_RESTART_REASON_MANUAL_RESET = 0x13,
	PON_RESTART_REASON_NORMALBOOT		= 0x14,
	PON_RESTART_REASON_DOWNLOAD		= 0x15,
	PON_RESTART_REASON_NVBACKUP		= 0x16,
	PON_RESTART_REASON_NVRESTORE		= 0x17,
	PON_RESTART_REASON_NVERASE		= 0x18,
	PON_RESTART_REASON_NVRECOVERY		= 0x19,
#ifdef CONFIG_SEC_PERIPHERAL_SECURE_CHK
	PON_RESTART_REASON_SECURE_CHECK_FAIL	= 0x1A,
#endif
	PON_RESTART_REASON_WATCH_DOG		= 0x1B,
	PON_RESTART_REASON_KERNEL_PANIC		= 0x1C,
	PON_RESTART_REASON_THERMAL		= 0x1D,
	PON_RESTART_REASON_POWER_RESET		= 0x1E,
	PON_RESTART_REASON_WTSR			= 0x1F,
/***********************************************/
	PON_RESTART_REASON_RORY_START		= 0x20,
   /* here is reserved for rory download. */
   /* don't use betwwen PON_RESTART_REASON_RORY_START */
   /*   & PON_RESTART_REASON_RORY_END */
	PON_RESTART_REASON_RORY_END		= 0x2A,
/***********************************************/
	PON_RESTART_REASON_CROSS_FAIL = 0x2C,
	PON_RESTART_REASON_SLT_COMPLETE = 0x2F,
	PON_RESTART_REASON_DBG_LOW		= 0x30,
	PON_RESTART_REASON_DBG_MID		= 0x31,
	PON_RESTART_REASON_DBG_HIGH		= 0x32,
	PON_RESTART_REASON_CP_DBG_ON		= 0x33,
	PON_RESTART_REASON_CP_DBG_OFF		= 0x34,
	PON_RESTART_REASON_CP_MEM_RESERVE_ON	= 0x35,
	PON_RESTART_REASON_CP_MEM_RESERVE_OFF	= 0x36,
	PON_RESTART_REASON_FIRMWAREUPDATE	= 0x37,
#ifdef CONFIG_MUIC_SUPPORT_RUSTPROOF
/* SWITCHSEL_MODE use 0x38 ~ 0x3D */
	PON_RESTART_REASON_SWITCHSEL		= 0x38, /* SET_SWITCHSEL_MODE */
	PON_RESTART_REASON_RUSTPROOF		= 0x3D, /* SET_RUSTPROOF_MODE */
#endif
	PON_RESTART_REASON_MBS_MEM_RESERVE_ON	= 0x3E,
	PON_RESTART_REASON_MBS_MEM_RESERVE_OFF	= 0x3F,
#if defined(CONFIG_SEC_ABC)
	PON_RESTART_REASON_USER_DRAM_TEST	= 0x40,
#endif
#if defined(CONFIG_SEC_QUEST)
	PON_RESTART_REASON_QUEST_UEFI_START = 0x41,
#endif
	PON_RESTART_REASON_FORCE_UPLOAD_ON	= 0x48,
	PON_RESTART_REASON_FORCE_UPLOAD_OFF	= 0x49,
	PON_RESTART_REASON_DUMP_SINK_BOOTDEV = 0x4D,
	PON_RESTART_REASON_DUMP_SINK_SDCARD = 0x4E,
	PON_RESTART_REASON_DUMP_SINK_USB = 0x4F,
#if defined(CONFIG_SEC_QUEST_UEFI_ENHENCEMENT)
	PON_RESTART_REASON_QUEST_NMCHECKER_START = 0x50,
#endif
	PON_RESTART_REASON_MAX			= 0x80
};

#ifdef CONFIG_SEC_PM
int qpnp_pon_check_chg_det(void);
ssize_t sec_get_pwrsrc(char *buf);
int qpnp_control_s2_reset_onoff(int on);
int qpnp_get_s2_reset_onoff(void);
char* qpnp_pon_get_off_reason(void);
#endif

#ifdef CONFIG_INPUT_QPNP_POWER_ON
int qpnp_pon_system_pwr_off(enum pon_power_off_type type);
int qpnp_pon_is_warm_reset(void);
int qpnp_pon_trigger_config(enum pon_trigger_source pon_src, bool enable);
int qpnp_pon_wd_config(bool enable);
int qpnp_pon_set_restart_reason(enum pon_restart_reason reason);
bool qpnp_pon_check_hard_reset_stored(void);

#ifdef CONFIG_SEC_PM
int qpnp_set_resin_wk_int(int en);
#endif /* CONFIG_SEC_PM */

#else
static int qpnp_pon_system_pwr_off(enum pon_power_off_type type)
{
	return -ENODEV;
}
static inline int qpnp_pon_is_warm_reset(void)
{
	return -ENODEV;
}
static inline int qpnp_pon_trigger_config(enum pon_trigger_source pon_src,
							bool enable)
{
	return -ENODEV;
}
int qpnp_pon_wd_config(bool enable)
{
	return -ENODEV;
}
static inline int qpnp_pon_set_restart_reason(enum pon_restart_reason reason)
{
	return -ENODEV;
}
static inline bool qpnp_pon_check_hard_reset_stored(void)
{
	return false;
}
#endif

#endif
