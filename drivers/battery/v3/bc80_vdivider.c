 /*
 * bc80_vdivider.c
 * Samsung BC80 Voltage Divider Driver
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
//#define DEBUG

#include <linux/err.h>
#include <linux/kernel.h>
#include <linux/version.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/mutex.h>
#include <linux/delay.h>
#include <linux/of_irq.h>
#include <linux/of_device.h>
#include <linux/pm_runtime.h>
#include <linux/power_supply.h>
#include <linux/gpio.h>
#include <linux/i2c.h>
#include <linux/regmap.h>
#include <linux/sysfs.h>
#include <linux/types.h>
#if defined (CONFIG_OF)
#include <linux/of_device.h>
#include <linux/of_gpio.h>
#endif /* CONFIG_OF */
#include "include/vdivider/bc80_vdivider.h"

static enum power_supply_property bc80_vdivider_props[] = {
};

static const struct regmap_config bc80_regmap_config = {
	.reg_bits	= 8,
	.val_bits	= 8,
	.max_register	= BC80_MAX_REGISTER,
};

static int bc80_read_reg(struct bc80_info *info, u8 addr, u8 *value)
{
	int ret = -EIO;
	unsigned int reg, val;

	reg = addr;
	ret = regmap_read(info->regmap, reg, &val);
	if (ret) {
		pr_debug("%s: error(%d), reg(0x%02x)\n", __func__, ret, reg);
		*value = 0xFF;
		return ret;
	}

	*value = (val & 0xFF);
	return ret;
}

#if 0 //temporary build
static int bc80_write_reg(struct bc80_info *info, u8 addr, u8 value)
{
	int ret = -EIO;
	unsigned int reg, val;
	
	reg = addr;
	val = value;
	ret = regmap_write(info->regmap, reg, val);
	if (ret) {
		pr_debug("%s: error(%d), reg(0x%02x), val(0x%02x)\n",
			__func__, ret, reg, val);
		return ret;
	}
	return ret;
}
#endif

static int bc80_update_reg(struct bc80_info *info, u8 addr, u8 value, u8 bits)
{
	int ret = -EIO;
	unsigned int reg, mask, val;

	reg = addr;
	mask = (bits & 0xFF);
	val = (value & 0xFF);
	ret = regmap_update_bits(info->regmap, reg, mask, val);
	if (ret) {
		pr_debug("%s: error(%d), reg(0x%02x), mask(0x%02x), val(0x%02x)\n",
			__func__, ret, reg, mask, val);
		return ret;
	}
	return ret;
}

static int bc80_bulk_read(struct bc80_info *info, u8 addr, u8 *val, size_t val_count)
{
	int ret = -EIO;
	unsigned int reg;

	reg = addr;
	ret = regmap_bulk_read(info->regmap, addr, val, val_count);
	if (reg) {
		pr_debug("%s: error(%d), reg(0x%02x), count(%d)\n",
			__func__, ret, reg, val_count);
		return ret;
	}

	return ret;
}

static int bc80_bulk_write(struct bc80_info *info, u8 addr, u8 *val, size_t val_count)
{
	int ret = -EIO;
	unsigned int reg;

	reg = addr;
	ret = regmap_bulk_write(info->regmap, addr, val, val_count);
	if (reg) {
		pr_debug("%s: error(%d), reg(0x%02x), count(%d)\n",
			__func__, ret, reg, val_count);
		return ret;
	}

	return ret;
}

static void bc80_test_read(struct bc80_info *info)
{
	u8 addr, val;
	char str[256] = { 0, };

	for (addr = BC80_REG_DEVICE_ID; addr <= BC80_REG_TRACK_CTRL; addr++) {
		if (BC80_REG_INT_DEVICE_0 <= addr && addr <= BC80_REG_INT_HV_SC_1)
			continue;
		bc80_read_reg(info, addr, &val);
		sprintf(str + strlen(str), "[0x%02X]0x%02X, ", addr, val);
	}

	addr = BC80_REG_STS_D;
	bc80_read_reg(info, addr, &val);
	sprintf(str + strlen(str), "[0x%02X]0x%02X, ", addr, val);

	if (info->pdata->en_gpio)
		pr_info("%s: [EN:%d] %s\n", __func__, gpio_get_value(info->pdata->en_gpio), str);
	else
		pr_info("%s: [EN:-] %s\n", __func__, str);
}

