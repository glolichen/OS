#ifndef PMM_H
#define PMM_H

#include <stdbool.h>
#include "multiboot.h"
#include "const.h"

void pmm_init(multiboot_info_t *info, u32 magic, u32 bitmap_location);
u32 *pmm_allocate_blocks(u32 size);
void pmm_free_blocks(u32 *address, u32 size);

#endif