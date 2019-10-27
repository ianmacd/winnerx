/*
 * Copyright (c) 2017, The Linux Foundation. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */
#define pr_fmt(fmt)	"[drm-dp] %s: " fmt, __func__

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/device.h>
#include <drm/drm_edid.h>
#ifdef CONFIG_SEC_DISPLAYPORT_BIGDATA
#include <linux/displayport_bigdata.h>
#endif

#include "secdp.h"
#include "secdp_sysfs.h"
#include "sde_edid_parser.h"
#include "secdp_unit_test.h"

enum secdp_unit_test_cmd {
	SECDP_UTCMD_EDID_PARSE = 0,
};

struct secdp_sysfs_private {
	struct device *dev;
	struct secdp_sysfs dp_sysfs;
	struct secdp_misc *sec;
	enum secdp_unit_test_cmd test_cmd;
};

struct secdp_sysfs_private *g_secdp_sysfs;

static inline char *secdp_utcmd_to_str(u32 cmd_type)
{
	switch (cmd_type) {
	case SECDP_UTCMD_EDID_PARSE:
		return SECDP_ENUM_STR(SECDP_UTCMD_EDID_PARSE);
	default:
		return "unknown";
	}
}

static ssize_t dp_sbu_sw_sel_store(struct class *dev,
				struct class_attribute *attr, const char *buf, size_t size)
{
	int val[10] = {0,};
	int sbu_sw_sel, sbu_sw_oe;

	get_options(buf, 10, val);

	sbu_sw_sel = val[1];
	sbu_sw_oe = val[2];
	pr_info("sbu_sw_sel(%d), sbu_sw_oe(%d)\n", sbu_sw_sel, sbu_sw_oe);

	if (sbu_sw_oe == 0/*on*/)
		secdp_config_gpios_factory(sbu_sw_sel, true);
	else if (sbu_sw_oe == 1/*off*/)
		secdp_config_gpios_factory(sbu_sw_sel, false);
	else
		pr_err("unknown sbu_sw_oe value: %d\n", sbu_sw_oe);

	return size;
}

static CLASS_ATTR_WO(dp_sbu_sw_sel);

static ssize_t dp_forced_resolution_show(struct class *class,
				struct class_attribute *attr, char *buf)
{
	int rc = 0;
	int vic = 1;

	if (forced_resolution) {
		rc = scnprintf(buf, PAGE_SIZE,
			"%d : %s\n", forced_resolution,
			secdp_vic_to_string(forced_resolution));

	} else {
		while (secdp_vic_to_string(vic) != NULL) {
			rc += scnprintf(buf + rc, PAGE_SIZE - rc,
					"%d : %s\n", vic, secdp_vic_to_string(vic));
			vic++;
		}
	}

	return rc;
}

static ssize_t dp_forced_resolution_store(struct class *dev,
				struct class_attribute *attr, const char *buf, size_t size)
{
	int val[10] = {0, };

	get_options(buf, 10, val);

	if (val[1] <= 0)
		forced_resolution = 0;
	else
		forced_resolution = val[1];

	return size;
}

static CLASS_ATTR_RW(dp_forced_resolution);

static ssize_t dex_show(struct class *class,
				struct class_attribute *attr, char *buf)
{
	int rc = 0;
	struct secdp_sysfs_private *sysfs = g_secdp_sysfs;
	struct secdp_dex *dex = &sysfs->sec->dex;

	if (!secdp_get_cable_status() || !secdp_get_hpd_status()) {
		pr_info("cable is out\n");
		dex->prev = dex->curr = DEX_DISABLED;
	}

	pr_info("prev: %d, curr: %d\n", dex->prev, dex->curr);
	rc = scnprintf(buf, PAGE_SIZE, "%d\n", dex->curr);

	if (dex->curr == DEX_DURING_MODE_CHANGE)
		dex->curr = DEX_ENABLED;

	return rc;
}

static ssize_t dex_store(struct class *class,
		struct class_attribute *attr, const char *buf, size_t size)
{
	int val[4] = {0,};
	int setting_ui;	/* setting has Dex mode? if yes, 1. otherwise 0 */
	int run;		/* dex is running now? if yes, 1. otherwise 0 */

