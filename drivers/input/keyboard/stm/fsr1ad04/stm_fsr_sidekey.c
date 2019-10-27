/* --------------- (C) COPYRIGHT 2012 STMicroelectronics ----------------------
 *
 * File Name	: stm_fsr_sidekey.c
 * Authors		: AMS(Analog Mems Sensor) Team
 * Description	: Strain gauge sidekey controller (Formosa + D3)
 *
 * -----------------------------------------------------------------------------
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * THE PRESENT SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES
 * OR CONDITIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED, FOR THE SOLE
 * PURPOSE TO SUPPORT YOUR APPLICATION DEVELOPMENT.
 * AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
 * INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
 * CONTENT OF SUCH SOFTWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
 * INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
 *
 * THIS SOFTWARE IS SPECIFICALLY DESIGNED FOR EXCLUSIVE USE WITH ST PARTS.
 * -----------------------------------------------------------------------------
 * REVISON HISTORY
 * DATE		 | DESCRIPTION
 * 01/03/2019| First Release
 * 01/23/2019| Changed release information to little endian.
 *           | Get system_info_addr from device tree
 *           | Enabled fsr_functions_init function
 * 01/28/2019| Add Reset in fsr_init function
 * 01/30/2019| Add fsr_read_system_info function
 * -----------------------------------------------------------------------------
 */

#include "stm_fsr_sidekey.h"

#ifdef USE_OPEN_CLOSE
static int fsr_input_open(struct input_dev *dev);
static void fsr_input_close(struct input_dev *dev);
#endif

int fsr_read_reg(struct fsr_sidekey_info *info, unsigned char *reg, int cnum,
		 unsigned char *buf, int num)
{
	struct i2c_msg xfer_msg[2];
	int ret;
	u8 *buff;
	u8 *msg_buff;

	msg_buff = kzalloc(cnum, GFP_KERNEL);
	if (!msg_buff)
		return -ENOMEM;

	memcpy(msg_buff, reg, cnum);

	buff = kzalloc(num, GFP_KERNEL);
	if (!buff) {
		kfree(msg_buff);
		return -ENOMEM;
	}

	mutex_lock(&info->i2c_mutex);

	xfer_msg[0].addr = info->client->addr;
	xfer_msg[0].len = cnum;
	xfer_msg[0].flags = 0;
	xfer_msg[0].buf = msg_buff;

	xfer_msg[1].addr = info->client->addr;
	xfer_msg[1].len = num;
	xfer_msg[1].flags = I2C_M_RD;
	xfer_msg[1].buf = buff;

	ret = i2c_transfer(info->client->adapter, xfer_msg, 2);
	if (ret < 0) {
		input_err(true, &info->client->dev,
				"%s failed. ret:%d, addr:%x\n",
				__func__, ret, xfer_msg[0].addr);
	}

	mutex_unlock(&info->i2c_mutex);

	memcpy(buf, buff, num);

	if (info->debug_string & FSR_DEBUG_PRINT_I2C_READ_CMD) {
		int i;

		pr_info("sec_input: i2c_cmd: R: ");
		for (i = 0; i < cnum; i++)
			pr_cont("%02X ", msg_buff[i]);
		pr_cont("|");
		for (i = 0; i < num; i++)
			pr_cont("%02X ", buff[i]);
		pr_cont("\n");
	}

	kfree(msg_buff);
	kfree(buff);

	return ret;
}

int fsr_write_reg(struct fsr_sidekey_info *info,
		  unsigned char *reg, unsigned short num_com)
{
	struct i2c_msg xfer_msg[2];
	int ret;
	u8 *buff;

	buff = kzalloc(num_com, GFP_KERNEL);
	if (!buff)
		return -ENOMEM;
	memcpy(buff, reg, num_com);

	mutex_lock(&info->i2c_mutex);

	xfer_msg[0].addr = info->client->addr;
	xfer_msg[0].len = num_com;
	xfer_msg[0].flags = 0;
	xfer_msg[0].buf = buff;

	ret = i2c_transfer(info->client->adapter, xfer_msg, 1);
	if (ret < 0) {
		input_err(true, &info->client->dev,
				"%s failed. ret:%d, addr:%x\n",
				__func__, ret, xfer_msg[0].addr);
	}

	mutex_unlock(&info->i2c_mutex);

	if (info->debug_string & FSR_DEBUG_PRINT_I2C_WRITE_CMD) {
		int i;

		pr_info("sec_input: i2c_cmd: W: ");
		for (i = 0; i < num_com; i++)
			pr_cont("%02X ", buff[i]);
		pr_cont("\n");
	}

	kfree(buff);

	return ret;
}

void fsr_delay(unsigned int ms)
{
	if (ms < 20)
		usleep_range(ms * 1000, ms * 1000);
	else
		msleep(ms);
}

