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
 * @file        android.c
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#ifdef LX_CONFIG_DEVICE_HAVE_VULKAN
#   include "../../core/device/vulkan/vk.h"
#   include <android/native_window.h>
#   include <android/native_window_jni.h>
#endif

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

// the android window type
typedef struct lx_window_android_t_ {
    lx_window_t                 base;
#ifdef LX_CONFIG_DEVICE_HAVE_VULKAN
    ANativeWindow*              window;
    VkInstance                  instance;
    VkSurfaceKHR                surface;
#   ifdef LX_DEBUG
    VkDebugReportCallbackEXT    debug_report_cb;
    VkDebugUtilsMessengerEXT    debug_messenger;
#   endif
#endif
} lx_window_android_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * private implementation
 */

static lx_void_t lx_window_android_draw(lx_window_ref_t self) {
    lx_window_android_t* window = (lx_window_android_t*)self;
    lx_assert(window && window->base.on_draw);

    // do draw
    window->base.on_draw((lx_window_ref_t)window, window->base.canvas);
}

static lx_void_t lx_window_android_resize(lx_window_ref_t self, lx_size_t width, lx_size_t height) {
    lx_window_android_t* window = (lx_window_android_t*)self;
    lx_check_return(width && height);

    lx_event_t event = {0};
    event.type = LX_EVENT_TYPE_ACTIVE;
    event.u.active.code = LX_ACTIVE_RESIZE_WINDOW;
    event.u.active.data[0] = (lx_size_t)width;
    event.u.active.data[1] = (lx_size_t)height;
    if (window->base.on_event) {
        window->base.on_event((lx_window_ref_t)window, &event);
    }
}

static lx_void_t lx_window_android_exit(lx_window_ref_t self) {
    lx_window_android_t* window = (lx_window_android_t*)self;
    if (window) {
#ifdef LX_CONFIG_DEVICE_HAVE_VULKAN
#   ifdef LX_DEBUG
        if (window->debug_messenger) {
            lx_vk_debug_messenger_cancel(window->instance, window->debug_messenger);
            window->debug_messenger = 0;
        } else if (window->debug_report_cb) {
            lx_vk_debug_report_cancel(window->instance, window->debug_report_cb);
            window->debug_report_cb = 0;
        }
#   endif
        if (window->surface) {
            vkDestroySurfaceKHR(window->instance, window->surface, lx_null);
            window->surface = 0;
        }
        if (window->instance) {
            vkDestroyInstance(window->instance, lx_null);
            window->instance = lx_null;
        }
        if (window->window) {
            ANativeWindow_release(window->window);
            window->window = lx_null;
        }
        lx_vk_context_exit();
#endif
        if (window->base.canvas) {
            lx_canvas_exit(window->base.canvas);
            window->base.canvas = lx_null;
        }
        if (window->base.device) {
            lx_device_exit(window->base.device);
            window->base.device = lx_null;
        }
        lx_free(window);
    }
}

