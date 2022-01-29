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
 * @file        renderer.c
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "renderer.h"
#include "pipeline.h"
#include "allocator.h"
#include "bitmap_shader.h"
#include "../../quality.h"
#include "../../tess/tess.h"
#include "../../shader.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * private implementation
 */
static lx_void_t lx_vk_renderer_set_imagelayout(VkCommandBuffer cmdbuffer, VkImage image,
    VkImageLayout oldlayout, VkImageLayout newlayout,
    VkPipelineStageFlags srcstages, VkPipelineStageFlags dststages) {

    VkImageMemoryBarrier image_memory_barrier = {};
    image_memory_barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    image_memory_barrier.pNext = lx_null;
    image_memory_barrier.srcAccessMask = 0;
    image_memory_barrier.dstAccessMask = 0;
    image_memory_barrier.oldLayout = oldlayout;
    image_memory_barrier.newLayout = newlayout;
    image_memory_barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    image_memory_barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    image_memory_barrier.image = image;
    image_memory_barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    image_memory_barrier.subresourceRange.baseMipLevel = 0;
    image_memory_barrier.subresourceRange.levelCount = 1;
    image_memory_barrier.subresourceRange.baseArrayLayer = 0;
    image_memory_barrier.subresourceRange.layerCount = 1;

    switch (oldlayout) {
    case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
        image_memory_barrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        break;
    case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
        image_memory_barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        break;
    case VK_IMAGE_LAYOUT_PREINITIALIZED:
        image_memory_barrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT;
        break;
    default:
        break;
    }

    switch (newlayout) {
    case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
        image_memory_barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        break;
    case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
        image_memory_barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
        break;
    case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
        image_memory_barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
        break;
    case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
        image_memory_barrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        break;
    case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
        image_memory_barrier.dstAccessMask =
            VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
        break;
    case VK_IMAGE_LAYOUT_PRESENT_SRC_KHR:
        image_memory_barrier.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
        break;
    default:
        break;
    }
    vkCmdPipelineBarrier(cmdbuffer, srcstages, dststages, 0, 0, lx_null, 0, lx_null, 1, &image_memory_barrier);
}

static lx_void_t lx_vk_renderer_apply_shader_bitmap(lx_vulkan_device_t* device, lx_shader_ref_t shader, lx_rect_ref_t bounds) {

    // get bitmap texture
    lx_bitmap_shader_devdata_t* devdata = lx_bitmap_shader_devdata(device, (lx_bitmap_shader_t*)shader);
    lx_assert(devdata);

    // get bitmap
    lx_bitmap_ref_t bitmap = ((lx_bitmap_shader_t*)shader)->bitmap;
    lx_assert(bitmap);

#if 0
    // get bitmap width and height
    lx_size_t width = lx_bitmap_width(bitmap);
    lx_size_t height = lx_bitmap_height(bitmap);

    // get paint
    lx_paint_ref_t paint = device->base.paint;
    lx_assert(paint);
#endif
}

static lx_inline lx_void_t lx_vk_renderer_apply_paint_shader(lx_vulkan_device_t* device, lx_shader_ref_t shader, lx_rect_ref_t bounds) {
    lx_size_t shader_type = lx_shader_type(shader);
    switch (shader_type) {
    case LX_SHADER_TYPE_BITMAP:
        lx_vk_renderer_apply_shader_bitmap(device, shader, bounds);
        break;
    default:
        lx_trace_e("not supported shader type!");
        break;
    }
}

