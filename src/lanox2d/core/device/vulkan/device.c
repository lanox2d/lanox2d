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
 * @file        device.c
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "device.h"
#ifdef LX_CONFIG_WINDOW_HAVE_GLFW
#   include <GLFW/glfw3.h>
#endif

/* //////////////////////////////////////////////////////////////////////////////////////
 * private implementation
 */

static lx_bool_t lx_device_vulkan_draw_lock(lx_device_ref_t self) {
    return lx_true;
}

static lx_void_t lx_device_vulkan_draw_commit(lx_device_ref_t self) {
}

static lx_void_t lx_device_vulkan_draw_clear(lx_device_ref_t self, lx_color_t color) {
}

static lx_void_t lx_device_vulkan_draw_lines(lx_device_ref_t self, lx_point_ref_t points, lx_size_t count, lx_rect_ref_t bounds) {
}

static lx_void_t lx_device_vulkan_draw_points(lx_device_ref_t self, lx_point_ref_t points, lx_size_t count, lx_rect_ref_t bounds) {
}

static lx_void_t lx_device_vulkan_draw_polygon(lx_device_ref_t self, lx_polygon_ref_t polygon, lx_shape_ref_t hint, lx_rect_ref_t bounds) {
}

static lx_void_t lx_device_vulkan_draw_path(lx_device_ref_t self, lx_path_ref_t path) {
}

static lx_void_t lx_device_vulkan_exit(lx_device_ref_t self) {
    lx_vulkan_device_t* device = (lx_vulkan_device_t*)self;
    if (device) {
        if (device->instance) {
            vkDestroyInstance(device->instance, lx_null);
            device->instance = lx_null;
        }
        lx_free(device);
    }
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
lx_device_ref_t lx_device_init_from_vulkan(lx_size_t width, lx_size_t height) {
    lx_assert_and_check_return_val(width && height, lx_null);

    lx_bool_t           ok = lx_false;
    lx_vulkan_device_t* device = lx_null;
    do {

        // init device
        device = lx_malloc0_type(lx_vulkan_device_t);
        lx_assert_and_check_break(device);

        device->base.draw_clear   = lx_device_vulkan_draw_clear;
        device->base.draw_lines   = lx_device_vulkan_draw_lines;
        device->base.draw_points  = lx_device_vulkan_draw_points;
        device->base.draw_polygon = lx_device_vulkan_draw_polygon;
        device->base.draw_path    = lx_device_vulkan_draw_path;
        device->base.draw_lock    = lx_device_vulkan_draw_lock;
        device->base.draw_commit  = lx_device_vulkan_draw_commit;
        device->base.exit         = lx_device_vulkan_exit;

        // init instance
        VkApplicationInfo appinfo  = {};
        appinfo.sType              = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appinfo.pApplicationName   = "Lanox2d";
        appinfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        appinfo.pEngineName        = "Lanox2d";
        appinfo.engineVersion      = VK_MAKE_VERSION(1, 0, 0);
        appinfo.apiVersion         = VK_API_VERSION_1_0;

        VkInstanceCreateInfo createinfo = {};
        createinfo.sType                = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createinfo.pApplicationInfo     = &appinfo;
        createinfo.enabledLayerCount    = 0;
#ifdef LX_CONFIG_WINDOW_HAVE_GLFW
        createinfo.ppEnabledExtensionNames = glfwGetRequiredInstanceExtensions(&createinfo.enabledExtensionCount);
#endif
        if (vkCreateInstance(&createinfo, lx_null, &device->instance) != VK_SUCCESS) {
            lx_trace_e("failed to create vulkan instance!");
            break;
        }

        // ok
        ok = lx_true;

    } while (0);

    // failed?
    if (!ok && device) {
        lx_device_exit((lx_device_ref_t)device);
        device = lx_null;
    }
    return (lx_device_ref_t)device;
}


