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
 * @file        paint.c
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "paint.h"
#include "quality.h"
#include "private/stroker.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */

// the default cap
#define LX_PAINT_DEFAULT_CAP                LX_PAINT_STROKE_CAP_BUTT

// the default join
#define LX_PAINT_DEFAULT_JOIN               LX_PAINT_STROKE_JOIN_MITER

// the default rule
#define LX_PAINT_DEFAULT_RULE               LX_PAINT_FILL_RULE_ODD

// the default width
#define LX_PAINT_DEFAULT_WIDTH              1.0f

// the default alpha
#define LX_PAINT_DEFAULT_ALPHA              (255)

// the default miter limit
#define LX_PAINT_DEFAULT_MITER              LX_STROKER_DEFAULT_MITER

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

// the paint type
typedef struct lx_paint_t_ {
    lx_uint32_t      mode    : 4;
    lx_uint32_t      flags   : 4;
    lx_uint32_t      cap     : 4;
    lx_uint32_t      join    : 4;
    lx_uint32_t      rule    : 1;
    lx_color_t       color;
    lx_byte_t        alpha;
    lx_float_t       width;
    lx_float_t       miter;
    lx_texture_ref_t texture;
}lx_paint_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
lx_paint_ref_t lx_paint_init() {
    lx_bool_t   ok = lx_false;
    lx_paint_t* paint = lx_null;
    do {

        paint = lx_malloc0_type(lx_paint_t);
        lx_assert_and_check_break(paint);

        lx_paint_clear((lx_paint_ref_t)paint);
        ok = lx_true;

    } while (0);

    if (!ok && paint) {
        lx_paint_exit((lx_paint_ref_t)paint);
        paint = lx_null;
    }
    return (lx_paint_ref_t)paint;
}

lx_void_t lx_paint_exit(lx_paint_ref_t self) {
    lx_paint_t* paint = (lx_paint_t*)self;
    if (paint) {
        lx_free(paint);
    }
}

lx_void_t lx_paint_clear(lx_paint_ref_t self) {
    lx_paint_t* paint = (lx_paint_t*)self;
    if (paint) {
        paint->mode  = LX_PAINT_MODE_NONE;
        paint->flags = LX_PAINT_FLAG_NONE;
        paint->cap   = LX_PAINT_DEFAULT_CAP;
        paint->join  = LX_PAINT_DEFAULT_JOIN;
        paint->rule  = LX_PAINT_DEFAULT_RULE;
        paint->width = LX_PAINT_DEFAULT_WIDTH;
        paint->color = LX_COLOR_DEFAULT;
        paint->alpha = LX_PAINT_DEFAULT_ALPHA;
        paint->miter = LX_PAINT_DEFAULT_MITER;
    }
}

lx_void_t lx_paint_copy(lx_paint_ref_t self, lx_paint_ref_t copied) {
    lx_paint_t* paint = (lx_paint_t*)self;
    lx_paint_t* paint_copied = (lx_paint_t*)copied;
    if (paint && paint_copied) {
        lx_memcpy(paint, paint_copied, sizeof(lx_paint_t));
    }
}

lx_size_t lx_paint_mode(lx_paint_ref_t self) {
    lx_paint_t* paint = (lx_paint_t*)self;
    return paint? paint->mode : LX_PAINT_MODE_NONE;
}

lx_void_t lx_paint_mode_set(lx_paint_ref_t self, lx_size_t mode) {
    lx_paint_t* paint = (lx_paint_t*)self;
    if (paint) {
        paint->mode = (lx_uint32_t)mode;
    }
}

lx_size_t lx_paint_flags(lx_paint_ref_t self) {
    lx_paint_t* paint = (lx_paint_t*)self;
    if (paint) {
        lx_size_t flags = paint->flags;
        if (lx_quality() > LX_QUALITY_LOW) flags |= (LX_PAINT_FLAG_ANTIALIASING | LX_PAINT_FLAG_FILTER_BITMAP);
        else flags &= ~(LX_PAINT_FLAG_ANTIALIASING | LX_PAINT_FLAG_FILTER_BITMAP);
        return flags;
    }
    return LX_PAINT_FLAG_NONE;
}