static lx_inline lx_void_t lx_vk_renderer_apply_paint_solid(lx_vulkan_device_t* device, lx_size_t pipeline_type) {
    VkCommandBuffer cmdbuffer = device->renderer_cmdbuffer;
    lx_paint_ref_t paint = device->base.paint;
    lx_assert(cmdbuffer && paint);

    // get color
    lx_color_t color = lx_paint_color(paint);
    lx_byte_t alpha = lx_paint_alpha(paint);
    if (alpha != 0xff) {
        color.a = alpha;
    }

    // enable color pipeline
    lx_vk_pipeline_ref_t pipeline = lx_null;
    switch (pipeline_type) {
    case LX_VK_PIPELINE_TYPE_SOLID:
        pipeline = lx_vk_pipeline_solid(device);
        break;
    case LX_VK_PIPELINE_TYPE_LINES:
        pipeline = lx_vk_pipeline_lines(device);
        break;
    case LX_VK_PIPELINE_TYPE_POINTS:
        pipeline = lx_vk_pipeline_points(device);
        break;
    default:
        break;
    }
    lx_assert_and_check_return(pipeline);
    vkCmdBindPipeline(cmdbuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, lx_vk_pipeline_native(pipeline));

    // apply color
    lx_float_t color_data[] = {(lx_float_t)color.r / 0xff, (lx_float_t)color.g / 0xff, (lx_float_t)color.b / 0xff, (lx_float_t)color.a / 0xff};
    vkCmdPushConstants(cmdbuffer, lx_vk_pipeline_layout(pipeline), VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(color_data), color_data);

    // set model matrix
    lx_vk_matrix_t model;
    lx_vk_matrix_convert(&model, device->base.matrix);
    lx_vk_pipeline_matrix_set_model(pipeline, &model);

    // bind descriptor set to pipeline (uniform buffer, ...)
    vkCmdBindDescriptorSets(cmdbuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
        lx_vk_pipeline_layout(pipeline), 0, lx_vk_pipeline_descriptor_sets_count(pipeline),
        lx_vk_pipeline_descriptor_sets(pipeline), 0, lx_null);
}

static lx_inline lx_void_t lx_vk_renderer_apply_paint(lx_vulkan_device_t* device, lx_rect_ref_t bounds) {
    lx_shader_ref_t shader = lx_paint_shader(device->base.paint);
    if (shader) {
        lx_vk_renderer_apply_paint_shader(device, shader, bounds);
    } else {
        lx_vk_renderer_apply_paint_solid(device, LX_VK_PIPELINE_TYPE_SOLID);
    }
}

static lx_inline lx_void_t lx_vk_renderer_fill_polygon(lx_vulkan_device_t* device, lx_polygon_ref_t polygon, lx_rect_ref_t bounds, lx_size_t rule) {
    lx_tessellator_rule_set(device->tessellator, rule);
    lx_polygon_ref_t result = lx_tessellator_make(device->tessellator, polygon, bounds);
    if (result && result->total > 0) {
        lx_vk_buffer_t vertex_buffer;
        lx_size_t size = sizeof(lx_point_t) * result->total;
        if (lx_vk_allocator_alloc(device->allocator_vertex, size, &vertex_buffer)) {
            lx_vk_allocator_copy(device->allocator_vertex, &vertex_buffer, 0, (lx_pointer_t)result->points, size);
            lx_array_insert_tail(device->vertex_buffers, &vertex_buffer);

            VkDeviceSize offset = 0;
            VkCommandBuffer cmdbuffer = device->renderer_cmdbuffer;
            vkCmdBindVertexBuffers(cmdbuffer, 0, 1, &vertex_buffer.buffer, &offset);
            vkCmdDraw(cmdbuffer, result->total, 1, 0, 0);
        }
    }
}

static lx_inline lx_void_t lx_vk_renderer_stroke_lines(lx_vulkan_device_t* device, lx_point_ref_t points, lx_size_t count) {
	lx_vk_buffer_t vertex_buffer;
	lx_size_t size = sizeof(lx_point_t) * count;
	if (lx_vk_allocator_alloc(device->allocator_vertex, size, &vertex_buffer)) {
		lx_vk_allocator_copy(device->allocator_vertex, &vertex_buffer, 0, (lx_pointer_t)points, size);
		lx_array_insert_tail(device->vertex_buffers, &vertex_buffer);

		VkDeviceSize offset = 0;
        VkCommandBuffer cmdbuffer = device->renderer_cmdbuffer;
		vkCmdBindVertexBuffers(cmdbuffer, 0, 1, &vertex_buffer.buffer, &offset);
		vkCmdDraw(cmdbuffer, count, 1, 0, 0);
	}
}

static lx_inline lx_void_t lx_vk_renderer_stroke_points(lx_vulkan_device_t* device, lx_point_ref_t points, lx_size_t count) {
	lx_vk_buffer_t vertex_buffer;
	lx_size_t size = sizeof(lx_point_t) * count;
	if (lx_vk_allocator_alloc(device->allocator_vertex, size, &vertex_buffer)) {
		lx_vk_allocator_copy(device->allocator_vertex, &vertex_buffer, 0, (lx_pointer_t)points, size);
		lx_array_insert_tail(device->vertex_buffers, &vertex_buffer);

		VkDeviceSize offset = 0;
        VkCommandBuffer cmdbuffer = device->renderer_cmdbuffer;
		vkCmdBindVertexBuffers(cmdbuffer, 0, 1, &vertex_buffer.buffer, &offset);
		vkCmdDraw(cmdbuffer, count, 1, 0, 0);
	}
}

