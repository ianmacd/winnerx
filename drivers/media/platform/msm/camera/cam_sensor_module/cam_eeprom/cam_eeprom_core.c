/* Copyright (c) 2017-2018, The Linux Foundation. All rights reserved.
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

#include <linux/module.h>
#include <linux/crc32.h>
#include <media/cam_sensor.h>

#include "cam_eeprom_core.h"
#include "cam_eeprom_soc.h"
#include "cam_debug_util.h"
#include "cam_common_util.h"
#include "cam_sensor_i2c.h"
#include <linux/ctype.h>

//#define CAM_EEPROM_DBG  1
//##define CAM_EEPROM_DBG_DUMP  1

#define MAX_READ_SIZE  0x80000

/* rear */
char hw_phone_info[HW_INFO_MAX_SIZE] = HW_INFO;
char sw_phone_info[SW_INFO_MAX_SIZE] = SW_INFO;
char vendor_phone_info[VENDOR_INFO_MAX_SIZE] = VENDOR_INFO;
char process_phone_info[PROCESS_INFO_MAX_SIZE] = PROCESS_INFO;
/* Module Manufacturer information  */
char rear_fw_ver[FROM_MODULE_FW_INFO_SIZE + 1] = "\0";
char rear_phone_fw_ver[FROM_MODULE_FW_INFO_SIZE + 1] = "\0";
char rear_load_fw_ver[FROM_MODULE_FW_INFO_SIZE + 1] = "\0";
char rear_cam_cal_check[SYSFS_FW_VER_SIZE] = "NULL";

/*front */
char front_hw_phone_info[HW_INFO_MAX_SIZE] = FRONT_HW_INFO;
char front_sw_phone_info[SW_INFO_MAX_SIZE] = FRONT_SW_INFO;
char front_vendor_phone_info[VENDOR_INFO_MAX_SIZE] = FRONT_VENDOR_INFO;
char front_process_phone_info[PROCESS_INFO_MAX_SIZE] = FRONT_PROCESS_INFO;
/* Module Manufacturer information  */
char front_fw_ver[FROM_MODULE_FW_INFO_SIZE + 1] = "\0";
char front_phone_fw_ver[FROM_MODULE_FW_INFO_SIZE + 1] = "\0";
char front_load_fw_ver[FROM_MODULE_FW_INFO_SIZE + 1] = "\0";
char front_cam_cal_check[SYSFS_FW_VER_SIZE] = "NULL";
#if defined(CONFIG_SAMSUNG_FRONT_DUAL)
char front2_hw_phone_info[HW_INFO_MAX_SIZE] = FRONT2_HW_INFO;
char front2_sw_phone_info[SW_INFO_MAX_SIZE] = FRONT2_SW_INFO;
char front2_vendor_phone_info[VENDOR_INFO_MAX_SIZE] = FRONT2_VENDOR_INFO;
char front2_process_phone_info[PROCESS_INFO_MAX_SIZE] = FRONT2_PROCESS_INFO;
/* Module Manufacturer information  */
char front2_fw_ver[FROM_MODULE_FW_INFO_SIZE + 1] = "\0";
char front2_phone_fw_ver[FROM_MODULE_FW_INFO_SIZE + 1] = "\0";
char front2_load_fw_ver[FROM_MODULE_FW_INFO_SIZE + 1] = "\0";
char front2_cam_cal_check[SYSFS_FW_VER_SIZE] = "NULL";
#endif
#if defined(CONFIG_SAMSUNG_FRONT_TOP)
char front3_hw_phone_info[HW_INFO_MAX_SIZE] = FRONT3_HW_INFO;
char front3_sw_phone_info[SW_INFO_MAX_SIZE] = FRONT3_SW_INFO;
char front3_vendor_phone_info[VENDOR_INFO_MAX_SIZE] = FRONT3_VENDOR_INFO;
char front3_process_phone_info[PROCESS_INFO_MAX_SIZE] = FRONT3_PROCESS_INFO;
/* Module Manufacturer information  */
char front3_fw_ver[FROM_MODULE_FW_INFO_SIZE + 1] = "\0";
char front3_phone_fw_ver[FROM_MODULE_FW_INFO_SIZE + 1] = "\0";
char front3_load_fw_ver[FROM_MODULE_FW_INFO_SIZE + 1] = "\0";
char front3_cam_cal_check[SYSFS_FW_VER_SIZE] = "NULL";
#endif
#if defined(CONFIG_SAMSUNG_REAR_DUAL) || defined(CONFIG_SAMSUNG_REAR_TRIPLE)
/* rear2 */
char rear2_hw_phone_info[HW_INFO_MAX_SIZE] = SW_HW_INFO;
char rear2_sw_phone_info[SW_INFO_MAX_SIZE] = SW_SW_INFO;
char rear2_vendor_phone_info[VENDOR_INFO_MAX_SIZE] = SW_VENDOR_INFO;
char rear2_process_phone_info[PROCESS_INFO_MAX_SIZE] = SW_PROCESS_INFO;
/* Module Manufacturer information  */
char rear2_fw_ver[FROM_MODULE_FW_INFO_SIZE + 1] = "\0";
char rear2_phone_fw_ver[FROM_MODULE_FW_INFO_SIZE + 1] = "\0";
char rear2_load_fw_ver[FROM_MODULE_FW_INFO_SIZE + 1] = "\0";
char rear2_cam_cal_check[SYSFS_FW_VER_SIZE] = "NULL";
#endif

#if defined(CONFIG_SAMSUNG_REAR_TRIPLE)
char rear3_hw_phone_info[HW_INFO_MAX_SIZE] = REAR3_HW_INFO;
char rear3_sw_phone_info[SW_INFO_MAX_SIZE] = REAR3_SW_INFO;
char rear3_vendor_phone_info[VENDOR_INFO_MAX_SIZE] = REAR3_VENDOR_INFO;
char rear3_process_phone_info[PROCESS_INFO_MAX_SIZE] = REAR3_PROCESS_INFO;

char rear3_fw_ver[FROM_MODULE_FW_INFO_SIZE + 1] = "\0";
char rear3_phone_fw_ver[FROM_MODULE_FW_INFO_SIZE + 1] = "\0";
char rear3_load_fw_ver[FROM_MODULE_FW_INFO_SIZE + 1] = "\0";
char rear3_cam_cal_check[SYSFS_FW_VER_SIZE] = "NULL";
#endif

#if defined(CONFIG_SAMSUNG_REAR_TOF)
char rear_tof_hw_phone_info[HW_INFO_MAX_SIZE] = FRONT_HW_INFO;
char rear_tof_sw_phone_info[SW_INFO_MAX_SIZE] = FRONT_SW_INFO;
char rear_tof_vendor_phone_info[VENDOR_INFO_MAX_SIZE] = FRONT_VENDOR_INFO;
char rear_tof_process_phone_info[PROCESS_INFO_MAX_SIZE] = FRONT_PROCESS_INFO;
/* Module Manufacturer information	*/
char rear_tof_fw_ver[FROM_MODULE_FW_INFO_SIZE + 1] = "\0";
char rear_tof_phone_fw_ver[FROM_MODULE_FW_INFO_SIZE + 1] = "\0";
char rear_tof_load_fw_ver[FROM_MODULE_FW_INFO_SIZE + 1] = "\0";
char rear_tof_cam_cal_check[SYSFS_FW_VER_SIZE] = "NULL";
#endif
#if defined(CONFIG_SAMSUNG_FRONT_TOF)
char front_tof_hw_phone_info[HW_INFO_MAX_SIZE] = FRONT_HW_INFO;
char front_tof_sw_phone_info[SW_INFO_MAX_SIZE] = FRONT_SW_INFO;
char front_tof_vendor_phone_info[VENDOR_INFO_MAX_SIZE] = FRONT_VENDOR_INFO;
char front_tof_process_phone_info[PROCESS_INFO_MAX_SIZE] = FRONT_PROCESS_INFO;
/* Module Manufacturer information	*/
char front_tof_fw_ver[FROM_MODULE_FW_INFO_SIZE + 1] = "\0";
char front_tof_phone_fw_ver[FROM_MODULE_FW_INFO_SIZE + 1] = "\0";
char front_tof_load_fw_ver[FROM_MODULE_FW_INFO_SIZE + 1] = "\0";
char front_tof_cam_cal_check[SYSFS_FW_VER_SIZE] = "NULL";

#endif

extern unsigned int sec_hw_rev(void);

#if defined(CONFIG_SAMSUNG_OIS_MCU_STM32)
uint8_t ois_wide_xygg[OIS_XYGG_SIZE] = { 0, };
uint8_t ois_wide_cal_mark = 0;
uint8_t ois_wide_xysr[OIS_XYSR_SIZE] = { 0, };
extern int ois_gain_rear_result;
extern int ois_sr_rear_result;
#if defined(CONFIG_SAMSUNG_REAR_TRIPLE)
uint8_t ois_wide_center_shift[OIS_CENTER_SHIFT_SIZE] = { 0, };
uint8_t ois_tele_xygg[OIS_XYGG_SIZE] = { 0, };
uint8_t ois_tele_center_shift[OIS_CENTER_SHIFT_SIZE] = { 0, };
uint8_t ois_tele_cal_mark = 0;
uint8_t ois_tele_xysr[OIS_XYSR_SIZE] = { 0, };
extern int ois_gain_rear3_result;
extern int ois_sr_rear3_result;
#endif
#endif

uint32_t CAMERA_NORMAL_CAL_CRC;

#ifdef CAM_EEPROM_DBG_DUMP
static int cam_eeprom_dump(uint32_t subdev_id, uint8_t *mapdata, uint32_t addr, uint32_t size)
{
	int rc = 0;
	int j;

	if (mapdata == NULL) {
		CAM_ERR(CAM_EEPROM, "mapdata is NULL");
		return -1;
	}
	if (size == 0) {
		CAM_ERR(CAM_EEPROM, "size is 0");
		return -1;
	}

	CAM_INFO(CAM_EEPROM, "subdev_id: %d, eeprom dump addr = 0x%04X, total read size = %d", subdev_id, addr, size);
	for (j = 0; j < size; j++)
		CAM_INFO(CAM_EEPROM, "addr = 0x%04X, data = 0x%02X", addr+j, mapdata[addr+j]);

	return rc;
}
#endif

