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
 * @file        glfw.c
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#if defined(LX_CONFIG_DEVICE_HAVE_OPENGL)
#   include "../../core/device/opengl/gl.h"
#elif defined(LX_CONFIG_DEVICE_HAVE_VULKAN)
#   include "../../core/device/vulkan/vk.h"
#endif
#include <GLFW/glfw3.h>

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

// the glfw window type
typedef struct lx_window_glfw_t_ {
    lx_window_t                 base;
    GLFWwindow*                 window;
    lx_bool_t                   is_quit;
    lx_hong_t                   fps_time;
    lx_hong_t                   fps_count;
    lx_int_t                    fps_delay;
#ifdef LX_CONFIG_DEVICE_HAVE_VULKAN
    VkInstance                  instance;
    VkSurfaceKHR                surface;
#   ifdef LX_DEBUG
    VkDebugReportCallbackEXT    debug_report_cb;
    VkDebugUtilsMessengerEXT    debug_messenger;
#   endif
#endif
} lx_window_glfw_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * private implementation
 */

static lx_void_t lx_window_glfw_fullscreen(lx_window_ref_t self, lx_bool_t is_fullscreen) {
    lx_window_glfw_t* window = (lx_window_glfw_t*)self;
    if (window) {
        // TODO
    }
}

static lx_void_t lx_window_glfw_mouse_button_callback(GLFWwindow* self, lx_int_t button, lx_int_t action, lx_int_t mode) {
    lx_assert(self);
    lx_window_glfw_t* window = (lx_window_glfw_t*)glfwGetWindowUserPointer(self);
    lx_assert(window && window->window);

    lx_double_t x, y;
    glfwGetCursorPos(window->window, &x, &y);

    lx_event_t event = {0};
    lx_point_make(&event.u.mouse.cursor, (lx_float_t)x, (lx_float_t)y);
    if (button == GLFW_MOUSE_BUTTON_LEFT)  {
        event.type              = LX_EVENT_TYPE_MOUSE;
        event.u.mouse.button    = LX_MOUSE_BUTTON_LEFT;
        event.u.mouse.code      = (action == GLFW_PRESS)? LX_MOUSE_DOWN : LX_MOUSE_UP;
    } else if (button == GLFW_MOUSE_BUTTON_RIGHT) {
        event.type              = LX_EVENT_TYPE_MOUSE;
        event.u.mouse.button    = LX_MOUSE_BUTTON_RIGHT;
        event.u.mouse.code      = (action == GLFW_PRESS)? LX_MOUSE_DOWN : LX_MOUSE_UP;
    } else if (button == GLFW_MOUSE_BUTTON_MIDDLE) {
        event.type              = LX_EVENT_TYPE_MOUSE;
        event.u.mouse.button    = LX_MOUSE_BUTTON_MIDDLE;
        event.u.mouse.code      = (action == GLFW_PRESS)? LX_MOUSE_DOWN : LX_MOUSE_UP;
    }
    if (window->base.on_event) {
        window->base.on_event((lx_window_ref_t)window, &event);
    }
}

static lx_void_t lx_window_glfw_cursor_pos_callback(GLFWwindow* self, lx_double_t x, lx_double_t y) {
    lx_assert(self);
    lx_window_glfw_t* window = (lx_window_glfw_t*)glfwGetWindowUserPointer(self);
    lx_assert(window && window->window);

    lx_event_t event = {0};
    event.type           = LX_EVENT_TYPE_MOUSE;
    event.u.mouse.code   = LX_MOUSE_MOVE;
    if (GLFW_PRESS == glfwGetMouseButton(window->window, GLFW_MOUSE_BUTTON_LEFT)) {
        event.u.mouse.button = LX_MOUSE_BUTTON_LEFT;
    } else if (GLFW_PRESS == glfwGetMouseButton(window->window, GLFW_MOUSE_BUTTON_RIGHT)) {
        event.u.mouse.button = LX_MOUSE_BUTTON_RIGHT;
    } else if (GLFW_PRESS == glfwGetMouseButton(window->window, GLFW_MOUSE_BUTTON_MIDDLE)) {
        event.u.mouse.button = LX_MOUSE_BUTTON_MIDDLE;
    }
    lx_point_make(&event.u.mouse.cursor, (lx_float_t)x, (lx_float_t)y);
    if (window->base.on_event) {
        window->base.on_event((lx_window_ref_t)window, &event);
    }
}