static lx_inline lx_void_t lx_vk_renderer_stroke_polygon(lx_vulkan_device_t* device, lx_polygon_ref_t polygon) {
    lx_assert(device && polygon && polygon->points && polygon->counts);

	lx_vk_buffer_t vertex_buffer;
	lx_size_t size = sizeof(lx_point_t) * polygon->total;
	if (lx_vk_allocator_alloc(device->allocator_vertex, size, &vertex_buffer)) {
		lx_vk_allocator_copy(device->allocator_vertex, &vertex_buffer, 0, (lx_pointer_t)polygon->points, size);
		lx_array_insert_tail(device->vertex_buffers, &vertex_buffer);

        lx_uint16_t     count;
        lx_size_t       index = 0;
        lx_uint16_t*    counts = polygon->counts;
        VkCommandBuffer cmdbuffer = device->renderer_cmdbuffer;
        while ((count = *counts++)) {
            VkDeviceSize offset = index * sizeof(lx_point_t);
            vkCmdBindVertexBuffers(cmdbuffer, 0, 1, &vertex_buffer.buffer, &offset);
            vkCmdDraw(cmdbuffer, count, 1, 0, 0);
            index += count;
        }
    }
}

static lx_void_t lx_vk_renderer_stroke_fill(lx_vulkan_device_t* device, lx_path_ref_t path) {
    lx_assert(device && device->stroker && device->base.paint && path);
    lx_check_return(!lx_path_empty(path));

    // the mode
    lx_size_t mode = lx_paint_mode(device->base.paint);

    // the rule
    lx_size_t rule = lx_paint_fill_rule(device->base.paint);

    // switch to the fill mode
    lx_paint_mode_set(device->base.paint, LX_PAINT_MODE_FILL);

    // switch to the non-zero fill rule
    lx_paint_fill_rule_set(device->base.paint, LX_PAINT_FILL_RULE_NONZERO);

    // draw path
    lx_vk_renderer_draw_path(device, path);

    // restore the mode
    lx_paint_mode_set(device->base.paint, mode);

    // restore the fill mode
    lx_paint_fill_rule_set(device->base.paint, rule);
}

static lx_inline lx_bool_t lx_vk_renderer_stroke_only(lx_vulkan_device_t* device) {
    lx_assert(device && device->base.paint && device->base.matrix);

    // width == 1 and solid? only stroke it
    return (    1.0f == lx_paint_stroke_width(device->base.paint)
        &&  1.0f == lx_abs(device->base.matrix->sx)
        &&  1.0f == lx_abs(device->base.matrix->sy)
        /*&&  !device->shader*/);
}