	struct secdp_sysfs_private *sysfs = g_secdp_sysfs;
	struct secdp_misc *sec = sysfs->sec;
	struct secdp_dex *dex = &sec->dex;

	if (secdp_check_if_lpm_mode()) {
		pr_info("it's LPM mode. skip\n");
		goto exit;
	}

	get_options(buf, 4, val);
	pr_info("%d(0x%02x)\n", val[1], val[1]);
	setting_ui = (val[1] & 0xf0) >> 4;
	run = (val[1] & 0x0f);

	pr_info("setting_ui: %d, run: %d, cable: %d\n",
		setting_ui, run, sec->cable_connected);

	dex->setting_ui = setting_ui;
	dex->curr = run;

	mutex_lock(&sec->notifier_lock);
	if (!sec->ccic_noti_registered) {
		int rc;

		pr_debug("notifier get registered by dex\n");

		/* cancel immediately */
		rc = cancel_delayed_work(&sec->ccic_noti_reg_work);
		pr_debug("cancel_work, rc(%d)\n", rc);
		destroy_delayed_work_on_stack(&sec->ccic_noti_reg_work);

		/* register */
		rc = secdp_ccic_noti_register_ex(sec, false);
		if (rc)
			pr_err("noti register fail, rc(%d)\n", rc);

		mutex_unlock(&sec->notifier_lock);
		goto exit;
	}
	mutex_unlock(&sec->notifier_lock);

	if (!secdp_get_cable_status() || !secdp_get_hpd_status()) {
		pr_info("cable is out\n");
		dex->prev = dex->curr = DEX_DISABLED;
		goto exit;
	}

	if (dex->curr == dex->prev) {
		pr_info("dex is %s already\n", dex->curr ? "enabled" : "disabled");
		goto exit;
	}
	
	if (dex->curr != dex->setting_ui) {
		pr_info("values of cur(%d) and setting_ui(%d) are difference\n", dex->curr, dex->setting_ui);
		goto exit;
	}

#ifdef CONFIG_SEC_DISPLAYPORT_BIGDATA
	if (run)
		secdp_bigdata_save_item(BD_DP_MODE, "DEX");
	else
		secdp_bigdata_save_item(BD_DP_MODE, "MIRROR");
#endif

	if (sec->dex.res == DEX_RES_NOT_SUPPORT) {
		pr_debug("this dongle does not support dex\n");
		goto exit;
	}

	if (!secdp_check_dex_reconnect()) {
		pr_info("not need reconnect\n");
		goto exit;
	}

	secdp_dex_do_reconnecting();

	dex->prev = run;
exit:
	return size;
}

static CLASS_ATTR_RW(dex);

static ssize_t dex_ver_show(struct class *class,
				struct class_attribute *attr, char *buf)
{
	int rc;
	struct secdp_sysfs_private *sysfs = g_secdp_sysfs;
	struct secdp_misc *sec = sysfs->sec;
	struct secdp_dex *dex = &sec->dex;

	pr_info("branch revision: HW(0x%X), SW(0x%X, 0x%X)\n",
		dex->fw_ver[0], dex->fw_ver[1], dex->fw_ver[2]);

	rc = scnprintf(buf, PAGE_SIZE, "%02X%02X\n",
		dex->fw_ver[1], dex->fw_ver[2]);

	return rc;
}

static CLASS_ATTR_RO(dex_ver);

/* note: needs test once wifi is fixed */
static ssize_t monitor_info_show(struct class *class,
				struct class_attribute *attr, char *buf)
{
	int rc = 0;
	short prod_id = 0;
	struct dp_panel *info = NULL;
	struct sde_edid_ctrl *edid_ctrl = NULL;
	struct edid *edid = NULL;

	info = secdp_get_panel_info();
	if (!info) {
		pr_err("unable to find panel info\n");
		goto exit;
	}

	edid_ctrl = info->edid_ctrl;
	if (!edid_ctrl) {
		pr_err("unable to find edid_ctrl\n");
		goto exit;
	}

