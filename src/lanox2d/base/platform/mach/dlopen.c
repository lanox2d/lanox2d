/*!A lightweight and fast 2D vector graphics engine
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * Copyright (C) 2021-present, Lanox2D Open Source Group.
 *
 * @author      ruki
 * @file        dlopen.c
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "../../libc/libc.h"
#include <mach/mach.h>
#include <mach/machine.h>
#include <mach-o/dyld.h>
#include <mach-o/nlist.h>
#include <objc/runtime.h>

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */
#if defined(__LP64__)
#   define NLIST                struct nlist_64
#else
#   define NLIST                struct nlist
#endif

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

// the dynamic library context type for fake dlopen
typedef struct lx_fake_dlctx_t_ {
    lx_size_t                   image_index;
    struct mach_header const*   image_header;
}lx_fake_dlctx_t, *lx_fake_dlctx_ref_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * private implementation
 */

// get first cmd after image_header
static lx_pointer_t lx_get_first_cmd_after_header(struct mach_header const* image_header) {
    switch (image_header->magic) {
        case MH_MAGIC:
        case MH_CIGAM:
            return (lx_pointer_t)(image_header + 1);
        case MH_MAGIC_64:
        case MH_CIGAM_64:
            return (lx_pointer_t)(((struct mach_header_64*)image_header) + 1);
        default:
            // image_header is corrupt
            return 0;
    }
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
lx_dlimage_ref_t lx_dlopen(lx_char_t const* filename, lx_int_t flag) {
    lx_assert_and_check_return_val(filename, lx_null);

    /* skip the first image (master app), becasue it's symtab will be stripped after be archived.
     * so we need to keep our behavior consistent for debug/release and archived packages.
     */
    struct mach_header const* image_header = lx_null;
    lx_size_t const           image_count = _dyld_image_count();
    for (lx_size_t image_index = 1; image_index < image_count; image_index++) {
        image_header = _dyld_get_image_header((uint32_t)image_index);
        if (image_header) {
            lx_char_t const* image_name = _dyld_get_image_name(image_index);
            if (image_name && 0 == strcmp(image_name, filename)) {
                lx_fake_dlctx_ref_t dlctx = lx_malloc0_type(lx_fake_dlctx_t);
                if (dlctx) {
                    dlctx->image_index  = image_index;
                    dlctx->image_header = image_header;
                }
                lx_trace_d("%s: found at %p/%d", image_name, image_header, (lx_int_t)image_index);
                return (lx_dlimage_ref_t)dlctx;
            }
        }
    }
    return lx_null;
}

lx_pointer_t lx_dlsym(lx_dlimage_ref_t dlimage, lx_char_t const* symbol) {
    lx_fake_dlctx_ref_t dlctx = (lx_fake_dlctx_ref_t)dlimage;
    lx_assert_and_check_return_val(dlctx && dlctx->image_header && symbol, lx_null);

    // skip '_'
    if (*symbol == '_') symbol++;

    // get command pointer
    struct mach_header const* image_header  = dlctx->image_header;
    uintptr_t cmd_ptr                       = (uintptr_t)lx_get_first_cmd_after_header(image_header);
    uintptr_t image_vmaddr_slide            = (uintptr_t)_dyld_get_image_vmaddr_slide(dlctx->image_index);
    if (cmd_ptr) {
        uintptr_t segment_base = 0;
        for (lx_size_t cmd_index = 0; cmd_index < image_header->ncmds; cmd_index++) {
            // get segment base address
            struct load_command const* load_cmd = (struct load_command*)cmd_ptr;
            if (load_cmd->cmd == LC_SEGMENT) {
                struct segment_command const* segment_cmd = (struct segment_command*)cmd_ptr;
                if (strcmp(segment_cmd->segname, SEG_LINKEDIT) == 0)
                    segment_base = (uintptr_t)(segment_cmd->vmaddr - segment_cmd->fileoff) + image_vmaddr_slide;
            } else if (load_cmd->cmd == LC_SEGMENT_64) {
                struct segment_command_64 const* segment_cmd = (struct segment_command_64*)cmd_ptr;
                if (strcmp(segment_cmd->segname, SEG_LINKEDIT) == 0)
                    segment_base = (uintptr_t)(segment_cmd->vmaddr - segment_cmd->fileoff) + image_vmaddr_slide;
            } else if (load_cmd->cmd == LC_SYMTAB && segment_base > 0) {
                struct symtab_command const* symbol_table_cmd = (struct symtab_command*)cmd_ptr;
                NLIST const*                 symbol_table = (NLIST*)(segment_base + symbol_table_cmd->symoff);
                uintptr_t                    string_table = segment_base + symbol_table_cmd->stroff;
                for (uint32_t symbol_index = 0; symbol_index < symbol_table_cmd->nsyms; symbol_index++) {
                    // if n_value is 0, the symbol refers to an external object.
                    if (symbol_table[symbol_index].n_value != 0) {
                        // get symbol name
                        NLIST const* item          = symbol_table + symbol_index;
                        lx_pointer_t dli_saddr     = (lx_pointer_t)(item->n_value + image_vmaddr_slide);
                        lx_char_t const* dli_sname = (lx_char_t*)((intptr_t)string_table + (intptr_t)item->n_un.n_strx);
                        if (*dli_sname == '_') dli_sname++;

                        // found and skip symbols with '0x...'?
                        if (*dli_sname != '0' && !strcmp(symbol, dli_sname)) {
                            // thumb function? fix address
#if defined(LX_ARCH_ARM) && !defined(LX_ARCH_ARM64)
#   ifdef LX_ARCH_ARM_THUMB
                            if (item->n_desc & N_ARM_THUMB_DEF)
                                dli_saddr = (lx_pointer_t)((lx_ulong_t)dli_saddr | 1);
#   else
                            if (item->n_desc & N_ARM_THUMB_DEF)
                                dli_saddr = (lx_pointer_t)((lx_ulong_t)dli_saddr & ~1);
#   endif
#endif
                            // trace
                            lx_trace_d("dlsym(%s): %p", dli_sname, dli_saddr);
                            return dli_saddr;
                        }
                    }
                }

                // reset the segment base address
                segment_base = 0;
            }
            cmd_ptr += load_cmd->cmdsize;
        }
    }
    return lx_null;
}

lx_int_t lx_dlclose(lx_dlimage_ref_t dlimage) {
    lx_fake_dlctx_ref_t dlctx = (lx_fake_dlctx_ref_t)dlimage;
    if (dlctx) {
        lx_free(dlctx);
        return 0;
    }
    return -1;
}
