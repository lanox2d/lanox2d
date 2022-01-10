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
#include "../../quality.h"
#include "../../tess/tess.h"

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

static lx_inline lx_void_t lx_vk_renderer_apply_paint_shader(lx_vulkan_device_t* device, lx_shader_ref_t shader, lx_rect_ref_t bounds) {
}

static lx_inline lx_void_t lx_vk_renderer_apply_paint_solid(lx_vulkan_device_t* device) {
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
    lx_vk_pipeline_ref_t pipeline = lx_vk_pipeline_solid(device);
    lx_assert_and_check_return(pipeline);
    vkCmdBindPipeline(cmdbuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, lx_vk_pipeline_native(pipeline));

    // apply color
    // TODO
}

static lx_inline lx_void_t lx_vk_renderer_apply_paint(lx_vulkan_device_t* device, lx_rect_ref_t bounds) {
    lx_assert(device);

    // set projection matrix
    // TODO

    // set model matrix
    // TODO

    // apply paint
    lx_shader_ref_t shader = lx_paint_shader(device->base.paint);
    if (shader) {
        lx_vk_renderer_apply_paint_shader(device, shader, bounds);
    } else {
        lx_vk_renderer_apply_paint_solid(device);
    }
}

static lx_inline lx_void_t lx_vk_renderer_fill_polygon(lx_vulkan_device_t* device, lx_polygon_ref_t polygon, lx_rect_ref_t bounds, lx_size_t rule) {
    VkCommandBuffer cmdbuffer = device->renderer_cmdbuffer;

    static const lx_float_t vertex_data[] = {
      -1.0f, -1.0f, 0.0f,
      1.0f, -1.0f, 0.0f,
      0.0f, 1.0f, 0.0f
    };

    static const lx_float_t color_data[] = {
        1.0f, 0.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 0.0f, 0.0f
    };

    lx_vk_buffer_t vertex_buffer;
    if (lx_vk_allocator_alloc(device->vertex_buffer_allocator, sizeof(vertex_data), &vertex_buffer)) {
        lx_vk_allocator_copy(device->vertex_buffer_allocator, &vertex_buffer, (lx_pointer_t)vertex_data, sizeof(vertex_data));
        lx_array_insert_tail(device->vertex_buffers, &vertex_buffer);
    }

    lx_vk_buffer_t color_buffer;
    if (lx_vk_allocator_alloc(device->vertex_buffer_allocator, sizeof(color_data), &color_buffer)) {
        lx_vk_allocator_copy(device->vertex_buffer_allocator, &color_buffer, (lx_pointer_t)color_data, sizeof(color_data));
        lx_array_insert_tail(device->vertex_buffers, &color_buffer);
    }

    VkDeviceSize offsets[2] = {0};
    VkBuffer buffers[2];
    buffers[0] = vertex_buffer.buffer;
    buffers[1] = color_buffer.buffer;
    vkCmdBindVertexBuffers(cmdbuffer, 0, 2, buffers, offsets);
    vkCmdDraw(cmdbuffer, 3, 1, 0, 0);
}

static lx_inline lx_void_t lx_vk_renderer_stroke_lines(lx_vulkan_device_t* device, lx_point_ref_t points, lx_size_t count) {
    lx_assert(device && points && count);
}

static lx_inline lx_void_t lx_vk_renderer_stroke_points(lx_vulkan_device_t* device, lx_point_ref_t points, lx_size_t count) {
    lx_assert(device && points && count);
}

static lx_inline lx_void_t lx_vk_renderer_stroke_polygon(lx_vulkan_device_t* device, lx_polygon_ref_t polygon) {
    lx_assert(device && polygon && polygon->points && polygon->counts);
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

    lx_vk_renderer_apply_paint(device, bounds);
    if (lx_vk_renderer_stroke_only(device)) {
        lx_vk_renderer_stroke_lines(device, points, count);
    } else {
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

    lx_vk_renderer_apply_paint(device, bounds);
    if (lx_vk_renderer_stroke_only(device)) {
        lx_vk_renderer_stroke_points(device, points, count);
    } else {
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

    lx_vk_renderer_apply_paint(device, bounds);
    lx_size_t mode = lx_paint_mode(device->base.paint);
    if (mode & LX_PAINT_MODE_FILL) {
        lx_vk_renderer_fill_polygon(device, polygon, bounds, lx_paint_fill_rule(device->base.paint));
    }

    if ((mode & LX_PAINT_MODE_STROKE) && (lx_paint_stroke_width(device->base.paint) > 0)) {
        if (lx_vk_renderer_stroke_only(device)) {
            lx_vk_renderer_stroke_polygon(device, polygon);
        } else {
            lx_vk_renderer_stroke_fill(device, lx_stroker_make_from_polygon(device->stroker, device->base.paint, polygon, hint));
        }
    }
}