void fsr_command(struct fsr_sidekey_info *info, unsigned char cmd)
{
	unsigned char regAdd = 0;
	int ret = 0;

	regAdd = cmd;
	ret = fsr_write_reg(info, &regAdd, 1);
	input_info(true, &info->client->dev, "FSR Command (%02X) , ret = %d \n", cmd, ret);
}

void fsr_interrupt_set(struct fsr_sidekey_info *info, int enable)
{
	unsigned char regAdd[4] = { 0xB6, 0x00, 0x2C, enable };

	if (enable) {
		regAdd[3] = INT_ENABLE_D3;
		input_info(true, &info->client->dev, "%s: Enable\n", __func__);
	} else {
		regAdd[3] = INT_DISABLE_D3;
		input_info(true, &info->client->dev, "%s: Disable\n", __func__);
	}

	fsr_write_reg(info, &regAdd[0], 4);
}

static int fsr_set_temp(struct fsr_sidekey_info *info)
{
	u8 regAdd[2] = {0xAE, 0};
	int ret = 0;

	if (!info->psy)
		info->psy = power_supply_get_by_name("battery");

	if (!info->psy) {
		input_err(true, &info->client->dev, "%s: Cannot find power supply\n", __func__);
		return -1;
	}

	ret = power_supply_get_property(info->psy, POWER_SUPPLY_PROP_TEMP, &info->psy_value);
	if (ret < 0) {
		input_err(true, &info->client->dev, "%s: Couldn't get aicl settled value ret=%d\n", __func__, ret);
		return ret;
	}

	regAdd[1] = (u8)(info->psy_value.intval / 10);
	if (regAdd[1] == info->temperature)
		return 0;

	ret = fsr_write_reg(info, regAdd, 2);
	if (ret < 0) {
		input_err(true, &info->client->dev, "%s: Failed to write\n", __func__);
		return ret;
	}
	info->temperature = regAdd[1];

	input_info(true, &info->client->dev, "%s: temperature %d\n", __func__, (s8)info->temperature);

	return ret;
}

static void fsr_set_temp_work(struct work_struct *work)
{
	struct fsr_sidekey_info *info = container_of(work, struct fsr_sidekey_info,
			temp_work.work);

	fsr_set_temp(info);
	schedule_delayed_work(&info->temp_work, msecs_to_jiffies(30000));
}

static int fsr_check_chip_id(struct fsr_sidekey_info *info) {

	unsigned char regAdd[3] = {0xB6, 0x00, 0x04};
	unsigned char val[7] = {0};
	int ret;

	ret = fsr_read_reg(info, regAdd, 3, (unsigned char *)val, 7);
	if (ret < 0) {
		input_err(true, &info->client->dev, "%s failed. ret: %d\n",
			__func__, ret);
		return ret;
	}

	input_info(true, &info->client->dev, "FTS %02X%02X%02X =  %02X %02X %02X %02X %02X %02X\n",
	       regAdd[0], regAdd[1], regAdd[2], val[1], val[2], val[3], val[4], val[5], val[6]);

	if(val[1] == FSR_ID0 && val[2] == FSR_ID1)
	{
		input_info(true, &info->client->dev,"FTS Chip ID : %02X %02X\n", val[1], val[2]);

	}
	else
		return -FSR_ERROR_INVALID_CHIP_ID;

	return ret;
}

int fsr_wait_for_ready(struct fsr_sidekey_info *info)
{
	int rc;
	unsigned char regAdd;
	unsigned char data[FSR_EVENT_SIZE];
	int retry = 0;
	int err_cnt = 0;

	memset(data, 0x0, FSR_EVENT_SIZE);

	regAdd = CMD_READ_EVENT;
	rc = -1;
	while (fsr_read_reg(info, &regAdd, 1, (unsigned char *)data, FSR_EVENT_SIZE)) {
		input_err(true, &info->client->dev, "%s: %02X %02X %02X %02X %02X %02X %02X %02X\n", __func__,
				data[0], data[1], data[2], data[3], data[4], data[5], data[6], data[7]);
		if (data[0] == EID_CONTROLLER_READY) {
			rc = 0;
			break;
		}

		if (data[0] == EID_ERROR) {
			if (data[1] == EID_ERROR_FLASH_CORRUPTION) {
				rc = -FSR_ERROR_EVENT_ID;
				input_err(true, &info->client->dev, "%s: flash corruption:%02X,%02X,%02X\n",
						__func__, data[0], data[1], data[2]);
				if (!info->fwup_is_on_going)
					break;
				input_err(true, &info->client->dev, "%s: fw_update is on going, keep going!\n",
						__func__);
			}

			if (err_cnt++ > 32) {
				rc = -FSR_ERROR_EVENT_ID;
				break;
			}
			continue;
		}

		if (retry++ > FSR_RETRY_COUNT) {
			rc = -FSR_ERROR_TIMEOUT;
			input_err(true, &info->client->dev, "%s: Time Over\n", __func__);

			break;
		}
		fsr_delay(5);
	}

	input_info(true, &info->client->dev,
		"%s: %02X, %02X, %02X, %02X, %02X, %02X, %02X, %02X\n",
		__func__, data[0], data[1], data[2], data[3],
		data[4], data[5], data[6], data[7]);

	return rc;
}

