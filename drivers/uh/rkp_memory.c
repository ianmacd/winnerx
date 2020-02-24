#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/uh.h>
#include <linux/memblock.h>

#define RKP_START_ADDR		(0xB0200000)
#define RKP_DT_END_ADDR		(0xB1D00000)
#define THE_BIGGEST_RAM		(0xC)

#ifndef CONFIG_NO_BOOTMEM
static inline void check_config_no_bootmem(void){
	/* ARM64 does not use BOOTMEM.
	   But if CONFIG_NO_BOOTMEM is not defined, 
	   memblock_free is not working. 
	*/
	printk("RKP: [ERR] CONFIG_NO_BOOTMEM is not set\n");
}
#endif

int __init rkp_memory(u32 ram_size)
{
	phys_addr_t free_addr = RKP_DT_END_ADDR;
	phys_addr_t free_size = 0;
	int ret = 0;

#ifndef CONFIG_NO_BOOTMEM
	check_config_no_bootmem();
#endif
	
	if(ram_size == THE_BIGGEST_RAM) 
		goto log;

	// PHYS_MAP(1M per 1G), RO_BUF(1M per 1G)
	free_size = (((THE_BIGGEST_RAM - ram_size) * 2) << 20);
	free_addr = RKP_DT_END_ADDR - free_size;
	printk("[RKP] Ram size: %d, free size: 0x%lx, free addr: %lx\n",
		ram_size, free_size, free_addr);

	set_memsize_reserved_name("rkp");
	ret = memblock_free(free_addr, free_size);
	unset_memsize_reserved_name();
	if (ret != 0) {
		printk("[RKP] Failed free memory: 0x%x, size: 0x%x, ret: %d\n",
			(unsigned int)free_addr, (unsigned int)free_size, ret);
		return -1;
	}

log:
	printk("RKP Memory Region : 0x%x - 0x%x\n",
		(unsigned int)RKP_START_ADDR, 
		(unsigned int)free_addr);

	return 0;
}