#ifdef LX_CONFIG_DEVICE_HAVE_VULKAN
static lx_bool_t lx_window_android_init_vulkan(lx_window_android_t* window, ANativeWindow* native_window) {
    lx_bool_t ok = lx_false;
    do {

        // init vulkan context
        if (!lx_vk_context_init()) {
            lx_trace_e("failed to init vulkan context!");
            break;
        }

        // save native window
        window->window = native_window;
        lx_assert_and_check_break(native_window);

        // init vulkan instance
        VkApplicationInfo appinfo  = {};
        appinfo.sType              = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appinfo.pApplicationName   = "Lanox2d";
        appinfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        appinfo.pEngineName        = "Lanox2d";
        appinfo.engineVersion      = VK_MAKE_VERSION(1, 0, 0);
        appinfo.apiVersion         = VK_API_VERSION_1_0;

        VkInstanceCreateInfo createinfo    = {};
        createinfo.sType                   = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createinfo.pApplicationInfo        = &appinfo;
#ifdef LX_DEBUG
        /* enable validation layers
         *
         * @see https://developer.android.com/ndk/guides/graphics/validation-layer
         */
        static lx_char_t const* validation_layers[] = {"VK_LAYER_KHRONOS_validation"};
        if (lx_vk_validation_layers_check(validation_layers, lx_arrayn(validation_layers))) {
            lx_vk_validation_layers_add(validation_layers, lx_arrayn(validation_layers));
        }

        // enable instance extensions: VK_KHR_surface, VK_KHR_android_surface
        static lx_char_t const* instance_extensions[] = {VK_KHR_SURFACE_EXTENSION_NAME, VK_KHR_ANDROID_SURFACE_EXTENSION_NAME};
        lx_vk_instance_extensions_add(instance_extensions, lx_arrayn(instance_extensions));

        // enable debug utils extension
        lx_bool_t has_debug_utils_extension = lx_false;
        static lx_char_t const* debug_utils_extensions[] = {VK_EXT_DEBUG_UTILS_EXTENSION_NAME};
        if (lx_vk_instance_extensions_check(debug_utils_extensions, lx_arrayn(debug_utils_extensions))) {
            lx_vk_instance_extensions_add(debug_utils_extensions, lx_arrayn(debug_utils_extensions));
            has_debug_utils_extension = lx_true;
        }

        // enable debug report extension
        lx_bool_t has_debug_report_extension = lx_false;
        static lx_char_t const* debug_report_extensions[] = {VK_EXT_DEBUG_REPORT_EXTENSION_NAME};
        if (lx_vk_instance_extensions_check(debug_report_extensions, lx_arrayn(debug_report_extensions))) {
            lx_vk_instance_extensions_add(debug_report_extensions, lx_arrayn(debug_report_extensions));
            has_debug_report_extension = lx_true;
        }
#endif
        createinfo.ppEnabledLayerNames     = lx_vk_validation_layers(&createinfo.enabledLayerCount);
        createinfo.ppEnabledExtensionNames = lx_vk_instance_extensions(&createinfo.enabledExtensionCount);
        if (vkCreateInstance(&createinfo, lx_null, &window->instance) != VK_SUCCESS) {
            lx_trace_e("failed to create vulkan instance!");
            break;
        }

        // create surface with vkinstance
        VkAndroidSurfaceCreateInfoKHR surface_createinfo;
        surface_createinfo.sType = VK_STRUCTURE_TYPE_ANDROID_SURFACE_CREATE_INFO_KHR;
        surface_createinfo.pNext = lx_null;
        surface_createinfo.flags = 0;
        surface_createinfo.window = native_window;
        if (vkCreateAndroidSurfaceKHR(window->instance, &surface_createinfo, lx_null, &window->surface) != VK_SUCCESS) {
            lx_trace_e("failed to create vulkan surface!");
            break;
        }

#ifdef LX_DEBUG
        // setup debug callback
        if (has_debug_utils_extension) {
            lx_vk_debug_messenger_setup(window->instance, &window->debug_messenger);
        } else if (has_debug_report_extension) {
            lx_vk_debug_report_setup(window->instance, &window->debug_report_cb);
        }
#endif

        ok = lx_true;
    } while (0);
    return ok;
}
#endif

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
lx_window_ref_t lx_window_init_android(lx_size_t width, lx_size_t height, lx_char_t const* title, lx_cpointer_t devdata) {
    lx_bool_t            ok = lx_false;
    lx_window_android_t* window = lx_null;
    do {

        // check
        lx_assert_and_check_break(width && height);

        // init window
        window = lx_malloc0_type(lx_window_android_t);
        lx_assert_and_check_break(window);

        window->base.width       = (lx_uint16_t)width;
        window->base.height      = (lx_uint16_t)height;
        window->base.title       = title;
        window->base.draw        = lx_window_android_draw;
        window->base.resize      = lx_window_android_resize;
        window->base.exit        = lx_window_android_exit;
        window->base.pixfmt      = LX_PIXFMT_RGBX8888 | LX_PIXFMT_BENDIAN;

        // init device
#if defined(LX_CONFIG_DEVICE_HAVE_OPENGL)
        window->base.device = lx_device_init_from_opengl(width, height, width, height);
#elif defined(LX_CONFIG_DEVICE_HAVE_VULKAN)
        if (!lx_window_android_init_vulkan(window, (ANativeWindow*)devdata)) {
            break;
        }
        window->base.device = lx_device_init_from_vulkan(width, height, window->instance, (lx_cpointer_t)window->surface);
#endif
        lx_assert_and_check_break(window->base.device);

        // init canvas
        window->base.canvas = lx_canvas_init(window->base.device);
        lx_assert_and_check_break(window->base.canvas);

        // ok
        ok = lx_true;

    } while (0);

    if (!ok && window) {
        lx_window_exit((lx_window_ref_t)window);
        window = lx_null;
    }
    return (lx_window_ref_t)window;
}