static void bc80_set_int_mask(struct bc80_info *info, bool msk)
{
	u8 int_msk[REG_INT_MAX];

	if (msk)
		memset(int_msk, 0xFF, sizeof(int_msk)); 
	else
		memset(int_msk, 0x00, sizeof(int_msk));

	bc80_bulk_write(info, BC80_REG_INT_DEVICE_0_MSK, int_msk, REG_INT_MAX);	
}

/*********************************************************************************
 * supported functionality
 *********************************************************************************/


static inline void bc80_set_infet(struct bc80_info *info, unsigned int enable)
{
#if 1
	u8 val;
	int ret;

	val = enable << MASK2SHIFT(BC80_BIT_INFET_EN);
	pr_info("%s: %s\n", __func__, enable ? "ON" : "OFF");
	ret = bc80_update_reg(info, BC80_REG_DEVICE_CTRL_0, val, BC80_BIT_INFET_EN);
#else
	regmap_update_bits(info->regmap, BC80_REG_DEVICE_CTRL_0,
				BC80_MASK_INFET_EN, 
				(enable & 0x01));
#endif
}

static inline void bc80_set_powerpath(struct bc80_info *info, bool forward_path) {
	info->reverse_power = (!forward_path);
}

static inline int bc80_get_opmode(struct bc80_info *info)
{
	int val;
	if (regmap_read(info->regmap, BC80_REG_HV_SC_CTRL_0, &val) < 0)
	   val = BC80_OPMODE_UNKNOWN;
	return (val & BC80_MASK_SC_OPERATION_MODE);
}



/* configure minimum set of control registers */
static inline void bc80_set_base_opt(struct bc80_info *info,
			unsigned int sc_out_precharge_cfg,
			unsigned int precharge_fault_chk_en,
			unsigned int track_cfg)
{
	regmap_update_bits(info->regmap, BC80_REG_HV_SC_CTRL_1, 0x20, 
				(sc_out_precharge_cfg<<5));
	regmap_update_bits(info->regmap, BC80_REG_TRACK_CTRL,
				0x30,
				(precharge_fault_chk_en<<5) |
				(track_cfg<<4));
}

static inline void bc80_set_fault_opt(struct bc80_info *info,
			bool set_ov,
			unsigned int disable_vbus_in_switch_ok,
			unsigned int disable_sc_out_min_ok,
			unsigned int disable_wpc_in_min_ok,
			unsigned int disable_vbus_in_min_ok)
{
	if (set_ov) {
	   regmap_update_bits(info->regmap, BC80_REG_FAULT_CTRL, 0x3F,
				(3<<4) |
				(disable_vbus_in_switch_ok<<3) |
				(disable_sc_out_min_ok<<2) |
				(disable_wpc_in_min_ok<<1) |
				(disable_vbus_in_min_ok));
	} else {
	   regmap_update_bits(info->regmap, BC80_REG_FAULT_CTRL, 0x3F,
				(disable_vbus_in_switch_ok<<3) |
				(disable_sc_out_min_ok<<2) |
				(disable_wpc_in_min_ok<<1) |
				(disable_vbus_in_min_ok));
	}
}


static void bc80_enter_standby(struct bc80_info *info)
{
	//update opmode
	regmap_update_bits(info->regmap, BC80_REG_HV_SC_CTRL_0, 
				BC80_MASK_SC_OPERATION_MODE,
				BC80_OPMODE_STANDBY);

	bc80_set_base_opt(info, 1, 1, 1);
	bc80_set_fault_opt(info, false, 0, 1, 0, 0);
}


static void bc80_enter_bypass(struct bc80_info *info)
{
	bc80_set_base_opt(info, 1, 1, 1);

	if (info->reverse_power)
	   bc80_set_fault_opt(info, false, 1, 0, 1, 1);
	else
	   bc80_set_fault_opt(info, false, 1, 1, 0, 0);

	//update opmode
	regmap_update_bits(info->regmap, BC80_REG_HV_SC_CTRL_0, 
			BC80_MASK_SC_OPERATION_MODE, BC80_OPMODE_BYPASS);
}

static void bc80_enter_switching(struct bc80_info *info)
{
	if (info->reverse_power) {
	   bc80_set_base_opt(info, 0, 0, 0);
	   bc80_set_fault_opt(info, true, 1, 1, 0, 1);//temporarily set
	} else {
#ifdef BC80_HW_REV_A1
	   bc80_set_base_opt(info, 0, 0, 0);
#else
	   bc80_set_base_opt(info, 0, 1, 0);
#endif
	   bc80_set_fault_opt(info, true, 1, 1, 0, 0);//temporarily set
	}

	//update opmode
	regmap_update_bits(info->regmap, BC80_REG_HV_SC_CTRL_0, 
			BC80_MASK_SC_OPERATION_MODE, BC80_OPMODE_SWITCHING);

	if (info->reverse_power)
	   bc80_set_fault_opt(info, false, 1, 0, 0, 1);
	else
	   bc80_set_fault_opt(info, false, 0, 1, 0, 0);
}