int fsr_systemreset(struct fsr_sidekey_info *info, unsigned int delay)
{
	unsigned char regAdd[4] = { 0xB6, 0x00, 0x28, 0x80 };
	int rc;

	input_info(true, &info->client->dev, "%s: FSR System Reset by I2C.\n",
		__func__);

	fsr_interrupt_set(info, INT_DISABLE);

	fsr_write_reg(info, &regAdd[0], 4);
	fsr_delay(delay);
	rc = fsr_wait_for_ready(info);

	fsr_command(info, CMD_SENSEON);
	fsr_command(info, CMD_NOTI_AP_BOOTUP);
	fsr_interrupt_set(info, INT_ENABLE);

	return rc;
}

int fsr_systemreset_gpio(struct fsr_sidekey_info *info, unsigned int delay)
{
	int rc;

	input_info(true, &info->client->dev, "%s: FSR System Reset by GPIO.\n",
		__func__);

	if (gpio_is_valid(info->board->rst_gpio)) {
		fsr_interrupt_set(info, INT_DISABLE);

		input_err(true, &info->client->dev, "%s: rst_gpio:%d\n", __func__, gpio_get_value(info->board->rst_gpio));
		if (info->board->rst_active_low) {
			gpio_set_value(info->board->rst_gpio, 0);
			input_err(true, &info->client->dev, "%s: set 0[%d]\n", __func__, gpio_get_value(info->board->rst_gpio));
			fsr_delay(20);
			gpio_set_value(info->board->rst_gpio, 1);
			input_err(true, &info->client->dev, "%s: set 1[%d]\n", __func__, gpio_get_value(info->board->rst_gpio));
		} else {
			gpio_set_value(info->board->rst_gpio, 1);
			input_err(true, &info->client->dev, "%s: set 1[%d]\n", __func__, gpio_get_value(info->board->rst_gpio));
			fsr_delay(20);
			gpio_set_value(info->board->rst_gpio, 0);
			input_err(true, &info->client->dev, "%s: set 0[%d]\n", __func__, gpio_get_value(info->board->rst_gpio));
		}

		fsr_delay(delay);
		rc = fsr_wait_for_ready(info);

		fsr_command(info, CMD_SENSEON);
		fsr_command(info, CMD_NOTI_AP_BOOTUP);
		fsr_interrupt_set(info, INT_ENABLE);

		return rc;
	} else {
		input_err(true, &info->client->dev,
				"%s: GPIO is not defined for Reset!\n",
				__func__);
		return -1;
	}
}

static void fsr_reset_work(struct work_struct *work)
{
	struct fsr_sidekey_info *info = container_of(work, struct fsr_sidekey_info,
			reset_work.work);
	
	input_info(true, &info->client->dev, "%s: Call Power-Off to recover IC\n", __func__);

	info->reset_is_on_going = true;

	fsr_systemreset_gpio(info, 50);

	info->reset_is_on_going = false;
}

int fsr_get_version_info(struct fsr_sidekey_info *info)
{
	int rc = 0;
	unsigned char regAdd[3];
	unsigned char data[FSR_EVENT_SIZE];

	// Read F/W Core version
	regAdd[0] = 0xB6;
	regAdd[1] = 0x00;
	regAdd[2] = 0x07;
	rc = fsr_read_reg(info, regAdd, 3, (unsigned char *)data, 4);
	if (rc < 0) {
		input_err(true, &info->client->dev,
				"%s fail to read FW core verion ret: %d\n",
				__func__, rc);
		return rc;
	}
	info->fw_version_of_ic = (data[3] << 8) + data[2];
	info->product_id_of_ic = data[1];

	// Read Config version
	regAdd[0] = 0xD0;
	regAdd[1] = 0xF0;
	regAdd[2] = 0x01;
	rc = fsr_read_reg(info, regAdd, 3, (unsigned char *)data, 3);
	if (rc < 0) {
		input_err(true, &info->client->dev,
				"%s fail to read FW core verion ret: %d\n",
				__func__, rc);
		return rc;
	}
	info->config_version_of_ic = (data[2] << 8) + data[1];

	// Read Release version
	regAdd[0] = 0xD0;
	regAdd[1] = 0xF7;
	regAdd[2] = 0xEC;
	rc = fsr_read_reg(info, regAdd, 3, (unsigned char *)data, 3);
	if (rc < 0) {
		input_err(true, &info->client->dev,
				"%s fail to read FW core verion ret: %d\n",
				__func__, rc);
		return rc;
	}
	info->fw_main_version_of_ic = (data[2] << 8) + data[1];

	input_info(true, &info->client->dev,
			"IC product id : 0x%02X "
			"IC Firmware Version : 0x%04X "
			"IC Config Version : 0x%04X "
			"IC Main Version : 0x%04X\n",
			info->product_id_of_ic,
			info->fw_version_of_ic,
			info->config_version_of_ic,
			info->fw_main_version_of_ic);

	return rc;
}