static lx_bool_t lx_vk_renderer_draw_prepare(lx_vulkan_device_t* device) {
    if (device->renderer_prepared) {
        return lx_true;
    }

    // get current command buffer
    lx_assert_and_check_return_val(device->imageindex < device->command_buffers_count, lx_false);
    VkCommandBuffer cmdbuffer = device->command_buffers[device->imageindex];

    // we start by creating and declare the "beginning" our command buffer
    VkCommandBufferBeginInfo cmdbuffer_begininfo = {};
    cmdbuffer_begininfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    cmdbuffer_begininfo.pNext = lx_null;
    cmdbuffer_begininfo.flags = 0;
    cmdbuffer_begininfo.pInheritanceInfo = lx_null;
    if (vkBeginCommandBuffer(cmdbuffer, &cmdbuffer_begininfo) != VK_SUCCESS) {
        return lx_false;
    }

    // transition the display image to color attachment layout
    lx_vk_renderer_set_imagelayout(cmdbuffer, device->images[device->imageindex],
        VK_IMAGE_LAYOUT_UNDEFINED,
        VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
        VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT);

    // now we start a renderpass. Any draw command has to be recorded in a renderpass
    VkClearValue clear_values;
    clear_values.color = device->renderer_clear_color;
    VkRenderPassBeginInfo renderpass_begininfo = {};
    renderpass_begininfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderpass_begininfo.pNext = lx_null;
    renderpass_begininfo.renderPass = device->renderpass;
    renderpass_begininfo.framebuffer = device->framebuffers[device->imageindex];
    renderpass_begininfo.renderArea.offset.x = 0;
    renderpass_begininfo.renderArea.offset.y = 0;
    renderpass_begininfo.renderArea.extent = device->framesize;
    renderpass_begininfo.clearValueCount = 1;
    renderpass_begininfo.pClearValues = &clear_values;
    vkCmdBeginRenderPass(cmdbuffer, &renderpass_begininfo, VK_SUBPASS_CONTENTS_INLINE);

    // prepare ok
    device->renderer_prepared = lx_true;
    device->renderer_cmdbuffer = cmdbuffer;
    return lx_true;
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
lx_bool_t lx_vk_renderer_draw_lock(lx_vulkan_device_t* device) {
    lx_assert(device && device->device && device->swapchain && device->semaphore);
    lx_assert(device->vertex_buffers);

    // get the framebuffer index we should draw in
    if (vkAcquireNextImageKHR(device->device, device->swapchain, UINT64_MAX, device->semaphore, VK_NULL_HANDLE, &device->imageindex) != VK_SUCCESS) {
        return lx_false;
    }
    lx_assert_and_check_return_val(device->imageindex < device->command_buffers_count, lx_false);
    if (vkResetFences(device->device, 1, &device->fence) != VK_SUCCESS) {
        return lx_false;
    }

    // reset renderer
    device->renderer_prepared = lx_false;
    lx_array_clear(device->vertex_buffers);
    return lx_true;
}

lx_void_t lx_vk_renderer_draw_commit(lx_vulkan_device_t* device) {
    lx_assert(device->imageindex < device->command_buffers_count);
    lx_assert_and_check_return(device->renderer_prepared);

    // command end
    VkCommandBuffer cmdbuffer = device->command_buffers[device->imageindex];
    vkCmdEndRenderPass(cmdbuffer);
    vkEndCommandBuffer(cmdbuffer);

    // submit command buffers
    VkPipelineStageFlags wait_stage_mask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    VkSubmitInfo submit_info = {};
    submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submit_info.pNext = lx_null;
    submit_info.waitSemaphoreCount = 1;
    submit_info.pWaitSemaphores = &device->semaphore;
    submit_info.pWaitDstStageMask = &wait_stage_mask;
    submit_info.commandBufferCount = 1;
    submit_info.pCommandBuffers = &device->command_buffers[device->imageindex];
    submit_info.signalSemaphoreCount = 0;
    submit_info.pSignalSemaphores = lx_null;
    if (vkQueueSubmit(device->queue, 1, &submit_info, device->fence) != VK_SUCCESS) {
        return;
    }
    if (vkWaitForFences(device->device, 1, &device->fence, VK_TRUE, 100000000) != VK_SUCCESS) {
        return;
    }

    // present frame
    VkResult result;
    VkPresentInfoKHR present_info = {};
    present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    present_info.pNext = lx_null;
    present_info.waitSemaphoreCount = 0;
    present_info.pWaitSemaphores = lx_null;
    present_info.swapchainCount = 1;
    present_info.pSwapchains = &device->swapchain;
    present_info.pImageIndices = &device->imageindex;
    present_info.pResults = &result;
    vkQueuePresentKHR(device->queue, &present_info);
}

lx_void_t lx_vk_renderer_draw_clear(lx_vulkan_device_t* device, lx_color_t color) {
    device->renderer_clear_color.float32[0] = (lx_float_t)color.r / 0xff;
    device->renderer_clear_color.float32[1] = (lx_float_t)color.g / 0xff;
    device->renderer_clear_color.float32[2] = (lx_float_t)color.b / 0xff;
    device->renderer_clear_color.float32[3] = (lx_float_t)color.a / 0xff;
    lx_vk_renderer_draw_prepare(device);
}

lx_void_t lx_vk_renderer_draw_path(lx_vulkan_device_t* device, lx_path_ref_t path) {
    lx_assert(device && device->base.paint && path);

    if (!lx_vk_renderer_draw_prepare(device)) {
        return ;
    }

    lx_size_t mode = lx_paint_mode(device->base.paint);
    if (mode & LX_PAINT_MODE_FILL) {
        lx_vk_renderer_draw_polygon(device, lx_path_polygon(path), lx_path_hint(path), lx_path_bounds(path));
    }

    if ((mode & LX_PAINT_MODE_STROKE) && (lx_paint_stroke_width(device->base.paint) > 0)) {
        if (lx_vk_renderer_stroke_only(device)) {
            lx_vk_renderer_draw_polygon(device, lx_path_polygon(path), lx_path_hint(path), lx_path_bounds(path));
        } else {
            lx_vk_renderer_stroke_fill(device, lx_stroker_make_from_path(device->stroker, device->base.paint, path));
        }
    }
}

lx_void_t lx_vk_renderer_draw_lines(lx_vulkan_device_t* device, lx_point_ref_t points, lx_size_t count, lx_rect_ref_t bounds) {
    lx_assert(device && device->base.paint && points && count);
    lx_check_return(lx_paint_mode(device->base.paint) & LX_PAINT_MODE_STROKE);
    lx_check_return((lx_paint_stroke_width(device->base.paint) > 0));

    if (!lx_vk_renderer_draw_prepare(device)) {
        return ;
    }

    if (lx_vk_renderer_stroke_only(device)) {
        lx_vk_renderer_apply_paint_solid(device, LX_VK_PIPELINE_TYPE_LINES);
        lx_vk_renderer_stroke_lines(device, points, count);
    } else {
        lx_vk_renderer_apply_paint(device, bounds);
        lx_vk_renderer_stroke_fill(device, lx_stroker_make_from_lines(device->stroker, device->base.paint, points, count));
    }
}

lx_void_t lx_vk_renderer_draw_points(lx_vulkan_device_t* device, lx_point_ref_t points, lx_size_t count, lx_rect_ref_t bounds) {
    lx_assert(device && device->base.paint && points && count);
    lx_check_return(lx_paint_mode(device->base.paint) & LX_PAINT_MODE_STROKE);
    lx_check_return((lx_paint_stroke_width(device->base.paint) > 0));

    if (!lx_vk_renderer_draw_prepare(device)) {
        return ;
    }

    if (lx_vk_renderer_stroke_only(device)) {
        lx_vk_renderer_apply_paint_solid(device, LX_VK_PIPELINE_TYPE_POINTS);
        lx_vk_renderer_stroke_points(device, points, count);
    } else {
        lx_vk_renderer_apply_paint(device, bounds);
        lx_vk_renderer_stroke_fill(device, lx_stroker_make_from_points(device->stroker, device->base.paint, points, count));
    }
}

lx_void_t lx_vk_renderer_draw_polygon(lx_vulkan_device_t* device, lx_polygon_ref_t polygon, lx_shape_ref_t hint, lx_rect_ref_t bounds) {
    lx_assert(device && device->base.paint && polygon && polygon->points && polygon->counts);

    if (!lx_vk_renderer_draw_prepare(device)) {
        return ;
    }

    if (hint && hint->type == LX_SHAPE_TYPE_LINE) {
        lx_point_t points[2];
        points[0] = hint->u.line.p0;
        points[1] = hint->u.line.p1;
        lx_vk_renderer_draw_lines(device, points, 2, bounds);
        return ;
    } else if (hint && hint->type == LX_SHAPE_TYPE_POINT) {
        lx_vk_renderer_draw_points(device, &hint->u.point, 1, bounds);
        return ;
    }

    lx_bool_t fill_applied = lx_false;
    lx_size_t mode = lx_paint_mode(device->base.paint);
    if (mode & LX_PAINT_MODE_FILL) {
        lx_vk_renderer_apply_paint(device, bounds);
        lx_vk_renderer_fill_polygon(device, polygon, bounds, lx_paint_fill_rule(device->base.paint));
        fill_applied = lx_true;
    }

    if ((mode & LX_PAINT_MODE_STROKE) && (lx_paint_stroke_width(device->base.paint) > 0)) {
        if (lx_vk_renderer_stroke_only(device)) {
            lx_vk_renderer_apply_paint_solid(device, LX_VK_PIPELINE_TYPE_LINES);
            lx_vk_renderer_stroke_polygon(device, polygon);
        } else {
            if (!fill_applied) {
                lx_vk_renderer_apply_paint(device, bounds);
            }
            lx_vk_renderer_stroke_fill(device, lx_stroker_make_from_polygon(device->stroker, device->base.paint, polygon, hint));
        }
    }
}

