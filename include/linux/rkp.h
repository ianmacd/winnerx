#ifndef _RKP_H
#define _RKP_H

#ifndef __ASSEMBLY__

/* uH_RKP Command ID */
enum {
	RKP_START = 1,
	RKP_DEFERRED_START,
	RKP_WRITE_PGT1,
	RKP_WRITE_PGT2,
	RKP_WRITE_PGT3,
	RKP_EMULT_TTBR0,
	RKP_EMULT_TTBR1,
	RKP_EMULT_DORESUME,
	RKP_FREE_PGD,
	RKP_NEW_PGD,
	RKP_KASLR_MEM,
	RKP_FIMC_VERIFY, /* CFP cmds */
	RKP_JOPP_INIT,
	RKP_ROPP_INIT,
	RKP_ROPP_SAVE,
	RKP_ROPP_RELOAD,
	/* and KDT cmds */
	RKP_NOSHIP,
#ifdef CONFIG_RKP_TEST
	CMD_ID_TEST_GET_PAR = 0x81,
	CMD_ID_TEST_GET_RO = 0x83,
	CMD_ID_TEST_GET_VA_XN,
	CMD_ID_TEST_GET_VMM_INFO,
#endif
	RKP_MAX
};

#ifdef CONFIG_RKP_TEST
#define RKP_INIT_MAGIC		0x5afe0002
#else
#define RKP_INIT_MAGIC		0x5afe0001
#endif

/* For RKP mem reserves */
extern u64 robuf_size_rkp;
extern u64 robuf_start_rkp;
extern u64 phys_map_size_rkp;

#define RKP_ROBUF_START		({ robuf_start_rkp; })
#define RKP_ROBUF_START_6G		(0xB080A000)
#define RKP_ROBUF_START_8G		(0xB0A0A000)
#define RKP_ROBUF_START_12G		(0xB0E0A000)

#define RKP_ROBUF_SIZE		({ robuf_size_rkp; })
#define RKP_ROBUF_SIZE_6G		(0x8F6000)
#define RKP_ROBUF_SIZE_8G		(0xAF6000)
#define RKP_ROBUF_SIZE_12G		(0xEF6000)

#define RKP_PHYS_MAP_START	(0xB020A000)
#define RKP_PHYS_MAP_SIZE	({ phys_map_size_rkp; })
#define RKP_PHYS_MAP_SIZE_6G	(6ULL << 20)
#define RKP_PHYS_MAP_SIZE_8G	(8ULL << 20)
#define RKP_PHYS_MAP_SIZE_12G	(4ULL << 20)

#define RKP_PGT_BITMAP_LEN	(0x60000)

/*
 * The following all assume PHYS_OFFSET is fix addr
 */
extern u64 max_pfn_for_rkp;
#define	PHYS_PFN_OFFSET_MIN_DRAM	(0x80000ULL)
#define	PHYS_PFN_OFFSET_MAX_DRAM	({ max_pfn_for_rkp; })

extern u64 pfn_min_dram2;
extern u64 pfn_max_dram2;
#define	PHYS_PFN_OFFSET_MIN_DRAM1	(0x80000ULL)
#define	PHYS_PFN_OFFSET_MAX_DRAM1	(0x180000ULL)
#define	PHYS_PFN_OFFSET_MIN_DRAM2	({ pfn_min_dram2; })
#define	PHYS_PFN_OFFSET_MAX_DRAM2	({ pfn_max_dram2; })
#define	PHYS_PFN_OFFSET_MIN_DRAM2_68G	(0x180000ULL)
#define	PHYS_PFN_OFFSET_MAX_DRAM2_68G	(0x280000ULL)
#define	PHYS_PFN_OFFSET_MIN_DRAM2_12G	(0x200000ULL)
#define	PHYS_PFN_OFFSET_MAX_DRAM2_12G	(0x400000ULL)
#define DRAM_PFN_GAP                    (PHYS_PFN_OFFSET_MIN_DRAM2 - PHYS_PFN_OFFSET_MAX_DRAM1)

#if 0
/* For RKP mem reserves */
#define RKP_PHYS_MAP_START		(0xB020A000)

#ifdef UH_RKP_SUPPORT_12G 
#define RKP_ROBUF_START			(0xB0E0A000)
#define RKP_ROBUF_SIZE			(0x4F6000)

#define RKP_PHYS_MAP_SIZE		(0xC << 20)
#define RKP_PGT_BITMAP_LEN		(0x60000)

#define	PHYS_PFN_OFFSET_MIN_DRAM	(0x80000ULL)
#define	PHYS_PFN_OFFSET_MAX_DRAM	(0x480000ULL)

#else /***************** FORR 6G/8G ******************/
#define RKP_ROBUF_START			(0xB0A0A000)
#define RKP_ROBUF_SIZE			(0x8F6000)

#define RKP_PHYS_MAP_SIZE		(0x8 << 20)
#define RKP_PGT_BITMAP_LEN		(0x40000)

#define	PHYS_PFN_OFFSET_MIN_DRAM	(0x80000ULL)
#define	PHYS_PFN_OFFSET_MAX_DRAM	(0x280000ULL)
#endif
#endif