void fsr_data_dump(struct fsr_sidekey_info *info, u8 *data, int len)
{
	char buf[50] = {0};
	char tmp[10] = {0};
	int i, j;

	for (i = 0; i < (len / 8); i++) {
		for (j = 0; j < 8; j++) {
			snprintf(tmp, sizeof(tmp), " %02X", data[i * 8 + j]);
			strlcat(buf, tmp, sizeof(buf));
		}
		input_info(true, &info->client->dev, "%s[%02d]:%s\n", __func__, i * 8, buf);
		memset(buf, 0, sizeof(buf));
	}

	for (j = 0; j < (len % 8); j++) {
		snprintf(tmp, sizeof(tmp), " %02X", data[i * 8 + j]);
		strlcat(buf, tmp, sizeof(buf));
	}
	input_info(true, &info->client->dev, "%s[%02d]:%s\n", __func__, i * 8, buf);
}

int fsr_read_system_info(struct fsr_sidekey_info *info)
{
	int retval;
	u8 regAdd[3] = {0xD0, 0x00, 0x00};

	input_info(true, &info->client->dev, "%s\n", __func__);

	// Read system info from IC
	regAdd[1] = (info->board->system_info_addr >> 8) & 0xff;
	regAdd[2] = (info->board->system_info_addr) & 0xff;
	retval = fsr_read_reg(info, &regAdd[0], 3, (u8*)&info->fsr_sys_info.dummy, sizeof(struct fsr_system_info));
	if (retval < 0) {
		input_err(true, &info->client->dev,
				"%s: Failed to read system info from IC\n", __func__);
		goto exit;
	}
	fsr_data_dump(info, &info->fsr_sys_info.dummy, sizeof(struct fsr_system_info));

	return 0;

exit:
	return retval;
}

#ifdef ENABLE_POWER_CONTROL
static int fsr_power_ctrl(void *data, bool on)
{
	struct fsr_sidekey_info *info = (struct fsr_sidekey_info *)data;
	const struct fsr_sidekey_plat_data *pdata = info->board;
	struct device *dev = &info->client->dev;
	struct regulator *regulator_dvdd = NULL;
	struct regulator *regulator_avdd = NULL;
	static bool enabled;
	int retval = 0;

	if (enabled == on)
		return retval;

	regulator_dvdd = regulator_get(NULL, pdata->regulator_dvdd);
	if (IS_ERR_OR_NULL(regulator_dvdd)) {
		input_err(true, dev, "%s: Failed to get %s regulator\n",
			 __func__, pdata->regulator_dvdd);
		goto out;
	}

	regulator_avdd = regulator_get(NULL, pdata->regulator_avdd);
	if (IS_ERR_OR_NULL(regulator_avdd)) {
		input_err(true, dev, "%s: Failed to get %s regulator\n",
			 __func__, pdata->regulator_avdd);
		goto out;
	}

	if (on) {
		retval = regulator_enable(regulator_avdd);
		if (retval) {
			input_err(true, dev, "%s: Failed to enable avdd: %d\n", __func__, retval);
			regulator_disable(regulator_avdd);
			goto out;
		}

		fsr_delay(1);

		retval = regulator_enable(regulator_dvdd);
		if (retval) {
			input_err(true, dev, "%s: Failed to enable vdd: %d\n", __func__, retval);
			regulator_disable(regulator_dvdd);
			regulator_disable(regulator_avdd);
			goto out;
		}

		fsr_delay(5);
	} else {
		regulator_disable(regulator_dvdd);
		regulator_disable(regulator_avdd);
	}

	enabled = on;

	input_err(true, dev, "%s: %s: avdd:%s, dvdd:%s\n", __func__, on ? "on" : "off",
		regulator_is_enabled(regulator_avdd) ? "on" : "off",
		regulator_is_enabled(regulator_dvdd) ? "on" : "off");

out:
	regulator_put(regulator_dvdd);
	regulator_put(regulator_avdd);

	return retval;
}
#endif