static lx_void_t lx_window_glfw_key_callback(GLFWwindow* self, lx_int_t key, lx_int_t scancode, lx_int_t action, lx_int_t mode) {
    lx_assert(self);
    lx_window_glfw_t* window = (lx_window_glfw_t*)glfwGetWindowUserPointer(self);
    lx_assert(window);
    if (action == GLFW_PRESS || action == GLFW_RELEASE || action == GLFW_REPEAT) {
        lx_event_t event = {0};
        event.type               = LX_EVENT_TYPE_KEYBOARD;
        event.u.keyboard.pressed = action != GLFW_RELEASE? lx_true : lx_false;
        switch (key) {
        case GLFW_KEY_F1:           event.u.keyboard.code = LX_KEY_F1;          break;
        case GLFW_KEY_F2:           event.u.keyboard.code = LX_KEY_F2;          break;
        case GLFW_KEY_F3:           event.u.keyboard.code = LX_KEY_F3;          break;
        case GLFW_KEY_F4:           event.u.keyboard.code = LX_KEY_F4;          break;
        case GLFW_KEY_F5:           event.u.keyboard.code = LX_KEY_F5;          break;
        case GLFW_KEY_F6:           event.u.keyboard.code = LX_KEY_F6;          break;
        case GLFW_KEY_F7:           event.u.keyboard.code = LX_KEY_F7;          break;
        case GLFW_KEY_F8:           event.u.keyboard.code = LX_KEY_F8;          break;
        case GLFW_KEY_F9:           event.u.keyboard.code = LX_KEY_F9;          break;
        case GLFW_KEY_F10:          event.u.keyboard.code = LX_KEY_F10;         break;
        case GLFW_KEY_F11:          event.u.keyboard.code = LX_KEY_F11;         break;
        case GLFW_KEY_F12:          event.u.keyboard.code = LX_KEY_F12;         break;

        case GLFW_KEY_LEFT:         event.u.keyboard.code = LX_KEY_LEFT;        break;
        case GLFW_KEY_UP:           event.u.keyboard.code = LX_KEY_UP;          break;
        case GLFW_KEY_RIGHT:        event.u.keyboard.code = LX_KEY_RIGHT;       break;
        case GLFW_KEY_DOWN:         event.u.keyboard.code = LX_KEY_DOWN;        break;

        case GLFW_KEY_HOME:         event.u.keyboard.code = LX_KEY_HOME;        break;
        case GLFW_KEY_END:          event.u.keyboard.code = LX_KEY_END;         break;
        case GLFW_KEY_INSERT:       event.u.keyboard.code = LX_KEY_INSERT;      break;
        case GLFW_KEY_PAGE_UP:      event.u.keyboard.code = LX_KEY_PAGEUP;      break;
        case GLFW_KEY_PAGE_DOWN:    event.u.keyboard.code = LX_KEY_PAGEDOWN;    break;

        case GLFW_KEY_MENU:         event.u.keyboard.code = LX_KEY_MENU;        break;

        case GLFW_KEY_CAPS_LOCK:    event.u.keyboard.code = LX_KEY_CAPSLOCK;    break;
        case GLFW_KEY_SCROLL_LOCK:  event.u.keyboard.code = LX_KEY_SCROLLLOCK;  break;
        case GLFW_KEY_RIGHT_SHIFT:  event.u.keyboard.code = LX_KEY_RSHIFT;      break;
        case GLFW_KEY_LEFT_SHIFT:   event.u.keyboard.code = LX_KEY_LSHIFT;      break;
        case GLFW_KEY_RIGHT_CONTROL:event.u.keyboard.code = LX_KEY_RCTRL;       break;
        case GLFW_KEY_LEFT_CONTROL: event.u.keyboard.code = LX_KEY_LCTRL;       break;
        case GLFW_KEY_RIGHT_ALT:    event.u.keyboard.code = LX_KEY_RALT;        break;
        case GLFW_KEY_LEFT_ALT:     event.u.keyboard.code = LX_KEY_LALT;        break;

        case GLFW_KEY_PAUSE:        event.u.keyboard.code = LX_KEY_PAUSE;       break;
        case GLFW_KEY_ENTER:        event.u.keyboard.code = LX_KEY_ENTER;       break;
        case GLFW_KEY_ESCAPE:       event.u.keyboard.code = LX_KEY_ESCAPE;       break;

        default :
            if (key < 256) {
                if (mode != GLFW_MOD_SHIFT && key >= 'A' && key <= 'Z') {
                    key = lx_tolower(key);
                } else if (mode == GLFW_MOD_SHIFT) {
                    switch (key) {
                    case GLFW_KEY_0:
                        key = ')';
                        break;
                    case GLFW_KEY_1:
                        key = '!';
                        break;
                    case GLFW_KEY_2:
                        key = '@';
                        break;
                    case GLFW_KEY_3:
                        key = '#';
                        break;
                    case GLFW_KEY_4:
                        key = '$';
                        break;
                    case GLFW_KEY_5:
                        key = '%';
                        break;
                    case GLFW_KEY_6:
                        key = '^';
                        break;
                    case GLFW_KEY_7:
                        key = '&';
                        break;
                    case GLFW_KEY_8:
                        key = '*';
                        break;
                    case GLFW_KEY_9:
                        key = '(';
                        break;
                    case '=':
                        key = '+';
                        break;
                    case '-':
                        key = '_';
                        break;
                    case '[':
                        key = '{';
                        break;
                    case ']':
                        key = '}';
                        break;
                    case '\\':
                        key = '|';
                        break;
                    case ';':
                        key = ':';
                        break;
                    case '\'':
                        key = '\"';
                        break;
                    case ',':
                        key = '<';
                        break;
                    case '.':
                        key = '>';
                        break;
                    case '/':
                        key = '?';
                        break;
                    case '`':
                        key = '~';
                        break;
                    default:
                        break;
                    }
                }
                event.u.keyboard.code = key;
            }
            break;
        }
        if (event.u.keyboard.code && window->base.on_event) {
            window->base.on_event((lx_window_ref_t)window, &event);
        }
    }
}

