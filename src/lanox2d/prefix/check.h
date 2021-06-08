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
 * @file        check.h
 *
 */
#ifndef LX_PREFIX_CHECK_H
#define LX_PREFIX_CHECK_H

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "config.h"
#include "abort.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */

// check
#define lx_check_return(x)                              do { if (!(x)) return ; } while (0)
#define lx_check_return_val(x, v)                       do { if (!(x)) return (v); } while (0)
#define lx_check_goto(x, b)                             do { if (!(x)) goto b; } while (0)
#define lx_check_break(x)                               { if (!(x)) break ; }
#define lx_check_abort(x)                               do { if (!(x)) lx_abort(); } while (0)
#define lx_check_continue(x)                            { if (!(x)) continue ; }
#define lx_check_break_state(x, s, v)                   { if (!(x)) { (s) = (v); break ;} }


#endif


