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
 * @file        image_view.h
 *
 */
#ifndef LX_CORE_DEVICE_VULKAN_IMAGE_VIEW_H
#define LX_CORE_DEVICE_VULKAN_IMAGE_VIEW_H

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "device.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
lx_extern_c_enter

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */

/* init image view
 *
 * @param device        the vulkan device
 * @param image         the image
 * @param format        the image format
 *
 * @return              the image view
 */
lx_vk_image_view_ref_t  lx_vk_image_view_init(lx_vulkan_device_t* device, VkImage image, VkFormat format);

/* exit image view
 *
 * @param image_view    the image view
 */
lx_void_t               lx_vk_image_view_exit(lx_vk_image_view_ref_t image_view);

/* get the vulkan image view
 *
 * @param image_view    the image view
 *
 * @return              the native image view
 */
VkImageView             lx_vk_image_view(lx_vk_image_view_ref_t image_view);

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
lx_extern_c_leave
#endif