#ifdef LX_CONFIG_DEVICE_HAVE_VULKAN
static lx_bool_t lx_window_glfw_init_vulkan(lx_window_glfw_t* window) {
    // init vulkan context
    if (!lx_vk_context_init()) {
        lx_trace_e("failed to init vulkan context!");
        return lx_false;
    }

    // init vulkan instance
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
#ifdef LX_DEBUG
    // enable validation layers
    static lx_char_t const* validation_layers[] = {"VK_LAYER_KHRONOS_validation"};
    if (lx_vk_validation_layers_check(validation_layers, lx_arrayn(validation_layers))) {
        lx_vk_validation_layers_add(validation_layers, lx_arrayn(validation_layers));
    }

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

    // enable glfw extensions
    lx_uint32_t glfw_extensions_count = 0;
    lx_char_t const** glfw_extensions = glfwGetRequiredInstanceExtensions(&glfw_extensions_count);
    if (glfw_extensions && glfw_extensions_count) {
        lx_vk_instance_extensions_add(glfw_extensions, glfw_extensions_count);
    }
    createinfo.ppEnabledLayerNames = lx_vk_validation_layers(&createinfo.enabledLayerCount);
    createinfo.ppEnabledExtensionNames = lx_vk_instance_extensions(&createinfo.enabledExtensionCount);
    if (vkCreateInstance(&createinfo, lx_null, &window->instance) != VK_SUCCESS) {
        lx_trace_e("failed to create vulkan instance!");
        return lx_false;
    }

#ifdef LX_DEBUG
    // setup debug callback
    if (has_debug_utils_extension) {
        lx_vk_debug_messenger_setup(window->instance, &window->debug_messenger);
    } else if (has_debug_report_extension) {
        lx_vk_debug_report_setup(window->instance, &window->debug_report_cb);
    }
#endif

    // create surface
    if (glfwCreateWindowSurface(window->instance, window->window, lx_null, &window->surface) != VK_SUCCESS) {
        lx_trace_e("failed to create vulkan surface!");
        return lx_false;
    }
    return lx_true;
}
#endif