/* main function for setting/changing operation mode */
static bool bc80_change_opmode(struct bc80_info *info, unsigned int target_mode)
{
	bool ret;
//	bool reverse;

	info->op_mode = bc80_get_opmode(info);

	if (target_mode < 0 || target_mode > BC80_OPMODE_SWITCHING_ALT) {
	   pr_err("%s: target operation mode (0x%02X) is invalid\n", 
		  __func__, target_mode);
	   return false;
	}

	pr_info("%s: current_mode(0x%02X)->target_mode(0x%02X)\n",
		__func__, info->op_mode, target_mode);

	/* NOTE: 
	 *      CUSTOMER should know/indicate if power path is forward/reverse mode
	 *      based on power connections before attempting to change operation mode
         */
	//info->reverse_power = false;

	ret = true;
	switch(target_mode) {
	  case BC80_OPMODE_STANDBY:
		bc80_enter_standby(info);
		break;
	  case BC80_OPMODE_BYPASS:
		bc80_enter_bypass(info);
		break;
	  case BC80_OPMODE_SWITCHING:
	  case BC80_OPMODE_SWITCHING_ALT:
		bc80_enter_switching(info);
		break;
	  default:
		ret = false;
	}
	return ret;
}

static int bc80_vdivider_get_property(struct power_supply *psy,
					enum power_supply_property psp,
					union power_supply_propval *val)
{
	struct bc80_info *info = power_supply_get_drvdata(psy);
	enum power_supply_ext_property ext_psp = (enum power_supply_ext_property) psp;

	switch (psp) {
	case POWER_SUPPLY_PROP_ONLINE:
		val->intval = info->cable_type;
		break;
	case POWER_SUPPLY_PROP_MAX ... POWER_SUPPLY_EXT_PROP_MAX:
		switch (ext_psp) {
		default:
			return -EINVAL;
		}
		break;
	default:
		return -EINVAL;
	}
	return 0;
}

static int bc80_vdivider_set_property(struct power_supply *psy,
					enum power_supply_property psp,
					const union power_supply_propval *val)
{
	struct bc80_info *info = power_supply_get_drvdata(psy);
	enum power_supply_ext_property ext_psp = (enum power_supply_ext_property) psp;

	switch (psp) {
	case POWER_SUPPLY_PROP_ONLINE:
		if (!is_nocharge_type(val->intval) &&
			info->cable_type != val->intval)
			bc80_set_int_mask(info, false);

		info->cable_type = val->intval;

		if (is_wired_type(info->cable_type)) {			
			bc80_change_opmode(info, BC80_OPMODE_BYPASS);
			bc80_set_infet(info, false);
		} else if (is_wireless_type(info->cable_type)) {
			bc80_set_infet(info, true);
			//bc80_change_opmode(info, BC80_OPMODE_STANDBY);
		} else if (is_pd_apdo_wire_type(info->cable_type)) {
			//bc80_change_opmode(info, BC80_OPMODE_SWITCHING);
			//bc80_set_infet(info, false);
		} else {
			// skip
		}

		if (!is_nocharge_type(info->cable_type))
			bc80_test_read(info);
		break;
	case POWER_SUPPLY_PROP_CHARGE_TYPE:
		bc80_change_opmode(info, val->intval);
		if (is_wired_type(val->intval))
			bc80_set_infet(info, false);
		break;
	case POWER_SUPPLY_PROP_MAX ... POWER_SUPPLY_EXT_PROP_MAX:
		switch (ext_psp) {
		case POWER_SUPPLY_EXT_PROP_WIRELESS_TX_ENABLE:
			bc80_set_infet(info, val->intval);
			break;
		default:
			return -EINVAL;
		}
		break;
	default:
		return -EINVAL;
	}
	return 0;
}