static int cam_eeprom_update_module_info(struct cam_eeprom_ctrl_t *e_ctrl)
{
	int rc = 0;
	uint8_t map_ver = 0;
	int i = 0;
	char tempbuf[10];
	uint8_t data[4] = { 0, };

	uint8_t loadfrom = 'N';
	char cal_ver[12] = "";
	uint8_t sensor_ver[2] = {0,};
	uint8_t dll_ver[2] = {0,};
	char ideal_ver[12] = "";

	uint32_t normal_is_supported = 0;
	uint8_t  normal_cri_rev = 0;
	uint8_t  bVerNull = FALSE;

	unsigned int rev = sec_hw_rev();

	if (!e_ctrl) {
		CAM_ERR(CAM_EEPROM, "e_ctrl is NULL");
		return -EINVAL;
	}

	if (e_ctrl->soc_info.index == CAM_EEPROM_IDX_FRONT) // Front cam
		map_ver = e_ctrl->cal_data.mapdata[FRONT_CAM_MAP_VERSION_ADDR];
#if defined(CONFIG_SAMSUNG_FRONT_TOP)
	else if (e_ctrl->soc_info.index == CAM_EEPROM_IDX_FRONT2) // Front Upper(TOP) cam
		map_ver = e_ctrl->cal_data.mapdata[FRONT_CAM_MAP_VERSION_ADDR];
#endif
	else if (e_ctrl->soc_info.index == CAM_EEPROM_IDX_BACK) // Rear cam
		map_ver = e_ctrl->cal_data.mapdata[REAR_CAM_MAP_VERSION_ADDR];
#if defined(CONFIG_SAMSUNG_REAR_DUAL) || defined(CONFIG_SAMSUNG_REAR_TRIPLE)
	else if (e_ctrl->soc_info.index == CAM_EEPROM_IDX_BACK2) // SW cam
		map_ver = e_ctrl->cal_data.mapdata[SW_CAM_MAP_VERSION_ADDR];
#endif
#if defined(CONFIG_SAMSUNG_REAR_TOF)
	else if (e_ctrl->soc_info.index == CAM_EEPROM_IDX_BACK_TOF) // SW cam
		map_ver = e_ctrl->cal_data.mapdata[SW_CAM_MAP_VERSION_ADDR];
#endif
#if defined(CONFIG_SAMSUNG_FRONT_TOF)
	else if (e_ctrl->soc_info.index == CAM_EEPROM_IDX_FRONT_TOF) // SW cam
		map_ver = e_ctrl->cal_data.mapdata[SW_CAM_MAP_VERSION_ADDR];
#endif
	else {
		CAM_INFO(CAM_EEPROM, "subdev_id: %d is not supported", e_ctrl->soc_info.index);
		return 0;
	}

	if(map_ver >= 80 || !isalnum(map_ver)) {
		CAM_INFO(CAM_EEPROM, "subdev_id: %d, map version = 0x%x", e_ctrl->soc_info.index, map_ver);
		map_ver = '0';
	} else {
		CAM_INFO(CAM_EEPROM, "subdev_id: %d, map version = %c [0x%x]", e_ctrl->soc_info.index, map_ver, map_ver);
	}

	if (e_ctrl->soc_info.index == CAM_EEPROM_IDX_FRONT) {
		/* front sensor id */
		memcpy(front_sensor_id, &e_ctrl->cal_data.mapdata[FROM_FRONT_SENSOR_ID_ADDR], FROM_SENSOR_ID_SIZE);
		front_sensor_id[FROM_SENSOR_ID_SIZE] = '\0';
		CAM_INFO(CAM_EEPROM,
			"front sensor id = %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x",
			front_sensor_id[0], front_sensor_id[1], front_sensor_id[2], front_sensor_id[3],
			front_sensor_id[4], front_sensor_id[5], front_sensor_id[6], front_sensor_id[7],
			front_sensor_id[8], front_sensor_id[9], front_sensor_id[10], front_sensor_id[11],
			front_sensor_id[12], front_sensor_id[13], front_sensor_id[14], front_sensor_id[15]);
#if defined(CONFIG_SAMSUNG_FRONT_DUAL)
		/* front2 sensor id */
		memcpy(front2_sensor_id, &e_ctrl->cal_data.mapdata[FROM_FRONT2_SENSOR_ID_ADDR], FROM_SENSOR_ID_SIZE);
		front2_sensor_id[FROM_SENSOR_ID_SIZE] = '\0';
		CAM_INFO(CAM_EEPROM,
			"front2 sensor id = %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x",
			front2_sensor_id[0], front2_sensor_id[1], front2_sensor_id[2], front2_sensor_id[3],
			front2_sensor_id[4], front2_sensor_id[5], front2_sensor_id[6], front2_sensor_id[7],
			front2_sensor_id[8], front2_sensor_id[9], front2_sensor_id[10], front2_sensor_id[11],
			front2_sensor_id[12], front2_sensor_id[13], front2_sensor_id[14], front2_sensor_id[15]);
#endif

		/* front module id */
		memcpy(front_module_id, &e_ctrl->cal_data.mapdata[FROM_MODULE_ID_ADDR], FROM_MODULE_ID_SIZE);
		front_module_id[FROM_MODULE_ID_SIZE] = '\0';
		CAM_INFO(CAM_EEPROM, "front_module_id = %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X",
			front_module_id[0], front_module_id[1], front_module_id[2], front_module_id[3],
			front_module_id[4], front_module_id[5], front_module_id[6], front_module_id[7],
			front_module_id[8], front_module_id[9]);

#ifdef CAM_EEPROM_DBG
		CAM_INFO(CAM_EEPROM, "front_module_id = %c %c %c %c %c %02X %02X %02X %02X %02X",
			front_module_id[0], front_module_id[1], front_module_id[2], front_module_id[3],
			front_module_id[4], front_module_id[5], front_module_id[6], front_module_id[7],
			front_module_id[8], front_module_id[9]);
#endif
#if defined(CONFIG_SAMSUNG_FRONT_DUAL)
			/* front2 module id */
			memcpy(front2_module_id, &e_ctrl->cal_data.mapdata[FROM_MODULE_ID_ADDR], FROM_MODULE_ID_SIZE);
			front2_module_id[FROM_MODULE_ID_SIZE] = '\0';
			CAM_INFO(CAM_EEPROM, "front2_module_id = %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X",
				front2_module_id[0], front2_module_id[1], front2_module_id[2], front2_module_id[3],
				front2_module_id[4], front2_module_id[5], front2_module_id[6], front2_module_id[7],
				front2_module_id[8], front2_module_id[9]);

#ifdef CAM_EEPROM_DBG
			CAM_INFO(CAM_EEPROM, "front2_module_id = %c %c %c %c %c %02X %02X %02X %02X %02X",
				front2_module_id[0], front2_module_id[1], front2_module_id[2], front2_module_id[3],
				front2_module_id[4], front2_module_id[5], front2_module_id[6], front2_module_id[7],
				front2_module_id[8], front2_module_id[9]);
#endif
#endif

#if !defined(CONFIG_SAMSUNG_FRONT_TOP_EEPROM)
		/* front af cal*/
		front_af_cal_pan = *((uint32_t *)&e_ctrl->cal_data.mapdata[FROM_FRONT_AF_CAL_PAN_ADDR]);
		front_af_cal_macro = *((uint32_t *)&e_ctrl->cal_data.mapdata[FROM_FRONT_AF_CAL_MACRO_ADDR]);
		CAM_DBG(CAM_EEPROM, "front_af_cal_pan: %d, front_af_cal_macro: %d",
			front_af_cal_pan, front_af_cal_macro);

#endif //!defined(CONFIG_SAMSUNG_FRONT_TOP_EEPROM)

		/* front mtf exif */
		memcpy(front_mtf_exif, &e_ctrl->cal_data.mapdata[FROM_FRONT_MTF_ADDR], FROM_MTF_SIZE);
		front_mtf_exif[FROM_MTF_SIZE] = '\0';
		CAM_DBG(CAM_EEPROM, "front mtf exif = %s", front_mtf_exif);

		/* front manufacturer info */
		memcpy(front_fw_ver, &e_ctrl->cal_data.mapdata[FRONT_MODULE_FW_VERSION], FROM_MODULE_FW_INFO_SIZE);
		front_fw_ver[FROM_MODULE_FW_INFO_SIZE] = '\0';
		CAM_DBG(CAM_EEPROM,
			"front manufacturer info = %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x",
			front_fw_ver[0], front_fw_ver[1], front_fw_ver[2], front_fw_ver[3], front_fw_ver[4],
			front_fw_ver[5], front_fw_ver[6], front_fw_ver[7], front_fw_ver[8], front_fw_ver[9],
			front_fw_ver[10]);

		/* temp front phone version */
		snprintf(front_phone_fw_ver, FROM_MODULE_FW_INFO_SIZE+1, "%s%s%s%s", front_hw_phone_info, front_sw_phone_info, front_vendor_phone_info, front_process_phone_info);

#if defined(CONFIG_SAMSUNG_FRONT_DUAL)
		/* front2 manufacturer info */
		memcpy(front2_fw_ver, &e_ctrl->cal_data.mapdata[FRONT2_MODULE_FW_VERSION], FROM_MODULE_FW_INFO_SIZE);
		front2_fw_ver[FROM_MODULE_FW_INFO_SIZE] = '\0';
		CAM_INFO(CAM_EEPROM,
			"front2 manufacturer info = %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x",
			front2_fw_ver[0], front2_fw_ver[1], front2_fw_ver[2], front2_fw_ver[3], front2_fw_ver[4],
			front2_fw_ver[5], front2_fw_ver[6], front2_fw_ver[7], front2_fw_ver[8], front2_fw_ver[9],
			front2_fw_ver[10]);

		/* temp front phone version */
		snprintf(front2_phone_fw_ver, FROM_MODULE_FW_INFO_SIZE+1, "%s%s%s%s", front2_hw_phone_info, front2_sw_phone_info, front2_vendor_phone_info, front2_process_phone_info);
#endif
		/* temp front load version */
		CAM_INFO(CAM_EEPROM, "Load from front EEPROM");
		strcpy(front_load_fw_ver, front_fw_ver);
		loadfrom = 'E';

		bVerNull = FALSE;
		for(i = 0; i < FROM_MODULE_FW_INFO_SIZE; i ++) {
			if(e_ctrl->cal_data.mapdata[FRONT_MODULE_FW_VERSION + i] >= 0x80
				|| !isalnum(e_ctrl->cal_data.mapdata[FRONT_MODULE_FW_VERSION + i])) {
				cal_ver[i] = ' ';
				bVerNull = TRUE;
			} else {
				cal_ver[i] = e_ctrl->cal_data.mapdata[FRONT_MODULE_FW_VERSION + i];
			}

			if(front_phone_fw_ver[i] >= 0x80 || !isalnum(front_phone_fw_ver[i]))
				front_phone_fw_ver[i] = ' ';
		}
		sensor_ver[0] = front_sensor_id[8];
		dll_ver[0] = e_ctrl->cal_data.mapdata[FRONT_DLL_VERSION_ADDR] - '0';

		sensor_ver[1] = 0;
		dll_ver[1] = 0;

		normal_is_supported = CAMERA_NORMAL_CAL_CRC;
		normal_cri_rev = CRITERION_REV;
		strcpy(ideal_ver, front_phone_fw_ver);
		if(front_phone_fw_ver[9] < 0x80 && isalnum(front_phone_fw_ver[9])) {
			ideal_ver[9] = front_phone_fw_ver[9];
		}
		if(front_phone_fw_ver[10] < 0x80 && isalnum(front_phone_fw_ver[10])) {
			ideal_ver[10] = front_phone_fw_ver[10];
		}

		if(rev < normal_cri_rev && bVerNull == TRUE)
		{
			strcpy(cal_ver, ideal_ver);
			CAM_ERR(CAM_EEPROM, "set tmp ver: %s", cal_ver);
		}

		snprintf(front_module_info, SYSFS_MODULE_INFO_SIZE, "SSCAL %c%s%04X%04XR%02dM%cD%02XD%02XS%02XS%02X/%s%04X%04XR%02d",
			loadfrom, cal_ver, (e_ctrl->is_supported >> 16) & 0xFFFF, e_ctrl->is_supported & 0xFFFF,
			rev & 0xFF, map_ver, dll_ver[0] & 0xFF, dll_ver[1] & 0xFF, sensor_ver[0] & 0xFF, sensor_ver[1] & 0xFF,
			ideal_ver, (normal_is_supported >> 16) & 0xFFFF, normal_is_supported & 0xFFFF, normal_cri_rev);
#ifdef CAM_EEPROM_DBG
		CAM_INFO(CAM_EEPROM, "front info = %s", front_module_info);
#endif

		/* update EEPROM fw version on sysfs */
		sprintf(front_cam_fw_ver, "%s %s\n", front_fw_ver, front_load_fw_ver);
		sprintf(front_cam_fw_full_ver, "%s N %s\n", front_fw_ver, front_load_fw_ver);
#ifdef CAM_EEPROM_DBG
		CAM_INFO(CAM_EEPROM, "front manufacturer info = %c %c %c %c %c %c %c %c %c %c %c",
			front_fw_ver[0], front_fw_ver[1], front_fw_ver[2], front_fw_ver[3], front_fw_ver[4],
			front_fw_ver[5], front_fw_ver[6], front_fw_ver[7], front_fw_ver[8], front_fw_ver[9],
			front_fw_ver[10]);
#endif
		memset(data, 0, sizeof(data));
		memcpy(data, &e_ctrl->cal_data.mapdata[FROM_FRONT_PAF_CAL_DATA_START_ADDR + FROM_PAF_CAL_ERR_CHECK_OFFSET], 4);
		front_paf_err_data_result = *data | ( *(data + 1) << 8) | ( *(data + 2) << 16) | (*(data + 3) << 24);

#if defined(CONFIG_SAMSUNG_FRONT_DUAL)
		/* temp front2 load version */
		CAM_INFO(CAM_EEPROM, "Load from front EEPROM");
		strcpy(front2_load_fw_ver, front2_fw_ver);
		loadfrom = 'E';

		bVerNull = FALSE;
		for(i = 0; i < FROM_MODULE_FW_INFO_SIZE; i ++) {
			if(e_ctrl->cal_data.mapdata[FRONT2_MODULE_FW_VERSION + i] >= 0x80
				|| !isalnum(e_ctrl->cal_data.mapdata[FRONT2_MODULE_FW_VERSION + i])) {
				cal_ver[i] = ' ';
				bVerNull = TRUE;
			} else {
				cal_ver[i] = e_ctrl->cal_data.mapdata[FRONT2_MODULE_FW_VERSION + i];
			}

			if(front2_phone_fw_ver[i] >= 0x80 || !isalnum(front2_phone_fw_ver[i]))
				front2_phone_fw_ver[i] = ' ';
		}
		sensor_ver[0] = front2_sensor_id[8];
		dll_ver[0] = e_ctrl->cal_data.mapdata[FRONT2_DLL_VERSION_ADDR] - '0';

		sensor_ver[1] = 0;
		dll_ver[1] = 0;

		normal_is_supported = CAMERA_NORMAL_CAL_CRC;
		normal_cri_rev = CRITERION_REV;
		strcpy(ideal_ver, front2_phone_fw_ver);
		if(front2_phone_fw_ver[9] < 0x80 && isalnum(front2_phone_fw_ver[9])) {
			ideal_ver[9] = front_phone_fw_ver[9];
		}
		if(front2_phone_fw_ver[10] < 0x80 && isalnum(front2_phone_fw_ver[10])) {
			ideal_ver[10] = front2_phone_fw_ver[10];
		}

		if(rev < normal_cri_rev && bVerNull == TRUE)
		{
			strcpy(cal_ver, ideal_ver);
			CAM_ERR(CAM_EEPROM, "set tmp ver: %s", cal_ver);
		}

		snprintf(front2_module_info, SYSFS_MODULE_INFO_SIZE, "SSCAL %c%s%04X%04XR%02dM%cD%02XD%02XS%02XS%02X/%s%04X%04XR%02d",
			loadfrom, cal_ver, (e_ctrl->is_supported >> 16) & 0xFFFF, e_ctrl->is_supported & 0xFFFF,
			rev & 0xFF, map_ver, dll_ver[0] & 0xFF, dll_ver[1] & 0xFF, sensor_ver[0] & 0xFF, sensor_ver[1] & 0xFF,
			ideal_ver, (normal_is_supported >> 16) & 0xFFFF, normal_is_supported & 0xFFFF, normal_cri_rev);
#ifdef CAM_EEPROM_DBG
		CAM_INFO(CAM_EEPROM, "front2 info = %s", front2_module_info);
#endif

		/* front2 dual cal */
		memcpy(front2_dual_cal, &e_ctrl->cal_data.mapdata[FROM_FRONT2_DUAL_CAL_ADDR], FROM_FRONT2_DUAL_CAL_SIZE);
		front2_dual_cal[FROM_FRONT2_DUAL_CAL_SIZE] = '\0';
		CAM_INFO(CAM_EEPROM, "front2 dual cal = %s", front2_dual_cal);

		/* rear3 tele tilt */
		memcpy(&front2_dual_tilt_x, &e_ctrl->cal_data.mapdata[FROM_FRONT2_DUAL_TILT_X], 4);
		memcpy(&front2_dual_tilt_y, &e_ctrl->cal_data.mapdata[FROM_FRONT2_DUAL_TILT_Y], 4);
		memcpy(&front2_dual_tilt_z, &e_ctrl->cal_data.mapdata[FROM_FRONT2_DUAL_TILT_Z], 4);
		memcpy(&front2_dual_tilt_sx, &e_ctrl->cal_data.mapdata[FROM_FRONT2_DUAL_TILT_SX], 4);
		memcpy(&front2_dual_tilt_sy, &e_ctrl->cal_data.mapdata[FROM_FRONT2_DUAL_TILT_SY], 4);

		memcpy(&front2_dual_tilt_range, &e_ctrl->cal_data.mapdata[FROM_FRONT2_DUAL_TILT_RANGE], 4);
		memcpy(&front2_dual_tilt_max_err, &e_ctrl->cal_data.mapdata[FROM_FRONT2_DUAL_TILT_MAX_ERR], 4);
		memcpy(&front2_dual_tilt_avg_err, &e_ctrl->cal_data.mapdata[FROM_FRONT2_DUAL_TILT_AVG_ERR], 4);
		memcpy(&front2_dual_tilt_dll_ver, &e_ctrl->cal_data.mapdata[FROM_FRONT2_DUAL_TILT_DLL_VERSION], 4);

		CAM_INFO(CAM_EEPROM,
			"front2 dual tilt x = %d, y = %d, z = %d, sx = %d, sy = %d, range = %d, max_err = %d, avg_err = %d, dll_ver = %d",
			front2_dual_tilt_x, front2_dual_tilt_y, front2_dual_tilt_z, front2_dual_tilt_sx,
			front2_dual_tilt_sy, front2_dual_tilt_range, front2_dual_tilt_max_err,
			front2_dual_tilt_avg_err, front2_dual_tilt_dll_ver);

		/* update EEPROM fw version on sysfs */
		sprintf(front2_cam_fw_ver, "%s %s\n", front2_fw_ver, front2_load_fw_ver);
		sprintf(front2_cam_fw_full_ver, "%s N %s\n", front2_fw_ver, front2_load_fw_ver);
#ifdef CAM_EEPROM_DBG
		CAM_INFO(CAM_EEPROM, "front2 manufacturer info = %c %c %c %c %c %c %c %c %c %c %c",
			front2_fw_ver[0], front2_fw_ver[1], front2_fw_ver[2], front2_fw_ver[3], front2_fw_ver[4],
			front2_fw_ver[5], front2_fw_ver[6], front2_fw_ver[7], front2_fw_ver[8], front2_fw_ver[9],
			front2_fw_ver[10]);
#endif
#endif //#if defined(CONFIG_SAMSUNG_FRONT_DUAL)

#if defined(CONFIG_SAMSUNG_FRONT_TOF)
		/* front tof dual cal */
		memcpy(front_tof_dual_cal, &e_ctrl->cal_data.mapdata[FRONT_TOF_DUAL_CAL_ADDR], FRONT_TOF_DUAL_CAL_SIZE);
		front_tof_dual_cal[FRONT_TOF_DUAL_CAL_SIZE] = '\0';
		CAM_DBG(CAM_EEPROM, "front tof dual cal = %s", front_tof_dual_cal);

		/* front tof tilt */
		memcpy(&front_tof_dual_tilt_x, &e_ctrl->cal_data.mapdata[FRONT_TOF_DUAL_TILT_X], 4);
		memcpy(&front_tof_dual_tilt_y, &e_ctrl->cal_data.mapdata[FRONT_TOF_DUAL_TILT_Y], 4);
		memcpy(&front_tof_dual_tilt_z, &e_ctrl->cal_data.mapdata[FRONT_TOF_DUAL_TILT_Z], 4);
		memcpy(&front_tof_dual_tilt_sx, &e_ctrl->cal_data.mapdata[FRONT_TOF_DUAL_TILT_SX], 4);
		memcpy(&front_tof_dual_tilt_sy, &e_ctrl->cal_data.mapdata[FRONT_TOF_DUAL_TILT_SY], 4);

		memcpy(&front_tof_dual_tilt_range, &e_ctrl->cal_data.mapdata[FRONT_TOF_DUAL_TILT_RANGE], 4);
		memcpy(&front_tof_dual_tilt_max_err, &e_ctrl->cal_data.mapdata[FRONT_TOF_DUAL_TILT_MAX_ERR], 4);
		memcpy(&front_tof_dual_tilt_avg_err, &e_ctrl->cal_data.mapdata[FRONT_TOF_DUAL_TILT_AVG_ERR], 4);
		memcpy(&front_tof_dual_tilt_dll_ver, &e_ctrl->cal_data.mapdata[FRONT_TOF_DUAL_TILT_DLL_VERSION], 4);

		CAM_INFO(CAM_EEPROM,
			"front tof dual tilt x = %d, y = %d, z = %d, sx = %d, sy = %d, range = %d, max_err = %d, avg_err = %d, dll_ver = %d",
			front_tof_dual_tilt_x, front_tof_dual_tilt_y, front_tof_dual_tilt_z, front_tof_dual_tilt_sx,
			front_tof_dual_tilt_sy, front_tof_dual_tilt_range, front_tof_dual_tilt_max_err,
			front_tof_dual_tilt_avg_err, front_tof_dual_tilt_dll_ver);
#endif
	}
#if defined(CONFIG_SAMSUNG_FRONT_TOP)
	else if (e_ctrl->soc_info.index == CAM_EEPROM_IDX_FRONT2)
    {
		/* front3 sensor id */
		memcpy(front3_sensor_id, &e_ctrl->cal_data.mapdata[FROM_FRONT_SENSOR_ID_ADDR], FROM_SENSOR_ID_SIZE);
		front3_sensor_id[FROM_SENSOR_ID_SIZE] = '\0';
		CAM_INFO(CAM_EEPROM,
			"front3 sensor id = %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x",
			front3_sensor_id[0], front3_sensor_id[1], front3_sensor_id[2], front3_sensor_id[3],
			front3_sensor_id[4], front3_sensor_id[5], front3_sensor_id[6], front3_sensor_id[7],
			front3_sensor_id[8], front3_sensor_id[9], front3_sensor_id[10], front3_sensor_id[11],
			front3_sensor_id[12], front3_sensor_id[13], front3_sensor_id[14], front3_sensor_id[15]);

		/* front3 module id */
		memcpy(front3_module_id, &e_ctrl->cal_data.mapdata[FROM_MODULE_ID_ADDR], FROM_MODULE_ID_SIZE);
		front3_module_id[FROM_MODULE_ID_SIZE] = '\0';
		CAM_INFO(CAM_EEPROM, "front2_module_id = %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X",
			front3_module_id[0], front3_module_id[1], front3_module_id[2], front3_module_id[3],
			front3_module_id[4], front3_module_id[5], front3_module_id[6], front3_module_id[7],
			front3_module_id[8], front3_module_id[9]);

#ifdef CAM_EEPROM_DBG
		CAM_INFO(CAM_EEPROM, "front2_module_id = %c %c %c %c %c %02X %02X %02X %02X %02X",
			front3_module_id[0], front3_module_id[1], front3_module_id[2], front3_module_id[3],
			front3_module_id[4], front3_module_id[5], front3_module_id[6], front3_module_id[7],
			front3_module_id[8], front3_module_id[9]);
#endif

#if !defined(CONFIG_SAMSUNG_FRONT_TOP_EEPROM)
		/* front3 af cal*/
		front3_af_cal_pan = *((uint32_t *)&e_ctrl->cal_data.mapdata[FROM_FRONT_AF_CAL_PAN_ADDR]);
		front3_af_cal_macro = *((uint32_t *)&e_ctrl->cal_data.mapdata[FROM_FRONT_AF_CAL_MACRO_ADDR]);
		CAM_DBG(CAM_EEPROM, "front3_af_cal_pan: %d, front3_af_cal_macro: %d",
			front3_af_cal_pan, front3_af_cal_macro);
#endif //!defined(CONFIG_SAMSUNG_FRONT_TOP_EEPROM)

		/* front3 manufacturer info */
		memcpy(front3_fw_ver, &e_ctrl->cal_data.mapdata[FRONT_MODULE_FW_VERSION], FROM_MODULE_FW_INFO_SIZE);
		front3_fw_ver[FROM_MODULE_FW_INFO_SIZE] = '\0';
		CAM_DBG(CAM_EEPROM,
			"front3 manufacturer info = %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x",
			front3_fw_ver[0], front3_fw_ver[1], front3_fw_ver[2], front3_fw_ver[3], front3_fw_ver[4],
			front3_fw_ver[5], front3_fw_ver[6], front3_fw_ver[7], front3_fw_ver[8], front3_fw_ver[9],
			front3_fw_ver[10]);

		/* temp front3 phone version */
		snprintf(front3_phone_fw_ver, FROM_MODULE_FW_INFO_SIZE+1, "%s%s%s%s", front3_hw_phone_info, front3_sw_phone_info, front3_vendor_phone_info, front3_process_phone_info);

		/* temp front3 load version */
		CAM_INFO(CAM_EEPROM, "Load from front EEPROM");
		strcpy(front3_load_fw_ver, front3_fw_ver);
		loadfrom = 'E';

		bVerNull = FALSE;
		for(i = 0; i < FROM_MODULE_FW_INFO_SIZE; i ++) {
			if(e_ctrl->cal_data.mapdata[FRONT_MODULE_FW_VERSION + i] >= 0x80
				|| !isalnum(e_ctrl->cal_data.mapdata[FRONT_MODULE_FW_VERSION + i])) {
				cal_ver[i] = ' ';
				bVerNull = TRUE;
			} else {
				cal_ver[i] = e_ctrl->cal_data.mapdata[FRONT_MODULE_FW_VERSION + i];
			}

			if(front3_phone_fw_ver[i] >= 0x80 || !isalnum(front3_phone_fw_ver[i]))
				front3_phone_fw_ver[i] = ' ';
		}
		sensor_ver[0] = front3_sensor_id[8];
		dll_ver[0] = e_ctrl->cal_data.mapdata[FRONT_DLL_VERSION_ADDR] - '0';

		sensor_ver[1] = 0;
		dll_ver[1] = 0;

		normal_is_supported = CAMERA_NORMAL_CAL_CRC;
		normal_cri_rev = CRITERION_REV;
		strcpy(ideal_ver, front3_phone_fw_ver);
		if(front3_phone_fw_ver[9] < 0x80 && isalnum(front3_phone_fw_ver[9])) {
			ideal_ver[9] = front3_phone_fw_ver[9];
		}
		if(front3_phone_fw_ver[10] < 0x80 && isalnum(front3_phone_fw_ver[10])) {
			ideal_ver[10] = front3_phone_fw_ver[10];
		}

		if(rev < normal_cri_rev && bVerNull == TRUE)
		{
			strcpy(cal_ver, ideal_ver);
			CAM_ERR(CAM_EEPROM, "set tmp ver: %s", cal_ver);
		}

		snprintf(front3_module_info, SYSFS_MODULE_INFO_SIZE, "SSCAL %c%s%04X%04XR%02dM%cD%02XD%02XS%02XS%02X/%s%04X%04XR%02d",
			loadfrom, cal_ver, (e_ctrl->is_supported >> 16) & 0xFFFF, e_ctrl->is_supported & 0xFFFF,
			rev & 0xFF, map_ver, dll_ver[0] & 0xFF, dll_ver[1] & 0xFF, sensor_ver[0] & 0xFF, sensor_ver[1] & 0xFF,
			ideal_ver, (normal_is_supported >> 16) & 0xFFFF, normal_is_supported & 0xFFFF, normal_cri_rev);
#ifdef CAM_EEPROM_DBG
		CAM_INFO(CAM_EEPROM, "front3 info = %s", front3_module_info);
#endif

		/* update EEPROM fw version on sysfs */
		sprintf(front3_cam_fw_ver, "%s %s\n", front3_fw_ver, front3_load_fw_ver);
		sprintf(front3_cam_fw_full_ver, "%s N %s\n", front3_fw_ver, front3_load_fw_ver);
#ifdef CAM_EEPROM_DBG
		CAM_INFO(CAM_EEPROM, "front3 manufacturer info = %c %c %c %c %c %c %c %c %c %c %c",
			front3_fw_ver[0], front3_fw_ver[1], front3_fw_ver[2], front3_fw_ver[3], front3_fw_ver[4],
			front3_fw_ver[5], front3_fw_ver[6], front3_fw_ver[7], front3_fw_ver[8], front3_fw_ver[9],
			front3_fw_ver[10]);
#endif
	}
#endif //#if defined(CONFIG_SAMSUNG_FRONT_TOP)
	else if (e_ctrl->soc_info.index == CAM_EEPROM_IDX_BACK) {
		/* rear sensor id */
		memcpy(rear_sensor_id, &e_ctrl->cal_data.mapdata[FROM_REAR_SENSOR_ID_ADDR], FROM_SENSOR_ID_SIZE);
		rear_sensor_id[FROM_SENSOR_ID_SIZE] = '\0';
		CAM_ERR(CAM_EEPROM,
			"rear sensor id = %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x",
			rear_sensor_id[0], rear_sensor_id[1], rear_sensor_id[2], rear_sensor_id[3],
			rear_sensor_id[4], rear_sensor_id[5], rear_sensor_id[6], rear_sensor_id[7],
			rear_sensor_id[8], rear_sensor_id[9], rear_sensor_id[10], rear_sensor_id[11],
			rear_sensor_id[12], rear_sensor_id[13], rear_sensor_id[14], rear_sensor_id[15]);

#if defined(CONFIG_SAMSUNG_REAR_TRIPLE)
		/* rear3 sensor id */
		memcpy(rear3_sensor_id, &e_ctrl->cal_data.mapdata[FROM_REAR3_SENSOR_ID_ADDR], FROM_SENSOR_ID_SIZE);
		rear3_sensor_id[FROM_SENSOR_ID_SIZE] = '\0';
		CAM_ERR(CAM_EEPROM,
			"rear3 sensor id = %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x",
			rear3_sensor_id[0], rear3_sensor_id[1], rear3_sensor_id[2], rear3_sensor_id[3],
			rear3_sensor_id[4], rear3_sensor_id[5], rear3_sensor_id[6], rear3_sensor_id[7],
			rear3_sensor_id[8], rear3_sensor_id[9], rear3_sensor_id[10], rear3_sensor_id[11],
			rear3_sensor_id[12], rear3_sensor_id[13], rear3_sensor_id[14], rear3_sensor_id[15]);

		/* rear3 module id */
		memcpy(rear3_module_id, &e_ctrl->cal_data.mapdata[FROM_MODULE_ID_ADDR], FROM_MODULE_ID_SIZE);
		rear3_module_id[FROM_MODULE_ID_SIZE] = '\0';
		CAM_ERR(CAM_EEPROM, "rear3_module_id = %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X",
			rear3_module_id[0], rear3_module_id[1], rear3_module_id[2], rear3_module_id[3], rear3_module_id[4],
			rear3_module_id[5], rear3_module_id[6], rear3_module_id[7], rear3_module_id[8], rear3_module_id[9]);

#ifdef CAM_EEPROM_DBG
		CAM_ERR(CAM_EEPROM, "rear3_module_id = %c %c %c %c %c %02X %02X %02X %02X %02X",
			rear3_module_id[0], rear3_module_id[1], rear3_module_id[2], rear3_module_id[3], rear3_module_id[4],
			rear3_module_id[5], rear3_module_id[6], rear3_module_id[7], rear3_module_id[8], rear3_module_id[9]);
#endif

#endif

		/* rear module id */
		memcpy(rear_module_id, &e_ctrl->cal_data.mapdata[FROM_MODULE_ID_ADDR], FROM_MODULE_ID_SIZE);
		rear_module_id[FROM_MODULE_ID_SIZE] = '\0';
		CAM_ERR(CAM_EEPROM, "rear_module_id = %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X",
			rear_module_id[0], rear_module_id[1], rear_module_id[2], rear_module_id[3], rear_module_id[4],
			rear_module_id[5], rear_module_id[6], rear_module_id[7], rear_module_id[8], rear_module_id[9]);

#ifdef CAM_EEPROM_DBG
		CAM_INFO(CAM_EEPROM, "rear_module_id = %c %c %c %c %c %02X %02X %02X %02X %02X",
			rear_module_id[0], rear_module_id[1], rear_module_id[2], rear_module_id[3], rear_module_id[4],
			rear_module_id[5], rear_module_id[6], rear_module_id[7], rear_module_id[8], rear_module_id[9]);
#endif

		/*rear af cal*/
#if  defined(FROM_REAR_AF_CAL_MACRO_ADDR)
		memcpy(&rear_af_cal[0], &e_ctrl->cal_data.mapdata[FROM_REAR_AF_CAL_MACRO_ADDR], 4);
#endif
#if  defined(FROM_REAR_AF_CAL_PAN_ADDR)
		memcpy(&rear_af_cal[9], &e_ctrl->cal_data.mapdata[FROM_REAR_AF_CAL_PAN_ADDR], 4);
#endif
		CAM_DBG(CAM_EEPROM, "rear_af_cal[0] macro: %d, rear_af_cal[9] pan: %d", rear_af_cal[0], rear_af_cal[9]);

		/* rear mtf exif */
		memcpy(rear_mtf_exif, &e_ctrl->cal_data.mapdata[FROM_REAR_MTF_ADDR], FROM_MTF_SIZE);
		rear_mtf_exif[FROM_MTF_SIZE] = '\0';
		CAM_DBG(CAM_EEPROM, "rear mtf exif = %s", rear_mtf_exif);

		/* rear mtf2 exif */
		memcpy(rear_mtf2_exif, &e_ctrl->cal_data.mapdata[FROM_REAR_MTF2_ADDR], FROM_MTF_SIZE);
		rear_mtf2_exif[FROM_MTF_SIZE] = '\0';
		CAM_DBG(CAM_EEPROM, "rear mtf2 exif = %s", rear_mtf2_exif);

		/* rear manufacturer info */
		memcpy(rear_fw_ver, &e_ctrl->cal_data.mapdata[REAR_MODULE_FW_VERSION], FROM_MODULE_FW_INFO_SIZE);
		rear_fw_ver[FROM_MODULE_FW_INFO_SIZE] = '\0';
		CAM_DBG(CAM_EEPROM,
			"rear manufacturer info = %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x",
			rear_fw_ver[0], rear_fw_ver[1], rear_fw_ver[2], rear_fw_ver[3], rear_fw_ver[4],
			rear_fw_ver[5], rear_fw_ver[6], rear_fw_ver[7], rear_fw_ver[8], rear_fw_ver[9],
			rear_fw_ver[10]);

		/* temp phone version */
		snprintf(rear_phone_fw_ver, FROM_MODULE_FW_INFO_SIZE+1, "%s%s%s%s", hw_phone_info, sw_phone_info, vendor_phone_info, process_phone_info);
		rear_phone_fw_ver[FROM_MODULE_FW_INFO_SIZE] = '\0';
		CAM_INFO(CAM_EEPROM,
			"rear rear_phone_fw_ver info = %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x",
			rear_phone_fw_ver[0], rear_phone_fw_ver[1], rear_phone_fw_ver[2], rear_phone_fw_ver[3], rear_phone_fw_ver[4],
			rear_phone_fw_ver[5], rear_phone_fw_ver[6], rear_phone_fw_ver[7], rear_phone_fw_ver[8], rear_phone_fw_ver[9],
			rear_phone_fw_ver[10]);

#ifdef CAM_EEPROM_DBG
		CAM_INFO(CAM_EEPROM, "rear rear_phone_fw_ver info = %c %c %c %c %c %c %c %c %c %c %c",
			rear_phone_fw_ver[0], rear_phone_fw_ver[1], rear_phone_fw_ver[2], rear_phone_fw_ver[3], rear_phone_fw_ver[4],
			rear_phone_fw_ver[5], rear_phone_fw_ver[6], rear_phone_fw_ver[7], rear_phone_fw_ver[8], rear_phone_fw_ver[9],
			rear_phone_fw_ver[10]);
#endif

		/* temp load version */

		if (strncmp(rear_phone_fw_ver, rear_fw_ver, HW_INFO_MAX_SIZE-1) == 0
			&& strncmp(&rear_phone_fw_ver[HW_INFO_MAX_SIZE-1], &rear_fw_ver[HW_INFO_MAX_SIZE-1], SW_INFO_MAX_SIZE-1) >= 0) {
			CAM_INFO(CAM_EEPROM, "Load from phone");
			strcpy(rear_load_fw_ver, rear_phone_fw_ver);
			loadfrom = 'P';
		} else {
			CAM_INFO(CAM_EEPROM, "Load from EEPROM");
			strcpy(rear_load_fw_ver, rear_fw_ver);
			loadfrom = 'E';
		}

		bVerNull = FALSE;
		for(i = 0; i < FROM_MODULE_FW_INFO_SIZE; i ++) {
			if(e_ctrl->cal_data.mapdata[REAR_MODULE_FW_VERSION + i] >= 0x80
				|| !isalnum(e_ctrl->cal_data.mapdata[REAR_MODULE_FW_VERSION + i])) {
				cal_ver[i] = ' ';
				bVerNull = TRUE;
			} else {
				cal_ver[i] = e_ctrl->cal_data.mapdata[REAR_MODULE_FW_VERSION + i];
			}

			if(rear_phone_fw_ver[i] >= 0x80
				|| !isalnum(rear_phone_fw_ver[i]))
				rear_phone_fw_ver[i] = ' ';
		}
		sensor_ver[0] = rear_sensor_id[8];
		dll_ver[0] = e_ctrl->cal_data.mapdata[REAR_DLL_VERSION_ADDR] - '0';
#if defined(CONFIG_SAMSUNG_REAR_TRIPLE)
		sensor_ver[1] = rear3_sensor_id[8];
		dll_ver[1] = e_ctrl->cal_data.mapdata[REAR3_DLL_VERSION_ADDR] - '0';
#else
		sensor_ver[1] = 0;
		dll_ver[1] = 0;
#endif

		normal_is_supported = CAMERA_NORMAL_CAL_CRC;
		normal_cri_rev = CRITERION_REV;
		strcpy(ideal_ver, rear_phone_fw_ver);
		if(rear_fw_ver[9] < 0x80 && isalnum(rear_fw_ver[9])) {
			ideal_ver[9] = rear_fw_ver[9];
		}
		if(rear_fw_ver[10] < 0x80 && isalnum(rear_fw_ver[10])) {
			ideal_ver[10] = rear_fw_ver[10];
		}

		if(rev < normal_cri_rev && bVerNull == TRUE)
		{
			strcpy(cal_ver, ideal_ver);
			loadfrom = 'P';
			CAM_ERR(CAM_EEPROM, "set tmp ver: %s", cal_ver);
		}

		snprintf(module_info, SYSFS_MODULE_INFO_SIZE, "SSCAL %c%s%04X%04XR%02dM%cD%02XD%02XS%02XS%02X/%s%04X%04XR%02d",
			loadfrom, cal_ver, (e_ctrl->is_supported >> 16) & 0xFFFF, e_ctrl->is_supported & 0xFFFF,
			rev & 0xFF, map_ver, dll_ver[0] & 0xFF, dll_ver[1] & 0xFF, sensor_ver[0] & 0xFF, sensor_ver[1] & 0xFF,
			ideal_ver, (normal_is_supported >> 16) & 0xFFFF, normal_is_supported & 0xFFFF, normal_cri_rev);
#ifdef CAM_EEPROM_DBG
		CAM_INFO(CAM_EEPROM, "rear info = %s", module_info);
#endif

		/* update EEPROM fw version on sysfs */
		sprintf(cam_fw_ver, "%s %s\n", rear_fw_ver, rear_load_fw_ver); //need check phone version
		sprintf(cam_fw_full_ver, "%s %s %s\n", rear_fw_ver, rear_phone_fw_ver, rear_load_fw_ver);// needed check phone version.

#ifdef CAM_EEPROM_DBG
		CAM_INFO(CAM_EEPROM, "rear manufacturer info = %c %c %c %c %c %c %c %c %c %c %c",
			rear_fw_ver[0], rear_fw_ver[1], rear_fw_ver[2], rear_fw_ver[3], rear_fw_ver[4],
			rear_fw_ver[5], rear_fw_ver[6], rear_fw_ver[7], rear_fw_ver[8], rear_fw_ver[9],
			rear_fw_ver[10]);
#endif


#if defined(CONFIG_SAMSUNG_REAR_TRIPLE)
		/* rear 3 temp phone version */
		snprintf(rear3_phone_fw_ver, FROM_MODULE_FW_INFO_SIZE+1, "%s%s%s%s", rear3_hw_phone_info, rear3_sw_phone_info, rear3_vendor_phone_info, rear3_process_phone_info);
		rear3_phone_fw_ver[FROM_MODULE_FW_INFO_SIZE] = '\0';
		CAM_INFO(CAM_EEPROM,
			"rear3 rear3_phone_fw_ver info = %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x",
			rear3_phone_fw_ver[0], rear3_phone_fw_ver[1], rear3_phone_fw_ver[2], rear3_phone_fw_ver[3], rear3_phone_fw_ver[4],
			rear3_phone_fw_ver[5], rear3_phone_fw_ver[6], rear3_phone_fw_ver[7], rear3_phone_fw_ver[8], rear3_phone_fw_ver[9],
			rear3_phone_fw_ver[10]);

#ifdef CAM_EEPROM_DBG
		CAM_INFO(CAM_EEPROM, "rear3 rear3_phone_fw_ver info = %c %c %c %c %c %c %c %c %c %c %c",
			rear3_phone_fw_ver[0], rear3_phone_fw_ver[1], rear3_phone_fw_ver[2], rear3_phone_fw_ver[3], rear3_phone_fw_ver[4],
			rear3_phone_fw_ver[5], rear3_phone_fw_ver[6], rear3_phone_fw_ver[7], rear3_phone_fw_ver[8], rear3_phone_fw_ver[9],
			rear3_phone_fw_ver[10]);
#endif

		/* rear 3 temp load version */

		if (strncmp(rear3_phone_fw_ver, rear3_fw_ver, HW_INFO_MAX_SIZE-1) == 0
			&& strncmp(&rear3_phone_fw_ver[HW_INFO_MAX_SIZE-1], &rear3_fw_ver[HW_INFO_MAX_SIZE-1], SW_INFO_MAX_SIZE-1) >= 0) {
			CAM_INFO(CAM_EEPROM, "Load from phone");
			strcpy(rear3_load_fw_ver, rear3_phone_fw_ver);
			loadfrom = 'P';
		} else {
			CAM_INFO(CAM_EEPROM, "Load from EEPROM");
			strcpy(rear3_load_fw_ver, rear3_fw_ver);
			loadfrom = 'E';
		}

		bVerNull = FALSE;
		for(i = 0; i < FROM_MODULE_FW_INFO_SIZE; i ++) {
			if(e_ctrl->cal_data.mapdata[REAR3_MODULE_FW_VERSION + i] >= 0x80
				|| !isalnum(e_ctrl->cal_data.mapdata[REAR3_MODULE_FW_VERSION + i])) {
				cal_ver[i] = ' ';
				bVerNull = TRUE;
			} else {
				cal_ver[i] = e_ctrl->cal_data.mapdata[REAR3_MODULE_FW_VERSION + i];
			}

			if(rear3_phone_fw_ver[i] >= 0x80
				|| !isalnum(rear3_phone_fw_ver[i]))
				rear3_phone_fw_ver[i] = ' ';
		}
		sensor_ver[0] = rear_sensor_id[8];
		dll_ver[0] = e_ctrl->cal_data.mapdata[REAR_DLL_VERSION_ADDR] - '0';
#if defined(CONFIG_SAMSUNG_REAR_TRIPLE)
		sensor_ver[1] = rear3_sensor_id[8];
		dll_ver[1] = e_ctrl->cal_data.mapdata[REAR3_DLL_VERSION_ADDR] - '0';
#else
		sensor_ver[1] = 0;
		dll_ver[1] = 0;
#endif

		normal_is_supported = CAMERA_NORMAL_CAL_CRC;
		normal_cri_rev = CRITERION_REV;
		strcpy(ideal_ver, rear3_phone_fw_ver);
		if(rear3_fw_ver[9] < 0x80 && isalnum(rear3_fw_ver[9])) {
			ideal_ver[9] = rear3_fw_ver[9];
		}
		if(rear3_fw_ver[10] < 0x80 && isalnum(rear3_fw_ver[10])) {
			ideal_ver[10] = rear3_fw_ver[10];
		}

		if(rev < normal_cri_rev && bVerNull == TRUE)
		{
			strcpy(cal_ver, ideal_ver);
			loadfrom = 'P';
			CAM_ERR(CAM_EEPROM, "set tmp ver: %s", cal_ver);
		}

		snprintf(module3_info, SYSFS_MODULE_INFO_SIZE, "SSCAL %c%s%04X%04XR%02dM%cD%02XD%02XS%02XS%02X/%s%04X%04XR%02d",
			loadfrom, cal_ver, (e_ctrl->is_supported >> 16) & 0xFFFF, e_ctrl->is_supported & 0xFFFF,
			rev & 0xFF, map_ver, dll_ver[0] & 0xFF, dll_ver[1] & 0xFF, sensor_ver[0] & 0xFF, sensor_ver[1] & 0xFF,
			ideal_ver, (normal_is_supported >> 16) & 0xFFFF, normal_is_supported & 0xFFFF, normal_cri_rev);
#ifdef CAM_EEPROM_DBG
		CAM_ERR(CAM_EEPROM, "rear3 info = %s", module3_info);
#endif
		/* rear3 manufacturer info */
		memcpy(rear3_fw_ver, &e_ctrl->cal_data.mapdata[REAR3_MODULE_FW_VERSION], FROM_MODULE_FW_INFO_SIZE);
		rear3_fw_ver[FROM_MODULE_FW_INFO_SIZE] = '\0';

		/* update EEPROM fw version on sysfs */
		sprintf(cam3_fw_ver, "%s %s\n", rear3_fw_ver, rear3_load_fw_ver); //need check phone version
		sprintf(cam3_fw_full_ver, "%s N %s\n", rear3_fw_ver, rear3_load_fw_ver );// needed check phone version.



#ifdef CAM_EEPROM_DBG
		CAM_ERR(CAM_EEPROM, "rear3 manufacturer info = %c %c %c %c %c %c %c %c %c %c %c",
			cam3_fw_ver[0], cam3_fw_ver[1], cam3_fw_ver[2], cam3_fw_ver[3], cam3_fw_ver[4],
			cam3_fw_ver[5], cam3_fw_ver[6], cam3_fw_ver[7], cam3_fw_ver[8], cam3_fw_ver[9],
			cam3_fw_ver[10]);
#endif



#endif

#if defined(CONFIG_SAMSUNG_REAR_TRIPLE)
		/* rear3 mtf exif */
		memcpy(rear3_mtf_exif, &e_ctrl->cal_data.mapdata[FROM_REAR3_MTF_ADDR], FROM_MTF_SIZE);
		rear3_mtf_exif[FROM_MTF_SIZE] = '\0';
		CAM_INFO(CAM_EEPROM, "rear3 mtf exif = %s", rear3_mtf_exif);


		if (e_ctrl->cal_data.num_data > 0xAF00) {
			/* rear3 tele dual cal */
			memcpy(rear3_dual_cal, &e_ctrl->cal_data.mapdata[FROM_REAR3_DUAL_CAL_ADDR], FROM_REAR3_DUAL_CAL_SIZE);
			rear3_dual_cal[FROM_REAR3_DUAL_CAL_SIZE] = '\0';
			CAM_INFO(CAM_EEPROM, "rear dual cal = %s", rear3_dual_cal);

			/* rear3 tele tilt */
		memcpy(&rear3_dual_tilt_x, &e_ctrl->cal_data.mapdata[FROM_REAR3_DUAL_TILT_X], 4);
		memcpy(&rear3_dual_tilt_y, &e_ctrl->cal_data.mapdata[FROM_REAR3_DUAL_TILT_Y], 4);
		memcpy(&rear3_dual_tilt_z, &e_ctrl->cal_data.mapdata[FROM_REAR3_DUAL_TILT_Z], 4);
		memcpy(&rear3_dual_tilt_sx, &e_ctrl->cal_data.mapdata[FROM_REAR3_DUAL_TILT_SX], 4);
		memcpy(&rear3_dual_tilt_sy, &e_ctrl->cal_data.mapdata[FROM_REAR3_DUAL_TILT_SY], 4);

		memcpy(&rear3_dual_tilt_range, &e_ctrl->cal_data.mapdata[FROM_REAR3_DUAL_TILT_RANGE], 4);
		memcpy(&rear3_dual_tilt_max_err, &e_ctrl->cal_data.mapdata[FROM_REAR3_DUAL_TILT_MAX_ERR], 4);
		memcpy(&rear3_dual_tilt_avg_err, &e_ctrl->cal_data.mapdata[FROM_REAR3_DUAL_TILT_AVG_ERR], 4);
		memcpy(&rear3_dual_tilt_dll_ver, &e_ctrl->cal_data.mapdata[FROM_REAR3_DUAL_TILT_DLL_VERSION], 4);

			CAM_INFO(CAM_EEPROM,
				"rear3 tele dual tilt x = %d, y = %d, z = %d, sx = %d, sy = %d, range = %d, max_err = %d, avg_err = %d, dll_ver = %d",
			rear3_dual_tilt_x, rear3_dual_tilt_y, rear3_dual_tilt_z, rear3_dual_tilt_sx,
			rear3_dual_tilt_sy, rear3_dual_tilt_range, rear3_dual_tilt_max_err,
			rear3_dual_tilt_avg_err, rear3_dual_tilt_dll_ver);
		} else {
			CAM_INFO(CAM_EEPROM, "this is old module, dual_tilt tests are not supported, will be filled zero");

			/* rear3 tele tilt */
			rear3_dual_tilt_x = 0;
			rear3_dual_tilt_y = 0;
			rear3_dual_tilt_z = 0;
			rear3_dual_tilt_sx = 0;
			rear3_dual_tilt_sy = 0;

			rear3_dual_tilt_range = 0;
			rear3_dual_tilt_max_err = 0;
			rear3_dual_tilt_avg_err = 0;
			rear3_dual_tilt_dll_ver = 0;
		}

		CAM_DBG(CAM_EEPROM, "rear_af_cal == TEST ");
		/*rear af cal*/
#if  defined(FROM_REAR_AF_CAL_MACRO_ADDR)
		memcpy(&rear_af_cal[0], &e_ctrl->cal_data.mapdata[FROM_REAR_AF_CAL_MACRO_ADDR], 4);
#endif
#if  defined(FROM_REAR_AF_CAL_D10_ADDR)
		memcpy(&rear_af_cal[1], &e_ctrl->cal_data.mapdata[FROM_REAR_AF_CAL_D10_ADDR], 4);
#endif
#if  defined(FROM_REAR_AF_CAL_D20_ADDR)
		memcpy(&rear_af_cal[2], &e_ctrl->cal_data.mapdata[FROM_REAR_AF_CAL_D20_ADDR], 4);
#endif
#if  defined(FROM_REAR_AF_CAL_D30_ADDR)
		memcpy(&rear_af_cal[3], &e_ctrl->cal_data.mapdata[FROM_REAR_AF_CAL_D30_ADDR], 4);
#endif
#if  defined(FROM_REAR_AF_CAL_D40_ADDR)
		memcpy(&rear_af_cal[4], &e_ctrl->cal_data.mapdata[FROM_REAR_AF_CAL_D40_ADDR], 4);
#endif
#if  defined(FROM_REAR_AF_CAL_D50_ADDR)
		memcpy(&rear_af_cal[5], &e_ctrl->cal_data.mapdata[FROM_REAR_AF_CAL_D50_ADDR], 4);
#endif
#if  defined(FROM_REAR_AF_CAL_D60_ADDR)
		memcpy(&rear_af_cal[6], &e_ctrl->cal_data.mapdata[FROM_REAR_AF_CAL_D60_ADDR], 4);
#endif
#if  defined(FROM_REAR_AF_CAL_D70_ADDR)
		memcpy(&rear_af_cal[7], &e_ctrl->cal_data.mapdata[FROM_REAR_AF_CAL_D70_ADDR], 4);
#endif
#if  defined(FROM_REAR_AF_CAL_D80_ADDR)
		memcpy(&rear_af_cal[8], &e_ctrl->cal_data.mapdata[FROM_REAR_AF_CAL_D80_ADDR], 4);
#endif
#if  defined(FROM_REAR_AF_CAL_PAN_ADDR)
		memcpy(&rear_af_cal[9], &e_ctrl->cal_data.mapdata[FROM_REAR_AF_CAL_PAN_ADDR], 4);
#endif

		/*rear3 af cal*/
#if  defined(FROM_REAR3_AF_CAL_MACRO_ADDR)
		memcpy(&rear3_af_cal[0], &e_ctrl->cal_data.mapdata[FROM_REAR3_AF_CAL_MACRO_ADDR], 4);
#endif
#if  defined(FROM_REAR3_AF_CAL_D10_ADDR)
		memcpy(&rear3_af_cal[1], &e_ctrl->cal_data.mapdata[FROM_REAR3_AF_CAL_D10_ADDR], 4);
#endif
#if  defined(FROM_REAR3_AF_CAL_D20_ADDR)
		memcpy(&rear3_af_cal[2], &e_ctrl->cal_data.mapdata[FROM_REAR3_AF_CAL_D20_ADDR], 4);
#endif
#if  defined(FROM_REAR3_AF_CAL_D30_ADDR)
		memcpy(&rear3_af_cal[3], &e_ctrl->cal_data.mapdata[FROM_REAR3_AF_CAL_D30_ADDR], 4);
#endif
#if  defined(FROM_REAR3_AF_CAL_D40_ADDR)
		memcpy(&rear3_af_cal[4], &e_ctrl->cal_data.mapdata[FROM_REAR3_AF_CAL_D40_ADDR], 4);
#endif
#if  defined(FROM_REAR3_AF_CAL_D50_ADDR)
		memcpy(&rear3_af_cal[5], &e_ctrl->cal_data.mapdata[FROM_REAR3_AF_CAL_D50_ADDR], 4);
#endif
#if  defined(FROM_REAR3_AF_CAL_D60_ADDR)
		memcpy(&rear3_af_cal[6], &e_ctrl->cal_data.mapdata[FROM_REAR3_AF_CAL_D60_ADDR], 4);
#endif
#if  defined(FROM_REAR3_AF_CAL_D70_ADDR)
		memcpy(&rear3_af_cal[7], &e_ctrl->cal_data.mapdata[FROM_REAR3_AF_CAL_D70_ADDR], 4);
#endif
#if  defined(FROM_REAR3_AF_CAL_D80_ADDR)
		memcpy(&rear3_af_cal[8], &e_ctrl->cal_data.mapdata[FROM_REAR3_AF_CAL_D80_ADDR], 4);
#endif
#if  defined(FROM_REAR3_AF_CAL_PAN_ADDR)
		memcpy(&rear3_af_cal[9], &e_ctrl->cal_data.mapdata[FROM_REAR3_AF_CAL_PAN_ADDR], 4);
#endif
#endif

#if defined(CONFIG_SAMSUNG_REAR_DUAL) || defined(CONFIG_SAMSUNG_REAR_TRIPLE)

			/* rear2 sw dual cal */
			memcpy(rear2_dual_cal, &e_ctrl->cal_data.mapdata[FROM_REAR2_DUAL_CAL_ADDR], FROM_REAR2_DUAL_CAL_SIZE);
			rear2_dual_cal[FROM_REAR2_DUAL_CAL_SIZE] = '\0';
			CAM_INFO(CAM_EEPROM, "rear2 dual cal = %s", rear2_dual_cal);

			/* rear2 sw tilt */
			memcpy(&rear2_dual_tilt_x, &e_ctrl->cal_data.mapdata[FROM_REAR2_DUAL_TILT_X], 4);
			memcpy(&rear2_dual_tilt_y, &e_ctrl->cal_data.mapdata[FROM_REAR2_DUAL_TILT_Y], 4);
			memcpy(&rear2_dual_tilt_z, &e_ctrl->cal_data.mapdata[FROM_REAR2_DUAL_TILT_Z], 4);
			memcpy(&rear2_dual_tilt_sx, &e_ctrl->cal_data.mapdata[FROM_REAR2_DUAL_TILT_SX], 4);
			memcpy(&rear2_dual_tilt_sy, &e_ctrl->cal_data.mapdata[FROM_REAR2_DUAL_TILT_SY], 4);

			memcpy(&rear2_dual_tilt_range, &e_ctrl->cal_data.mapdata[FROM_REAR2_DUAL_TILT_RANGE], 4);
			memcpy(&rear2_dual_tilt_max_err, &e_ctrl->cal_data.mapdata[FROM_REAR2_DUAL_TILT_MAX_ERR], 4);
			memcpy(&rear2_dual_tilt_avg_err, &e_ctrl->cal_data.mapdata[FROM_REAR2_DUAL_TILT_AVG_ERR], 4);
			memcpy(&rear2_dual_tilt_dll_ver, &e_ctrl->cal_data.mapdata[FROM_REAR2_DUAL_TILT_DLL_VERSION], 4);

			CAM_INFO(CAM_EEPROM,
				"rear2 dual tilt x = %d, y = %d, z = %d, sx = %d, sy = %d, range = %d, max_err = %d, avg_err = %d, dll_ver = %d",
				rear2_dual_tilt_x, rear2_dual_tilt_y, rear2_dual_tilt_z, rear2_dual_tilt_sx,
				rear2_dual_tilt_sy, rear2_dual_tilt_range, rear2_dual_tilt_max_err,
				rear2_dual_tilt_avg_err, rear2_dual_tilt_dll_ver);

#endif

		memset(rear_paf_cal_data_mid, 0, sizeof(rear_paf_cal_data_mid));
		for (i = 0; i < FROM_PAF_OFFSET_MID_SIZE / 8 - 1; i++) {
			sprintf(tempbuf, "%d,", *((s16 *)&e_ctrl->cal_data.mapdata[FROM_PAF_OFFSET_MID_ADDR + 8 * i]));
			strncat(rear_paf_cal_data_mid, tempbuf, strlen(tempbuf));
			memset(tempbuf, 0, strlen(tempbuf));
		}
		sprintf(tempbuf, "%d", *((s16 *)&e_ctrl->cal_data.mapdata[FROM_PAF_OFFSET_MID_ADDR + 8 * i]));
		strncat(rear_paf_cal_data_mid, tempbuf, strlen(tempbuf));
		strncat(rear_paf_cal_data_mid, "\n", strlen("\n"));
		CAM_DBG(CAM_EEPROM, "rear_paf_cal_data_mid = %s", rear_paf_cal_data_mid);

		memset(rear_paf_cal_data_far, 0, sizeof(rear_paf_cal_data_far));
		for (i = 0; i < FROM_PAF_OFFSET_FAR_SIZE / 2 - 1; i++) {
			sprintf(tempbuf, "%d,", *((s16 *)&e_ctrl->cal_data.mapdata[FROM_PAF_OFFSET_FAR_ADDR + 2 * i]));
			strncat(rear_paf_cal_data_far, tempbuf, strlen(tempbuf));
			memset(tempbuf, 0, strlen(tempbuf));
		}
		sprintf(tempbuf, "%d", *((s16 *)&e_ctrl->cal_data.mapdata[FROM_PAF_OFFSET_FAR_ADDR + 2 * i]));
		strncat(rear_paf_cal_data_far, tempbuf, strlen(tempbuf));
		strncat(rear_paf_cal_data_far, "\n", strlen("\n"));
		CAM_DBG(CAM_EEPROM, "rear_paf_cal_data_far = %s", rear_paf_cal_data_far);

		memset(rear_f2_paf_cal_data_mid, 0, sizeof(rear_f2_paf_cal_data_mid));
		for (i = 0; i < FROM_PAF_OFFSET_MID_SIZE / 8 - 1; i++) {
			sprintf(tempbuf, "%d,", *((s16 *)&e_ctrl->cal_data.mapdata[FROM_F2_PAF_OFFSET_MID_ADDR + 8 * i]));
			strncat(rear_f2_paf_cal_data_mid, tempbuf, strlen(tempbuf));
			memset(tempbuf, 0, strlen(tempbuf));
		}
		sprintf(tempbuf, "%d", *((s16 *)&e_ctrl->cal_data.mapdata[FROM_F2_PAF_OFFSET_MID_ADDR + 8 * i]));
		strncat(rear_f2_paf_cal_data_mid, tempbuf, strlen(tempbuf));
		strncat(rear_f2_paf_cal_data_mid, "\n", strlen("\n"));
		CAM_DBG(CAM_EEPROM, "rear_f2_paf_cal_data_mid = %s", rear_f2_paf_cal_data_mid);

		memset(rear_f2_paf_cal_data_far, 0, sizeof(rear_f2_paf_cal_data_far));
		for (i = 0; i < FROM_PAF_OFFSET_FAR_SIZE / 2 - 1; i++) {
			sprintf(tempbuf, "%d,", *((s16 *)&e_ctrl->cal_data.mapdata[FROM_F2_PAF_OFFSET_FAR_ADDR + 2 * i]));
			strncat(rear_f2_paf_cal_data_far, tempbuf, strlen(tempbuf));
			memset(tempbuf, 0, strlen(tempbuf));
		}
		sprintf(tempbuf, "%d", *((s16 *)&e_ctrl->cal_data.mapdata[FROM_F2_PAF_OFFSET_FAR_ADDR + 2 * i]));
		strncat(rear_f2_paf_cal_data_far, tempbuf, strlen(tempbuf));
		strncat(rear_f2_paf_cal_data_far, "\n", strlen("\n"));
		CAM_DBG(CAM_EEPROM, "rear_f2_paf_cal_data_far = %s", rear_f2_paf_cal_data_far);

		memset(data, 0, sizeof(data));
		memcpy(data, &e_ctrl->cal_data.mapdata[FROM_PAF_CAL_DATA_START_ADDR + FROM_PAF_CAL_ERR_CHECK_OFFSET], 4);
		paf_err_data_result = *data | ( *(data + 1) << 8) | ( *(data + 2) << 16) | (*(data + 3) << 24);

		memset(data, 0, sizeof(data));
		memcpy(data, &e_ctrl->cal_data.mapdata[FROM_F2_PAF_CAL_DATA_START_ADDR + FROM_PAF_CAL_ERR_CHECK_OFFSET], 4);
		f2_paf_err_data_result = *data | ( *(data + 1) << 8) | ( *(data + 2) << 16) | (*(data + 3) << 24);
#if defined(CONFIG_SAMSUNG_REAR_TRIPLE)
		memset(data, 0, sizeof(data));
		memcpy(data, &e_ctrl->cal_data.mapdata[FROM_REAR3_PAF_CAL_DATA_START_ADDR + FROM_PAF_CAL_ERR_CHECK_OFFSET], 4);
		rear3_paf_err_data_result = *data | ( *(data + 1) << 8) | ( *(data + 2) << 16) | (*(data + 3) << 24);
#endif
#if defined(CONFIG_SAMSUNG_OIS_MCU_STM32)
		ois_wide_cal_mark = e_ctrl->cal_data.mapdata[WIDE_OIS_CAL_MARK_START_ADDR];
		if (ois_wide_cal_mark == 0xBB) {
			ois_gain_rear_result = 0;
			ois_sr_rear_result = 0;
		} else {
			ois_gain_rear_result = 1;
			ois_sr_rear_result = 1;
		}
		memcpy(ois_wide_xygg, &e_ctrl->cal_data.mapdata[WIDE_OIS_XYGG_START_ADDR], OIS_XYGG_SIZE);
		memcpy(ois_wide_xysr, &e_ctrl->cal_data.mapdata[WIDE_OIS_XYSR_START_ADDR], OIS_XYSR_SIZE);

#if defined(CONFIG_SAMSUNG_REAR_TRIPLE)
#if defined(CONFIG_SEC_D2XQ_PROJECT)
		if (e_ctrl->cal_data.num_data > 0xE900) {
#else
		if (e_ctrl->cal_data.num_data > 0xAF00) {
#endif
			memcpy(ois_wide_center_shift, &e_ctrl->cal_data.mapdata[WIDE_OIS_CENTER_SHIFT_START_ADDR], OIS_CENTER_SHIFT_SIZE);
			ois_tele_cal_mark = e_ctrl->cal_data.mapdata[TELE_OIS_CAL_MARK_START_ADDR];
			if (ois_tele_cal_mark == 0xBB) {
				ois_gain_rear3_result = 0;
				ois_sr_rear3_result = 0;
			} else {
				ois_gain_rear3_result = 1;
				ois_sr_rear3_result = 1;
			}
			memcpy(ois_tele_xygg, &e_ctrl->cal_data.mapdata[TELE_OIS_XYGG_START_ADDR], OIS_XYGG_SIZE);
			memcpy(ois_tele_xysr, &e_ctrl->cal_data.mapdata[TELE_OIS_XYSR_START_ADDR], OIS_XYSR_SIZE);
			memcpy(ois_tele_center_shift, &e_ctrl->cal_data.mapdata[TELE_OIS_CENTER_SHIFT_START_ADDR], OIS_CENTER_SHIFT_SIZE);
		}
#endif
#endif
#if defined(CONFIG_SAMSUNG_REAR_TOF)
		/* rear tof dual cal */
		memcpy(rear_tof_dual_cal, &e_ctrl->cal_data.mapdata[REAR_TOF_DUAL_CAL_ADDR], REAR_TOF_DUAL_CAL_SIZE);
		rear_tof_dual_cal[REAR_TOF_DUAL_CAL_SIZE] = '\0';
		CAM_DBG(CAM_EEPROM, "rear tof dual cal = %s", rear_tof_dual_cal);

		/* rear tof tilt */
		memcpy(&rear_tof_dual_tilt_x, &e_ctrl->cal_data.mapdata[REAR_TOF_DUAL_TILT_X], 4);
		memcpy(&rear_tof_dual_tilt_y, &e_ctrl->cal_data.mapdata[REAR_TOF_DUAL_TILT_Y], 4);
		memcpy(&rear_tof_dual_tilt_z, &e_ctrl->cal_data.mapdata[REAR_TOF_DUAL_TILT_Z], 4);
		memcpy(&rear_tof_dual_tilt_sx, &e_ctrl->cal_data.mapdata[REAR_TOF_DUAL_TILT_SX], 4);
		memcpy(&rear_tof_dual_tilt_sy, &e_ctrl->cal_data.mapdata[REAR_TOF_DUAL_TILT_SY], 4);

		memcpy(&rear_tof_dual_tilt_range, &e_ctrl->cal_data.mapdata[REAR_TOF_DUAL_TILT_RANGE], 4);
		memcpy(&rear_tof_dual_tilt_max_err, &e_ctrl->cal_data.mapdata[REAR_TOF_DUAL_TILT_MAX_ERR], 4);
		memcpy(&rear_tof_dual_tilt_avg_err, &e_ctrl->cal_data.mapdata[REAR_TOF_DUAL_TILT_AVG_ERR], 4);
		memcpy(&rear_tof_dual_tilt_dll_ver, &e_ctrl->cal_data.mapdata[REAR_TOF_DUAL_TILT_DLL_VERSION], 4);

		CAM_INFO(CAM_EEPROM,
			"rear tof dual tilt x = %d, y = %d, z = %d, sx = %d, sy = %d, range = %d, max_err = %d, avg_err = %d, dll_ver = %d",
			rear_tof_dual_tilt_x, rear_tof_dual_tilt_y, rear_tof_dual_tilt_z, rear_tof_dual_tilt_sx,
			rear_tof_dual_tilt_sy, rear_tof_dual_tilt_range, rear_tof_dual_tilt_max_err,
			rear_tof_dual_tilt_avg_err, rear_tof_dual_tilt_dll_ver);

		/* rear2 tof tilt */
		memcpy(&rear2_tof_dual_tilt_x, &e_ctrl->cal_data.mapdata[REAR2_TOF_DUAL_TILT_X], 4);
		memcpy(&rear2_tof_dual_tilt_y, &e_ctrl->cal_data.mapdata[REAR2_TOF_DUAL_TILT_Y], 4);
		memcpy(&rear2_tof_dual_tilt_z, &e_ctrl->cal_data.mapdata[REAR2_TOF_DUAL_TILT_Z], 4);
		memcpy(&rear2_tof_dual_tilt_sx, &e_ctrl->cal_data.mapdata[REAR2_TOF_DUAL_TILT_SX], 4);
		memcpy(&rear2_tof_dual_tilt_sy, &e_ctrl->cal_data.mapdata[REAR2_TOF_DUAL_TILT_SY], 4);

		memcpy(&rear2_tof_dual_tilt_range, &e_ctrl->cal_data.mapdata[REAR2_TOF_DUAL_TILT_RANGE], 4);
		memcpy(&rear2_tof_dual_tilt_max_err, &e_ctrl->cal_data.mapdata[REAR2_TOF_DUAL_TILT_MAX_ERR], 4);
		memcpy(&rear2_tof_dual_tilt_avg_err, &e_ctrl->cal_data.mapdata[REAR2_TOF_DUAL_TILT_AVG_ERR], 4);
		memcpy(&rear2_tof_dual_tilt_dll_ver, &e_ctrl->cal_data.mapdata[REAR2_TOF_DUAL_TILT_DLL_VERSION], 4);

		CAM_INFO(CAM_EEPROM,
			"rear2 tof dual tilt x = %d, y = %d, z = %d, sx = %d, sy = %d, range = %d, max_err = %d, avg_err = %d, dll_ver = %d",
			rear2_tof_dual_tilt_x, rear2_tof_dual_tilt_y, rear2_tof_dual_tilt_z, rear2_tof_dual_tilt_sx,
			rear2_tof_dual_tilt_sy, rear2_tof_dual_tilt_range, rear2_tof_dual_tilt_max_err,
			rear2_tof_dual_tilt_avg_err, rear2_tof_dual_tilt_dll_ver);
#endif
	}
#if defined(CONFIG_SAMSUNG_REAR_DUAL) || defined(CONFIG_SAMSUNG_REAR_TRIPLE)
	else if (e_ctrl->soc_info.index == CAM_EEPROM_IDX_BACK2) {
		/* sw sensor id */
		memcpy(rear2_sensor_id, &e_ctrl->cal_data.mapdata[SW_SENSOR_ID_ADDR], FROM_SENSOR_ID_SIZE);
		rear2_sensor_id[FROM_SENSOR_ID_SIZE] = '\0';
		CAM_ERR(CAM_EEPROM,
			"rear2 sensor id = %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x",
			rear2_sensor_id[0], rear2_sensor_id[1], rear2_sensor_id[2], rear2_sensor_id[3],
			rear2_sensor_id[4], rear2_sensor_id[5], rear2_sensor_id[6], rear2_sensor_id[7],
			rear2_sensor_id[8], rear2_sensor_id[9], rear2_sensor_id[10], rear2_sensor_id[11],
			rear2_sensor_id[12], rear2_sensor_id[13], rear2_sensor_id[14], rear2_sensor_id[15]);

		/* rear2 module id */
		memcpy(rear2_module_id, &e_ctrl->cal_data.mapdata[FROM_MODULE_ID_ADDR], FROM_MODULE_ID_SIZE);
		rear2_module_id[FROM_MODULE_ID_SIZE] = '\0';
		CAM_DBG(CAM_EEPROM, "rear2_module_id = %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X",
			rear2_module_id[0], rear2_module_id[1], rear2_module_id[2], rear2_module_id[3], rear2_module_id[4],
			rear2_module_id[5], rear2_module_id[6], rear2_module_id[7], rear2_module_id[8], rear2_module_id[9]);

		/* rear2 manufacturer info */
		memcpy(rear2_fw_ver, &e_ctrl->cal_data.mapdata[SW_MODULE_FW_VERSION], FROM_MODULE_FW_INFO_SIZE);
		rear2_fw_ver[FROM_MODULE_FW_INFO_SIZE] = '\0';
		CAM_ERR(CAM_EEPROM,
			"rear2 manufacturer info = %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x",
			rear2_fw_ver[0], rear2_fw_ver[1], rear2_fw_ver[2], rear2_fw_ver[3], rear2_fw_ver[4],
			rear2_fw_ver[5], rear2_fw_ver[6], rear2_fw_ver[7], rear2_fw_ver[8], rear2_fw_ver[9],
			rear2_fw_ver[10]);

		/* rear2 mtf exif */
		memcpy(rear2_mtf_exif, &e_ctrl->cal_data.mapdata[SW_MTF_ADDR], FROM_MTF_SIZE);
		rear2_mtf_exif[FROM_MTF_SIZE] = '\0';
		CAM_INFO(CAM_EEPROM, "rear2 mtf exif = %s", rear2_mtf_exif);

		/* temp rear2 phone version */
		snprintf(rear2_phone_fw_ver, FROM_MODULE_FW_INFO_SIZE+1, "%s%s%s%s", rear2_hw_phone_info, rear2_sw_phone_info, rear2_vendor_phone_info, rear2_process_phone_info);

		/* temp sw load version */
		CAM_INFO(CAM_EEPROM, "Load from sw EEPROM");
		strcpy(rear2_load_fw_ver, rear2_fw_ver);
		loadfrom = 'E';

		bVerNull = FALSE;
		for(i = 0; i < FROM_MODULE_FW_INFO_SIZE; i ++) {
			if(e_ctrl->cal_data.mapdata[SW_MODULE_FW_VERSION + i] >= 0x80
				|| !isalnum(e_ctrl->cal_data.mapdata[SW_MODULE_FW_VERSION + i])) {
				cal_ver[i] = ' ';
				bVerNull = TRUE;
			} else {
				cal_ver[i] = e_ctrl->cal_data.mapdata[SW_MODULE_FW_VERSION + i];
			}

			if(rear2_phone_fw_ver[i] >= 0x80 || !isalnum(rear2_phone_fw_ver[i]))
				rear2_phone_fw_ver[i] = ' ';
		}
		sensor_ver[0] = rear2_sensor_id[8];
		dll_ver[0] = e_ctrl->cal_data.mapdata[SW_DLL_VERSION_ADDR] - '0';

		sensor_ver[1] = 0;
		dll_ver[1] = 0;

		normal_is_supported = CAMERA_NORMAL_CAL_CRC;
		normal_cri_rev = CRITERION_REV;
		strcpy(ideal_ver, rear2_phone_fw_ver);
		if(rear2_phone_fw_ver[9] < 0x80 && isalnum(rear2_phone_fw_ver[9])) {
			ideal_ver[9] = rear2_phone_fw_ver[9];
		}
		if(rear2_phone_fw_ver[10] < 0x80 && isalnum(rear2_phone_fw_ver[10])) {
			ideal_ver[10] = rear2_phone_fw_ver[10];
		}

		if(rev < normal_cri_rev && bVerNull == TRUE)
		{
			strcpy(cal_ver, ideal_ver);
			CAM_ERR(CAM_EEPROM, "set tmp ver: %s", cal_ver);
		}

		snprintf(module2_info, SYSFS_MODULE_INFO_SIZE, "SSCAL %c%s%04X%04XR%02dM%cD%02XD%02XS%02XS%02X/%s%04X%04XR%02d",
			loadfrom, cal_ver, (e_ctrl->is_supported >> 16) & 0xFFFF, e_ctrl->is_supported & 0xFFFF,
			rev & 0xFF, map_ver, dll_ver[0] & 0xFF, dll_ver[1] & 0xFF, sensor_ver[0] & 0xFF, sensor_ver[1] & 0xFF,
			ideal_ver, (normal_is_supported >> 16) & 0xFFFF, normal_is_supported & 0xFFFF, normal_cri_rev);
#ifdef CAM_EEPROM_DBG
		CAM_INFO(CAM_EEPROM, "sw info = %s", module2_info);
#endif

		/* update EEPROM fw version on sysfs */
		sprintf(cam2_fw_ver, "%s %s\n", rear2_fw_ver, rear2_load_fw_ver);
		sprintf(cam2_fw_full_ver, "%s N %s\n", rear2_fw_ver, rear2_load_fw_ver);
#ifdef CAM_EEPROM_DBG
		CAM_INFO(CAM_EEPROM, "rear2 manufacturer info = %c %c %c %c %c %c %c %c %c %c %c",
			cam2_fw_ver[0], cam2_fw_ver[1], cam2_fw_ver[2], cam2_fw_ver[3], cam2_fw_ver[4],
			cam2_fw_ver[5], cam2_fw_ver[6], cam2_fw_ver[7], cam2_fw_ver[8], cam2_fw_ver[9],
			cam2_fw_ver[10]);
#endif
	}
#endif
#if defined(CONFIG_SAMSUNG_REAR_TOF)
	else if (e_ctrl->soc_info.index == CAM_EEPROM_IDX_BACK_TOF) {
		/* rear tof sensor id */
		memcpy(rear_tof_sensor_id, &e_ctrl->cal_data.mapdata[REAR_TOF_SENSOR_ID_ADDR], FROM_SENSOR_ID_SIZE);
		rear_tof_sensor_id[FROM_SENSOR_ID_SIZE] = '\0';
		CAM_ERR(CAM_EEPROM,
			"rear tof sensor id = %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x",
			rear_tof_sensor_id[0], rear_tof_sensor_id[1], rear_tof_sensor_id[2], rear_tof_sensor_id[3],
			rear_tof_sensor_id[4], rear_tof_sensor_id[5], rear_tof_sensor_id[6], rear_tof_sensor_id[7],
			rear_tof_sensor_id[8], rear_tof_sensor_id[9], rear_tof_sensor_id[10], rear_tof_sensor_id[11],
			rear_tof_sensor_id[12], rear_tof_sensor_id[13], rear_tof_sensor_id[14], rear_tof_sensor_id[15]);

		/* rear tof module id */
		memcpy(rear4_module_id, &e_ctrl->cal_data.mapdata[REAR_TOF_MODULE_ID_ADDR], FROM_MODULE_ID_SIZE);
		rear4_module_id[FROM_MODULE_ID_SIZE] = '\0';
		CAM_DBG(CAM_EEPROM, "rear4_module_id = %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X",
			rear4_module_id[0], rear4_module_id[1], rear4_module_id[2], rear4_module_id[3], rear4_module_id[4],
			rear4_module_id[5], rear4_module_id[6], rear4_module_id[7], rear4_module_id[8], rear4_module_id[9]);

		/* rear tof manufacturer info */
		memcpy(rear_tof_fw_ver, &e_ctrl->cal_data.mapdata[REAR_TOF_MODULE_FW_VERSION], FROM_MODULE_FW_INFO_SIZE);
		rear_tof_fw_ver[FROM_MODULE_FW_INFO_SIZE] = '\0';
		CAM_ERR(CAM_EEPROM,
			"rear tof manufacturer info = %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x",
			rear_tof_fw_ver[0], rear_tof_fw_ver[1], rear_tof_fw_ver[2], rear_tof_fw_ver[3], rear_tof_fw_ver[4],
			rear_tof_fw_ver[5], rear_tof_fw_ver[6], rear_tof_fw_ver[7], rear_tof_fw_ver[8], rear_tof_fw_ver[9],
			rear_tof_fw_ver[10]);

		/* temp rear tof phone version */
		snprintf(rear_tof_phone_fw_ver, FROM_MODULE_FW_INFO_SIZE+1, "%s%s%s%s", rear_tof_hw_phone_info, rear_tof_sw_phone_info, rear_tof_vendor_phone_info, rear_tof_process_phone_info);

		/* temp rear tof load version */
		CAM_INFO(CAM_EEPROM, "Load from rear tof EEPROM");
		strcpy(rear_tof_load_fw_ver, rear_tof_fw_ver);
		loadfrom = 'E';

		bVerNull = FALSE;
		for(i = 0; i < FROM_MODULE_FW_INFO_SIZE; i ++) {
			if(e_ctrl->cal_data.mapdata[REAR_TOF_MODULE_FW_VERSION + i] >= 0x80
				|| !isalnum(e_ctrl->cal_data.mapdata[REAR_TOF_MODULE_FW_VERSION + i])) {
				cal_ver[i] = ' ';
				bVerNull = TRUE;
			} else {
				cal_ver[i] = e_ctrl->cal_data.mapdata[REAR_TOF_MODULE_FW_VERSION + i];
			}

			if(rear_tof_phone_fw_ver[i] >= 0x80 || !isalnum(rear_tof_phone_fw_ver[i]))
				rear_tof_phone_fw_ver[i] = ' ';
		}
		sensor_ver[0] = rear_tof_sensor_id[8];
		dll_ver[0] = e_ctrl->cal_data.mapdata[REAR_TOF_DLL_VERSION_ADDR] - '0';

		sensor_ver[1] = 0;
		dll_ver[1] = 0;

		normal_is_supported = CAMERA_NORMAL_CAL_CRC;
		normal_cri_rev = CRITERION_REV;
		strcpy(ideal_ver, rear_tof_phone_fw_ver);
		if(rear_tof_phone_fw_ver[9] < 0x80 && isalnum(rear_tof_phone_fw_ver[9])) {
			ideal_ver[9] = rear_tof_phone_fw_ver[9];
		}
		if(rear_tof_phone_fw_ver[10] < 0x80 && isalnum(rear_tof_phone_fw_ver[10])) {
			ideal_ver[10] = rear_tof_phone_fw_ver[10];
		}

		if(rev < normal_cri_rev && bVerNull == TRUE)
		{
			strcpy(cal_ver, ideal_ver);
			CAM_ERR(CAM_EEPROM, "set tmp ver: %s", cal_ver);
		}

		snprintf(rear_tof_module_info, SYSFS_MODULE_INFO_SIZE, "SSCAL %c%s%04X%04XR%02dM%cD%02XD%02XS%02XS%02X/%s%04X%04XR%02d",
			loadfrom, cal_ver, (e_ctrl->is_supported >> 16) & 0xFFFF, e_ctrl->is_supported & 0xFFFF,
			rev & 0xFF, map_ver, dll_ver[0] & 0xFF, dll_ver[1] & 0xFF, sensor_ver[0] & 0xFF, sensor_ver[1] & 0xFF,
			ideal_ver, (normal_is_supported >> 16) & 0xFFFF, normal_is_supported & 0xFFFF, normal_cri_rev);
#ifdef CAM_EEPROM_DBG
		CAM_INFO(CAM_EEPROM, "rear tof_info = %s", rear_tof_module_info);
#endif

		/* update EEPROM fw version on sysfs */
		sprintf(cam_tof_fw_ver, "%s %s\n", rear_tof_fw_ver, rear_tof_load_fw_ver);
		sprintf(cam_tof_fw_full_ver, "%s N %s\n", rear_tof_fw_ver, rear_tof_load_fw_ver);
#ifdef CAM_EEPROM_DBG
		CAM_INFO(CAM_EEPROM, "rear tof manufacturer info = %c %c %c %c %c %c %c %c %c %c %c",
			cam_tof_fw_ver[0], cam_tof_fw_ver[1], cam_tof_fw_ver[2], cam_tof_fw_ver[3], cam_tof_fw_ver[4],
			cam_tof_fw_ver[5], cam_tof_fw_ver[6], cam_tof_fw_ver[7], cam_tof_fw_ver[8], cam_tof_fw_ver[9],
			cam_tof_fw_ver[10]);
#endif

		memcpy(&rear_tof_uid, &e_ctrl->cal_data.mapdata[REAR_TOFCAL_UID_ADDR], 4);
		CAM_INFO(CAM_EEPROM, "rear_tof_uid = %d", rear_tof_uid);

		memcpy(rear_tof_cal, &e_ctrl->cal_data.mapdata[REAR_TOFCAL_START_ADDR], REAR_TOFCAL_SIZE);
		rear_tof_cal[REAR_TOFCAL_SIZE] = '\0';
		CAM_DBG(CAM_EEPROM, "rear tof cal = %s", rear_tof_cal);

		memcpy(rear_tof_cal_extra, &e_ctrl->cal_data.mapdata[REAR_TOFCAL_START_ADDR + REAR_TOFCAL_SIZE], REAR_TOFCAL_EXTRA_SIZE);
		rear_tof_cal_extra[REAR_TOFCAL_EXTRA_SIZE] = '\0';
		CAM_DBG(CAM_EEPROM, "rear tof cal ext = %s", rear_tof_cal_extra);

		CAM_INFO(CAM_EEPROM, "FRONT_TOFCAL_RESULT_ADDR 0x%x 0x%x 0x%x",
			e_ctrl->cal_data.mapdata[REAR_TOFCAL_RESULT_ADDR],
			e_ctrl->cal_data.mapdata[REAR_TOFCAL_RESULT_ADDR + 2],
			e_ctrl->cal_data.mapdata[REAR_TOFCAL_RESULT_ADDR + 4]);
		if (e_ctrl->cal_data.mapdata[REAR_TOFCAL_RESULT_ADDR] == 0x11 &&
			e_ctrl->cal_data.mapdata[REAR_TOFCAL_RESULT_ADDR + 2] == 0x11 &&
			e_ctrl->cal_data.mapdata[REAR_TOFCAL_RESULT_ADDR + 4] == 0x11)
			rear_tof_cal_result = 1;
	}
#endif
#if defined(CONFIG_SAMSUNG_FRONT_TOF)
	else if (e_ctrl->soc_info.index == CAM_EEPROM_IDX_FRONT_TOF) {
		/* front tof sensor id */
		memcpy(front_tof_sensor_id, &e_ctrl->cal_data.mapdata[FRONT_TOF_SENSOR_ID_ADDR], FROM_SENSOR_ID_SIZE);
		front_tof_sensor_id[FROM_SENSOR_ID_SIZE] = '\0';
		CAM_ERR(CAM_EEPROM,
			"front tof sensor id = %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x",
			front_tof_sensor_id[0], front_tof_sensor_id[1], front_tof_sensor_id[2], front_tof_sensor_id[3],
			front_tof_sensor_id[4], front_tof_sensor_id[5], front_tof_sensor_id[6], front_tof_sensor_id[7],
			front_tof_sensor_id[8], front_tof_sensor_id[9], front_tof_sensor_id[10], front_tof_sensor_id[11],
			front_tof_sensor_id[12], front_tof_sensor_id[13], front_tof_sensor_id[14], front_tof_sensor_id[15]);

		/* front tof module id */
		memcpy(front2_module_id, &e_ctrl->cal_data.mapdata[FRONT_TOF_MODULE_ID_ADDR], FROM_MODULE_ID_SIZE);
		front2_module_id[FROM_MODULE_ID_SIZE] = '\0';
		CAM_DBG(CAM_EEPROM, "front2_module_id = %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X",
			front2_module_id[0], front2_module_id[1], front2_module_id[2], front2_module_id[3], front2_module_id[4],
			front2_module_id[5], front2_module_id[6], front2_module_id[7], front2_module_id[8], front2_module_id[9]);

		/* front tof manufacturer info */
		memcpy(front_tof_fw_ver, &e_ctrl->cal_data.mapdata[FRONT_TOF_MODULE_FW_VERSION], FROM_MODULE_FW_INFO_SIZE);
		front_tof_fw_ver[FROM_MODULE_FW_INFO_SIZE] = '\0';
		CAM_ERR(CAM_EEPROM,
			"front tof manufacturer info = %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x",
			front_tof_fw_ver[0], front_tof_fw_ver[1], front_tof_fw_ver[2], front_tof_fw_ver[3], front_tof_fw_ver[4],
			front_tof_fw_ver[5], front_tof_fw_ver[6], front_tof_fw_ver[7], front_tof_fw_ver[8], front_tof_fw_ver[9],
			front_tof_fw_ver[10]);

		/* temp front tof phone version */
		snprintf(front_tof_phone_fw_ver, FROM_MODULE_FW_INFO_SIZE+1, "%s%s%s%s", front_tof_hw_phone_info, front_tof_sw_phone_info, front_tof_vendor_phone_info, front_tof_process_phone_info);

		/* temp front tof load version */
		CAM_INFO(CAM_EEPROM, "Load from front tof EEPROM");
		strcpy(front_tof_load_fw_ver, front_tof_fw_ver);
		loadfrom = 'E';

		bVerNull = FALSE;
		for(i = 0; i < FROM_MODULE_FW_INFO_SIZE; i ++) {
			if(e_ctrl->cal_data.mapdata[FRONT_TOF_MODULE_FW_VERSION + i] >= 0x80
				|| !isalnum(e_ctrl->cal_data.mapdata[FRONT_TOF_MODULE_FW_VERSION + i])) {
				cal_ver[i] = ' ';
				bVerNull = TRUE;
			} else {
				cal_ver[i] = e_ctrl->cal_data.mapdata[FRONT_TOF_MODULE_FW_VERSION + i];
			}

			if(front_tof_phone_fw_ver[i] >= 0x80 || !isalnum(front_tof_phone_fw_ver[i]))
				front_tof_phone_fw_ver[i] = ' ';
		}
		sensor_ver[0] = front_tof_sensor_id[8];
		dll_ver[0] = e_ctrl->cal_data.mapdata[FRONT_TOF_DLL_VERSION_ADDR] - '0';

		sensor_ver[1] = 0;
		dll_ver[1] = 0;

		normal_is_supported = CAMERA_NORMAL_CAL_CRC;
		normal_cri_rev = CRITERION_REV;
		strcpy(ideal_ver, front_tof_phone_fw_ver);
		if(front_tof_phone_fw_ver[9] < 0x80 && isalnum(front_tof_phone_fw_ver[9])) {
			ideal_ver[9] = front_phone_fw_ver[9];
		}
		if(front_tof_phone_fw_ver[10] < 0x80 && isalnum(front_tof_phone_fw_ver[10])) {
			ideal_ver[10] = front_tof_phone_fw_ver[10];
		}

		if(rev < normal_cri_rev && bVerNull == TRUE)
		{
			strcpy(cal_ver, ideal_ver);
			CAM_ERR(CAM_EEPROM, "set tmp ver: %s", cal_ver);
		}

		snprintf(front_tof_module_info, SYSFS_MODULE_INFO_SIZE, "SSCAL %c%s%04X%04XR%02dM%cD%02XD%02XS%02XS%02X/%s%04X%04XR%02d",
			loadfrom, cal_ver, (e_ctrl->is_supported >> 16) & 0xFFFF, e_ctrl->is_supported & 0xFFFF,
			rev & 0xFF, map_ver, dll_ver[0] & 0xFF, dll_ver[1] & 0xFF, sensor_ver[0] & 0xFF, sensor_ver[1] & 0xFF,
			ideal_ver, (normal_is_supported >> 16) & 0xFFFF, normal_is_supported & 0xFFFF, normal_cri_rev);
#ifdef CAM_EEPROM_DBG
		CAM_INFO(CAM_EEPROM, "front tof_info = %s", front_tof_module_info);
#endif

		/* update EEPROM fw version on sysfs */
		sprintf(front_tof_cam_fw_ver, "%s %s\n", front_tof_fw_ver, front_tof_load_fw_ver);
		sprintf(front_tof_cam_fw_full_ver, "%s N %s\n", front_tof_fw_ver, front_tof_load_fw_ver);
#ifdef CAM_EEPROM_DBG
		CAM_INFO(CAM_EEPROM, "front tof manufacturer info = %c %c %c %c %c %c %c %c %c %c %c",
			front_tof_fw_ver[0], front_tof_fw_ver[1], front_tof_fw_ver[2], front_tof_fw_ver[3], front_tof_fw_ver[4],
			front_tof_fw_ver[5], front_tof_fw_ver[6], front_tof_fw_ver[7], front_tof_fw_ver[8], front_tof_fw_ver[9],
			front_tof_fw_ver[10]);
#endif

		if (e_ctrl->cal_data.mapdata[FRONT_TOF_CAM_MAP_VERSION_ADDR] < '5') {
			CAM_INFO(CAM_EEPROM, "invalid front tof uid(map_ver %c), force chage to 0xCB29",
				e_ctrl->cal_data.mapdata[FRONT_TOF_CAM_MAP_VERSION_ADDR]);
			front_tof_uid = 52009; // 0xCB29
		}
		else
			memcpy(&front_tof_uid, &e_ctrl->cal_data.mapdata[FRONT_TOFCAL_UID_ADDR], 4);
		CAM_INFO(CAM_EEPROM, "front tof uid = %d", front_tof_uid);

		memcpy(front_tof_cal, &e_ctrl->cal_data.mapdata[FRONT_TOFCAL_START_ADDR], FRONT_TOFCAL_SIZE);
		front_tof_cal[FRONT_TOFCAL_SIZE] = '\0';
		CAM_DBG(CAM_EEPROM, "front tof cal = %s", front_tof_cal);

		memcpy(front_tof_cal_extra, &e_ctrl->cal_data.mapdata[FRONT_TOFCAL_START_ADDR + FRONT_TOFCAL_SIZE], FRONT_TOFCAL_EXTRA_SIZE);
		front_tof_cal_extra[FRONT_TOFCAL_EXTRA_SIZE] = '\0';
		CAM_DBG(CAM_EEPROM, "front tof cal ext = %s", front_tof_cal_extra);

		CAM_INFO(CAM_EEPROM, "FRONT_TOFCAL_RESULT_ADDR 0x%x 0x%x 0x%x",
			e_ctrl->cal_data.mapdata[FRONT_TOFCAL_RESULT_ADDR],
			e_ctrl->cal_data.mapdata[FRONT_TOFCAL_RESULT_ADDR + 2],
			e_ctrl->cal_data.mapdata[FRONT_TOFCAL_RESULT_ADDR + 4]);
		if (e_ctrl->cal_data.mapdata[FRONT_TOFCAL_RESULT_ADDR] == 0x11 &&
			e_ctrl->cal_data.mapdata[FRONT_TOFCAL_RESULT_ADDR + 2] == 0x11 &&
			e_ctrl->cal_data.mapdata[FRONT_TOFCAL_RESULT_ADDR + 4] == 0x11)
			front_tof_cal_result = 1;
	}
#endif


	/*rear cal check*/
	if (e_ctrl->soc_info.index == CAM_EEPROM_IDX_FRONT)
		rc = cam_eeprom_check_firmware_cal(e_ctrl->is_supported, map_ver, CAM_EEPROM_IDX_FRONT);
#if defined(CONFIG_SAMSUNG_FRONT_TOP)
	else if (e_ctrl->soc_info.index == CAM_EEPROM_IDX_FRONT2)
		rc = cam_eeprom_check_firmware_cal(e_ctrl->is_supported, map_ver, CAM_EEPROM_IDX_FRONT2);
#endif
	else if (e_ctrl->soc_info.index == CAM_EEPROM_IDX_BACK)
		rc = cam_eeprom_check_firmware_cal(e_ctrl->is_supported, map_ver, CAM_EEPROM_IDX_BACK);
#if defined(CONFIG_SAMSUNG_REAR_DUAL) || defined(CONFIG_SAMSUNG_REAR_TRIPLE)
	else if (e_ctrl->soc_info.index == CAM_EEPROM_IDX_BACK2)
		rc = cam_eeprom_check_firmware_cal(e_ctrl->is_supported, map_ver, CAM_EEPROM_IDX_BACK2);
#endif
#if defined(CONFIG_SAMSUNG_REAR_TOF)
	else if (e_ctrl->soc_info.index == CAM_EEPROM_IDX_BACK_TOF)
		rc = cam_eeprom_check_firmware_cal(e_ctrl->is_supported, map_ver, CAM_EEPROM_IDX_BACK_TOF);
#endif
#if defined(CONFIG_SAMSUNG_FRONT_TOF)
	else if (e_ctrl->soc_info.index == CAM_EEPROM_IDX_FRONT_TOF)
		rc = cam_eeprom_check_firmware_cal(e_ctrl->is_supported, map_ver, CAM_EEPROM_IDX_FRONT_TOF);
#endif

	return rc;
}

void cam_eeprom_update_sysfs_fw_version(
	const char *update_fw_ver, cam_eeprom_fw_version_idx update_fw_index, cam_eeprom_idx_type idx)
{
	char *pCAM_fw_version, *pCAM_fw_full_version;
	char *pEEPROM_fw_version, *pPHONE_fw_version, *pLOAD_fw_version;

	CAM_INFO(CAM_EEPROM, "camera_idx: %d, update_fw_ver: %s, update_fw_index: %d",
		idx, update_fw_ver, update_fw_index);

	if (idx == CAM_EEPROM_IDX_BACK) {
		pEEPROM_fw_version = rear_fw_ver;
		pPHONE_fw_version = rear_phone_fw_ver;
		pLOAD_fw_version = rear_load_fw_ver;
		pCAM_fw_version = cam_fw_ver;
		pCAM_fw_full_version = cam_fw_full_ver;
	} else if (idx == CAM_EEPROM_IDX_FRONT) {
		pEEPROM_fw_version = front_fw_ver;
		pPHONE_fw_version = "N";
		pLOAD_fw_version = front_load_fw_ver;
		pCAM_fw_version = front_cam_fw_ver;
		pCAM_fw_full_version = front_cam_fw_full_ver;
	}
#if defined(CONFIG_SAMSUNG_REAR_DUAL) || defined(CONFIG_SAMSUNG_REAR_TRIPLE)
	else if (idx == CAM_EEPROM_IDX_BACK2) {
		pEEPROM_fw_version = rear2_fw_ver;
		pPHONE_fw_version = "N";
		pLOAD_fw_version = rear2_load_fw_ver;
		pCAM_fw_version = cam2_fw_ver;
		pCAM_fw_full_version = cam2_fw_full_ver;
	}
#endif
#if defined(CONFIG_SAMSUNG_FRONT_TOP)
	else if (idx == CAM_EEPROM_IDX_FRONT2) {
		pEEPROM_fw_version = front3_fw_ver;
		pPHONE_fw_version = "N";
		pLOAD_fw_version = front3_load_fw_ver;
		pCAM_fw_version = front3_cam_fw_ver;
		pCAM_fw_full_version = front3_cam_fw_full_ver;
	}
#endif
#if defined(CONFIG_SAMSUNG_REAR_TOF)
	else if (idx == CAM_EEPROM_IDX_BACK_TOF) {
		pEEPROM_fw_version = rear_tof_fw_ver;
		pPHONE_fw_version = "N";
		pLOAD_fw_version = rear_tof_load_fw_ver;
		pCAM_fw_version = cam_tof_fw_ver;
		pCAM_fw_full_version = cam_tof_fw_full_ver;
	}
#endif
#if defined(CONFIG_SAMSUNG_FRONT_TOF)
	else if (idx == CAM_EEPROM_IDX_FRONT_TOF) {
		pEEPROM_fw_version = front_tof_fw_ver;
		pPHONE_fw_version = "N";
		pLOAD_fw_version = front_tof_load_fw_ver;
		pCAM_fw_version = front_tof_cam_fw_ver;
		pCAM_fw_full_version = front_tof_cam_fw_full_ver;
	}
#endif
	else {
		pEEPROM_fw_version = "N";
		pPHONE_fw_version = "N";
		pLOAD_fw_version = "N";
		pCAM_fw_version = "NULL NULL\n";
		pCAM_fw_full_version = "NULL NULL NULL\n";
	}

	if (update_fw_index == EEPROM_FW_VER)
		strlcpy(pEEPROM_fw_version, update_fw_ver, FROM_MODULE_FW_INFO_SIZE + 1);
	else if (update_fw_index == PHONE_FW_VER)
		strlcpy(pPHONE_fw_version, update_fw_ver, FROM_MODULE_FW_INFO_SIZE + 1);
	else
		strlcpy(pLOAD_fw_version, update_fw_ver, FROM_MODULE_FW_INFO_SIZE + 1);

	sprintf(pCAM_fw_version, "%s %s\n", pEEPROM_fw_version, pLOAD_fw_version);
	sprintf(pCAM_fw_full_version, "%s %s %s\n", pEEPROM_fw_version, pPHONE_fw_version, pLOAD_fw_version);

	CAM_ERR(CAM_EEPROM, "camera_idx: %d, pCAM_fw_full_version: %s", idx, pCAM_fw_full_version);
}

int32_t cam_eeprom_check_firmware_cal(uint32_t camera_cal_crc, uint8_t cal_map_version, cam_eeprom_idx_type idx)
{
	int rc = 0;
	char final_cmd_ack[SYSFS_FW_VER_SIZE] = "NG_";
	char rear_cam_cal_ack[SYSFS_FW_VER_SIZE] = "NULL";
	char front_cal_ack[SYSFS_FW_VER_SIZE] = "NULL";
#if defined(CONFIG_SAMSUNG_REAR_DUAL) || defined(CONFIG_SAMSUNG_REAR_TRIPLE)
	char rear2_cal_ack[SYSFS_FW_VER_SIZE] = "NULL";
#endif
#if defined(CONFIG_SAMSUNG_FRONT_TOP)
	char front3_cal_ack[SYSFS_FW_VER_SIZE] = "NULL";
#endif
#if defined(CONFIG_SAMSUNG_REAR_TOF)
	char rear_tof_cal_ack[SYSFS_FW_VER_SIZE] = "NULL";
#endif
#if defined(CONFIG_SAMSUNG_FRONT_TOF)
	char front_tof_cal_ack[SYSFS_FW_VER_SIZE] = "NULL";
#endif
	uint8_t isNeedUpdate = TRUE;
	uint8_t version_isp = 0, version_module_maker_ver = 0;
	uint8_t isValid_EEPROM_data = TRUE;
	uint8_t isQCmodule = TRUE;
	uint8_t camera_cal_ack = OK;
	uint8_t camera_fw_crc = OK;
	uint8_t camera_fw_ack = OK;

	if (idx == CAM_EEPROM_IDX_BACK) {
		version_isp = rear_fw_ver[3];
		version_module_maker_ver = rear_fw_ver[10];
	} else if (idx == CAM_EEPROM_IDX_FRONT) {
		version_isp = front_fw_ver[3];
		version_module_maker_ver = front_fw_ver[10];
	}
#if defined(CONFIG_SAMSUNG_REAR_DUAL) || defined(CONFIG_SAMSUNG_REAR_TRIPLE)
	else if (idx == CAM_EEPROM_IDX_BACK2) {
		version_isp = rear2_fw_ver[3];
		version_module_maker_ver = rear2_fw_ver[10];
	}
#endif
#if defined(CONFIG_SAMSUNG_FRONT_TOP)
	else if (idx == CAM_EEPROM_IDX_FRONT2) {
		version_isp = front3_fw_ver[3];
		version_module_maker_ver = front3_fw_ver[10];
	}
#endif
#if defined(CONFIG_SAMSUNG_REAR_TOF)
	else if (idx == CAM_EEPROM_IDX_BACK_TOF) {
		version_isp = cam_tof_fw_ver[3];
		version_module_maker_ver = cam_tof_fw_ver[10];
	}
#endif
#if defined(CONFIG_SAMSUNG_FRONT_TOF)
	else if (idx == CAM_EEPROM_IDX_FRONT_TOF) {
		version_isp = front_tof_fw_ver[3];
		version_module_maker_ver = front_tof_fw_ver[10];
	}
#endif
	else {
		CAM_INFO(CAM_EEPROM, "subdev_id: %d is not supported", idx);
		return 0;
	}

	if (version_isp == 0xff || version_module_maker_ver == 0xff) {
		CAM_ERR(CAM_EEPROM, "invalid eeprom data");
		isValid_EEPROM_data = FALSE;
		cam_eeprom_update_sysfs_fw_version("NULL", EEPROM_FW_VER, idx);
	}

	/* 1. check camera firmware and cal data */
	CAM_INFO(CAM_EEPROM, "camera_cal_crc: 0x%x, camera_fw_crc: 0x%x", camera_cal_crc, camera_fw_crc);

	if (idx == CAM_EEPROM_IDX_BACK) {
		if (camera_fw_crc == OK)
			camera_fw_ack = OK;
		else {
			camera_fw_ack = CRASH;
			strncat(final_cmd_ack, "FW", 2);
		}
	} else
		camera_fw_ack = OK;

	if (idx == CAM_EEPROM_IDX_BACK) {
		if (camera_cal_crc == CAMERA_NORMAL_CAL_CRC) {
			camera_cal_ack = OK;
			strncpy(rear_cam_cal_ack, "Normal", SYSFS_FW_VER_SIZE);
		} else {
			camera_cal_ack = CRASH;
			strncpy(rear_cam_cal_ack, "Abnormal", SYSFS_FW_VER_SIZE);
#if defined(CONFIG_SAMSUNG_REAR_TRIPLE)
			if ((camera_cal_crc & CAMERA_CAL_CRC_WIDE) != CAMERA_CAL_CRC_WIDE)
				strncat(final_cmd_ack, "CD", 2);
			else
				strncat(final_cmd_ack, "CD4", 3);
#else
			strncat(final_cmd_ack, "CD", 2);
#endif
		}
	} else if (idx == CAM_EEPROM_IDX_FRONT) {
#if defined(UNUSE_FRONT_EEPROM)
		strncpy(front_cal_ack, "NULL", SYSFS_FW_VER_SIZE);
#else
		if (camera_cal_crc == CAMERA_NORMAL_CAL_CRC) {
			camera_cal_ack = OK;
			strncpy(front_cal_ack, "Normal", SYSFS_FW_VER_SIZE);
		} else {
			camera_cal_ack = CRASH;
			strncpy(front_cal_ack, "Abnormal", SYSFS_FW_VER_SIZE);
			strncat(final_cmd_ack, "CD3", 3);
		}
#endif
	}
#if defined(CONFIG_SAMSUNG_REAR_DUAL) || defined(CONFIG_SAMSUNG_REAR_TRIPLE)
	else if (idx == CAM_EEPROM_IDX_BACK2) {
		if (camera_cal_crc == CAMERA_NORMAL_CAL_CRC) {
			camera_cal_ack = OK;
			strncpy(rear2_cal_ack, "Normal", SYSFS_FW_VER_SIZE);
		} else {
			camera_cal_ack = CRASH;
			strncpy(rear2_cal_ack, "Abnormal", SYSFS_FW_VER_SIZE);
			strncat(final_cmd_ack, "CD3", 3);
		}
	}
#endif
#if defined(CONFIG_SAMSUNG_FRONT_TOP)
    else if (idx == CAM_EEPROM_IDX_FRONT2) {
#if defined(UNUSE_FRONT_EEPROM)
		strncpy(front3_cal_ack, "NULL", SYSFS_FW_VER_SIZE);
#else
		if (camera_cal_crc == CAMERA_NORMAL_CAL_CRC) {
			camera_cal_ack = OK;
			strncpy(front3_cal_ack, "Normal", SYSFS_FW_VER_SIZE);
		} else {
			camera_cal_ack = CRASH;
			strncpy(front3_cal_ack, "Abnormal", SYSFS_FW_VER_SIZE);
			strncat(final_cmd_ack, "CD3", 3);
		}
#endif
	}
#endif
#if defined(CONFIG_SAMSUNG_REAR_TOF)
	else if (idx == CAM_EEPROM_IDX_BACK_TOF) {
		if (camera_cal_crc == CAMERA_NORMAL_CAL_CRC) {
			camera_cal_ack = OK;
			strncpy(rear_tof_cal_ack, "Normal", SYSFS_FW_VER_SIZE);
		} else {
			camera_cal_ack = CRASH;
			strncpy(rear_tof_cal_ack, "Abnormal", SYSFS_FW_VER_SIZE);
			strncat(final_cmd_ack, "CD3", 3);
		}
	}
#endif
#if defined(CONFIG_SAMSUNG_FRONT_TOF)
	else if (idx == CAM_EEPROM_IDX_FRONT_TOF) {
		if (camera_cal_crc == CAMERA_NORMAL_CAL_CRC) {
			camera_cal_ack = OK;
			strncpy(front_tof_cal_ack, "Normal", SYSFS_FW_VER_SIZE);
		} else {
			camera_cal_ack = CRASH;
			strncpy(front_tof_cal_ack, "Abnormal", SYSFS_FW_VER_SIZE);
			strncat(final_cmd_ack, "CD3", 3);
		}
	}
#endif

	/* 3-1. all success case: display LOAD FW */
	if (camera_fw_ack && camera_cal_ack)
		isNeedUpdate = FALSE;

	/* 3-2. fail case: update CMD_ACK on sysfs (load fw) */
	// If not QC module, return NG.
	if(version_isp >= 0x80 || !isalnum(version_isp))
		CAM_INFO(CAM_EEPROM, "ISP Ver : 0x%x", version_isp);
	else
		CAM_INFO(CAM_EEPROM, "ISP Ver : %c", version_isp);

	if (version_isp != 'Q' && version_isp != 'U' && version_isp != 'A' && version_isp != 'X') {
		CAM_ERR(CAM_EEPROM, "This is not Qualcomm module!");

		if (idx == CAM_EEPROM_IDX_BACK) {
			strncpy(final_cmd_ack, "NG_FWCD", SYSFS_FW_VER_SIZE);
			strncpy(rear_cam_cal_ack, "Abnormal", SYSFS_FW_VER_SIZE);
		} else if (idx == CAM_EEPROM_IDX_FRONT) {
			strncpy(final_cmd_ack, "NG_CD3_L", SYSFS_FW_VER_SIZE);
			strncpy(front_cal_ack, "Abnormal", SYSFS_FW_VER_SIZE);
		}
#if defined(CONFIG_SAMSUNG_REAR_DUAL) || defined(CONFIG_SAMSUNG_REAR_TRIPLE)
		else if (idx == CAM_EEPROM_IDX_BACK2) {
			strncpy(final_cmd_ack, "NG_CD3_L", SYSFS_FW_VER_SIZE);
			strncpy(rear2_cal_ack, "Abnormal", SYSFS_FW_VER_SIZE);
		}
#endif
#if defined(CONFIG_SAMSUNG_FRONT_TOP)
		else if (idx == CAM_EEPROM_IDX_FRONT2) {
			strncpy(final_cmd_ack, "NG_CD3_L", SYSFS_FW_VER_SIZE);
			strncpy(front3_cal_ack, "Abnormal", SYSFS_FW_VER_SIZE);
		}
#endif
#if defined(CONFIG_SAMSUNG_REAR_TOF)
		else if (idx == CAM_EEPROM_IDX_BACK_TOF) {
			strncpy(final_cmd_ack, "NG_CD3_L", SYSFS_FW_VER_SIZE);
			strncpy(rear_tof_cal_ack, "Abnormal", SYSFS_FW_VER_SIZE);
		}
#endif
#if defined(CONFIG_SAMSUNG_FRONT_TOF)
		else if (idx == CAM_EEPROM_IDX_FRONT_TOF) {
			strncpy(final_cmd_ack, "NG_CD3_L", SYSFS_FW_VER_SIZE);
			strncpy(front_tof_cal_ack, "Abnormal", SYSFS_FW_VER_SIZE);
		}
#endif
		isNeedUpdate = TRUE;
		isQCmodule = FALSE;
		camera_cal_ack = CRASH;
	}

	if (isNeedUpdate) {
		CAM_ERR(CAM_EEPROM, "final_cmd_ack : %s", final_cmd_ack);
		cam_eeprom_update_sysfs_fw_version(final_cmd_ack, LOAD_FW_VER, idx);
	} else {
		// just display success fw version log
		CAM_INFO(CAM_EEPROM, "final_cmd_ack : %s", final_cmd_ack);
		memset(final_cmd_ack, 0, sizeof(final_cmd_ack));
		if (idx == CAM_EEPROM_IDX_BACK)
			strcpy(final_cmd_ack, cam_fw_full_ver);
		else if (idx == CAM_EEPROM_IDX_FRONT)
			strcpy(final_cmd_ack, front_cam_fw_full_ver);
#if defined(CONFIG_SAMSUNG_REAR_DUAL) || defined(CONFIG_SAMSUNG_REAR_TRIPLE)
		else if (idx == CAM_EEPROM_IDX_BACK2)
			strcpy(final_cmd_ack, cam2_fw_full_ver);
#endif
#if defined(CONFIG_SAMSUNG_FRONT_TOP)
		else if (idx == CAM_EEPROM_IDX_FRONT2)
			strcpy(final_cmd_ack, front3_cam_fw_full_ver);
#endif
#if defined(CONFIG_SAMSUNG_REAR_TOF)
		else if (idx == CAM_EEPROM_IDX_BACK_TOF)
			strcpy(final_cmd_ack, cam_tof_fw_full_ver);
#endif
#if defined(CONFIG_SAMSUNG_FRONT_TOF)
		else if (idx == CAM_EEPROM_IDX_FRONT_TOF)
			strcpy(final_cmd_ack, front_tof_cam_fw_full_ver);
#endif
		CAM_INFO(CAM_EEPROM, "final_cmd_ack : %s", final_cmd_ack);
	}

	/* 4. update CAL check ack on sysfs rear_calcheck */
	if (idx == CAM_EEPROM_IDX_BACK)
		strlcpy(rear_cam_cal_check, rear_cam_cal_ack, SYSFS_FW_VER_SIZE);
	else if (idx == CAM_EEPROM_IDX_FRONT)
		strlcpy(front_cam_cal_check, front_cal_ack, SYSFS_FW_VER_SIZE);
#if defined(CONFIG_SAMSUNG_REAR_DUAL) || defined(CONFIG_SAMSUNG_REAR_TRIPLE)
	else if (idx == CAM_EEPROM_IDX_BACK2)
		strlcpy(rear2_cam_cal_check, rear2_cal_ack, SYSFS_FW_VER_SIZE);
#endif
#if defined(CONFIG_SAMSUNG_FRONT_TOP)
	else if (idx == CAM_EEPROM_IDX_FRONT2)
		strlcpy(front_cam_cal_check, front3_cal_ack, SYSFS_FW_VER_SIZE);
#endif
#if defined(CONFIG_SAMSUNG_REAR_TOF)
	else if (idx == CAM_EEPROM_IDX_BACK_TOF)
		strlcpy(rear_tof_cam_cal_check, rear_tof_cal_ack, SYSFS_FW_VER_SIZE);
#endif
#if defined(CONFIG_SAMSUNG_FRONT_TOF)
	else if (idx == CAM_EEPROM_IDX_FRONT_TOF)
		strlcpy(front_tof_cam_cal_check, front_tof_cal_ack, SYSFS_FW_VER_SIZE);
#endif
	snprintf(cal_crc, SYSFS_FW_VER_SIZE, "%s %s\n", rear_cam_cal_check, front_cam_cal_check);

	/* 5. update module maker ver check on sysfs checkfw_user, checkfw_factory*/
	if (idx == CAM_EEPROM_IDX_BACK) {
		CAM_INFO(CAM_EEPROM,
			"version_module_maker: 0x%x, MODULE_VER_ON_PVR: 0x%x, MODULE_VER_ON_SRA: 0x%x",
			version_module_maker_ver, MODULE_VER_ON_PVR, MODULE_VER_ON_SRA);
		CAM_INFO(CAM_EEPROM,
			"cal_map_version: 0x%x vs FROM_CAL_MAP_VERSION: 0x%x",
			cal_map_version, FROM_CAL_MAP_VERSION);

		if ((isQCmodule == TRUE) && ((isValid_EEPROM_data == FALSE) || (cal_map_version < FROM_CAL_MAP_VERSION)
			|| (version_module_maker_ver < MODULE_VER_ON_PVR))) {
			strncpy(cam_fw_user_ver, "NG", SYSFS_FW_VER_SIZE);
		} else {
			if (camera_cal_ack == CRASH)
				strncpy(cam_fw_user_ver, "NG", SYSFS_FW_VER_SIZE);
			else
				strncpy(cam_fw_user_ver, "OK", SYSFS_FW_VER_SIZE);
		}

		if ((isQCmodule == TRUE) && ((isValid_EEPROM_data == FALSE) || (cal_map_version < FROM_CAL_MAP_VERSION)
			|| (version_module_maker_ver < MODULE_VER_ON_SRA))) {
			strncpy(cam_fw_factory_ver, "NG_VER", SYSFS_FW_VER_SIZE);
		} else {
			if (camera_cal_ack == CRASH)
				strncpy(cam_fw_factory_ver, "NG_VER", SYSFS_FW_VER_SIZE);
			else
				strncpy(cam_fw_factory_ver, "OK", SYSFS_FW_VER_SIZE);
		}
	} else if (idx == CAM_EEPROM_IDX_FRONT) {
		CAM_INFO(CAM_EEPROM,
			"front_version_module_maker: 0x%x, FRONT_MODULE_VER_ON_PVR: 0x%x, FRONT_MODULE_VER_ON_SRA: 0x%x",
			version_module_maker_ver, FRONT_MODULE_VER_ON_PVR, FRONT_MODULE_VER_ON_SRA);
		CAM_INFO(CAM_EEPROM,
			"front_cal_map_version: 0x%x vs FRONT_FROM_CAL_MAP_VERSION: 0x%x",
			cal_map_version, FRONT_FROM_CAL_MAP_VERSION);

		if ((isQCmodule == TRUE) && ((isValid_EEPROM_data == FALSE) || (cal_map_version < FRONT_FROM_CAL_MAP_VERSION)
			|| (version_module_maker_ver < FRONT_MODULE_VER_ON_PVR))) {
			strncpy(front_cam_fw_user_ver, "NG", SYSFS_FW_VER_SIZE);
		} else {
			if (camera_cal_ack == CRASH)
				strncpy(front_cam_fw_user_ver, "NG", SYSFS_FW_VER_SIZE);
			else
				strncpy(front_cam_fw_user_ver, "OK", SYSFS_FW_VER_SIZE);
		}

		if ((isQCmodule == TRUE) && ((isValid_EEPROM_data == FALSE) || (cal_map_version < FRONT_FROM_CAL_MAP_VERSION)
			|| (version_module_maker_ver < FRONT_MODULE_VER_ON_SRA))) {
			strncpy(front_cam_fw_factory_ver, "NG_VER", SYSFS_FW_VER_SIZE);
		} else {
			if (camera_cal_ack == CRASH)
				strncpy(front_cam_fw_factory_ver, "NG_CRC", SYSFS_FW_VER_SIZE);
			else
				strncpy(front_cam_fw_factory_ver, "OK", SYSFS_FW_VER_SIZE);
		}
	}
#if defined(CONFIG_SAMSUNG_REAR_DUAL) || defined(CONFIG_SAMSUNG_REAR_TRIPLE)
	else if (idx == CAM_EEPROM_IDX_BACK2) {
		CAM_INFO(CAM_EEPROM,
			"sw_version_module_maker: 0x%x, SW_MODULE_VER_ON_PVR: 0x%x, SW_MODULE_VER_ON_SRA: 0x%x",
			version_module_maker_ver, SW_MODULE_VER_ON_PVR, SW_MODULE_VER_ON_SRA);
		CAM_INFO(CAM_EEPROM,
			"sw_cal_map_version: 0x%x vs SW_FROM_CAL_MAP_VERSION: 0x%x",
			cal_map_version, SW_FROM_CAL_MAP_VERSION);

		if ((isQCmodule == TRUE) && ((isValid_EEPROM_data == FALSE) || (cal_map_version < SW_FROM_CAL_MAP_VERSION)
			|| (version_module_maker_ver < SW_MODULE_VER_ON_PVR))) {
			strncpy(cam2_fw_user_ver, "NG", SYSFS_FW_VER_SIZE);
		} else {
			if (camera_cal_ack == CRASH)
				strncpy(cam2_fw_user_ver, "NG", SYSFS_FW_VER_SIZE);
			else
				strncpy(cam2_fw_user_ver, "OK", SYSFS_FW_VER_SIZE);
		}

		if ((isQCmodule == TRUE) && ((isValid_EEPROM_data == FALSE) || (cal_map_version < SW_FROM_CAL_MAP_VERSION)
			|| (version_module_maker_ver < SW_MODULE_VER_ON_SRA))) {
			strncpy(cam2_fw_factory_ver, "NG_VER", SYSFS_FW_VER_SIZE);
		} else {
			if (camera_cal_ack == CRASH)
				strncpy(cam2_fw_factory_ver, "NG_CRC", SYSFS_FW_VER_SIZE);
			else
				strncpy(cam2_fw_factory_ver, "OK", SYSFS_FW_VER_SIZE);
		}
	}
#endif
#if defined(CONFIG_SAMSUNG_FRONT_TOP)
	else if (idx == CAM_EEPROM_IDX_FRONT2) {
		CAM_INFO(CAM_EEPROM,
			"front3_version_module_maker: 0x%x, FRONT3_MODULE_VER_ON_PVR: 0x%x, FRONT3_MODULE_VER_ON_SRA: 0x%x",
			version_module_maker_ver, FRONT_MODULE_VER_ON_PVR, FRONT_MODULE_VER_ON_SRA);
		CAM_INFO(CAM_EEPROM,
			"front3_cal_map_version: 0x%x vs FRONT_FROM_CAL_MAP_VERSION: 0x%x",
			cal_map_version, FRONT_FROM_CAL_MAP_VERSION);

		if ((isQCmodule == TRUE) && ((isValid_EEPROM_data == FALSE) || (cal_map_version < FRONT_FROM_CAL_MAP_VERSION)
			|| (version_module_maker_ver < FRONT_MODULE_VER_ON_PVR))) {
			strncpy(front3_cam_fw_user_ver, "NG", SYSFS_FW_VER_SIZE);
		} else {
			if (camera_cal_ack == CRASH)
				strncpy(front3_cam_fw_user_ver, "NG", SYSFS_FW_VER_SIZE);
			else
				strncpy(front3_cam_fw_user_ver, "OK", SYSFS_FW_VER_SIZE);
		}

		if ((isQCmodule == TRUE) && ((isValid_EEPROM_data == FALSE) || (cal_map_version < FRONT_FROM_CAL_MAP_VERSION)
			|| (version_module_maker_ver < FRONT_MODULE_VER_ON_SRA))) {
			strncpy(front3_cam_fw_factory_ver, "NG_VER", SYSFS_FW_VER_SIZE);
		} else {
			if (camera_cal_ack == CRASH)
				strncpy(front3_cam_fw_factory_ver, "NG_CRC", SYSFS_FW_VER_SIZE);
			else
				strncpy(front3_cam_fw_factory_ver, "OK", SYSFS_FW_VER_SIZE);
		}
	}
#endif
#if defined(CONFIG_SAMSUNG_REAR_TOF)
	else if (idx == CAM_EEPROM_IDX_BACK_TOF) {
		CAM_INFO(CAM_EEPROM,
			"rear_tof_version_module_maker: 0x%x, REAR_TOF_MODULE_VER_ON_PVR: 0x%x, REAR_TOF_MODULE_VER_ON_SRA: 0x%x",
			version_module_maker_ver, REAR_TOF_MODULE_VER_ON_PVR, REAR_TOF_MODULE_VER_ON_SRA);
		CAM_INFO(CAM_EEPROM,
			"rear_tof_cal_map_version: 0x%x vs REAR_TOF_FROM_CAL_MAP_VERSION: 0x%x",
			cal_map_version, REAR_TOF_FROM_CAL_MAP_VERSION);

		if ((isQCmodule == TRUE) && ((isValid_EEPROM_data == FALSE) || (cal_map_version < REAR_TOF_FROM_CAL_MAP_VERSION)
			|| (version_module_maker_ver < REAR_TOF_MODULE_VER_ON_PVR))) {
			strncpy(cam_tof_fw_user_ver, "NG", SYSFS_FW_VER_SIZE);
		} else {
			if (camera_cal_ack == CRASH)
				strncpy(cam_tof_fw_user_ver, "NG", SYSFS_FW_VER_SIZE);
			else
				strncpy(cam_tof_fw_user_ver, "OK", SYSFS_FW_VER_SIZE);
		}

		if ((isQCmodule == TRUE) && ((isValid_EEPROM_data == FALSE) || (cal_map_version < REAR_TOF_FROM_CAL_MAP_VERSION)
			|| (version_module_maker_ver < REAR_TOF_MODULE_VER_ON_SRA))) {
			strncpy(cam_tof_fw_factory_ver, "NG_VER", SYSFS_FW_VER_SIZE);
		} else {
			if (camera_cal_ack == CRASH)
				strncpy(cam_tof_fw_factory_ver, "NG_CRC", SYSFS_FW_VER_SIZE);
			else
				strncpy(cam_tof_fw_factory_ver, "OK", SYSFS_FW_VER_SIZE);
		}
	}
#endif
#if defined(CONFIG_SAMSUNG_FRONT_TOF)
	else if (idx == CAM_EEPROM_IDX_FRONT_TOF) {
		CAM_INFO(CAM_EEPROM,
			"front_tof_version_module_maker: 0x%x, FRONT_TOF_MODULE_VER_ON_PVR: 0x%x, FRONT_TOF_MODULE_VER_ON_SRA: 0x%x",
			version_module_maker_ver, FRONT_TOF_MODULE_VER_ON_PVR, FRONT_TOF_MODULE_VER_ON_SRA);
		CAM_INFO(CAM_EEPROM,
			"front_tof_cal_map_version: 0x%x vs FRONT_FROM_CAL_MAP_VERSION: 0x%x",
			cal_map_version, FRONT_TOF_FROM_CAL_MAP_VERSION);

		if ((isQCmodule == TRUE) && ((isValid_EEPROM_data == FALSE) || (cal_map_version < FRONT_TOF_FROM_CAL_MAP_VERSION)
			|| (version_module_maker_ver < FRONT_TOF_MODULE_VER_ON_PVR))) {
			strncpy(front_tof_cam_fw_user_ver, "NG", SYSFS_FW_VER_SIZE);
		} else {
			if (camera_cal_ack == CRASH)
				strncpy(front_tof_cam_fw_user_ver, "NG", SYSFS_FW_VER_SIZE);
			else
				strncpy(front_tof_cam_fw_user_ver, "OK", SYSFS_FW_VER_SIZE);
		}

		if ((isQCmodule == TRUE) && ((isValid_EEPROM_data == FALSE) || (cal_map_version < FRONT_TOF_FROM_CAL_MAP_VERSION)
			|| (version_module_maker_ver < FRONT_TOF_MODULE_VER_ON_SRA))) {
			strncpy(front_tof_cam_fw_factory_ver, "NG_VER", SYSFS_FW_VER_SIZE);
		} else {
			if (camera_cal_ack == CRASH)
				strncpy(front_tof_cam_fw_factory_ver, "NG_CRC", SYSFS_FW_VER_SIZE);
			else
				strncpy(front_tof_cam_fw_factory_ver, "OK", SYSFS_FW_VER_SIZE);
		}
	}
#endif

	return rc;
}
/**
 * cam_eeprom_verify_sum - verify crc32 checksum
 * @mem:			data buffer
 * @size:			size of data buffer
 * @sum:			expected checksum
 * @rev_endian:	compare reversed endian (0:little, 1:big)
 *
 * Returns 0 if checksum match, -EINVAL otherwise.
 */
static int cam_eeprom_verify_sum(const char *mem, uint32_t size, uint32_t sum, uint32_t rev_endian)
{
	uint32_t crc = ~0;
	uint32_t cmp_crc = 0;

	/* check overflow */
	if (size > crc - sizeof(uint32_t))
		return -EINVAL;

	crc = crc32_le(crc, mem, size);

	crc = ~crc;
	if (rev_endian == 1) {
		cmp_crc = (((crc) & 0xFF) << 24)
				| (((crc) & 0xFF00) << 8)
				| (((crc) >> 8) & 0xFF00)
				| ((crc) >> 24);
	} else {
		cmp_crc = crc;
	}
	CAM_DBG(CAM_EEPROM, "endian %d, expect 0x%x, result 0x%x", rev_endian, sum, cmp_crc);

	if (cmp_crc != sum) {
		CAM_ERR(CAM_EEPROM, "endian %d, expect 0x%x, result 0x%x", rev_endian, sum, cmp_crc);
		return -EINVAL;
	} else {
		CAM_ERR(CAM_EEPROM, "checksum pass 0x%x", sum);
		return 0;
	}
}

/**
 * cam_eeprom_match_crc - verify multiple regions using crc
 * @data:	data block to be verified
 *
 * Iterates through all regions stored in @data.  Regions with odd index
 * are treated as data, and its next region is treated as checksum.  Thus
 * regions of even index must have valid_size of 4 or 0 (skip verification).
 * Returns a bitmask of verified regions, starting from LSB.  1 indicates
 * a checksum match, while 0 indicates checksum mismatch or not verified.
 */
static uint32_t cam_eeprom_match_crc(struct cam_eeprom_memory_block_t *data, uint32_t subdev_id)
{
	int j, rc;
	uint32_t *sum;
	uint32_t ret = 0;
	uint8_t *memptr, *memptr_crc;
	struct cam_eeprom_memory_map_t *map;

	if (!data) {
		CAM_ERR(CAM_EEPROM, "data is NULL");
		return -EINVAL;
	}
	map = data->map;

#if 1
{
	uint8_t map_ver = 0;

	if (subdev_id == CAM_EEPROM_IDX_FRONT) // Front cam
		map_ver = data->mapdata[FRONT_CAM_MAP_VERSION_ADDR];
	else if (subdev_id == CAM_EEPROM_IDX_BACK) //Rear cam
		map_ver = data->mapdata[REAR_CAM_MAP_VERSION_ADDR];
#if defined(CONFIG_SAMSUNG_REAR_DUAL) || defined(CONFIG_SAMSUNG_REAR_TRIPLE)
	else if (subdev_id == CAM_EEPROM_IDX_BACK2) //SW cam
		map_ver = data->mapdata[SW_CAM_MAP_VERSION_ADDR];
#endif
#if defined(CONFIG_SAMSUNG_FRONT_TOP)
	else if (subdev_id == CAM_EEPROM_IDX_FRONT2) // Front Upper(TOP) cam
		map_ver = data->mapdata[FRONT_CAM_MAP_VERSION_ADDR];
#endif
#if defined(CONFIG_SAMSUNG_REAR_TOF)
	else if (subdev_id == CAM_EEPROM_IDX_BACK_TOF)
		map_ver = data->mapdata[REAR_TOF_CAM_MAP_VERSION_ADDR];
#endif
#if defined(CONFIG_SAMSUNG_FRONT_TOF)
	else if (subdev_id == CAM_EEPROM_IDX_FRONT_TOF)
		map_ver = data->mapdata[FRONT_TOF_CAM_MAP_VERSION_ADDR];
#endif
	else {
		CAM_INFO(CAM_EEPROM, "subdev_id: %d is not supported", subdev_id);
		return 0;
	}

	if(map_ver >= 0x80 || !isalnum(map_ver))
		CAM_INFO(CAM_EEPROM, "map subdev_id = %d, version = 0x%x", subdev_id, map_ver);
	else
		CAM_INFO(CAM_EEPROM, "map subdev_id = %d, version = %c [0x%x]", subdev_id, map_ver, map_ver);
}
#endif

	//  idx 0 is the actual reading section (whole data)
	//  from idx 1, start to compare CRC checksum
	//  (1: CRC area for header, 2: CRC value)
	for (j = 1; j + 1 < data->num_map; j += 2) {
		memptr = data->mapdata + map[j].mem.addr;
		memptr_crc = data->mapdata + map[j+1].mem.addr;

		/* empty table or no checksum */
		if (!map[j].mem.valid_size || !map[j+1].mem.valid_size) {
			CAM_ERR(CAM_EEPROM, "continue");
			continue;
		}

		if (map[j+1].mem.valid_size < sizeof(uint32_t)) {
			CAM_ERR(CAM_EEPROM, "[%d : size 0x%X] malformatted data mapping", j+1, map[j+1].mem.valid_size);
			return -EINVAL;
		}
		CAM_DBG(CAM_EEPROM, "[%d] memptr 0x%x, memptr_crc 0x%x", j, map[j].mem.addr, map[j + 1].mem.addr);
		sum = (uint32_t *) (memptr_crc + map[j+1].mem.valid_size - sizeof(uint32_t));
		rc = cam_eeprom_verify_sum(memptr, map[j].mem.valid_size, *sum, 0);

		if (!rc)
			ret |= 1 << (j/2);
	}

#if 0
	//  if PAF cal data has error (even though CRC is correct),
	//  set crc value of PAF cal data to 0.
	if(subdev_id != 1 && data->mapdata[REAR_MODULE_FW_VERSION+10] == 'M')
	{
		uint32_t PAF_err = 0;
		uint32_t PAF_bit = 0;

		PAF_err = *((uint32_t *)(&data->mapdata[FROM_PAF_CAL_DATA_START_ADDR + 0x14]));
		if(PAF_err != 0)
		{
			PAF_bit |= 0x08;	//	refer to the start addr of PAF cal data at the calibration map
			CAM_ERR(CAM_EEPROM, "Wide1 PAF_err = 0x%08X, Wide1 PAF_bit = 0x%08X", PAF_err, PAF_bit);
		}

		PAF_err = *((uint32_t *)(&data->mapdata[FROM_F2_PAF_CAL_DATA_START_ADDR + 0x14]));
		if(PAF_err != 0)
		{
			PAF_bit |= 0x20;
			CAM_ERR(CAM_EEPROM, "Wide2 PAF_err = 0x%08X, Wide2 PAF_bit = 0x%08X", PAF_err, PAF_bit);
		}

#if defined(CONFIG_SAMSUNG_REAR_TRIPLE)
		PAF_err = *((uint32_t *)(&data->mapdata[FROM_REAR3_PAF_CAL_DATA_START_ADDR + 0x14]));
		if(PAF_err != 0)
		{
			PAF_bit |= 0x8000;
			CAM_ERR(CAM_EEPROM, "Tele PAF_err = 0x%08X, Tele PAF_bit = 0x%08X", PAF_err, PAF_bit);
		}
#endif

		ret &= ~PAF_bit;
	}
#endif

	CAM_INFO(CAM_EEPROM, "CRC result = 0x%08X", ret);

	return ret;
}


/**
 * cam_eeprom_read_memory() - read map data into buffer
 * @e_ctrl:     eeprom control struct
 * @block:      block to be read
 *
 * This function iterates through blocks stored in block->map, reads each
 * region and concatenate them into the pre-allocated block->mapdata
 */
static int cam_eeprom_read_memory(struct cam_eeprom_ctrl_t *e_ctrl,
	struct cam_eeprom_memory_block_t *block)
{
	int                                rc = 0;
	int                                j;
	struct cam_sensor_i2c_reg_setting  i2c_reg_settings = {0};
	struct cam_sensor_i2c_reg_array    i2c_reg_array = {0};
	struct cam_eeprom_memory_map_t    *emap = block->map;
	struct cam_eeprom_soc_private     *eb_info = NULL;
	uint8_t                           *memptr = block->mapdata;
#if 1
	uint32_t                          addr = 0, size = 0, read_size = 0;
#endif

	if (!e_ctrl) {
		CAM_ERR(CAM_EEPROM, "e_ctrl is NULL");
		return -EINVAL;
	}

	eb_info = (struct cam_eeprom_soc_private *)e_ctrl->soc_info.soc_private;

	for (j = 0; j < block->num_map; j++) {
		CAM_DBG(CAM_EEPROM, "slave-addr = 0x%X", emap[j].saddr);
		if (emap[j].saddr) {
			eb_info->i2c_info.slave_addr = emap[j].saddr;
			rc = cam_eeprom_update_i2c_info(e_ctrl,
				&eb_info->i2c_info);
			if (rc) {
				CAM_ERR(CAM_EEPROM,
					"failed: to update i2c info rc %d",
					rc);
				return rc;
			}
		}

		if (emap[j].page.valid_size) {
			i2c_reg_settings.addr_type = emap[j].page.addr_type;
			i2c_reg_settings.data_type = emap[j].page.data_type;
			i2c_reg_settings.size = 1;
			i2c_reg_array.reg_addr = emap[j].page.addr;
			i2c_reg_array.reg_data = emap[j].page.data;
			i2c_reg_array.delay = emap[j].page.delay;
			i2c_reg_settings.reg_setting = &i2c_reg_array;
			rc = camera_io_dev_write(&e_ctrl->io_master_info,
				&i2c_reg_settings);
			if (rc) {
				CAM_ERR(CAM_EEPROM, "page write failed rc %d",
					rc);
				return rc;
			}
		}

		if (emap[j].pageen.valid_size) {
			i2c_reg_settings.addr_type = emap[j].pageen.addr_type;
			i2c_reg_settings.data_type = emap[j].pageen.data_type;
			i2c_reg_settings.size = 1;
			i2c_reg_array.reg_addr = emap[j].pageen.addr;
			i2c_reg_array.reg_data = emap[j].pageen.data;
			i2c_reg_array.delay = emap[j].pageen.delay;
			i2c_reg_settings.reg_setting = &i2c_reg_array;
			rc = camera_io_dev_write(&e_ctrl->io_master_info,
				&i2c_reg_settings);
			if (rc) {
				CAM_ERR(CAM_EEPROM, "page enable failed rc %d",
					rc);
				return rc;
			}
		}

		if (emap[j].poll.valid_size) {
			rc = camera_io_dev_poll(&e_ctrl->io_master_info,
				emap[j].poll.addr, emap[j].poll.data,
				0, emap[j].poll.addr_type,
				emap[j].poll.data_type,
				emap[j].poll.delay);
			if (rc) {
				CAM_ERR(CAM_EEPROM, "poll failed rc %d",
					rc);
				return rc;
			}
		}

		if (emap[j].mem.valid_size) {
#if 1
			size = emap[j].mem.valid_size;
			addr = emap[j].mem.addr;
			memptr = block->mapdata + addr;

			CAM_DBG(CAM_EEPROM, "[%d / %d] memptr = %p, addr = 0x%X, size = 0x%X, subdev = %d",
				j, block->num_map, memptr, emap[j].mem.addr, emap[j].mem.valid_size, e_ctrl->soc_info.index);

			CAM_DBG(CAM_EEPROM, "addr_type = %d, data_type = %d, device_type = %d",
				emap[j].mem.addr_type, emap[j].mem.data_type, e_ctrl->eeprom_device_type);
			if ((e_ctrl->eeprom_device_type == MSM_CAMERA_SPI_DEVICE
				|| e_ctrl->eeprom_device_type == MSM_CAMERA_I2C_DEVICE)
				&& emap[j].mem.data_type == 0) {
				CAM_DBG(CAM_EEPROM,
					"skipping read as data_type 0, skipped:%d",
					read_size);
				continue;
			}

			while(size > 0) {
				read_size = size;
				if (size > I2C_REG_DATA_MAX) {
					read_size = I2C_REG_DATA_MAX;
				}
				rc = camera_io_dev_read_seq(&e_ctrl->io_master_info,
					addr, memptr,
					emap[j].mem.addr_type,
					emap[j].mem.data_type,
					read_size);
				if (rc < 0) {
					CAM_ERR(CAM_EEPROM, "read failed rc %d",
						rc);
					return rc;
				}
				size -= read_size;
				addr += read_size;
				memptr += read_size;
			}
#else
			rc = camera_io_dev_read_seq(&e_ctrl->io_master_info,
				emap[j].mem.addr, memptr,
				emap[j].mem.data_type,
				emap[j].mem.valid_size);
			if (rc < 0) {
				CAM_ERR(CAM_EEPROM, "read failed rc %d",
					rc);
				return rc;
			}
			memptr += emap[j].mem.valid_size;
#endif
		}

		if (emap[j].pageen.valid_size) {
			i2c_reg_settings.addr_type = emap[j].pageen.addr_type;
			i2c_reg_settings.data_type = emap[j].pageen.data_type;
			i2c_reg_settings.size = 1;
			i2c_reg_array.reg_addr = emap[j].pageen.addr;
			i2c_reg_array.reg_data = 0;
			i2c_reg_array.delay = emap[j].pageen.delay;
			i2c_reg_settings.reg_setting = &i2c_reg_array;
			rc = camera_io_dev_write(&e_ctrl->io_master_info,
				&i2c_reg_settings);
			if (rc < 0) {
				CAM_ERR(CAM_EEPROM,
					"page disable failed rc %d",
					rc);
				return rc;
			}
		}
	}
	return rc;
}

/**
 * cam_eeprom_power_up - Power up eeprom hardware
 * @e_ctrl:     ctrl structure
 * @power_info: power up/down info for eeprom
 *
 * Returns success or failure
 */
static int cam_eeprom_power_up(struct cam_eeprom_ctrl_t *e_ctrl,
	struct cam_sensor_power_ctrl_t *power_info)
{
	int32_t                 rc = 0;
	struct cam_hw_soc_info *soc_info =
		&e_ctrl->soc_info;
	int32_t                 SLEEP_MS = 3;

	/* Parse and fill vreg params for power up settings */
	rc = msm_camera_fill_vreg_params(
		&e_ctrl->soc_info,
		power_info->power_setting,
		power_info->power_setting_size);
	if (rc) {
		CAM_ERR(CAM_EEPROM,
			"failed to fill power up vreg params rc:%d", rc);
		return rc;
	}

	/* Parse and fill vreg params for power down settings*/
	rc = msm_camera_fill_vreg_params(
		&e_ctrl->soc_info,
		power_info->power_down_setting,
		power_info->power_down_setting_size);
	if (rc) {
		CAM_ERR(CAM_EEPROM,
			"failed to fill power down vreg params  rc:%d", rc);
		return rc;
	}

	power_info->dev = soc_info->dev;

	rc = cam_sensor_core_power_up(power_info, soc_info);
	if (rc) {
		CAM_ERR(CAM_EEPROM, "failed in eeprom power up rc %d", rc);
		return rc;
	}
	/* EEPROM i2c can start at least 1ms after VIO power on according to spec. */
	if (SLEEP_MS > 20) // due to deviation of msleep
		msleep(SLEEP_MS);
	else if (SLEEP_MS)
		usleep_range(SLEEP_MS * 1000, (SLEEP_MS * 1000) + 1000);

	if (e_ctrl->io_master_info.master_type == CCI_MASTER) {
		rc = camera_io_init(&(e_ctrl->io_master_info));
		if (rc) {
			CAM_ERR(CAM_EEPROM, "cci_init failed");
			return -EINVAL;
		}
	}
	return rc;
}

/**
 * cam_eeprom_power_down - Power down eeprom hardware
 * @e_ctrl:    ctrl structure
 *
 * Returns success or failure
 */
static int cam_eeprom_power_down(struct cam_eeprom_ctrl_t *e_ctrl)
{
	struct cam_sensor_power_ctrl_t *power_info;
	struct cam_hw_soc_info         *soc_info;
	struct cam_eeprom_soc_private  *soc_private;
	int                             rc = 0;

	if (!e_ctrl) {
		CAM_ERR(CAM_EEPROM, "failed: e_ctrl %pK", e_ctrl);
		return -EINVAL;
	}

	soc_private =
		(struct cam_eeprom_soc_private *)e_ctrl->soc_info.soc_private;
	power_info = &soc_private->power_info;
	soc_info = &e_ctrl->soc_info;

	if (!power_info) {
		CAM_ERR(CAM_EEPROM, "failed: power_info %pK", power_info);
		return -EINVAL;
	}
#if defined(CONFIG_SENSOR_RETENTION)
	rc = cam_sensor_util_power_down(power_info, soc_info, 0);
#else
	rc = cam_sensor_util_power_down(power_info, soc_info);
#endif
	if (rc) {
		CAM_ERR(CAM_EEPROM, "power down the core is failed:%d", rc);
		return rc;
	}

	if (e_ctrl->io_master_info.master_type == CCI_MASTER)
		camera_io_release(&(e_ctrl->io_master_info));

	return rc;
}

#if defined(CONFIG_EEPROM_FORCE_DOWN)
/**
 * cam_eeprom_force_power_down - Power down eeprom hardware forcely
 * @e_ctrl:    ctrl structure
 *
 * Returns success or failure
 */
static int cam_eeprom_force_power_down(struct cam_eeprom_ctrl_t *e_ctrl)
{
	struct cam_sensor_power_ctrl_t *power_info;
	struct cam_hw_soc_info         *soc_info;
	struct cam_eeprom_soc_private  *soc_private;
	int                             rc = 0;

	if (!e_ctrl) {
		CAM_ERR(CAM_EEPROM, "failed: e_ctrl %pK", e_ctrl);
		return -EINVAL;
	}

	soc_private =
		(struct cam_eeprom_soc_private *)e_ctrl->soc_info.soc_private;
	power_info = &soc_private->power_info;
	soc_info = &e_ctrl->soc_info;

	if (!power_info) {
		CAM_ERR(CAM_EEPROM, "failed: power_info %pK", power_info);
		return -EINVAL;
	}
#if defined(CONFIG_SENSOR_RETENTION)
	rc = cam_sensor_util_power_down(power_info, soc_info, 2);
#else
	rc = cam_sensor_util_power_down(power_info, soc_info);
#endif
	if (rc) {
		CAM_ERR(CAM_EEPROM, "power down the core is failed:%d", rc);
		return rc;
	}

	if (e_ctrl->io_master_info.master_type == CCI_MASTER)
		camera_io_release(&(e_ctrl->io_master_info));

	return rc;
}
#endif

/**
 * cam_eeprom_match_id - match eeprom id
 * @e_ctrl:     ctrl structure
 *
 * Returns success or failure
 */
static int cam_eeprom_match_id(struct cam_eeprom_ctrl_t *e_ctrl)
{
	int                      rc;
	struct camera_io_master *client = &e_ctrl->io_master_info;
	uint8_t                  id[2];

	rc = cam_spi_query_id(client, 0, CAMERA_SENSOR_I2C_TYPE_WORD,
		&id[0], 2);
	if (rc)
		return rc;
	CAM_DBG(CAM_EEPROM, "read 0x%x 0x%x, check 0x%x 0x%x",
		id[0], id[1], client->spi_client->mfr_id0,
		client->spi_client->device_id0);
	if (id[0] != client->spi_client->mfr_id0
		|| id[1] != client->spi_client->device_id0)
		return -ENODEV;
	return 0;
}

/**
 * cam_eeprom_parse_read_memory_map - Parse memory map
 * @of_node:    device node
 * @e_ctrl:     ctrl structure
 *
 * Returns success or failure
 */
int32_t cam_eeprom_parse_read_memory_map(struct device_node *of_node,
	struct cam_eeprom_ctrl_t *e_ctrl)
{
	int32_t                         rc = 0;
	struct cam_eeprom_soc_private  *soc_private;
	struct cam_sensor_power_ctrl_t *power_info;
	int i;
	int normal_crc_value = 0;

	if (!e_ctrl || !of_node) {
		CAM_ERR(CAM_EEPROM, "failed: e_ctrl is NULL");
		return -EINVAL;
	}

	soc_private =
		(struct cam_eeprom_soc_private *)e_ctrl->soc_info.soc_private;
	power_info = &soc_private->power_info;

	rc = cam_eeprom_parse_dt_memory_map(of_node, &e_ctrl->cal_data);
	if (rc) {
		CAM_ERR(CAM_EEPROM, "failed: eeprom dt parse rc %d", rc);
		return rc;
	}
	rc = cam_eeprom_power_up(e_ctrl, power_info);
	if (rc) {
		CAM_ERR(CAM_EEPROM, "failed: eeprom power up rc %d", rc);
		goto data_mem_free;
	}

	e_ctrl->cam_eeprom_state = CAM_EEPROM_CONFIG;
	if (e_ctrl->eeprom_device_type == MSM_CAMERA_SPI_DEVICE) {
		rc = cam_eeprom_match_id(e_ctrl);
		if (rc) {
			CAM_DBG(CAM_EEPROM, "eeprom not matching %d", rc);
			//goto power_down;
			rc = 0;
		}
	}

	normal_crc_value = 0;
	for (i = 0; i < e_ctrl->cal_data.num_map>>1; i++)
		normal_crc_value |= (1 << i);

	CAMERA_NORMAL_CAL_CRC = normal_crc_value;
	CAM_INFO(CAM_EEPROM, "num_map = %d, CAMERA_NORMAL_CAL_CRC = 0x%X",
		e_ctrl->cal_data.num_map, CAMERA_NORMAL_CAL_CRC);

	rc = cam_eeprom_read_memory(e_ctrl, &e_ctrl->cal_data);
	if (rc < 0) {
		CAM_ERR(CAM_EEPROM, "read_eeprom_memory failed");
		goto power_down;
	}
	e_ctrl->is_supported |= cam_eeprom_match_crc(&e_ctrl->cal_data, e_ctrl->soc_info.index);

	if (e_ctrl->is_supported != normal_crc_value)
		CAM_ERR(CAM_EEPROM, "Any CRC values at F-ROM are not matched.");
	else
		CAM_INFO(CAM_EEPROM, "All CRC values are matched.");

	rc = cam_eeprom_update_module_info(e_ctrl);
	if (rc < 0) {
		CAM_ERR(CAM_EEPROM, "cam_eeprom_update_module_info failed");
		goto power_down;
	}

#ifdef CAM_EEPROM_DBG_DUMP
	if (e_ctrl->soc_info.index == 1) {
		rc = cam_eeprom_dump(e_ctrl->soc_info.index, e_ctrl->cal_data.mapdata, 0x0000, 0x200);
	}
#endif

	rc = cam_eeprom_power_down(e_ctrl);
	if (rc < 0)
		CAM_ERR(CAM_EEPROM, "failed: eeprom power down rc %d", rc);

	return rc;
power_down:
	cam_eeprom_power_down(e_ctrl);
data_mem_free:
	vfree(e_ctrl->cal_data.mapdata);
	vfree(e_ctrl->cal_data.map);
	e_ctrl->cal_data.num_data = 0;
	e_ctrl->cal_data.num_map = 0;
	return rc;
}

/**
 * cam_eeprom_get_dev_handle - get device handle
 * @e_ctrl:     ctrl structure
 * @arg:        Camera control command argument
 *
 * Returns success or failure
 */
static int32_t cam_eeprom_get_dev_handle(struct cam_eeprom_ctrl_t *e_ctrl,
	void *arg)
{
	struct cam_sensor_acquire_dev    eeprom_acq_dev;
	struct cam_create_dev_hdl        bridge_params;
	struct cam_control              *cmd = (struct cam_control *)arg;

	if (e_ctrl->bridge_intf.device_hdl != -1) {
		CAM_ERR(CAM_EEPROM, "Device is already acquired");
		return -EFAULT;
	}
	if (copy_from_user(&eeprom_acq_dev,
		u64_to_user_ptr(cmd->handle),
		sizeof(eeprom_acq_dev))) {
		CAM_ERR(CAM_EEPROM,
			"EEPROM:ACQUIRE_DEV: copy from user failed");
		return -EFAULT;
	}

	bridge_params.session_hdl = eeprom_acq_dev.session_handle;
	bridge_params.ops = &e_ctrl->bridge_intf.ops;
	bridge_params.v4l2_sub_dev_flag = 0;
	bridge_params.media_entity_flag = 0;
	bridge_params.priv = e_ctrl;

	eeprom_acq_dev.device_handle =
		cam_create_device_hdl(&bridge_params);
	e_ctrl->bridge_intf.device_hdl = eeprom_acq_dev.device_handle;
	e_ctrl->bridge_intf.session_hdl = eeprom_acq_dev.session_handle;

	CAM_DBG(CAM_EEPROM, "Device Handle: %d", eeprom_acq_dev.device_handle);
	if (copy_to_user(u64_to_user_ptr(cmd->handle),
		&eeprom_acq_dev, sizeof(struct cam_sensor_acquire_dev))) {
		CAM_ERR(CAM_EEPROM, "EEPROM:ACQUIRE_DEV: copy to user failed");
		return -EFAULT;
	}
	return 0;
}

/**
 * cam_eeprom_update_slaveInfo - Update slave info
 * @e_ctrl:     ctrl structure
 * @cmd_buf:    command buffer
 *
 * Returns success or failure
 */
static int32_t cam_eeprom_update_slaveInfo(struct cam_eeprom_ctrl_t *e_ctrl,
	void *cmd_buf)
{
	int32_t                         rc = 0;
	struct cam_eeprom_soc_private  *soc_private;
	struct cam_cmd_i2c_info        *cmd_i2c_info = NULL;

	soc_private =
		(struct cam_eeprom_soc_private *)e_ctrl->soc_info.soc_private;
	cmd_i2c_info = (struct cam_cmd_i2c_info *)cmd_buf;
	if (cmd_i2c_info->slave_addr > 0)
		soc_private->i2c_info.slave_addr = cmd_i2c_info->slave_addr;
	soc_private->i2c_info.i2c_freq_mode = cmd_i2c_info->i2c_freq_mode;

	rc = cam_eeprom_update_i2c_info(e_ctrl,
		&soc_private->i2c_info);
	CAM_DBG(CAM_EEPROM, "Slave addr: 0x%x Freq Mode: %d",
		soc_private->i2c_info.slave_addr,
		soc_private->i2c_info.i2c_freq_mode);

	return rc;
}

/**
 * cam_eeprom_parse_memory_map - Parse memory map info
 * @data:             memory block data
 * @cmd_buf:          command buffer
 * @cmd_length:       command buffer length
 * @num_map:          memory map size
 * @cmd_length_bytes: command length processed in this function
 *
 * Returns success or failure
 */
static int32_t cam_eeprom_parse_memory_map(
	struct cam_eeprom_memory_block_t *data,
	void *cmd_buf, int cmd_length, uint16_t *cmd_length_bytes,
	int *num_map)
{
	int32_t                            rc = 0;
	int32_t                            cnt = 0;
	int32_t                            processed_size = 0;
	uint8_t                            generic_op_code;
	struct cam_eeprom_memory_map_t    *map = data->map;
	struct common_header              *cmm_hdr =
		(struct common_header *)cmd_buf;
	uint16_t                           cmd_length_in_bytes = 0;
	struct cam_cmd_i2c_random_wr      *i2c_random_wr = NULL;
	struct cam_cmd_i2c_continuous_rd  *i2c_cont_rd = NULL;
	struct cam_cmd_conditional_wait   *i2c_poll = NULL;
	struct cam_cmd_unconditional_wait *i2c_uncond_wait = NULL;

	generic_op_code = cmm_hdr->third_byte;
	switch (cmm_hdr->cmd_type) {
	case CAMERA_SENSOR_CMD_TYPE_I2C_RNDM_WR:
		i2c_random_wr = (struct cam_cmd_i2c_random_wr *)cmd_buf;
		cmd_length_in_bytes   = sizeof(struct cam_cmd_i2c_random_wr) +
			((i2c_random_wr->header.count - 1) *
			sizeof(struct i2c_random_wr_payload));

		for (cnt = 0; cnt < (i2c_random_wr->header.count);
			cnt++) {
			map[*num_map + cnt].page.addr =
				i2c_random_wr->random_wr_payload[cnt].reg_addr;
			map[*num_map + cnt].page.addr_type =
				i2c_random_wr->header.addr_type;
			map[*num_map + cnt].page.data =
				i2c_random_wr->random_wr_payload[cnt].reg_data;
			map[*num_map + cnt].page.data_type =
				i2c_random_wr->header.data_type;
			map[*num_map + cnt].page.valid_size = 1;
		}

		*num_map += (i2c_random_wr->header.count - 1);
		cmd_buf += cmd_length_in_bytes / sizeof(int32_t);
		processed_size +=
			cmd_length_in_bytes;
		break;
	case CAMERA_SENSOR_CMD_TYPE_I2C_CONT_RD:
		i2c_cont_rd = (struct cam_cmd_i2c_continuous_rd *)cmd_buf;
		cmd_length_in_bytes = sizeof(struct cam_cmd_i2c_continuous_rd);

		map[*num_map].mem.addr = i2c_cont_rd->reg_addr;
		map[*num_map].mem.addr_type = i2c_cont_rd->header.addr_type;
		map[*num_map].mem.data_type = i2c_cont_rd->header.data_type;
		map[*num_map].mem.valid_size =
			i2c_cont_rd->header.count;
		cmd_buf += cmd_length_in_bytes / sizeof(int32_t);
		processed_size +=
			cmd_length_in_bytes;
		data->num_data += map[*num_map].mem.valid_size;
		break;
	case CAMERA_SENSOR_CMD_TYPE_WAIT:
		if (generic_op_code ==
			CAMERA_SENSOR_WAIT_OP_HW_UCND ||
			generic_op_code ==
			CAMERA_SENSOR_WAIT_OP_SW_UCND) {
			i2c_uncond_wait =
				(struct cam_cmd_unconditional_wait *)cmd_buf;
			cmd_length_in_bytes =
				sizeof(struct cam_cmd_unconditional_wait);

			if (*num_map < 1) {
				CAM_ERR(CAM_EEPROM,
					"invalid map number, num_map=%d",
					*num_map);
				return -EINVAL;
			}

			/*
			 * Though delay is added all of them, but delay will
			 * be applicable to only one of them as only one of
			 * them will have valid_size set to >= 1.
			 */
			map[*num_map - 1].mem.delay = i2c_uncond_wait->delay;
			map[*num_map - 1].page.delay = i2c_uncond_wait->delay;
			map[*num_map - 1].pageen.delay = i2c_uncond_wait->delay;
		} else if (generic_op_code ==
			CAMERA_SENSOR_WAIT_OP_COND) {
			i2c_poll = (struct cam_cmd_conditional_wait *)cmd_buf;
			cmd_length_in_bytes =
				sizeof(struct cam_cmd_conditional_wait);

			map[*num_map].poll.addr = i2c_poll->reg_addr;
			map[*num_map].poll.addr_type = i2c_poll->addr_type;
			map[*num_map].poll.data = i2c_poll->reg_data;
			map[*num_map].poll.data_type = i2c_poll->data_type;
			map[*num_map].poll.delay = i2c_poll->timeout;
			map[*num_map].poll.valid_size = 1;
		}
		cmd_buf += cmd_length_in_bytes / sizeof(int32_t);
		processed_size +=
			cmd_length_in_bytes;
		break;
	default:
		break;
	}

	*cmd_length_bytes = processed_size;
	return rc;
}

/**
 * cam_eeprom_calc_calmap_size - Calculate cal array size based on the cal map
 * @e_ctrl:       ctrl structure
 *
 * Returns size of cal array
 */
static int32_t cam_eeprom_calc_calmap_size(struct cam_eeprom_ctrl_t *e_ctrl)
{
	struct cam_eeprom_memory_map_t    *map = NULL;
	uint32_t minMap, maxMap, minLocal, maxLocal;
	int32_t i;
	int32_t calmap_size = 0;

	if (e_ctrl == NULL ||
		(e_ctrl->cal_data.num_map == 0) ||
		(e_ctrl->cal_data.map == NULL)) {
		CAM_INFO(CAM_EEPROM, "cal size is wrong");
		return calmap_size;
	}

	map = e_ctrl->cal_data.map;
	minMap = minLocal = 0xFFFFFFFF;
	maxMap = maxLocal = 0x00;

	for (i = 0; i < e_ctrl->cal_data.num_map; i++) {
		minLocal = map[i].mem.addr;
		maxLocal = minLocal + map[i].mem.valid_size;

		if(minMap > minLocal)
		{
			minMap = minLocal;
		}

		if(maxMap < maxLocal)
		{
			maxMap = maxLocal;
		}

		CAM_INFO(CAM_EEPROM, "[%d / %d] minLocal = 0x%X, minMap = 0x%X, maxLocal = 0x%X, maxMap = 0x%X",
			i+1, e_ctrl->cal_data.num_map, minLocal, minMap, maxLocal, maxMap);
	}
	calmap_size = maxMap - minMap;

	CAM_INFO(CAM_EEPROM, "calmap_size = 0x%X, minMap = 0x%X, maxMap = 0x%X",
		calmap_size, minMap, maxMap);

	return calmap_size;
}

/**
 * cam_eeprom_init_pkt_parser - Parse eeprom packet
 * @e_ctrl:       ctrl structure
 * @csl_packet:	  csl packet received
 *
 * Returns success or failure
 */
static int32_t cam_eeprom_init_pkt_parser(struct cam_eeprom_ctrl_t *e_ctrl,
	struct cam_packet *csl_packet)
{
	int32_t                         rc = 0;
	int                             i = 0;
	struct cam_cmd_buf_desc        *cmd_desc = NULL;
	uint32_t                       *offset = NULL;
	uint32_t                       *cmd_buf = NULL;
	uintptr_t                        generic_pkt_addr;
	size_t                          pkt_len = 0;
	uint32_t                        total_cmd_buf_in_bytes = 0;
	uint32_t                        processed_cmd_buf_in_bytes = 0;
	struct common_header           *cmm_hdr = NULL;
	uint16_t                        cmd_length_in_bytes = 0;
	struct cam_cmd_i2c_info        *i2c_info = NULL;
	int                             num_map = -1;
	struct cam_eeprom_memory_map_t *map = NULL;
	struct cam_eeprom_soc_private  *soc_private =
		(struct cam_eeprom_soc_private *)e_ctrl->soc_info.soc_private;
	struct cam_sensor_power_ctrl_t *power_info = &soc_private->power_info;

	e_ctrl->cal_data.map = vmalloc((MSM_EEPROM_MEMORY_MAP_MAX_SIZE *
		MSM_EEPROM_MAX_MEM_MAP_CNT) *
		(sizeof(struct cam_eeprom_memory_map_t)));
	if (!e_ctrl->cal_data.map) {
		rc = -ENOMEM;
		CAM_ERR(CAM_EEPROM, "failed");
		return rc;
	}
	memset(e_ctrl->cal_data.map, 0x00, (MSM_EEPROM_MEMORY_MAP_MAX_SIZE *
		MSM_EEPROM_MAX_MEM_MAP_CNT) *
		(sizeof(struct cam_eeprom_memory_map_t)));
	map = e_ctrl->cal_data.map;

	offset = (uint32_t *)&csl_packet->payload;
	offset += (csl_packet->cmd_buf_offset / sizeof(uint32_t));
	cmd_desc = (struct cam_cmd_buf_desc *)(offset);

	/* Loop through multiple command buffers */
	for (i = 0; i < csl_packet->num_cmd_buf; i++) {
		total_cmd_buf_in_bytes = cmd_desc[i].length;
		processed_cmd_buf_in_bytes = 0;
		if (!total_cmd_buf_in_bytes)
			continue;
		rc = cam_mem_get_cpu_buf(cmd_desc[i].mem_handle,
			&generic_pkt_addr, &pkt_len);
		if (rc) {
			CAM_ERR(CAM_EEPROM, "Failed to get cpu buf");
			return rc;
		}
		cmd_buf = (uint32_t *)generic_pkt_addr;
		if (!cmd_buf) {
			CAM_ERR(CAM_EEPROM, "invalid cmd buf");
			return -EINVAL;
		}
		cmd_buf += cmd_desc[i].offset / sizeof(uint32_t);
		/* Loop through multiple cmd formats in one cmd buffer */
		while (processed_cmd_buf_in_bytes < total_cmd_buf_in_bytes) {
			cmm_hdr = (struct common_header *)cmd_buf;
			switch (cmm_hdr->cmd_type) {
			case CAMERA_SENSOR_CMD_TYPE_I2C_INFO:
				i2c_info = (struct cam_cmd_i2c_info *)cmd_buf;
				/* Configure the following map slave address */
				map[num_map + 1].saddr = i2c_info->slave_addr;
				rc = cam_eeprom_update_slaveInfo(e_ctrl,
					cmd_buf);
				cmd_length_in_bytes =
					sizeof(struct cam_cmd_i2c_info);
				processed_cmd_buf_in_bytes +=
					cmd_length_in_bytes;
				cmd_buf += cmd_length_in_bytes/
					sizeof(uint32_t);
				break;
			case CAMERA_SENSOR_CMD_TYPE_PWR_UP:
			case CAMERA_SENSOR_CMD_TYPE_PWR_DOWN:
				cmd_length_in_bytes = total_cmd_buf_in_bytes;
				rc = cam_sensor_update_power_settings(cmd_buf,
					cmd_length_in_bytes, power_info);
				processed_cmd_buf_in_bytes +=
					cmd_length_in_bytes;
				cmd_buf += cmd_length_in_bytes/
					sizeof(uint32_t);
				if (rc) {
					CAM_ERR(CAM_EEPROM, "Failed");
					return rc;
				}
				break;
			case CAMERA_SENSOR_CMD_TYPE_I2C_RNDM_WR:
			case CAMERA_SENSOR_CMD_TYPE_I2C_CONT_RD:
			case CAMERA_SENSOR_CMD_TYPE_WAIT:
				num_map++;
				rc = cam_eeprom_parse_memory_map(
					&e_ctrl->cal_data, cmd_buf,
					total_cmd_buf_in_bytes,
					&cmd_length_in_bytes, &num_map);
				processed_cmd_buf_in_bytes +=
					cmd_length_in_bytes;
				cmd_buf += cmd_length_in_bytes/sizeof(uint32_t);
				break;
			default:
				break;
			}
		}
		e_ctrl->cal_data.num_map = num_map + 1;
	}
	return rc;
}

/**
 * cam_eeprom_get_cal_data - parse the userspace IO config and
 *                                        copy read data to share with userspace
 * @e_ctrl:     ctrl structure
 * @csl_packet: csl packet received
 *
 * Returns success or failure
 */
static int32_t cam_eeprom_get_cal_data(struct cam_eeprom_ctrl_t *e_ctrl,
	struct cam_packet *csl_packet)
{
	struct cam_buf_io_cfg *io_cfg;
	uint32_t              i = 0;
	int                   rc = 0;
	uintptr_t             buf_addr;
	size_t                buf_size;
	uint8_t               *read_buffer;

	io_cfg = (struct cam_buf_io_cfg *) ((uint8_t *)
		&csl_packet->payload +
		csl_packet->io_configs_offset);

	CAM_DBG(CAM_EEPROM, "number of IO configs: %d:",
		csl_packet->num_io_configs);

	for (i = 0; i < csl_packet->num_io_configs; i++) {
		CAM_DBG(CAM_EEPROM, "Direction: %d:", io_cfg->direction);
		if (io_cfg->direction == CAM_BUF_OUTPUT) {
			rc = cam_mem_get_cpu_buf(io_cfg->mem_handle[0],
				&buf_addr, &buf_size);
			CAM_DBG(CAM_EEPROM, "buf_addr : %pK, buf_size : %zu\n",
				(void *)buf_addr, buf_size);

			read_buffer = (uint8_t *)buf_addr;
			if (!read_buffer) {
				CAM_ERR(CAM_EEPROM,
					"invalid buffer to copy data");
				return -EINVAL;
			}
			read_buffer += io_cfg->offsets[0];

			if (buf_size < e_ctrl->cal_data.num_data) {
				CAM_ERR(CAM_EEPROM,
					"failed to copy, Invalid size");
				return -EINVAL;
			}

			CAM_DBG(CAM_EEPROM, "copy the data, len:%d",
				e_ctrl->cal_data.num_data);

			memcpy(read_buffer, e_ctrl->cal_data.mapdata,
					e_ctrl->cal_data.num_data);

		} else {
			CAM_ERR(CAM_EEPROM, "Invalid direction");
			rc = -EINVAL;
		}
	}
	return rc;
}

/**
 * cam_eeprom_get_phone_ver - parse the userspace IO config and
 *                            read phone version at eebindriver.bin
 * @e_ctrl:     ctrl structure
 * @csl_packet: csl packet received
 *
 * Returns success or failure
 */
static int32_t cam_eeprom_get_phone_ver(struct cam_eeprom_ctrl_t *e_ctrl,
	struct cam_packet *csl_packet)
{
	struct cam_buf_io_cfg *io_cfg;
	uint32_t              i = 0, j = 0;
	int                   rc = 0;
	uintptr_t             buf_addr;
	size_t                buf_size;
	uint8_t               *read_buffer;

	int                   nVer = 0;
	uint8_t               *pBuf = NULL;
	uint8_t	              bVerNormal = TRUE;

	char                  tmp_hw_info[HW_INFO_MAX_SIZE] = HW_INFO;
	char                  tmp_sw_info[SW_INFO_MAX_SIZE] = SW_INFO;
	char                  tmp_vendor_info[VENDOR_INFO_MAX_SIZE] = VENDOR_INFO;
	char                  tmp_process_info[PROCESS_INFO_MAX_SIZE] = PROCESS_INFO;
	unsigned int          tmp_rev = 0;

	io_cfg = (struct cam_buf_io_cfg *) ((uint8_t *)
		&csl_packet->payload +
		csl_packet->io_configs_offset);

	CAM_INFO(CAM_EEPROM, "number of IO configs: %d:",
		csl_packet->num_io_configs);

	for (i = 0; i < csl_packet->num_io_configs; i++) {
		CAM_INFO(CAM_EEPROM, "Direction: %d:", io_cfg->direction);
		if (io_cfg->direction == CAM_BUF_OUTPUT) {
			rc = cam_mem_get_cpu_buf(io_cfg->mem_handle[0],
				&buf_addr, &buf_size);
			CAM_INFO(CAM_EEPROM, "buf_addr : %pK, buf_size : %zu",
				(void *)buf_addr, buf_size);

			read_buffer = (uint8_t *)buf_addr;
			if (!read_buffer) {
				CAM_ERR(CAM_EEPROM,
					"invalid buffer to copy data");
				return -EINVAL;
			}
			read_buffer += io_cfg->offsets[0];

			if (buf_size < e_ctrl->cal_data.num_data) {
				CAM_ERR(CAM_EEPROM,
					"failed to copy, Invalid size");
				return -EINVAL;
			}

			CAM_INFO(CAM_EEPROM, "copy the data, len:%d, read_buffer[0] = %d, read_buffer[4] = %d",
				e_ctrl->cal_data.num_data, read_buffer[0], read_buffer[4]);

			pBuf = read_buffer;
			memcpy(&nVer, pBuf, sizeof(int));
			pBuf += sizeof(int);

			memcpy(&tmp_rev, pBuf, sizeof(int));
			pBuf += sizeof(int);

			bVerNormal = TRUE;
			for(j = 0; j < FROM_MODULE_FW_INFO_SIZE; j ++) {
				CAM_DBG(CAM_EEPROM, "mapdata[0x%04X] = 0x%02X",
					REAR_MODULE_FW_VERSION + j,
					e_ctrl->cal_data.mapdata[REAR_MODULE_FW_VERSION + j]);

				if(e_ctrl->cal_data.mapdata[REAR_MODULE_FW_VERSION + j] >= 0x80
					|| !isalnum(e_ctrl->cal_data.mapdata[REAR_MODULE_FW_VERSION + j] & 0xFF)) {
					CAM_ERR(CAM_EEPROM, "Invalid Version");
					bVerNormal = FALSE;
					break;
				}
			}

			if(bVerNormal == TRUE) {
				memcpy(hw_phone_info, &e_ctrl->cal_data.mapdata[REAR_MODULE_FW_VERSION],
					HW_INFO_MAX_SIZE);
				hw_phone_info[HW_INFO_MAX_SIZE-1] = '\0';
				CAM_INFO(CAM_EEPROM, "hw_phone_info: %s", hw_phone_info);
			} else {
				memcpy(hw_phone_info, HW_INFO, HW_INFO_MAX_SIZE);
				memcpy(sw_phone_info, SW_INFO, SW_INFO_MAX_SIZE);
				memcpy(vendor_phone_info, VENDOR_INFO, VENDOR_INFO_MAX_SIZE);
				memcpy(process_phone_info, PROCESS_INFO, PROCESS_INFO_MAX_SIZE);
				CAM_INFO(CAM_EEPROM, "Set Ver : %s %s %s %s",
					hw_phone_info, sw_phone_info,
					vendor_phone_info, process_phone_info);
			}

			CAM_INFO(CAM_EEPROM, "hw_phone_info: %s", hw_phone_info);

			for (i = 0; i < nVer; i++) {
				memcpy(tmp_hw_info, pBuf, HW_INFO_MAX_SIZE);
				pBuf += HW_INFO_MAX_SIZE;

				memcpy(tmp_sw_info, pBuf, SW_INFO_MAX_SIZE);
				pBuf += SW_INFO_MAX_SIZE;

				memcpy(tmp_vendor_info, pBuf, VENDOR_INFO_MAX_SIZE);
				tmp_vendor_info[VENDOR_INFO_MAX_SIZE-1] = '\0';
				pBuf += VENDOR_INFO_MAX_SIZE-1;

				memcpy(tmp_process_info, pBuf, PROCESS_INFO_MAX_SIZE);
				tmp_process_info[PROCESS_INFO_MAX_SIZE-1] = '\0';
				pBuf += PROCESS_INFO_MAX_SIZE;

				CAM_INFO(CAM_EEPROM, "[temp %d/%d] : %s %s %s %s",
					i, nVer, tmp_hw_info, tmp_sw_info,
					tmp_vendor_info, tmp_process_info);
				if (strcmp(hw_phone_info, tmp_hw_info) == 0) {
					memcpy(sw_phone_info, tmp_sw_info, SW_INFO_MAX_SIZE);
					memcpy(vendor_phone_info, tmp_vendor_info, VENDOR_INFO_MAX_SIZE);
					memcpy(process_phone_info, tmp_process_info, PROCESS_INFO_MAX_SIZE);
					CAM_INFO(CAM_EEPROM, "rear [%d] : %s %s %s %s",
						i, hw_phone_info, sw_phone_info,
						vendor_phone_info, process_phone_info);
				}
#if defined(CONFIG_SAMSUNG_REAR_DUAL) || defined(CONFIG_SAMSUNG_REAR_TRIPLE)
				else if (strcmp(rear2_hw_phone_info, tmp_hw_info) == 0) {
					memcpy(rear2_sw_phone_info, tmp_sw_info, SW_INFO_MAX_SIZE);
					memcpy(rear2_vendor_phone_info, tmp_vendor_info, VENDOR_INFO_MAX_SIZE);
					memcpy(rear2_process_phone_info, tmp_process_info, PROCESS_INFO_MAX_SIZE);
					CAM_INFO(CAM_EEPROM, "rear2 [%d] : %s %s %s %s",
						i, rear2_hw_phone_info, rear2_sw_phone_info,
						rear2_vendor_phone_info, rear2_process_phone_info);
				}
#if defined(CONFIG_SAMSUNG_REAR_TRIPLE)
				else if (strcmp(rear3_hw_phone_info, tmp_hw_info) == 0) {
					memcpy(rear3_sw_phone_info, tmp_sw_info, SW_INFO_MAX_SIZE);
					memcpy(rear3_vendor_phone_info, tmp_vendor_info, VENDOR_INFO_MAX_SIZE);
					memcpy(rear3_process_phone_info, tmp_process_info, PROCESS_INFO_MAX_SIZE);
					CAM_INFO(CAM_EEPROM, "rear3 [%d] : %s %s %s %s",
						i, rear3_hw_phone_info, rear3_sw_phone_info,
						rear3_vendor_phone_info, rear3_process_phone_info);
				}
#endif
#endif
				else if (strcmp(front_hw_phone_info, tmp_hw_info) == 0) {
					memcpy(front_sw_phone_info, tmp_sw_info, SW_INFO_MAX_SIZE);
					memcpy(front_vendor_phone_info, tmp_vendor_info, VENDOR_INFO_MAX_SIZE);
					memcpy(front_process_phone_info, tmp_process_info, PROCESS_INFO_MAX_SIZE);
					CAM_INFO(CAM_EEPROM, "front [%d] : %s %s %s %s",
						i, front_hw_phone_info, front_sw_phone_info,
						front_vendor_phone_info, front_process_phone_info);
				}
#if defined(CONFIG_SAMSUNG_FRONT_DUAL)
				else if (strcmp(front2_hw_phone_info, tmp_hw_info) == 0) {
					memcpy(front2_sw_phone_info, tmp_sw_info, SW_INFO_MAX_SIZE);
					memcpy(front2_vendor_phone_info, tmp_vendor_info, VENDOR_INFO_MAX_SIZE);
					memcpy(front2_process_phone_info, tmp_process_info, PROCESS_INFO_MAX_SIZE);
					CAM_INFO(CAM_EEPROM, "front2 [%d] : %s %s %s %s",
						i, front2_hw_phone_info, front2_sw_phone_info,
						front2_vendor_phone_info, front2_process_phone_info);
				}
#endif
#if defined(CONFIG_SAMSUNG_FRONT_TOP)
				else if (strcmp(front3_hw_phone_info, tmp_hw_info) == 0) {
					memcpy(front3_sw_phone_info, tmp_sw_info, SW_INFO_MAX_SIZE);
					memcpy(front3_vendor_phone_info, tmp_vendor_info, VENDOR_INFO_MAX_SIZE);
					memcpy(front3_process_phone_info, tmp_process_info, PROCESS_INFO_MAX_SIZE);
					CAM_INFO(CAM_EEPROM, "front3 [%d] : %s %s %s %s",
						i, front3_hw_phone_info, front3_sw_phone_info,
						front3_vendor_phone_info, front3_process_phone_info);
				}
#endif
				else {
					CAM_INFO(CAM_EEPROM, "invalid hwinfo: %s", tmp_hw_info);
				}
			}
			memset(read_buffer, 0x00, e_ctrl->cal_data.num_data);
		} else {
			CAM_ERR(CAM_EEPROM, "Invalid direction");
			rc = -EINVAL;
		}
	}

	return rc;
}

/**
 * cam_eeprom_pkt_parse - Parse csl packet
 * @e_ctrl:     ctrl structure
 * @arg:        Camera control command argument
 *
 * Returns success or failure
 */
static int32_t cam_eeprom_pkt_parse(struct cam_eeprom_ctrl_t *e_ctrl, void *arg)
{
	int32_t                         rc = 0;
	struct cam_control             *ioctl_ctrl = NULL;
	struct cam_config_dev_cmd       dev_config;
	uintptr_t                        generic_pkt_addr;
	size_t                          pkt_len;
	struct cam_packet              *csl_packet = NULL;
	struct cam_eeprom_soc_private  *soc_private =
		(struct cam_eeprom_soc_private *)e_ctrl->soc_info.soc_private;
	struct cam_sensor_power_ctrl_t *power_info = &soc_private->power_info;

	ioctl_ctrl = (struct cam_control *)arg;

	if (copy_from_user(&dev_config,
		u64_to_user_ptr(ioctl_ctrl->handle),
		sizeof(dev_config)))
		return -EFAULT;
	rc = cam_mem_get_cpu_buf(dev_config.packet_handle,
		&generic_pkt_addr, &pkt_len);
	if (rc) {
		CAM_ERR(CAM_EEPROM,
			"error in converting command Handle Error: %d", rc);
		return rc;
	}

	if (dev_config.offset > pkt_len) {
		CAM_ERR(CAM_EEPROM,
			"Offset is out of bound: off: %lld, %zu",
			dev_config.offset, pkt_len);
		return -EINVAL;
	}

	csl_packet = (struct cam_packet *)
		(generic_pkt_addr + (uint32_t)dev_config.offset);
	switch (csl_packet->header.op_code & 0xFFFFFF) {
	case CAM_EEPROM_PACKET_OPCODE_INIT:
		CAM_INFO(CAM_EEPROM, "e_ctrl->userspace_probe : %d", e_ctrl->userspace_probe);
		if (e_ctrl->userspace_probe == false) {
			CAM_ERR(CAM_EEPROM, "VR:: KERNEL PROBE ");
			rc = cam_eeprom_parse_read_memory_map(
					e_ctrl->soc_info.dev->of_node, e_ctrl);
			if (rc < 0) {
				CAM_ERR(CAM_EEPROM, "Failed: rc : %d", rc);
				return rc;
			}
			rc = cam_eeprom_get_cal_data(e_ctrl, csl_packet);

			vfree(e_ctrl->cal_data.mapdata);
			vfree(e_ctrl->cal_data.map);
			e_ctrl->cal_data.num_data = 0;
			e_ctrl->cal_data.num_map = 0;
			CAM_DBG(CAM_EEPROM,
				"Returning the data using kernel probe");
		} else {
			rc = cam_eeprom_init_pkt_parser(e_ctrl, csl_packet);
			if (rc) {
				CAM_ERR(CAM_EEPROM,
					"Failed in parsing the pkt");
				return rc;
			}

			if ((e_ctrl->cal_data.num_map == 0) &&
				(e_ctrl->cal_data.map != NULL)) {
				vfree(e_ctrl->cal_data.map);
				CAM_INFO(CAM_EEPROM, "No read settings privided");
				return rc;
			}

			e_ctrl->cal_data.num_data = cam_eeprom_calc_calmap_size(e_ctrl);

			if (e_ctrl->cal_data.num_data == 0) {
				rc = -ENOMEM;
				CAM_ERR(CAM_EEPROM, "failed");
				goto error;
			}

			e_ctrl->cal_data.mapdata =
				vmalloc(e_ctrl->cal_data.num_data);
			if (!e_ctrl->cal_data.mapdata) {
				rc = -ENOMEM;
				CAM_ERR(CAM_EEPROM, "failed");
				goto error;
			}
			memset(e_ctrl->cal_data.mapdata, 0xFF, e_ctrl->cal_data.num_data);

			rc = cam_eeprom_power_up(e_ctrl,
				&soc_private->power_info);
			if (rc) {
				CAM_ERR(CAM_EEPROM, "failed rc %d", rc);
				goto memdata_free;
			}

			e_ctrl->cam_eeprom_state = CAM_EEPROM_CONFIG;

			{
				int i;
				int normal_crc_value = 0;

				if (e_ctrl->eeprom_device_type == MSM_CAMERA_SPI_DEVICE) {
					rc = cam_eeprom_match_id(e_ctrl);
					if (rc) {
						CAM_DBG(CAM_EEPROM, "eeprom not matching %d", rc);
						//goto power_down;
						rc = 0;
					}
				}

				normal_crc_value = 0;
				for (i = 0; i < e_ctrl->cal_data.num_map>>1; i++)
					normal_crc_value |= (1 << i);

				CAMERA_NORMAL_CAL_CRC = normal_crc_value;
				CAM_INFO(CAM_EEPROM, "num_map = %d, CAMERA_NORMAL_CAL_CRC = 0x%X",
					e_ctrl->cal_data.num_map, CAMERA_NORMAL_CAL_CRC);

				rc = cam_eeprom_read_memory(e_ctrl, &e_ctrl->cal_data);
				if (rc < 0) {
					CAM_ERR(CAM_EEPROM, "read_eeprom_memory failed");
					goto power_down;
				}

				if (1 < e_ctrl->cal_data.num_map) {
					e_ctrl->is_supported |= cam_eeprom_match_crc(&e_ctrl->cal_data,
						e_ctrl->soc_info.index);

					if (e_ctrl->is_supported != normal_crc_value)
						CAM_ERR(CAM_EEPROM, "Any CRC values at F-ROM are not matched.");
					else
						CAM_INFO(CAM_EEPROM, "All CRC values are matched.");

					rc = cam_eeprom_update_module_info(e_ctrl);
					if (rc < 0) {
						CAM_ERR(CAM_EEPROM, "cam_eeprom_update_module_info failed");
						goto power_down;
					}

#ifdef CAM_EEPROM_DBG_DUMP
					if (e_ctrl->soc_info.index == 0)
						rc = cam_eeprom_dump(e_ctrl->soc_info.index,
							e_ctrl->cal_data.mapdata, 0x4680, 0x2C);
					else
						rc = cam_eeprom_dump(e_ctrl->soc_info.index,
							e_ctrl->cal_data.mapdata, 0xB0, 0x60);
#endif
				} else if (e_ctrl->cal_data.num_map == 1 &&
					e_ctrl->cal_data.num_data == FROM_REAR_HEADER_SIZE) {
					// run this on eebin check
					rc = cam_eeprom_get_phone_ver(e_ctrl, csl_packet);
				}
			}

			rc = cam_eeprom_get_cal_data(e_ctrl, csl_packet);
			rc = cam_eeprom_power_down(e_ctrl);
			e_ctrl->cam_eeprom_state = CAM_EEPROM_ACQUIRE;
			vfree(e_ctrl->cal_data.mapdata);
			vfree(e_ctrl->cal_data.map);
			kfree(power_info->power_setting);
			kfree(power_info->power_down_setting);
			power_info->power_setting = NULL;
			power_info->power_down_setting = NULL;
			power_info->power_setting_size = 0;
			power_info->power_down_setting_size = 0;
			e_ctrl->cal_data.num_data = 0;
			e_ctrl->cal_data.num_map = 0;
		}
		break;
	default:
		break;
	}
	return rc;
power_down:
#if defined(CONFIG_EEPROM_FORCE_DOWN)
	cam_eeprom_force_power_down(e_ctrl);
#else
	cam_eeprom_power_down(e_ctrl);
#endif
memdata_free:
	vfree(e_ctrl->cal_data.mapdata);
error:
	kfree(power_info->power_setting);
	kfree(power_info->power_down_setting);
	power_info->power_setting = NULL;
	power_info->power_down_setting = NULL;
	vfree(e_ctrl->cal_data.map);
	e_ctrl->cal_data.num_data = 0;
	e_ctrl->cal_data.num_map = 0;
	e_ctrl->cam_eeprom_state = CAM_EEPROM_ACQUIRE;
	return rc;
}

void cam_eeprom_shutdown(struct cam_eeprom_ctrl_t *e_ctrl)
{
	int rc;
	struct cam_eeprom_soc_private  *soc_private =
		(struct cam_eeprom_soc_private *)e_ctrl->soc_info.soc_private;
	struct cam_sensor_power_ctrl_t *power_info = &soc_private->power_info;

	if (e_ctrl->cam_eeprom_state == CAM_EEPROM_INIT)
		return;

	if (e_ctrl->cam_eeprom_state == CAM_EEPROM_CONFIG) {
		rc = cam_eeprom_power_down(e_ctrl);
		if (rc < 0)
			CAM_ERR(CAM_EEPROM, "EEPROM Power down failed");
		e_ctrl->cam_eeprom_state = CAM_EEPROM_ACQUIRE;
	}

	if (e_ctrl->cam_eeprom_state == CAM_EEPROM_ACQUIRE) {
		rc = cam_destroy_device_hdl(e_ctrl->bridge_intf.device_hdl);
		if (rc < 0)
			CAM_ERR(CAM_EEPROM, "destroying the device hdl");

		e_ctrl->bridge_intf.device_hdl = -1;
		e_ctrl->bridge_intf.link_hdl = -1;
		e_ctrl->bridge_intf.session_hdl = -1;

		kfree(power_info->power_setting);
		kfree(power_info->power_down_setting);
		power_info->power_setting = NULL;
		power_info->power_down_setting = NULL;
		power_info->power_setting_size = 0;
		power_info->power_down_setting_size = 0;
	}

	e_ctrl->cam_eeprom_state = CAM_EEPROM_INIT;
}

/**
 * cam_eeprom_driver_cmd - Handle eeprom cmds
 * @e_ctrl:     ctrl structure
 * @arg:        Camera control command argument
 *
 * Returns success or failure
 */
int32_t cam_eeprom_driver_cmd(struct cam_eeprom_ctrl_t *e_ctrl, void *arg)
{
	int                            rc = 0;
	struct cam_eeprom_query_cap_t  eeprom_cap = {0};
	struct cam_control            *cmd = (struct cam_control *)arg;

	if (!e_ctrl || !cmd) {
		CAM_ERR(CAM_EEPROM, "Invalid Arguments");
		return -EINVAL;
	}

	if (cmd->handle_type != CAM_HANDLE_USER_POINTER) {
		CAM_ERR(CAM_EEPROM, "Invalid handle type: %d",
			cmd->handle_type);
		return -EINVAL;
	}

	mutex_lock(&(e_ctrl->eeprom_mutex));
	switch (cmd->op_code) {
	case CAM_QUERY_CAP:
		eeprom_cap.slot_info = e_ctrl->soc_info.index;
		if (e_ctrl->userspace_probe == false)
			eeprom_cap.eeprom_kernel_probe = true;
		else
			eeprom_cap.eeprom_kernel_probe = false;

		eeprom_cap.is_multimodule_node =
			e_ctrl->is_multimodule_node;
		CAM_INFO(CAM_EEPROM, "eeprom_cap.is_multimodule_node: %d, e_ctrl->is_multimodule_node: %d",
			eeprom_cap.is_multimodule_node, e_ctrl->is_multimodule_node);

		if (copy_to_user(u64_to_user_ptr(cmd->handle),
			&eeprom_cap,
			sizeof(struct cam_eeprom_query_cap_t))) {
			CAM_ERR(CAM_EEPROM, "Failed Copy to User");
			rc = -EFAULT;
			goto release_mutex;
		}
		CAM_DBG(CAM_EEPROM, "eeprom_cap: ID: %d", eeprom_cap.slot_info);
		break;
	case CAM_ACQUIRE_DEV:
		rc = cam_eeprom_get_dev_handle(e_ctrl, arg);
		if (rc) {
			CAM_ERR(CAM_EEPROM, "Failed to acquire dev");
			goto release_mutex;
		}
		e_ctrl->cam_eeprom_state = CAM_EEPROM_ACQUIRE;
		break;
	case CAM_RELEASE_DEV:
		if (e_ctrl->cam_eeprom_state != CAM_EEPROM_ACQUIRE) {
			rc = -EINVAL;
			CAM_WARN(CAM_EEPROM,
			"Not in right state to release : %d",
			e_ctrl->cam_eeprom_state);
			goto release_mutex;
		}

		if (e_ctrl->bridge_intf.device_hdl == -1) {
			CAM_ERR(CAM_EEPROM,
				"Invalid Handles: link hdl: %d device hdl: %d",
				e_ctrl->bridge_intf.device_hdl,
				e_ctrl->bridge_intf.link_hdl);
			rc = -EINVAL;
			goto release_mutex;
		}
		rc = cam_destroy_device_hdl(e_ctrl->bridge_intf.device_hdl);
		if (rc < 0)
			CAM_ERR(CAM_EEPROM,
				"failed in destroying the device hdl");
		e_ctrl->bridge_intf.device_hdl = -1;
		e_ctrl->bridge_intf.link_hdl = -1;
		e_ctrl->bridge_intf.session_hdl = -1;
		e_ctrl->cam_eeprom_state = CAM_EEPROM_INIT;
		break;
	case CAM_CONFIG_DEV:
		rc = cam_eeprom_pkt_parse(e_ctrl, arg);
		if (rc) {
			CAM_ERR(CAM_EEPROM, "Failed in eeprom pkt Parsing");
			goto release_mutex;
		}
		break;
	default:
		CAM_DBG(CAM_EEPROM, "invalid opcode");
		break;
	}

release_mutex:
	mutex_unlock(&(e_ctrl->eeprom_mutex));

	return rc;
}