static int fsr_parse_dt(struct i2c_client *client)
{
	int retval = 0;
	struct device *dev = &client->dev;
	struct fsr_sidekey_plat_data *pdata = dev->platform_data;
	struct device_node *np = dev->of_node;

#ifdef ENABLE_IRQ_HANDLER
	pdata->irq_gpio = of_get_named_gpio(np, "stm,irq_gpio", 0);
	if (gpio_is_valid(pdata->irq_gpio)) {
		retval = gpio_request_one(pdata->irq_gpio, GPIOF_DIR_IN, "stm,sidekey_int");
		if (retval) {
			input_err(true, dev, "Unable to request tsp_int [%d]\n", pdata->irq_gpio);
			return -EINVAL;
		}
	} else {
		input_err(true, dev, "Failed to get irq gpio\n");
		return -EINVAL;
	}
	client->irq = gpio_to_irq(pdata->irq_gpio);
	input_err(true, dev, "%s: irq_gpio:%d, irq:%d\n", __func__, pdata->irq_gpio, client->irq);

	if (of_property_read_u32(np, "stm,irq_type", &pdata->irq_type)) {
		input_err(true, dev, "%s: Failed to get irq_type property\n", __func__);
		return -EINVAL;
	}
#endif

#ifdef ENABLE_RESET_GPIO
	pdata->rst_gpio = of_get_named_gpio_flags(np, "stm,rst_gpio", 0, (enum of_gpio_flags *)&pdata->rst_active_low);
	if (gpio_is_valid(pdata->rst_gpio)) {
		retval = gpio_request(pdata->rst_gpio, "fsr_reset");
		if (retval) {
			input_err(true, dev, "%s: failed to request gpio reset\n", __func__);
			return -EINVAL;
		}
		input_err(true, dev, "%s: rst_gpio:%d[%d], ACTIVE_%s\n", __func__, pdata->rst_gpio,
			gpio_get_value(pdata->rst_gpio), pdata->rst_active_low ? "LOW" : "HIGH");
		gpio_direction_output(pdata->rst_gpio, !!pdata->rst_active_low);
		input_err(true, dev, "%s: rst_gpio after: %d\n", __func__, gpio_get_value(pdata->rst_gpio));
	} else {
		input_err(true, dev, "%s: Failed to get reset gpio\n", __func__);
	}
#else
	pdata->rst_gpio = -EINVAL;
#endif

#ifdef ENABLE_POWER_CONTROL
	if (of_property_read_string(np, "stm,regulator_dvdd", &pdata->regulator_dvdd)) {
		input_err(true, dev,  "%s: Failed to get regulator_dvdd name property\n", __func__);
		return -EINVAL;
	}

	if (of_property_read_string(np, "stm,regulator_avdd", &pdata->regulator_avdd)) {
		input_err(true, dev,  "%s: Failed to get regulator_avdd name property\n", __func__);
		return -EINVAL;
	}
	pdata->power = fsr_power_ctrl;
#endif

	if (of_property_read_string_index(np, "stm,firmware_name", 0, &pdata->firmware_name))
		input_err(true, dev, "%s: skipped to get firmware_name property\n", __func__);
	input_err(true, dev, "%s: firmware_name: %s\n", __func__, pdata->firmware_name);
	if (of_property_read_string_index(np, "stm,project_name", 0, &pdata->project_name))
		input_err(true, dev, "%s: skipped to get project_name property\n", __func__);
	if (of_property_read_string_index(np, "stm,project_name", 1, &pdata->model_name))
		input_err(true, dev, "%s: skipped to get model_name property\n", __func__);

	if (of_property_read_u16(np, "stm,system_info_addr", &pdata->system_info_addr))
		input_err(true, dev, "%s: skipped to get system_info_addr property\n", __func__);

	return retval;
}

static int fsr_setup_drv_data(struct i2c_client *client)
{
	int retval = 0;
	struct fsr_sidekey_plat_data *pdata;
	struct fsr_sidekey_info *info;

	/* parse dt */
	if (client->dev.of_node) {
		pdata = devm_kzalloc(&client->dev,
			sizeof(struct fsr_sidekey_plat_data), GFP_KERNEL);

		if (!pdata) {
			input_err(true, &client->dev, "%s: Failed to allocate platform data\n", __func__);
			return -ENOMEM;
		}

		client->dev.platform_data = pdata;
		retval = fsr_parse_dt(client);
		if (retval) {
			input_err(true, &client->dev, "%s: Failed to parse dt\n", __func__);
			return retval;
		}
	} else {
		pdata = client->dev.platform_data;
	}

	if (!pdata) {
		input_err(true, &client->dev, "%s: No platform data found\n", __func__);
		return -EINVAL;
	}

	info = kzalloc(sizeof(struct fsr_sidekey_info), GFP_KERNEL);
	if (!info)
		return -ENOMEM;

	info->client = client;
	info->board = pdata;
	info->irq = client->irq;
	info->irq_type = info->board->irq_type;
	info->irq_enabled = false;

	i2c_set_clientdata(client, info);

	INIT_DELAYED_WORK(&info->reset_work, fsr_reset_work);
	INIT_DELAYED_WORK(&info->temp_work, fsr_set_temp_work);

	return retval;
}