	edid = edid_ctrl->edid;
	if (!edid) {
		pr_err("unable to find edid\n");
		goto exit;
	}

	pr_debug("prod_code[0]: %02x, [1]: %02x\n", edid->prod_code[0], edid->prod_code[1]);
	prod_id |= (edid->prod_code[0] << 8) | (edid->prod_code[1]);
	pr_debug("prod_id: %04x\n", prod_id);

	rc = sprintf(buf, "%s,0x%x,0x%x\n",
			edid_ctrl->vendor_id, prod_id, edid->serial); /* byte order? */

exit:
	return rc;
}

static CLASS_ATTR_RO(monitor_info);

static ssize_t dp_unit_test_show(struct class *class,
				struct class_attribute *attr, char *buf)
{
	struct secdp_sysfs_private *sysfs = g_secdp_sysfs;
	int rc, cmd = sysfs->test_cmd;
	bool res = false;

	pr_info("test_cmd: %s\n", secdp_utcmd_to_str(cmd));

	switch (cmd) {
	case SECDP_UTCMD_EDID_PARSE:
		res = secdp_unit_test_edid_parse();
		break;
	default:
		pr_info("invalid test_cmd: %d\n", cmd);
		break;
	}

	rc = scnprintf(buf, 3, "%d\n", res ? 1 : 0);
	return rc;
}

static ssize_t dp_unit_test_store(struct class *dev,
				struct class_attribute *attr, const char *buf, size_t size)
{
	struct secdp_sysfs_private *sysfs = g_secdp_sysfs;
	int val[10] = {0, };

	get_options(buf, 10, val);
	sysfs->test_cmd = val[1];

	pr_info("test_cmd: %d...%s\n", sysfs->test_cmd, secdp_utcmd_to_str(sysfs->test_cmd));

	return size;
}

static CLASS_ATTR_RW(dp_unit_test);

#ifdef CONFIG_SEC_DISPLAYPORT_BIGDATA
static ssize_t dp_error_info_show(struct class *class,
				struct class_attribute *attr, char *buf)
{
	return _secdp_bigdata_show(class, attr, buf);
}

static ssize_t dp_error_info_store(struct class *dev,
				struct class_attribute *attr, const char *buf, size_t size)
{
	return _secdp_bigdata_store(dev, attr, buf, size);
}

static CLASS_ATTR_RW(dp_error_info);
#endif

#ifdef SECDP_CALIBRATE_VXPX
static ssize_t dp_vx_lvl_show(struct class *class,
				struct class_attribute *attr, char *buf)
{
	pr_debug("+++\n");
	secdp_catalog_vx_show();
	return 0;
}

static ssize_t dp_vx_lvl_store(struct class *dev,
				struct class_attribute *attr, const char *buf, size_t size)
{
	int i, val[30] = {0, };

	pr_debug("+++, size(%d)\n", (int)size);

	get_options(buf, 20, val);
	for (i = 0; i < 16; i=i+4)
		pr_debug("%02x,%02x,%02x,%02x\n", val[i+1],val[i+2],val[i+3],val[i+4]);

	secdp_catalog_vx_store(&val[1], 16);
	return size;
}

static CLASS_ATTR_RW(dp_vx_lvl);

static ssize_t dp_px_lvl_show(struct class *class,
				struct class_attribute *attr, char *buf)
{
	pr_debug("+++\n");
	secdp_catalog_px_show();
	return 0;
}

static ssize_t dp_px_lvl_store(struct class *dev,
				struct class_attribute *attr, const char *buf, size_t size)
{
	int i, val[30] = {0, };

	pr_debug("+++, size(%d)\n", (int)size);

	get_options(buf, 20, val);
	for (i = 0; i < 16; i=i+4)
		pr_debug("%02x,%02x,%02x,%02x\n", val[i+1],val[i+2],val[i+3],val[i+4]);

	secdp_catalog_px_store(&val[1], 16);
	return size;
}

static CLASS_ATTR_RW(dp_px_lvl);
#endif

