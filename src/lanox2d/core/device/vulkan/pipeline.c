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
    VkShaderModule vshader_module = lx_null;
    VkShaderModule fshader_module = lx_null;
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

        // create vertex shader
        VkShaderModuleCreateInfo vshader_createinfo = {};
        vshader_createinfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        vshader_createinfo.pNext = lx_null;
        vshader_createinfo.flags = 0;
        vshader_createinfo.codeSize = (lx_uint32_t)lx_strlen(vshader);
        vshader_createinfo.pCode = (lx_uint32_t*)vshader;
        if (vkCreateShaderModule(device->device, &vshader_createinfo, lx_null, &vshader_module) != VK_SUCCESS) {
            break;
        }

        // create fragment shader
        VkShaderModuleCreateInfo fshader_createinfo = {};
        fshader_createinfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        fshader_createinfo.pNext = lx_null;
        fshader_createinfo.flags = 0;
        fshader_createinfo.codeSize = (lx_uint32_t)lx_strlen(fshader);
        fshader_createinfo.pCode = (lx_uint32_t*)fshader;
        if (vkCreateShaderModule(device->device, &fshader_createinfo, lx_null, &fshader_module) != VK_SUCCESS) {
            break;
        }

        ok = lx_true;
    } while (0);

    // free shaders
    if (vshader_module) {
        vkDestroyShaderModule(device->device, vshader_module, lx_null);
        vshader_module = lx_null;
    }
    if (fshader_module) {
        vkDestroyShaderModule(device->device, fshader_module, lx_null);
        fshader_module = lx_null;
    }

    // free pipeline if failed
    if (!ok && pipeline) {
        lx_pipeline_exit((lx_pipeline_ref_t)pipeline);
        pipeline = lx_null;
    }
    return (lx_pipeline_ref_t)pipeline;
}

lx_pipeline_ref_t lx_pipeline_init_solid(lx_vulkan_device_t* device) {
    static lx_char_t const vshader[] = {
#include "solid.vs.h"
    };
    static lx_char_t const fshader[] = {
#include "solid.fs.h"
    };
    return lx_pipeline_init(device, LX_PIPELINE_TYPE_SOLID, vshader, fshader);
}

lx_pipeline_ref_t lx_pipeline_init_texture(lx_vulkan_device_t* device) {
    static lx_char_t const vshader[] = {
#include "texture.vs.h"
    };
    static lx_char_t const fshader[] = {
#include "texture.fs.h"
    };
    return lx_pipeline_init(device, LX_PIPELINE_TYPE_TEXTURE, vshader, fshader);
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
