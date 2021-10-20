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
 * @file        pipeline.c
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "pipeline.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

// the pipeline type
typedef struct lx_pipeline_t {
    lx_size_t           type;
    VkPipeline          pipeline;
    VkPipelineCache     pipeline_cache;
    VkPipelineLayout    pipeline_layout;
    lx_vulkan_device_t* device;
}lx_pipeline_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
lx_pipeline_ref_t lx_pipeline_init(lx_vulkan_device_t* device, lx_size_t type, lx_char_t const* vshader, lx_char_t const* fshader) {
    lx_assert_and_check_return_val(device && device->device && vshader && fshader, lx_null);

    lx_bool_t ok = lx_false;
    lx_pipeline_t* pipeline = lx_null;
    do {
        // init pipeline
        pipeline = lx_malloc0_type(lx_pipeline_t);
        lx_assert_and_check_break(pipeline);

        pipeline->type   = type;
        pipeline->device = device;

        // create pipeline layout (empty)
        VkPipelineLayoutCreateInfo pipeline_layout_createinfo = {};
        pipeline_layout_createinfo.sType                  = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipeline_layout_createinfo.pNext                  = lx_null;
        pipeline_layout_createinfo.setLayoutCount         = 0;
        pipeline_layout_createinfo.pSetLayouts            = lx_null;
        pipeline_layout_createinfo.pushConstantRangeCount = 0;
        pipeline_layout_createinfo.pPushConstantRanges    = lx_null;
        if (vkCreatePipelineLayout(device->device, &pipeline_layout_createinfo, lx_null, &pipeline->pipeline_layout) != VK_SUCCESS) {
            break;
        }

        ok = lx_true;
    } while (0);

    if (!ok && pipeline) {
        lx_pipeline_exit((lx_pipeline_ref_t)pipeline);
        pipeline = lx_null;
    }
    return (lx_pipeline_ref_t)pipeline;
}

lx_pipeline_ref_t lx_pipeline_init_solid() {
    return lx_null;
}

lx_pipeline_ref_t lx_pipeline_init_texture() {
    return lx_null;
}

lx_void_t lx_pipeline_exit(lx_pipeline_ref_t self) {
    lx_pipeline_t* pipeline = (lx_pipeline_t*)self;
    if (pipeline) {
        lx_assert(pipeline->device && pipeline->device->device);
        if (pipeline->pipeline) {
            vkDestroyPipeline(pipeline->device->device, pipeline->pipeline, lx_null);
            pipeline->pipeline = lx_null;
        }
        if (pipeline->pipeline_cache) {
            vkDestroyPipelineCache(pipeline->device->device, pipeline->pipeline_cache, lx_null);
            pipeline->pipeline_cache = lx_null;
        }
        if (pipeline->pipeline_layout) {
            vkDestroyPipelineLayout(pipeline->device->device, pipeline->pipeline_layout, lx_null);
            pipeline->pipeline_layout = lx_null;
        }
        lx_free(pipeline);
    }
}
