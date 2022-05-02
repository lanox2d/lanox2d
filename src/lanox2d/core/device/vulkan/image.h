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
 * @file        image.h
 *
 */
#ifndef LX_CORE_DEVICE_VULKAN_IMAGE_H
#define LX_CORE_DEVICE_VULKAN_IMAGE_H

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

/* init MSAA image
 *
 * @param device        the vulkan device
 * @param format        the image format
 * @param width         the image width
 * @param height        the image height
 *
 * @return              the image
 */
lx_vk_image_ref_t       lx_vk_image_init_msaa(lx_vulkan_device_t* device, VkFormat format, lx_size_t width, lx_size_t height);

/* init stencil image
 *
 * @param device        the vulkan device
 * @param format        the image format
 * @param width         the image width
 * @param height        the image height
 *
 * @return              the image
 */
lx_vk_image_ref_t       lx_vk_image_init_stencil(lx_vulkan_device_t* device, VkFormat format, lx_size_t width, lx_size_t height);

/* init texture image
 *
 * @param device        the vulkan device
 * @param format        the image format
 * @param width         the image width
 * @param height        the image height
 *
 * @return              the image
 */
lx_vk_image_ref_t       lx_vk_image_init_texture(lx_vulkan_device_t* device, VkFormat format, lx_size_t width, lx_size_t height);

/* exit image
 *
 * @param image         the image
 */
lx_void_t               lx_vk_image_exit(lx_vk_image_ref_t image);

/* get the vulkan image
 *
 * @param image         the image
 *
 * @return              the native image
 */
VkImage                 lx_vk_image(lx_vk_image_ref_t image);

/* get the texture view
 *
 * @param image         the image
 *
 * @return              the texture view
 */
lx_vk_image_view_ref_t  lx_vk_image_texture_view(lx_vk_image_ref_t image);

/* get the framebuffer view
 *
 * @param image         the image
 *
 * @return              the framebuffer view
 */
lx_vk_image_view_ref_t  lx_vk_image_framebuffer_view(lx_vk_image_ref_t image);

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
lx_extern_c_leave
#endif