static u8 fsr_event_handler(struct fsr_sidekey_info *info) {
	u8 regAdd;
	int left_event_count = 0;
	int EventNum = 0;
	u8 data[FSR_FIFO_MAX * FSR_EVENT_SIZE] = {0};

	u8 *event_buff;
	u8 event_id = 0;
	u8 status_id;

	regAdd = CMD_READ_EVENT;
	fsr_read_reg(info, &regAdd, 1, (u8 *)&data[0 * FSR_EVENT_SIZE], FSR_EVENT_SIZE);
	left_event_count = (data[7] & 0x3F);

	if (left_event_count >= FSR_FIFO_MAX)
		left_event_count = FSR_FIFO_MAX - 1;

	if (left_event_count > 0) {
		regAdd = CMD_READ_EVENT;
		fsr_read_reg(info, &regAdd, 1, (u8 *)&data[1 * FSR_EVENT_SIZE], FSR_EVENT_SIZE * (left_event_count));
	}

	do {
		event_buff = (u8 *) &data[EventNum * FSR_EVENT_SIZE];
		event_id = event_buff[0];

		switch (event_id) {
		case EID_STATUS_EVENT:
			status_id = event_buff[1];
			switch (status_id) {
			case STATUS_EVENT_ESD_FAILURE:
				input_err(true, &info->client->dev, "%s: ESD detected [Raw:%d, CH:%d]. run reset\n",
							__func__, (event_buff[2] << 8) + event_buff[3], event_buff[3]);
				if (!info->reset_is_on_going)
					schedule_delayed_work(&info->reset_work, msecs_to_jiffies(10));
				break;
			default:
				input_info(true, &info->client->dev,
						"%s: STATUS %02X %02X %02X %02X %02X %02X %02X %02X\n", __func__,
						event_buff[0], event_buff[1], event_buff[2], event_buff[3],
						event_buff[4], event_buff[5], event_buff[6], event_buff[7]);
				break;
			}
			break;
		case EID_EVENT:
			break;
		default:
			input_info(true, &info->client->dev,
					"%s: unknown %02X %02X %02X %02X %02X %02X %02X %02X\n", __func__,
					event_buff[0], event_buff[1], event_buff[2], event_buff[3],
					event_buff[4], event_buff[5], event_buff[6], event_buff[7]);
			break;
		}

		EventNum++;
		left_event_count--;
	} while (left_event_count >= 0);

	return 0;
}

static irqreturn_t fsr_interrupt_handler(int irq, void *handle)
{
	struct fsr_sidekey_info *info = handle;
	int ret;

	mutex_lock(&info->eventlock);

	ret = fsr_event_handler(info);

	mutex_unlock(&info->eventlock);

	return IRQ_HANDLED;
}

int fsr_irq_enable(struct fsr_sidekey_info *info,
		bool enable)
{
	int retval = 0;

	if (enable) {
		if (info->irq_enabled)
			return retval;

		retval = request_threaded_irq(info->irq, NULL,
				fsr_interrupt_handler, info->irq_type,
				STM_FSR_DRV_NAME, info);
		if (retval < 0) {
			input_err(true, &info->client->dev,
					"%s: Failed to create irq thread %d\n",
					__func__, retval);
			return retval;
		}

		info->irq_enabled = true;
	} else {
		if (info->irq_enabled) {
			disable_irq(info->irq);
			free_irq(info->irq, info);
			info->irq_enabled = false;
		}
	}

	return retval;
}


static int fsr_init(struct fsr_sidekey_info *info)
{
	int rc;

	fsr_command(info, CMD_SENSEON);
	fsr_command(info, CMD_NOTI_AP_BOOTUP);
	fsr_interrupt_set(info, INT_ENABLE);

	rc = fsr_check_chip_id(info);
	if (rc < 0)
	{
		input_err(true, &info->client->dev, "Please check the IC for sidekey!!!");
		return FSR_ERROR_INVALID_CHIP_ID;
	}

	info->product_id_of_ic = 0;
	info->fw_version_of_ic = 0;
	info->config_version_of_ic = 0;
	info->fw_main_version_of_ic = 0;

	rc = fsr_get_version_info(info);
	if (rc < 0)
		input_err(true, &info->client->dev, "Fail to get version information!!!");

	rc  = fsr_fw_update_on_probe(info);
	if (rc  < 0)
		input_err(true, &info->client->dev, "%s: Failed to firmware update\n",
				__func__);

	return FSR_NOT_ERROR;
}