static lx_bool_t lx_window_glfw_start(lx_window_glfw_t* window) {
    lx_bool_t ok = lx_false;
    do {

        // init glfw
        glfwInit();
#if defined(LX_CONFIG_DEVICE_HAVE_OPENGL)
#   if LX_GL_API_VERSION >= 30
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#   else
#       ifdef LX_GL_API_ES
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
        glfwWindowHint(GLFW_CONTEXT_CREATION_API, GLFW_EGL_CONTEXT_API);
#       else
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_ANY_PROFILE);
#       endif
#   endif
#elif defined(LX_CONFIG_DEVICE_HAVE_VULKAN)
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
#endif
        glfwWindowHint(GLFW_RESIZABLE, (window->base.flags & LX_WINDOW_FLAG_NOT_REISZE)? lx_false : lx_true);
        if (window->base.flags & LX_WINDOW_FLAG_HIDE_TITLEBAR) {
            glfwWindowHint(GLFW_DECORATED, lx_false);
        }
#ifdef LX_CONFIG_OS_MACOSX
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, lx_true);
#endif

        // init window
        window->window = glfwCreateWindow(window->base.width, window->base.height,
            window->base.title? window->base.title : "lanox2d (GLFW)", lx_null, lx_null);
        lx_assert_and_check_break(window->window);
        glfwMakeContextCurrent(window->window);
        glfwSetKeyCallback(window->window, lx_window_glfw_key_callback);
        glfwSetMouseButtonCallback(window->window, lx_window_glfw_mouse_button_callback);
        glfwSetCursorPosCallback(window->window, lx_window_glfw_cursor_pos_callback);
        glfwSetWindowUserPointer(window->window, window);

        // hide window
        if (window->base.flags & LX_WINDOW_FLAG_HIDDEN) {
            glfwHideWindow(window->window);
        }

        // hide window cursor
        if (window->base.flags & LX_WINDOW_FLAG_HIDE_CURSOR) {
            glfwSetInputMode(window->window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
        }

        // fullscreen
        if (window->base.flags & LX_WINDOW_FLAG_FULLSCREEN) {
            lx_window_glfw_fullscreen((lx_window_ref_t)window, lx_true);
        }

        // init viewport
        lx_int_t framewidth = 0;
        lx_int_t frameheight = 0;
        glfwGetFramebufferSize(window->window, &framewidth, &frameheight);

        // init device
#if defined(LX_CONFIG_DEVICE_HAVE_OPENGL)
        window->base.device = lx_device_init_from_opengl(window->base.width, window->base.height, framewidth, frameheight);
#elif defined(LX_CONFIG_DEVICE_HAVE_VULKAN)
        if (!lx_window_glfw_init_vulkan(window)) {
            break;
        }
        window->base.device = lx_device_init_from_vulkan(window->base.width, window->base.height, window->instance, window->surface);
#elif defined(LX_CONFIG_DEVICE_HAVE_SKIA)
        window->base.device = lx_device_init_from_skia(window->base.width, window->base.height, lx_null);
#endif
        lx_assert_and_check_break(window->base.device);

        // init canvas
        window->base.canvas = lx_canvas_init(window->base.device);
        lx_assert_and_check_break(window->base.canvas);

        // ok
        ok = lx_true;
    } while (0);

    // failed? exit glfw
    if (!ok) {
        glfwTerminate();
    }
    return ok;
}

