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
 * @file        pipeline.h
 *
 */
#ifndef LX_CORE_DEVICE_VULKAN_PIPELINE_H
#define LX_CORE_DEVICE_VULKAN_PIPELINE_H

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

/* get solid pipeline
 *
 * @param device        the vulkan device
 *
 * @return              the pipeline
 */
lx_vk_pipeline_ref_t    lx_vk_pipeline_solid(lx_vulkan_device_t* device);

/* get texture pipeline
 *
 * @param device        the vulkan device
 *
 * @return              the pipeline
 */
lx_vk_pipeline_ref_t    lx_vk_pipeline_texture(lx_vulkan_device_t* device);

/* exit pipeline
 *
 * @param pipeline      the pipeline
 */
lx_void_t               lx_vk_pipeline_exit(lx_vk_pipeline_ref_t pipeline);

/* get native pipeline
 *
 * @param pipeline      the pipeline
 *
 * @return              the native pipeline
 */
VkPipeline              lx_vk_pipeline_native(lx_vk_pipeline_ref_t pipeline);

/* get pipeline layout
 *
 * @param pipeline      the pipeline
 *
 * @return              the pipeline layout
 */
VkPipelineLayout        lx_vk_pipeline_layout(lx_vk_pipeline_ref_t pipeline);

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
lx_extern_c_leave
#endif