static irqreturn_t bc80_interrupt_handler(int irq, void *data)
{
	struct bc80_info *info = data;
	u8 irq_reg[REG_INT_MAX], sts_reg[REG_STS_MAX];

	bc80_set_int_mask(info, true);

	bc80_bulk_read(info, BC80_REG_INT_DEVICE_0, irq_reg, REG_INT_MAX);
	bc80_bulk_read(info, BC80_REG_INT_DEVICE_0_STS, sts_reg, REG_STS_MAX);

	pr_info("%s: irq(0x%02X,0x%02X,0x%02X,0x%02X), sts(0x%02X,0x%02X,0x%02X,0x%02X)\n",
		__func__,
		irq_reg[0], irq_reg[1], irq_reg[2], irq_reg[3],
		sts_reg[0], sts_reg[1], sts_reg[2], sts_reg[3]);

	bc80_set_int_mask(info, false);

	return IRQ_HANDLED;
}

/*********************************************************************************
 * device layer
 *********************************************************************************/
static int bc80_irq_init(struct bc80_info *info)
{
	const bc80_vdivider_platform_data_t *pdata = info->pdata;
	int ret;

	info->irq = gpio_to_irq(pdata->irq_gpio);	
	pr_info("%s: irq=%d, irq_gpio=%d\n", __func__,
			info->irq, pdata->irq_gpio);
	ret = gpio_request_one(pdata->irq_gpio, GPIOF_IN, "bc80-irq");
	if (ret < 0)
		goto fail;

	ret = request_threaded_irq(info->irq, NULL, bc80_interrupt_handler,
				   IRQF_TRIGGER_LOW | IRQF_ONESHOT,
				   "bc80-irq", info);
	if (ret < 0)
		goto fail_gpio;

	return 0;

fail_gpio:
	gpio_free(pdata->irq_gpio);
fail:
	info->irq = 0;
	return ret;
}

static int bc80_hw_init(struct bc80_info *info)
{
	/* CUSTOMER should add basic initialization tasks
	 * (unmasking interrupts, changing control settings, etc)
	 */
	pr_info("%s: HW initialization\n", __func__);

	return 0;
}

#ifdef CONFIG_OF
static int bc80_vdivider_parse_dt(struct device *dev, bc80_vdivider_platform_data_t *pdata)
{
	struct device_node *np_bc80  = dev->of_node;
	int ret = 0;

	/* nINT gpio */
	ret = pdata->irq_gpio = of_get_named_gpio(np_bc80, "bc80,irq-gpio", 0);
	if (ret < 0)
		pr_info("%s: can't get irq-gpio (%d)\n", __func__, pdata->irq_gpio);

	/* nEN gpio */
	ret = pdata->en_gpio = of_get_named_gpio(np_bc80, "bc80,en-gpio", 0);
	if (ret < 0)
		pr_info("%s: can't get en-gpio (%d)\n", __func__, pdata->en_gpio);

	pr_info("%s: irq-gpio: %u, en-gpio: %u\n",
		__func__, pdata->irq_gpio, pdata->en_gpio);

	return 0;
}
#endif

static const struct power_supply_desc bc80_vdivider_power_supply_desc = {
	.name = BC80_DRIVER_NAME,
	.type = POWER_SUPPLY_TYPE_UNKNOWN,
	.properties = bc80_vdivider_props,
	.num_properties = ARRAY_SIZE(bc80_vdivider_props),
	.get_property = bc80_vdivider_get_property,
	.set_property = bc80_vdivider_set_property,
	.no_thermal = true,
};

