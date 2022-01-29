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
 * @file        bitmap_shader.c
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "bitmap_shader.h"
#include "../../shader.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * private implementation
 */
static lx_void_t lx_bitmap_shader_devdata_free(lx_pointer_t devdata) {
    lx_bitmap_shader_devdata_t* bitmap_devdata = (lx_bitmap_shader_devdata_t*)devdata;
    if (bitmap_devdata) {
        // TODO
        lx_free(bitmap_devdata);
    }
}

static lx_bool_t lx_bitmap_shader_load_texture(lx_bitmap_shader_devdata_t* devdata, lx_bitmap_ref_t bitmap) {
    return lx_true;
}

static lx_bitmap_shader_devdata_t* lx_bitmap_shader_init_devdata(lx_bitmap_shader_t* shader) {
    lx_assert(shader);

    // get bitmap
    lx_bitmap_ref_t bitmap = shader->bitmap;
    lx_assert(bitmap);

    lx_bool_t ok = lx_false;
    lx_bitmap_shader_devdata_t* devdata = lx_null;
    do {

        // init bitmap shader devdata
        devdata = lx_malloc0_type(lx_bitmap_shader_devdata_t);
        lx_assert_and_check_return_val(devdata, lx_null);

        // load texture from bitmap
        if (!lx_bitmap_shader_load_texture(devdata, bitmap)) {
            break;
        }

        ok = lx_true;
    } while (0);

    if (!ok && devdata) {
        lx_bitmap_shader_devdata_free(devdata);
        devdata = lx_null;
    }
    return devdata;
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
lx_bitmap_shader_devdata_t* lx_bitmap_shader_devdata(lx_bitmap_shader_t* shader) {
    lx_bitmap_shader_devdata_t* devdata = (lx_bitmap_shader_devdata_t*)shader->base.devdata;
    if (!devdata) {
        devdata = lx_bitmap_shader_init_devdata(shader);
        if (devdata) {
            shader->base.devdata_free = lx_bitmap_shader_devdata_free;
            shader->base.devdata = devdata;
        }
    }
    return devdata;
}
