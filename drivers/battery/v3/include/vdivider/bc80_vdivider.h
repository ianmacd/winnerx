/*
 * bc80_vdivider.h
 * Samsung BC80 Voltage Regulator Header
 *
 * Copyright (C) 2019 Samsung Electronics, Inc.
 *
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#ifndef __BC80_VDIVIDER_H
#define __BC80_VDIVIDER_H __FILE__

#include "../sec_charging_common.h"

#define BITS(_end, _start) ((BIT(_end) - BIT(_start)) + BIT(_end))
#define MASK2SHIFT(_mask)	__ffs(_mask)
#define MIN(a, b)	   ((a < b) ? (a):(b))

#define BC80_DRIVER_NAME	"bc80-vdivider"
#define BC80_HW_REV_A1	//HW rev.

/*********************************************************************************
 * register map
 *********************************************************************************/

// masks
#define BC80_MASK_INFET_EN		0x01
#define BC80_MASK_SC_OPERATION_MODE	0x03

#ifdef BC80_HW_REV_A1
   #define BC80_MASK_BC_SYS		0x0F
#elif  BC80_HW_REV_B0
   #define BC80_MASK_BC_SYS		0x1F
#endif

// register addresses
#define BC80_REG_DEVICE_ID		0x00
#define BC80_REG_INT_DEVICE_0		0x01
#define BC80_REG_INT_DEVICE_1		0x02
#define BC80_REG_INT_HV_SC_0		0x03
#define BC80_REG_INT_HV_SC_1		0x04
#define BC80_REG_INT_DEVICE_0_MSK	0x05
#define BC80_REG_INT_DEVICE_1_MSK	0x06
#define BC80_REG_INT_HV_SC_0_MSK	0x07
#define BC80_REG_INT_HV_SC_1_MSK	0x08
#define BC80_REG_INT_DEVICE_0_STS	0x09
#define BC80_REG_INT_DEVICE_1_STS	0x0A
#define BC80_REG_INT_HV_SC_0_STS	0x0B
#define BC80_REG_INT_HV_SC_1_STS	0x0C
#define BC80_REG_DEVICE_CTRL_0	0x0D
#define BC80_REG_DEVICE_CTRL_1	0x0E
#define BC80_REG_HV_SC_CTRL_0		0x0F
#define BC80_REG_HV_SC_CTRL_1		0x10
#define BC80_REG_HV_SC_CTRL_2		0x11
#define BC80_REG_SC_DITHER_CTRL	0x12
#define BC80_REG_GLITCH_CTRL		0x13
#define BC80_REG_FAULT_CTRL		0x14
#define BC80_REG_TRACK_CTRL		0x15

#define BC80_REG_STS_D		0x3A
#define BC80_REG_DEVICE_MARKER	0x46
#define BC80_MAX_REGISTER		BC80_REG_DEVICE_MARKER

/*********************************************************************************
 * data structures / platform data
 *********************************************************************************/

// (high-level) operation mode
enum {
    BC80_OPMODE_UNKNOWN = -1,
    BC80_OPMODE_STANDBY = 0,
    BC80_OPMODE_BYPASS  = 1,
    BC80_OPMODE_SWITCHING = 2,
    BC80_OPMODE_SWITCHING_ALT = 3,
};

// chip (internal) system state
enum {
    BC80_STATE_UNKNOWN = -1,
    BC80_STATE_IDLE = 2,
    BC80_STATE_SW_ACTIVE  = 7,
    BC80_STATE_BYPASS_ACTIVE  = 12,
};

//
enum {
	REG_INT_DEVICE_0,
	REG_INT_DEVICE_1,
	REG_INT_HV_SC_0,
	REG_INT_HV_SC_1,
	REG_INT_MAX
};

enum {	
	REG_DEVICE_0_STS,
	REG_DEVICE_1_STS,
	REG_HV_SC_0_STS,
	REG_HV_SC_1_STS,
	REG_STS_MAX
};

/* BC80_REG_DEVICE_CTRL_0 0x0D */
#define BC80_BIT_INFET_EN			BIT(0)

/* BC80_REG_HV_SC_CTRL_0 0x0F */
#define BC80_BITS_FSW_CFG				BITS(7,3)
#define BC80_BIT_STANDBY_EN				BIT(2)
#define BC80_BITS_SC_OPERATION_MODE		BITS(1,0)

struct bc80_vdivider_platform_data {
	int irq_gpio;
	int en_gpio;
};

#define bc80_vdivider_platform_data_t \
	struct bc80_vdivider_platform_data

/**
 * struct bc80_info - bc80 regulator instance
 * @monitor_wake_lock: lock to enter the suspend mode
 * @lock: protects concurrent access to online variables
 * @client: pointer to client
 * @regmap: pointer to driver regmap
 * @op_mode : chip operation mode (STANDBY, BYPASS, SWITCHING)
 * @reverse_power : enable reverse power path
 * @pdata: pointer to platform data
 */
struct bc80_info {
	struct i2c_client       *client;
	struct device           *dev;
	struct mutex		lock;
	struct regmap		*regmap;

	int irq;

	struct power_supply *psy_vdivider;
	bc80_vdivider_platform_data_t 	*pdata;

	/* battery info */
	int cable_type;

	unsigned int		op_mode;
	bool			reverse_power;
	bool			auto_recovery;

};

#endif /* __BC80_VDIVIDER_H */