static int bc80_vdivider_probe(struct i2c_client *client,
			const struct i2c_device_id *id)
{ 
	struct bc80_info *info;
	bc80_vdivider_platform_data_t *pdata = client->dev.platform_data;
	struct device *dev = &client->dev;
	struct power_supply_config vdivider_cfg = {};
	int ret = 0;

	pr_info("%s: Driver Loading\n", __func__);

	info = devm_kzalloc(dev, sizeof(*info), GFP_KERNEL);
	if (!info)
		return -ENOMEM;

#if defined(CONFIG_OF)
	if (client->dev.of_node) {
		pdata = devm_kzalloc(&client->dev, sizeof(bc80_vdivider_platform_data_t),
				GFP_KERNEL);
		if (!pdata) {
			dev_err(&client->dev, "Failed to allocate memory \n");
			ret = -ENOMEM;
			goto err_pdata_nomem;
		}

		ret = bc80_vdivider_parse_dt(&client->dev, pdata);
		if (ret < 0){
			dev_err(&client->dev, "Failed to get device of_node \n");
			ret = -ENOMEM;
			goto err_parse_dt;
		}

		client->dev.platform_data = pdata;
	} else {
		pdata = client->dev.platform_data;
	}
#else
	pdata = dev->platform_data;
#endif
	if (!pdata) {
		ret = -EINVAL;
		goto err_pdata_nomem;
	}

	info->client = client;
	info->dev = &client->dev;
	info->pdata = pdata;

	// init variables
	info->cable_type = SEC_BATTERY_CABLE_NONE;

	i2c_set_clientdata(client, info);

	info->regmap = devm_regmap_init_i2c(client, &bc80_regmap_config);
	if (IS_ERR(info->regmap)) {
		ret = PTR_ERR(info->regmap);
		goto err_regmap_init;
	}

	if (bc80_hw_init(info) < 0) {
	   pr_err("%s: hardware initialization error\n", __func__);
	   ret = -EINVAL;
	   goto err_hw_init;
	}

	// register_power_supply
	vdivider_cfg.drv_data = info;
	info->psy_vdivider = power_supply_register(dev,
		&bc80_vdivider_power_supply_desc, &vdivider_cfg);
	if (IS_ERR(info->psy_vdivider)) {
		ret = PTR_ERR(info->psy_vdivider);
		goto err_power_supply_register;
	}

	if (info->pdata->irq_gpio >= 0) {
		ret = bc80_irq_init(info);
		if (ret < 0)
			pr_warn("%s: failed to initialize IRQ(%d), disable IRQ\n",
				__func__, info->pdata->irq_gpio);
	}

	pr_info("%s: Loaded\n", __func__);

	return 0;

err_power_supply_register:
err_hw_init:
err_regmap_init:
err_parse_dt:
	devm_kfree(&client->dev, pdata);
err_pdata_nomem:
	kfree(info);

	return ret;
}

static int bc80_vdivider_remove(struct i2c_client *client)
{
	struct bc80_info *info = i2c_get_clientdata(client);

	pr_info("%s: ++\n", __func__);

	if (info->irq) {
		free_irq(info->irq, info);
		gpio_free(info->pdata->irq_gpio);
	}

	if (info->psy_vdivider)
		power_supply_unregister(info->psy_vdivider);

	pr_info("%s: --\n", __func__);

	return 0;
}

#ifdef CONFIG_PM
static int bc80_vdivider_suspend(struct device *dev)
{
#if 0
	struct i2c_client *client = to_i2c_client(dev);
	struct bc80_info *info = i2c_get_clientdata(client);

	bc80_change_opmode(info, BC80_OPMODE_BYPASS);

	pr_info("%s: cancel delayed work\n", __func__);
#endif

	return 0;
} 

static int bc80_vdivider_resume(struct device *dev)
{
#if 0
	struct i2c_client *client = to_i2c_client(dev);
	struct bc80_info *info = i2c_get_clientdata(client);

	pr_info("%s: update/resume\n", __func__);
#endif

	return 0;
}
#else
#define bc80_vdivider_suspend NULL
#define bc80_vdivider_resume NULL
#endif
static SIMPLE_DEV_PM_OPS(bc80_vdivider_pm_ops,
			bc80_vdivider_suspend, bc80_vdivider_resume);

static void bc80_vdivider_shutdown(struct i2c_client *client)
{
	pr_info("%s: ++\n", __func__);
	pr_info("%s: --\n", __func__);
}

#ifdef CONFIG_OF
static const struct of_device_id bc80_vdivider_dt_match_table[] = {
	{ .compatible = "maxim,bc80" },
	{ },
};
MODULE_DEVICE_TABLE(of, bc80_vdivider_dt_match_table);
#else
#define bc80_vdivider_dt_match_table NULL
#endif

static const struct i2c_device_id bc80_vdivider_id_table[] = {
	{ BC80_DRIVER_NAME, 0 },
	{ }
};
MODULE_DEVICE_TABLE(i2c, bc80_vdivider_id_table);

static struct i2c_driver bc80_vdivider = {
	.driver   = {
		.name = BC80_DRIVER_NAME,
		.owner	= THIS_MODULE,
		.of_match_table = bc80_vdivider_dt_match_table,
#if defined(CONFIG_PM)
		.pm = &bc80_vdivider_pm_ops,
#endif /* CONFIG_PM */
	},
	.probe    = bc80_vdivider_probe,
	.remove   = bc80_vdivider_remove,
	.shutdown	= bc80_vdivider_shutdown,
	.id_table = bc80_vdivider_id_table,
};
module_i2c_driver(bc80_vdivider);

MODULE_AUTHOR("Samsung Electronics");
MODULE_DESCRIPTION("BC80 driver");
MODULE_LICENSE("GPL v2");
MODULE_VERSION("0.2.0");
