/*
 * This file is part of the coreboot project.
 *
 * Copyright 2015 Google Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <arch/cache.h>
#include <arch/exception.h>
#include <arch/stages.h>
#include <armv7.h>
#include <cbfs.h>
#include <cbmem.h>
#include <console/console.h>
#include <delay.h>
#include <program_loading.h>
#include <soc/sdram.h>
#include <stdlib.h>
#include <symbols.h>
#include <timestamp.h>
#include <types.h>
#include <vendorcode/google/chromeos/chromeos.h>

void main(void)
{
#if CONFIG_COLLECT_TIMESTAMPS
	uint64_t start_romstage_time;
	uint64_t before_dram_time;
	uint64_t after_dram_time;
	uint64_t base_time = timestamp_get();
	start_romstage_time = timestamp_get();
#endif

	console_init();

#if CONFIG_COLLECT_TIMESTAMPS
	before_dram_time = timestamp_get();
#endif

	sdram_init();

#if CONFIG_COLLECT_TIMESTAMPS
	after_dram_time = timestamp_get();
#endif

	/* Now that DRAM is up, add mappings for it and DMA coherency buffer. */
	mmu_config_range((uintptr_t)_dram/MiB,
			 sdram_size_mb(), DCACHE_WRITEBACK);
	mmu_config_range((uintptr_t)_dma_coherent/MiB,
			 _dma_coherent_size/MiB, DCACHE_OFF);

	cbmem_initialize_empty();

#if CONFIG_COLLECT_TIMESTAMPS
	timestamp_init(base_time);
	timestamp_add(TS_START_ROMSTAGE, start_romstage_time);
	timestamp_add(TS_BEFORE_INITRAM, before_dram_time);
	timestamp_add(TS_AFTER_INITRAM, after_dram_time);

	timestamp_add_now(TS_END_ROMSTAGE);
#endif

	run_ramstage();
}