#define RKP_RBUF_VA		(phys_to_virt(RKP_ROBUF_START))
#define RO_PAGES_12G		(RKP_ROBUF_SIZE_12G >> PAGE_SHIFT) // (RKP_ROBUF_SIZE/PAGE_SIZE)
#define RO_PAGES		(RKP_ROBUF_SIZE >> PAGE_SHIFT) // (RKP_ROBUF_SIZE/PAGE_SIZE)
#define CRED_JAR_RO		"cred_jar_ro"
#define TSEC_JAR		"tsec_jar"
#define VFSMNT_JAR		"vfsmnt_cache"

extern u8 rkp_pgt_bitmap[];
extern u8 rkp_map_bitmap[];
extern u8 rkp_def_init_done;
extern u8 rkp_started;
extern void *rkp_ro_alloc(void);
extern void rkp_ro_free(void *free_addr);
extern unsigned int is_rkp_ro_page(u64 addr);
int rkp_memory(u32 ram_size);

struct rkp_init { //copy from uh (app/rkp/rkp.h)
	u32 magic;
	u64 vmalloc_start;
	u64 vmalloc_end;
	u64 init_mm_pgd;
	u64 id_map_pgd;
	u64 zero_pg_addr;
	u64 rkp_pgt_bitmap;
	u64 rkp_dbl_bitmap;
	u32 rkp_bitmap_size;
	u32 no_fimc_verify;
	u64 fimc_phys_addr;
	u64 _text;
	u64 _etext;
	u64 extra_memory_addr;
	u32 extra_memory_size;
	u64 physmap_addr; //not used. what is this for?
	u64 _srodata;
	u64 _erodata;
	u32 large_memory;
};

#ifdef CONFIG_RKP_KDP
typedef struct __attribute__((__packed__)) kdp_init_struct {
	u32 credSize;
	u32 sp_size;
	u32 pgd_mm;
	u32 uid_cred;
	u32 euid_cred;
	u32 gid_cred;
	u32 egid_cred;
	u32 bp_pgd_cred;
	u32 bp_task_cred;
	u32 type_cred;
	u32 security_cred;
	u32 usage_cred;
	u32 cred_task;
	u32 mm_task;
	u32 pid_task;
	u32 rp_task;
	u32 comm_task;
	u32 bp_cred_secptr;
	u64 verifiedbootstate;
} kdp_init_t;
#endif  /* CONFIG_RKP_KDP */

#ifdef CONFIG_RKP_NS_PROT
typedef struct ns_param {
	u32 ns_buff_size;
	u32 ns_size;
	u32 bp_offset;
	u32 sb_offset;
	u32 flag_offset;
	u32 data_offset;
}ns_param_t;

#define rkp_ns_fill_params(nsparam,buff_size,size,bp,sb,flag,data)	\
do {						\
	nsparam.ns_buff_size = (u64)buff_size;		\
	nsparam.ns_size  = (u64)size;		\
	nsparam.bp_offset = (u64)bp;		\
	nsparam.sb_offset = (u64)sb;		\
	nsparam.flag_offset = (u64)flag;		\
	nsparam.data_offset = (u64)data;		\
} while(0)
#endif /* CONFIG_RKP_NS_PROT */

#define rkp_is_pg_protected(va)	rkp_is_protected(va, __pa(va), (u64 *)rkp_pgt_bitmap, 1)
#define rkp_is_pg_dbl_mapped(pa) rkp_is_protected((u64)__va(pa), pa, (u64 *)rkp_map_bitmap, 0)

#define RKP_PHYS_ADDR_MASK		((1ULL << 40)-1)

struct test_case_struct {
	int (* fn)(void); //test case func
	char * describe;
};

static inline void rkp_deferred_init(){
	rkp_def_init_done = 1;
	isb();
	uh_call(UH_APP_RKP, RKP_DEFERRED_START, 0, 0, 0, 0);
}

static inline u64 rkp_get_sys_index(u64 pfn)
{
        if (pfn >= PHYS_PFN_OFFSET_MIN_DRAM1
                && pfn < PHYS_PFN_OFFSET_MAX_DRAM1) {
                return ((pfn) - PHYS_PFN_OFFSET);
        }
        if (pfn >= PHYS_PFN_OFFSET_MIN_DRAM2
                && pfn < PHYS_PFN_OFFSET_MAX_DRAM2) {
                return ((pfn) - PHYS_PFN_OFFSET - DRAM_PFN_GAP);
        }
        return (~0ULL);
}

static inline u8 rkp_is_protected(u64 va, u64 pa, u64 *base_addr, u64 type)
{
	u64 phys_addr = pa & (RKP_PHYS_ADDR_MASK);
	u64 *p = base_addr;
	u64 rindex;
	u8 val;
	u64 index = rkp_get_sys_index((phys_addr>>PAGE_SHIFT));

	if (index == (~0ULL))
		return 0;

	p += (index>>6);
	rindex = index % 64;
	val = (((*p) & (1ULL<<rindex))?1:0);
	return val;
}
#endif //__ASSEMBLY__
#endif //_RKP_H
