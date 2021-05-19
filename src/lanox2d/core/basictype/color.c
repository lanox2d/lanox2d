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
 * @file        color.c
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "color.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */

// init color from argb
#ifdef LX_WORDS_BIGENDIAN
#   define LX_COLOR_INIT_ARGB(a, r, g, b)       { a, r, g, b }
#else
#   define LX_COLOR_INIT_ARGB(a, r, g, b)       { b, g, r, a }
#endif

// init color from name & argb
#define LX_COLOR_INIT_NAME(name, a, r, g, b)    {name, LX_COLOR_INIT_ARGB(a, r, g, b)}

// init color contants
#define LX_COLOR_INIT_BLACK             LX_COLOR_INIT_NAME( "black",        0xff,   0x00,   0x00,   0x00    )
#define LX_COLOR_INIT_BLUE              LX_COLOR_INIT_NAME( "blue",         0xff,   0x00,   0x00,   0xff    )
#define LX_COLOR_INIT_BROWN             LX_COLOR_INIT_NAME( "brown",        0xff,   0x80,   0x00,   0x00    )
#define LX_COLOR_INIT_DARKBLUE          LX_COLOR_INIT_NAME( "darkblue",     0xff,   0x00,   0x00,   0x8b    )
#define LX_COLOR_INIT_GOLD              LX_COLOR_INIT_NAME( "gold",         0xff,   0xff,   0xd7,   0x00    )
#define LX_COLOR_INIT_GRAY              LX_COLOR_INIT_NAME( "gray",         0xff,   0x80,   0x80,   0x80    )
#define LX_COLOR_INIT_GREY              LX_COLOR_INIT_NAME( "grey",         0xff,   0x80,   0x80,   0x80    )
#define LX_COLOR_INIT_GREEN             LX_COLOR_INIT_NAME( "green",        0xff,   0x00,   0xff,   0x00    )
#define LX_COLOR_INIT_LIGHTBLUE         LX_COLOR_INIT_NAME( "lightblue",    0xff,   0xad,   0xd8,   0xa6    )
#define LX_COLOR_INIT_LIGHTGRAY         LX_COLOR_INIT_NAME( "lightgray",    0xff,   0xd3,   0xd3,   0xd3    )
#define LX_COLOR_INIT_LIGHTGREY         LX_COLOR_INIT_NAME( "lightgrey",    0xff,   0xd3,   0xd3,   0xd3    )
#define LX_COLOR_INIT_LIGHTPINK         LX_COLOR_INIT_NAME( "lightpink",    0xff,   0xff,   0xb6,   0xc1    )
#define LX_COLOR_INIT_LIGHTYELLOW       LX_COLOR_INIT_NAME( "lightyellow",  0xff,   0xff,   0xff,   0xe0    )
#define LX_COLOR_INIT_NAVY              LX_COLOR_INIT_NAME( "navy",         0xff,   0x00,   0x00,   0x80    )
#define LX_COLOR_INIT_ORANGE            LX_COLOR_INIT_NAME( "orange",       0xff,   0xff,   0xa5,   0x00    )
#define LX_COLOR_INIT_PINK              LX_COLOR_INIT_NAME( "pink",         0xff,   0xff,   0x00,   0xff    )
#define LX_COLOR_INIT_PURPLE            LX_COLOR_INIT_NAME( "purple",       0xff,   0x80,   0x00,   0x80    )
#define LX_COLOR_INIT_RED               LX_COLOR_INIT_NAME( "red",          0xff,   0xff,   0x00,   0x00    )
#define LX_COLOR_INIT_SNOW              LX_COLOR_INIT_NAME( "snow",         0xff,   0xff,   0xfa,   0xfa    )
#define LX_COLOR_INIT_TOMATO            LX_COLOR_INIT_NAME( "tomato",       0xff,   0xff,   0x63,   0x47    )
#define LX_COLOR_INIT_YELLOW            LX_COLOR_INIT_NAME( "yellow",       0xff,   0xff,   0xff,   0x00    )
#define LX_COLOR_INIT_WHEAT             LX_COLOR_INIT_NAME( "wheat",        0xff,   0xf5,   0xde,   0xb3    )
#define LX_COLOR_INIT_WHITE             LX_COLOR_INIT_NAME( "white",        0xff,   0xff,   0xff,   0xff    )

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

// the named color type
typedef struct lx_named_color_t_ {
    lx_char_t const*    name;
    lx_color_t          color;
}lx_named_color_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * globals
 */

// the colors
static lx_named_color_t g_named_colors[] = {
    LX_COLOR_INIT_BLACK
,   LX_COLOR_INIT_BLUE
,   LX_COLOR_INIT_BROWN
,   LX_COLOR_INIT_DARKBLUE
,   LX_COLOR_INIT_GOLD
,   LX_COLOR_INIT_GRAY
,   LX_COLOR_INIT_GREEN
,   LX_COLOR_INIT_GREY
,   LX_COLOR_INIT_LIGHTBLUE
,   LX_COLOR_INIT_LIGHTGRAY
,   LX_COLOR_INIT_LIGHTGREY
,   LX_COLOR_INIT_LIGHTPINK
,   LX_COLOR_INIT_LIGHTYELLOW
,   LX_COLOR_INIT_NAVY
,   LX_COLOR_INIT_ORANGE
,   LX_COLOR_INIT_PINK
,   LX_COLOR_INIT_PURPLE
,   LX_COLOR_INIT_RED
,   LX_COLOR_INIT_SNOW
,   LX_COLOR_INIT_TOMATO
,   LX_COLOR_INIT_YELLOW
,   LX_COLOR_INIT_WHEAT
,   LX_COLOR_INIT_WHITE
};

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
lx_color_t const lx_color_from_index(lx_size_t index) {
    lx_assert_and_check_return_val(index < lx_arrayn(g_named_colors), LX_COLOR_DEFAULT);
    return g_named_colors[index].color;
}