static lx_void_t lx_window_glfw_runloop(lx_window_ref_t self) {
    lx_window_glfw_t* window = (lx_window_glfw_t*)self;
    lx_assert(window && window->base.on_draw);

    // start glfw window
    if (!lx_window_glfw_start(window)) {
        lx_trace_e("start glfw window failed!");
        return ;
    }

    // do loop
    while (!window->is_quit && !glfwWindowShouldClose(window->window)) {

        // poll events
        glfwPollEvents();

        // draw
        lx_hong_t starttime = lx_mclock();
        window->base.on_draw((lx_window_ref_t)window, window->base.canvas);

        // flush
        glfwSwapBuffers(window->window);

        // compute delay for framerate
        lx_hong_t time = lx_mclock();
        lx_int_t  fps_drawtime = (lx_int_t)(time - starttime);
        lx_int_t  fps_delay = 1000 / window->base.fps;
        window->fps_delay = fps_delay >= fps_drawtime? fps_delay - fps_drawtime : 1;

        // compute framerate
        if (window->base.flags & LX_WINDOW_FLAG_SHOW_FPS) {
            if (!window->fps_time) window->fps_time = time;
            else window->fps_count++;
            if (time > window->fps_time + 1000) {
                lx_float_t framerate = (lx_float_t)(window->fps_count * 1000) / (lx_float_t)(time - window->fps_time);
                lx_char_t title[256];
                lx_snprintf(title, sizeof(title), "%s (%0.2f fps)", window->base.title, framerate);
                glfwSetWindowTitle(window->window, title);
                window->fps_count = 0;
                window->fps_time = time;
            }
        }
    }

    // exit glfw
    glfwTerminate();
}

static lx_void_t lx_window_glfw_show(lx_window_ref_t self, lx_bool_t is_show) {
    lx_window_glfw_t* window = (lx_window_glfw_t*)self;
    if (window && window->window) {
        if (is_show) {
            glfwShowWindow(window->window);
        } else {
            glfwHideWindow(window->window);
        }
    }
}

static lx_void_t lx_window_glfw_show_cursor(lx_window_ref_t self, lx_bool_t is_show) {
    lx_window_glfw_t* window = (lx_window_glfw_t*)self;
    if (window && window->window) {
        glfwSetInputMode(window->window, GLFW_CURSOR, is_show? GLFW_CURSOR_NORMAL : GLFW_CURSOR_HIDDEN);
    }
}

static lx_void_t lx_window_glfw_quit(lx_window_ref_t self) {
    lx_window_glfw_t* window = (lx_window_glfw_t*)self;
    if (window) {
        window->is_quit = lx_true;
    }
}

static lx_void_t lx_window_glfw_exit(lx_window_ref_t self) {
    lx_window_glfw_t* window = (lx_window_glfw_t*)self;
    if (window) {
#ifdef LX_CONFIG_DEVICE_HAVE_VULKAN
#   ifdef LX_DEBUG
        if (window->debug_messenger) {
            lx_vk_debug_messenger_cancel(window->instance, window->debug_messenger);
            window->debug_messenger = 0;
        }
#   endif
        if (window->surface) {
            vkDestroySurfaceKHR(window->instance, window->surface, lx_null);
            window->surface = lx_null;
        }
        if (window->instance) {
            vkDestroyInstance(window->instance, lx_null);
            window->instance = lx_null;
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

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
lx_window_ref_t lx_window_init_glfw(lx_size_t width, lx_size_t height, lx_char_t const* title, lx_cpointer_t devdata) {
    lx_bool_t         ok = lx_false;
    lx_window_glfw_t* window = lx_null;
    do {

        // check
        lx_assert_and_check_break(width && height);

        // init window
        window = lx_malloc0_type(lx_window_glfw_t);
        lx_assert_and_check_break(window);

        window->base.fps         = 60;
        window->base.width       = (lx_uint16_t)width;
        window->base.height      = (lx_uint16_t)height;
        window->base.title       = title;
        window->base.runloop     = lx_window_glfw_runloop;
        window->base.quit        = lx_window_glfw_quit;
        window->base.fullscreen  = lx_window_glfw_fullscreen;
        window->base.show        = lx_window_glfw_show;
        window->base.show_cursor = lx_window_glfw_show_cursor;
        window->base.exit        = lx_window_glfw_exit;
        window->fps_delay        = 1000 / window->base.fps;

        // init pixfmt
        window->base.pixfmt = lx_quality() < LX_QUALITY_TOP? LX_PIXFMT_RGB565 : (LX_PIXFMT_RGBX8888 | LX_PIXFMT_BENDIAN);

        // ok
        ok = lx_true;

    } while (0);

    if (!ok && window) {
        lx_window_exit((lx_window_ref_t)window);
        window = lx_null;
    }
    return (lx_window_ref_t)window;
}
