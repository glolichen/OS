#include <stdbool.h>
#include "pmm.h"
#include "printf.h"
#include "const.h"
#include "panic.h"

// each bit represents whether a block of memory is free
// because we do not use PSE for now, block size is 4KB = 4096B
u32 *bitmap;
u64 memory_size, total_blocks, used_blocks;

void pmm_set_block(u64 bit) {
	used_blocks++;
	bitmap[bit / 32] |= 1 << (31 - bit % 32);
}

void pmm_unset_block(u64 bit) {
	used_blocks--;
	bitmap[bit / 32] &= ~(1 << (31 - bit % 32));
}

void pmm_set_region(u32 base, u64 size) {
	base /= PMM_BLOCK_SIZE;
	size /= PMM_BLOCK_SIZE;
	for (u64 i = base; i < base + size; i++)
		pmm_set_block(i);
}

void pmm_unset_region(u32 base, u64 size) {
	base /= PMM_BLOCK_SIZE;
	size /= PMM_BLOCK_SIZE;
	for (u64 i = base; i < base + size; i++)
		pmm_unset_block(i);
	pmm_set_block(0);
}

bool pmm_is_block_free(u64 bit) {
	return (bitmap[bit / 32] >> (31 - bit % 32)) & 1;
}

u64 pmm_get_first_free() {
	// TODO
	return 0;
}

void pmm_init(multiboot_info_t *info, u32 magic, u32 bitmap_location) {
	if (magic != MULTIBOOT_BOOTLOADER_MAGIC)
		panic("Invalid multiboot magic number!");
	if (!(info->flags >> 6 & 0x1))
		panic("Invalid multiboot memory map!");

	// in bytes
	memory_size = 0;
	serial_info("Memory map:");
	for (u32 i = 0; i < info->mmap_length; i += sizeof(multiboot_memory_map_t)) {
		multiboot_memory_map_t *map = (multiboot_memory_map_t *) (info->mmap_addr + i);
		serial_info(
			"Region %d: start %x, length %x, size %d, type %d (%s)",
			i / sizeof(multiboot_memory_map_t),
			map->addr, map->len, map->size, map->type,
			MULTIBOOT_ENTRY_TYPES[map->type]
		);
		if (map->type == MULTIBOOT_MEMORY_AVAILABLE) {
			memory_size += map->len;
		}
	}
	total_blocks = memory_size / PMM_BLOCK_SIZE;
	used_blocks = total_blocks;
	serial_info("Detected memory size: %xKiB (%x blocks)", memory_size / 1024, total_blocks);

	bitmap = (u32 *) bitmap_location;
	serial_info("Bitmap placed at %d", bitmap);
	for (u64 i = 0; i < memory_size / 4; i++)
		bitmap[i] = 0xFFFFFFFF;
	
	for (u32 i = 0; i < info->mmap_length; i += sizeof(multiboot_memory_map_t)) {
		multiboot_memory_map_t *map = (multiboot_memory_map_t *) (info->mmap_addr + i);
		if (map->type == MULTIBOOT_MEMORY_AVAILABLE)
			pmm_unset_region(map->addr, map->len);
	}

	serial_info("%x used blocks, %x free blocks", used_blocks, total_blocks - used_blocks);
}

u32 *pmm_allocate_blocks(u32 size) {
	// TODO
	return (u32 *) 0;
}
void pmm_free_blocks(u32 *address, u32 size) {
	// TODO
}