static void fsr_set_input_prop(struct fsr_sidekey_info *info, struct input_dev *dev)
{
	static char fsr_ts_phys[64] = { 0 };

	info->input_dev->name = "sec_sidekey";

	dev->dev.parent = &info->client->dev;

	snprintf(fsr_ts_phys, sizeof(fsr_ts_phys), "%s/input1", dev->name);
	dev->phys = fsr_ts_phys;
	dev->id.bustype = BUS_I2C;

	set_bit(EV_SYN, dev->evbit);
	set_bit(EV_KEY, dev->evbit);
	set_bit(KEY_VOLUMEUP, dev->keybit);
	set_bit(KEY_VOLUMEDOWN, dev->keybit);
	set_bit(KEY_WINK, dev->keybit);

	input_set_drvdata(dev, info);

#ifdef USE_OPEN_CLOSE
	info->input_dev->open = fsr_input_open;
	info->input_dev->close = fsr_input_close;
#endif
}

static int fsr_probe(struct i2c_client *client, const struct i2c_device_id *idp)
{
	int retval;
	struct fsr_sidekey_info *info = NULL;

	input_info(true, &client->dev, "%s: STM Sidekey Driver [%s]\n", __func__,
			STM_FSR_DRV_VERSION);

	if (!i2c_check_functionality(client->adapter, I2C_FUNC_I2C)) {
		input_err(true, &client->dev, "%s: FSR err = EIO!\n", __func__);
		return -EIO;
	}

	/* Build up driver data */
	retval = fsr_setup_drv_data(client);
	if (retval < 0) {
		input_err(true, &client->dev, "%s: Failed to set up driver data\n", __func__);
		goto err_setup_drv_data;
	}

	info = (struct fsr_sidekey_info *)i2c_get_clientdata(client);
	if (!info) {
		input_err(true, &client->dev, "%s: Failed to get driver data\n", __func__);
		retval = -ENODEV;
		goto err_get_drv_data;
	}

	info->probe_done = false;

#ifdef ENABLE_POWER_CONTROL
	if (info->board->power)
		info->board->power(info, true);
#endif

/*	info->board->pinctrl = devm_pinctrl_get(&client->dev);
	if (IS_ERR(info->board->pinctrl))
		input_err(true, &client->dev, "%s: could not get pinctrl\n", __func__);*/

	info->input_dev = input_allocate_device();
	if (!info->input_dev) {
		dev_err(&client->dev, "%s: Failed to alloc input_dev\n", __func__);
		retval = -ENOMEM;
		goto err_input_allocate_device;
	}

	mutex_init(&info->i2c_mutex);
	mutex_init(&info->eventlock);

	retval = fsr_init(info);
	if (retval) {
		input_err(true, &info->client->dev, "%s: fsr_init fail!\n", __func__);
		goto err_fsr_init;
	}

	fsr_set_input_prop(info, info->input_dev);
	retval = input_register_device(info->input_dev);
	if (retval) {
		dev_err(&info->client->dev, "%s: input_register_device fail!\n", __func__);
		goto err_register_input;
	}

#ifdef ENABLE_IRQ_HANDLER
	retval = fsr_irq_enable(info, true);
	if (retval < 0) {
		input_err(true, &info->client->dev,
				"%s: Failed to enable attention interrupt\n",
				__func__);
		goto err_enable_irq;
	}
#endif

	fsr_functions_init(info);
	device_init_wakeup(&client->dev, true);

	info->probe_done = true;

	schedule_delayed_work(&info->temp_work, msecs_to_jiffies(30000));

	input_err(true, &info->client->dev, "%s: done\n", __func__);
	input_log_fix();

	return 0;

#ifdef ENABLE_IRQ_HANDLER
err_enable_irq:
#endif
	input_unregister_device(info->input_dev);
	info->input_dev = NULL;
err_register_input:

err_fsr_init:
	mutex_destroy(&info->eventlock);
	mutex_destroy(&info->i2c_mutex);
	if (!info->input_dev)
		input_free_device(info->input_dev);
err_input_allocate_device:
#ifdef ENABLE_POWER_CONTROL
	if (info->board->power)
		info->board->power(info, false);
#endif

	kfree(info);
err_get_drv_data:
err_setup_drv_data:
	input_err(true, &client->dev, "%s: failed(%d)\n", __func__, retval);
	input_log_fix();
	return retval;
}