enum {
	DP_SBU_SW_SEL = 0,
	DP_FORCED_RES,
	DEX,
	DEX_VER,
	MONITOR_INFO,
	DP_UNIT_TEST,
#ifdef CONFIG_SEC_DISPLAYPORT_BIGDATA
	DP_ERROR_INFO,
#endif
#ifdef SECDP_CALIBRATE_VXPX
	DP_VX_LVL,
	DP_PX_LVL,
#endif
};

static struct attribute *secdp_class_attrs[] = {
	[DP_SBU_SW_SEL]			= &class_attr_dp_sbu_sw_sel.attr,
	[DP_FORCED_RES]			= &class_attr_dp_forced_resolution.attr,
	[DEX]					= &class_attr_dex.attr,
	[DEX_VER]				= &class_attr_dex_ver.attr,
	[MONITOR_INFO]			= &class_attr_monitor_info.attr,
	[DP_UNIT_TEST]			= &class_attr_dp_unit_test.attr,
#ifdef CONFIG_SEC_DISPLAYPORT_BIGDATA
	[DP_ERROR_INFO]			= &class_attr_dp_error_info.attr,
#endif
#ifdef SECDP_CALIBRATE_VXPX
	[DP_VX_LVL]				= &class_attr_dp_vx_lvl.attr,
	[DP_PX_LVL]				= &class_attr_dp_px_lvl.attr,
#endif
	NULL,
};
ATTRIBUTE_GROUPS(secdp_class);

struct secdp_sysfs* secdp_sysfs_init(void)
{
	struct class *dp_class;
	struct secdp_sysfs *sysfs;
	int rc = -1;

	dp_class = kzalloc(sizeof(*dp_class), GFP_KERNEL);
	if (!dp_class) {
		pr_err("fail to alloc sysfs->dp_class\n");
		goto err_exit;
	}

	dp_class->name = "dp_sec";
	dp_class->owner = THIS_MODULE;
	dp_class->class_groups = secdp_class_groups;

	rc = class_register(dp_class);
	if (rc < 0) {
		pr_err("couldn't register secdp sysfs class, rc: %d\n", rc);
		goto free_exit;
	}

	sysfs = kzalloc(sizeof(*sysfs), GFP_KERNEL);
	if (!sysfs) {
		pr_err("fail to alloc sysfs\n");
		goto free_exit;
	}

	sysfs->dp_class = dp_class;

#ifdef CONFIG_SEC_DISPLAYPORT_BIGDATA
	secdp_bigdata_init(dp_class);
#endif

	pr_debug("success\n", rc);
	return sysfs;

free_exit:
	kzfree(dp_class);
err_exit:
	return NULL;
}

void secdp_sysfs_deinit(struct secdp_sysfs *sysfs)
{
	pr_debug("+++\n");

	if (sysfs) {
		if (sysfs->dp_class) {
			class_unregister(sysfs->dp_class);
			kzfree(sysfs->dp_class);
			sysfs->dp_class = NULL;
			pr_debug("freeing dp_class done\n");
		}

		kzfree(sysfs);
	}
}

struct secdp_sysfs *secdp_sysfs_get(struct device *dev, struct secdp_misc *sec)
{
	int rc = 0;
	struct secdp_sysfs_private *sysfs;
	struct secdp_sysfs *dp_sysfs;

	if (!dev || !sec) {
		pr_err("invalid input\n");
		rc = -EINVAL;
		goto error;
	}

	sysfs = devm_kzalloc(dev, sizeof(*sysfs), GFP_KERNEL);
	if (!sysfs) {
		rc = -EINVAL;
		goto error;
	}

	sysfs->dev   = dev;
	sysfs->sec   = sec;
	dp_sysfs = &sysfs->dp_sysfs;

	g_secdp_sysfs = sysfs;
	return dp_sysfs;

error:
	return ERR_PTR(rc);
}

void secdp_sysfs_put(struct secdp_sysfs *dp_sysfs)
{
	struct secdp_sysfs_private *sysfs;

	if (!dp_sysfs)
		return;

	sysfs = container_of(dp_sysfs, struct secdp_sysfs_private, dp_sysfs);
	devm_kfree(sysfs->dev, sysfs);

	g_secdp_sysfs = NULL;
}