lx_void_t lx_paint_flag_set(lx_paint_ref_t self, lx_size_t flags) {
    lx_paint_t* paint = (lx_paint_t*)self;
    if (paint) {
        paint->flags = (lx_uint32_t)flags;
    }
}

lx_color_t lx_paint_color(lx_paint_ref_t self) {
    lx_paint_t* paint = (lx_paint_t*)self;
    return paint? paint->color : LX_COLOR_DEFAULT;
}

lx_void_t lx_paint_color_set(lx_paint_ref_t self, lx_color_t color) {
    lx_paint_t* paint = (lx_paint_t*)self;
    if (paint) {
        paint->color = color;
    }
}

lx_byte_t lx_paint_alpha(lx_paint_ref_t self) {
    lx_paint_t* paint = (lx_paint_t*)self;
    return paint? paint->alpha : 0xff;
}

lx_void_t lx_paint_alpha_set(lx_paint_ref_t self, lx_byte_t alpha) {
    lx_paint_t* paint = (lx_paint_t*)self;
    if (paint) {
        paint->alpha = alpha;
    }
}

lx_float_t lx_paint_stroke_width(lx_paint_ref_t self) {
    lx_paint_t* paint = (lx_paint_t*)self;
    return paint? paint->width : LX_PAINT_DEFAULT_WIDTH;
}

lx_void_t lx_paint_stroke_width_set(lx_paint_ref_t self, lx_float_t width) {
    lx_paint_t* paint = (lx_paint_t*)self;
    if (paint) {
        lx_assert(width >= 0);
        paint->width = width;
    }
}

lx_size_t lx_paint_stroke_cap(lx_paint_ref_t self) {
    lx_paint_t* paint = (lx_paint_t*)self;
    return paint? paint->cap : LX_PAINT_DEFAULT_CAP;
}

lx_void_t lx_paint_stroke_cap_set(lx_paint_ref_t self, lx_size_t cap) {
    lx_paint_t* paint = (lx_paint_t*)self;
    if (paint) {
        paint->cap = (lx_uint32_t)cap;
    }
}

lx_size_t lx_paint_stroke_join(lx_paint_ref_t self) {
    lx_paint_t* paint = (lx_paint_t*)self;
    return paint? paint->join : LX_PAINT_DEFAULT_JOIN;
}

lx_void_t lx_paint_stroke_join_set(lx_paint_ref_t self, lx_size_t join) {
    lx_paint_t* paint = (lx_paint_t*)self;
    if (paint) {
        paint->join = (lx_uint32_t)join;
    }
}

lx_float_t lx_paint_stroke_miter(lx_paint_ref_t self) {
    lx_paint_t* paint = (lx_paint_t*)self;
    return paint? paint->miter : LX_PAINT_DEFAULT_MITER;
}

lx_void_t lx_paint_stroke_miter_set(lx_paint_ref_t self, lx_float_t miter) {
    lx_paint_t* paint = (lx_paint_t*)self;
    if (paint) {
        paint->miter = miter;
    }
}

lx_size_t lx_paint_fill_rule(lx_paint_ref_t self) {
    lx_paint_t* paint = (lx_paint_t*)self;
    return paint? paint->rule : LX_PAINT_DEFAULT_RULE;
}

lx_void_t lx_paint_fill_rule_set(lx_paint_ref_t self, lx_size_t rule) {
    lx_paint_t* paint = (lx_paint_t*)self;
    if (paint) {
        paint->rule = (lx_uint32_t)rule;
    }
}

lx_texture_ref_t lx_paint_fill_texture(lx_paint_ref_t self) {
    lx_paint_t* paint = (lx_paint_t*)self;
    return paint? paint->texture : lx_null;
}

lx_void_t lx_paint_fill_texture_set(lx_paint_ref_t self, lx_texture_ref_t texture) {
    lx_paint_t* paint = (lx_paint_t*)self;
    if (paint) {
        paint->texture = texture;
    }
}
