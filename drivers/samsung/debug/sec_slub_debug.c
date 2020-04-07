// SPDX-License-Identifier: GPL-2.0
/*
 * drivers/samsung/debug/sec_slub_debug.c
 *
 * COPYRIGHT(C) 2019 Samsung Electronics Co., Ltd. All Right Reserved.
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

#define pr_fmt(fmt)     KBUILD_MODNAME ":%s() " fmt, __func__

#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/of.h>
#include <linux/mm.h>
#include <linux/slab.h>
#include <linux/slub_def.h>
#include <linux/spinlock.h>
#include <linux/stacktrace.h>

/* [[BEGIN>> mm/slub.c */
#define TRACK_ADDRS_COUNT	16
/* <<END]] mm/slub.c */

struct free_object {
	unsigned long fp;
	unsigned long entries[];
};

struct free_track_config {
	const char *name;
	int skip;
	struct kmem_cache *kmem_cache;
	unsigned int max_entries;
};

static struct free_track_config *free_track_black_list;
static unsigned int sz_black_list __read_mostly;
static unsigned int sz_initialized __read_mostly;

static int __init __slub_debug_init_free_trace_config(struct device_node *np)
{
	int nr_names, nr_skips;
	int i;

	nr_names = of_property_count_strings(np, "free_track,black_list-names");
	nr_skips = of_property_count_u32_elems(np, "free_track,black_list-skips");
	if ((nr_names != nr_skips) || !nr_names || !nr_skips)
		return -EINVAL;

	free_track_black_list = alloc_pages_exact(nr_names * sizeof(struct free_track_config),
			GFP_KERNEL);
	if (!free_track_black_list)
		return -ENOMEM;

	for (i = 0; i < nr_names; i++) {
		of_property_read_string_index(np, "free_track,black_list-names",
					   i, &free_track_black_list[i].name);
		of_property_read_u32_index(np, "free_track,black_list-skips",
					   i, &free_track_black_list[i].skip);

		free_track_black_list[i].kmem_cache = NULL;
		free_track_black_list[i].max_entries = 0;
	}

	sz_black_list = nr_names;

	return 0;
}

static struct free_track_config *__slub_debug_free_track_black_list_fast_path(struct kmem_cache *s)
{
	size_t i;

	for (i = 0; i < sz_black_list; i++)
		if (s == free_track_black_list[i].kmem_cache)
			return &free_track_black_list[i];

	return NULL;
}

static struct free_track_config *__slub_debug_free_track_black_list_slow_path(struct kmem_cache *s)
{
	struct free_track_config *found;
	unsigned int max_entries;
	const char *name;
	size_t i;

	for (i = 0; i < sz_black_list; i++) {
		if (s == free_track_black_list[i].kmem_cache)
			return &free_track_black_list[i];
		else if (free_track_black_list[i].kmem_cache)
			continue;

		name = free_track_black_list[i].name;
		if (!strncmp(name, s->name, strlen(name))) {
			found = &free_track_black_list[i];
			goto init_free_track_config;
		}
	}

	return NULL;

init_free_track_config:
	found->kmem_cache = s;

	max_entries = (s->object_size - sizeof(struct free_object)) / sizeof(unsigned long);
	found->max_entries = min_t(unsigned int, max_entries, TRACK_ADDRS_COUNT);

	sz_initialized++;

	return found;
}

static struct free_track_config *__slub_debug_free_track_black_list(struct kmem_cache *s)
{
	struct free_track_config *ft_cfg;
	static DEFINE_SPINLOCK(lock);
	unsigned long flags;

	if (likely(!sz_black_list))
		return NULL;

	spin_lock_irqsave(&lock, flags);

	if (likely(sz_black_list == sz_initialized))
		ft_cfg = __slub_debug_free_track_black_list_fast_path(s);
	else
		ft_cfg = __slub_debug_free_track_black_list_slow_path(s);

	spin_unlock_irqrestore(&lock, flags);

	return ft_cfg;
}

void sec_slub_debug_save_free_track(struct kmem_cache *s, void *x)
{
	struct free_track_config *ft_cfg;
	struct stack_trace trace;
	unsigned long i;
	struct free_object *fobj = x;

	ft_cfg = __slub_debug_free_track_black_list(s);
	if (likely(!ft_cfg))
		return;

	trace.nr_entries = 0;
	trace.max_entries = ft_cfg->max_entries;
	trace.entries = fobj->entries;
	trace.skip = ft_cfg->skip;

	save_stack_trace(&trace);

	if (trace.nr_entries != 0 &&
		trace.entries[trace.nr_entries - 1] == ULONG_MAX)
		trace.nr_entries--;

	for (i = trace.nr_entries; i < trace.max_entries; i++)
		trace.entries[i] = 0;
}

void sec_slub_debug_panic_on_fp_corrupted(struct kmem_cache *s, void *x, void *fp)
{
	struct free_track_config *ft_cfg;
	struct stack_trace trace;
	struct free_object *fobj = x;

	ft_cfg = __slub_debug_free_track_black_list(s);
	if (likely(!ft_cfg))
		return;

	if (((fp == NULL) || (PAGE_OFFSET == ((uintptr_t)fp & PAGE_OFFSET))))
		return;

	for (trace.nr_entries = 0; trace.nr_entries < trace.max_entries; trace.nr_entries++) {
		if (!trace.entries[trace.nr_entries])
			break;
	}

	pr_warn("SLUB - stack trace can be tainted!\n");

	trace.max_entries = ft_cfg->max_entries;
	trace.entries = fobj->entries;
	trace.skip = ft_cfg->skip;

	print_stack_trace(&trace, 0);

	panic("SLUB - Invalid fp in the freed object.");
}

static int __init __sec_debug_init_of(void)
{
	struct device_node *np;
	int err;

	np = of_find_node_by_name(NULL, "samsung,sec_slub_debug");
	if (!np)
		return -ENODEV;

	err = __slub_debug_init_free_trace_config(np);
	if (err)
		return err;

	return 0;
}

static int __init sec_slub_debug_init(void)
{
	int err;

	err = __sec_debug_init_of();
	if (err)
		return err;

	return 0;
}
early_initcall(sec_slub_debug_init);