static int fsr_stop_device(struct fsr_sidekey_info *info, bool lpmode)
{
	input_info(true, &info->client->dev, "%s\n", __func__);

	fsr_command(info, CMD_NOTI_SCREEN_OFF);

	cancel_delayed_work_sync(&info->reset_work);

	return 0;
}

static int fsr_start_device(struct fsr_sidekey_info *info)
{
	input_info(true, &info->client->dev, "%s\n", __func__);

	fsr_command(info, CMD_NOTI_SCREEN_ON);

	return 0;
}

#ifdef USE_OPEN_CLOSE
static int fsr_input_open(struct input_dev *dev)
{
	struct fsr_sidekey_info *info = input_get_drvdata(dev);
	int retval = 0;

	input_info(true, &info->client->dev, "%s\n", __func__);

	retval = fsr_start_device(info);
	if (retval < 0) {
		dev_err(&info->client->dev,
				"%s: Failed to start device\n", __func__);
		goto out;
	}
out:
	return 0;
}

static void fsr_input_close(struct input_dev *dev)
{
	struct fsr_sidekey_info *info = input_get_drvdata(dev);

	input_info(true, &info->client->dev, "%s\n", __func__);

	fsr_stop_device(info, 0);
}
#endif

static int fsr_remove(struct i2c_client *client)
{
	struct fsr_sidekey_info *info = i2c_get_clientdata(client);
#ifdef ENABLE_IRQ_HANDLER
	int retval;
#endif

	input_info(true, &info->client->dev, "%s\n", __func__);

	cancel_delayed_work_sync(&info->temp_work);

	fsr_command(info, CMD_ENTER_LPM);
	fsr_interrupt_set(info, INT_DISABLE);

	cancel_delayed_work_sync(&info->reset_work);

#ifdef ENABLE_IRQ_HANDLER
	retval = fsr_irq_enable(info, false);
	if (retval < 0) {
		dev_err(&info->client->dev,
				"%s: Failed to disable attention interrupt\n",
				__func__);
	}
#endif

	fsr_functions_remove(info);

	input_unregister_device(info->input_dev);
	info->input_dev = NULL;

	kfree(info);

	return 0;
}

static void fsr_shutdown(struct i2c_client *client)
{
	struct fsr_sidekey_info *info = i2c_get_clientdata(client);

	input_info(true, &info->client->dev, "%s\n", __func__);

	fsr_remove(client);
}

#ifdef CONFIG_PM
static int fsr_pm_suspend(struct device *dev)
{
	struct fsr_sidekey_info *info = dev_get_drvdata(dev);

	input_dbg(true, &info->client->dev, "%s\n", __func__);

	cancel_delayed_work_sync(&info->temp_work);
#ifdef ENABLE_IRQ_HANDLER
	disable_irq(info->irq);
#endif

	return 0;
}

static int fsr_pm_resume(struct device *dev)
{
	struct fsr_sidekey_info *info = dev_get_drvdata(dev);

	input_dbg(true, &info->client->dev, "%s\n", __func__);

#ifdef ENABLE_IRQ_HANDLER
	enable_irq(info->irq);
#endif
	schedule_work(&info->temp_work.work);

	return 0;
}
#endif


static const struct i2c_device_id fsr_device_id[] = {
	{STM_FSR_DRV_NAME, 0},
	{}
};

#ifdef CONFIG_PM
static const struct dev_pm_ops fsr_dev_pm_ops = {
	.suspend = fsr_pm_suspend,
	.resume = fsr_pm_resume,
};
#endif

#ifdef CONFIG_OF
static const struct of_device_id fsr_match_table[] = {
	{.compatible = "stm,fsr_sidekey",},
	{},
};
#else
#define fsr_match_table NULL
#endif

static struct i2c_driver fsr_i2c_driver = {
	.driver = {
		   .name = STM_FSR_DRV_NAME,
		   .owner = THIS_MODULE,
#ifdef CONFIG_OF
		   .of_match_table = fsr_match_table,
#endif
#ifdef CONFIG_PM
		   .pm = &fsr_dev_pm_ops,
#endif
		   },
	.probe = fsr_probe,
	.remove = fsr_remove,
	.shutdown = fsr_shutdown,
	.id_table = fsr_device_id,
};

static int __init fsr_driver_init(void)
{
	pr_err("%s %s\n", SECLOG, __func__);
	return i2c_add_driver(&fsr_i2c_driver);
}

static void __exit fsr_driver_exit(void)
{
	i2c_del_driver(&fsr_i2c_driver);
}

MODULE_DESCRIPTION("STMicroelectronics Sidekey Driver");
MODULE_AUTHOR("STMicroelectronics, Inc.");
MODULE_LICENSE("GPL v2");

module_init(fsr_driver_init);
module_exit(fsr_driver_exit);
