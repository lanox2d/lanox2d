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
 * @file        time.h
 *
 */
#ifndef LX_BASE_PLATFORM_TIME_H
#define LX_BASE_PLATFORM_TIME_H

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
lx_extern_c_enter

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */

/*! usleep
 *
 * @param us    the microsecond time
 */
lx_void_t       lx_usleep(lx_size_t us);

/*! msleep
 *
 * @param ms    the millisecond time
 */
lx_void_t       lx_msleep(lx_size_t ms);

/*! clock, ms
 *
 * @return      the mclock
 */
lx_hong_t       lx_mclock(lx_noarg_t);

/*! uclock, us
 *
 * @return      the uclock
 */
lx_hong_t       lx_uclock(lx_noarg_t);

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
lx_extern_c_leave

#endif


