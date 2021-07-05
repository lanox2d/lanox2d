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
 * @file        glut.c
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#ifdef LX_CONFIG_OS_MACOSX
#   include <GLUT/glut.h>
#else
#   include <GL/freeglut.h>
#endif

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

// the glut window type
typedef struct lx_window_glut_t_ {
    lx_window_t     base;
    lx_bool_t       is_quit;
    lx_int_t        id;
    lx_size_t       button;
    lx_uint16_t     normal_width;
    lx_uint16_t     normal_height;
    lx_hong_t       fps_drawtime;
    lx_hong_t       fps_time;
    lx_hong_t       fps_count;
} lx_window_glut_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * globals
 */

// the windows
static lx_window_glut_t*   g_windows[16] = {lx_null};

/* //////////////////////////////////////////////////////////////////////////////////////
 * private implementation
 */
static lx_inline lx_window_glut_t* lx_window_glut_get() {
    lx_int_t id = glutGetWindow();
    return (id < lx_arrayn(g_windows))? g_windows[id] : lx_null;
}

static lx_void_t lx_window_glut_display() {
    lx_window_glut_t* window = lx_window_glut_get();
    lx_assert(window && window->base.canvas && window->base.on_draw);

    // draw
    lx_hong_t starttime = lx_mclock();
    window->base.on_draw((lx_window_ref_t)window, window->base.canvas);

    // flush
    glutSwapBuffers();
    window->fps_drawtime = lx_mclock() - starttime;
}

static lx_void_t lx_window_glut_quit(lx_window_ref_t self) {
    lx_window_glut_t* window = (lx_window_glut_t*)self;
    if (window) {
        window->is_quit = lx_true;
#ifdef __FREEGLUT_EXT_H__
        glutLeaveMainLoop();
#endif
    }
}

#if defined(LX_CONFIG_OS_MACOSX) || defined(__FREEGLUT_EXT_H__)
static lx_void_t lx_window_glut_close() {
    lx_window_glut_t* window = lx_window_glut_get();
    if (window) {
        lx_window_glut_quit((lx_window_ref_t)window);
    }
}
#endif

static lx_void_t lx_window_glut_keyboard(lx_byte_t key, lx_int_t x, lx_int_t y) {
    lx_window_glut_t* window = lx_window_glut_get();
    lx_assert_and_check_return(window);

    lx_event_t event = {0};
    event.type               = LX_EVENT_TYPE_KEYBOARD;
    event.u.keyboard.code    = key;
    event.u.keyboard.pressed = lx_true;
    if (window->base.on_event) {
        window->base.on_event((lx_window_ref_t)window, &event);
    }
}

static lx_void_t lx_window_glut_keyboard_up(lx_byte_t key, lx_int_t x, lx_int_t y) {
    lx_window_glut_t* window = lx_window_glut_get();
    lx_assert_and_check_return(window);

    lx_event_t event = {0};
    event.type               = LX_EVENT_TYPE_KEYBOARD;
    event.u.keyboard.code    = key;
    event.u.keyboard.pressed = lx_false;
    if (window->base.on_event) {
        window->base.on_event((lx_window_ref_t)window, &event);
    }
}

static lx_uint16_t lx_window_glut_special_map(lx_int_t key) {
    lx_uint16_t code = LX_KEY_NUL;
    switch (key) {
    case GLUT_KEY_F1:           code = LX_KEY_F1;           break;
    case GLUT_KEY_F2:           code = LX_KEY_F2;           break;
    case GLUT_KEY_F3:           code = LX_KEY_F3;           break;
    case GLUT_KEY_F4:           code = LX_KEY_F4;           break;
    case GLUT_KEY_F5:           code = LX_KEY_F5;           break;
    case GLUT_KEY_F6:           code = LX_KEY_F6;           break;
    case GLUT_KEY_F7:           code = LX_KEY_F7;           break;
    case GLUT_KEY_F8:           code = LX_KEY_F8;           break;
    case GLUT_KEY_F9:           code = LX_KEY_F9;           break;
    case GLUT_KEY_F10:          code = LX_KEY_F10;          break;
    case GLUT_KEY_F11:          code = LX_KEY_F11;          break;
    case GLUT_KEY_F12:          code = LX_KEY_F12;          break;

    case GLUT_KEY_LEFT:         code = LX_KEY_LEFT;         break;
    case GLUT_KEY_UP:           code = LX_KEY_UP;           break;
    case GLUT_KEY_RIGHT:        code = LX_KEY_RIGHT;        break;
    case GLUT_KEY_DOWN:         code = LX_KEY_DOWN;         break;

    case GLUT_KEY_HOME:         code = LX_KEY_HOME;         break;
    case GLUT_KEY_END:          code = LX_KEY_END;          break;
    case GLUT_KEY_INSERT:       code = LX_KEY_INSERT;       break;
    case GLUT_KEY_PAGE_UP:      code = LX_KEY_PAGEUP;       break;
    case GLUT_KEY_PAGE_DOWN:    code = LX_KEY_PAGEDOWN;     break;

    case 0x19:                  code = LX_KEY_TABBACK;      break;

    default:
        lx_trace_e("unknown key: %d", key);
        break;
    }
    return code;
}

static lx_void_t lx_window_glut_special(lx_int_t key, lx_int_t x, lx_int_t y) {
    lx_window_glut_t* window = lx_window_glut_get();
    lx_assert_and_check_return(window);

    // init event
    lx_event_t event = {0};
    event.type               = LX_EVENT_TYPE_KEYBOARD;
    event.u.keyboard.code    = lx_window_glut_special_map(key);
    event.u.keyboard.pressed = lx_true;
    if (window->base.on_event) {
        window->base.on_event((lx_window_ref_t)window, &event);
    }
}

static lx_void_t lx_window_glut_special_up(lx_int_t key, lx_int_t x, lx_int_t y) {
    lx_window_glut_t* window = lx_window_glut_get();
    lx_assert_and_check_return(window);

    lx_event_t event = {0};
    event.type               = LX_EVENT_TYPE_KEYBOARD;
    event.u.keyboard.code    = lx_window_glut_special_map(key);
    event.u.keyboard.pressed = lx_false;
    if (window->base.on_event) {
        window->base.on_event((lx_window_ref_t)window, &event);
    }
}

static lx_void_t lx_window_glut_mouse(lx_int_t button, lx_int_t state, lx_int_t x, lx_int_t y) {
    lx_window_glut_t* window = lx_window_glut_get();
    lx_assert_and_check_return(window);

    lx_event_t event = {0};
    lx_point_imake(&event.u.mouse.cursor, x, y);
    if (button == GLUT_LEFT_BUTTON)  {
        event.type              = LX_EVENT_TYPE_MOUSE;
        event.u.mouse.button    = LX_MOUSE_BUTTON_LEFT;
        event.u.mouse.code      = (state == GLUT_DOWN)? LX_MOUSE_DOWN : LX_MOUSE_UP;
        window->button            = (state == GLUT_DOWN)? LX_MOUSE_BUTTON_LEFT : LX_MOUSE_BUTTON_NONE;
    } else if (button == GLUT_RIGHT_BUTTON) {
        event.type              = LX_EVENT_TYPE_MOUSE;
        event.u.mouse.button    = LX_MOUSE_BUTTON_RIGHT;
        event.u.mouse.code      = (state == GLUT_DOWN)? LX_MOUSE_DOWN : LX_MOUSE_UP;
        window->button            = (state == GLUT_DOWN)? LX_MOUSE_BUTTON_RIGHT : LX_MOUSE_BUTTON_NONE;
    } else if (button == GLUT_MIDDLE_BUTTON) {
        event.type              = LX_EVENT_TYPE_MOUSE;
        event.u.mouse.button    = LX_MOUSE_BUTTON_MIDDLE;
        event.u.mouse.code      = (state == GLUT_DOWN)? LX_MOUSE_DOWN : LX_MOUSE_UP;
        window->button            = (state == GLUT_DOWN)? LX_MOUSE_BUTTON_MIDDLE : LX_MOUSE_BUTTON_NONE;
    } else if ((button == 3 || button == 4) && (state == GLUT_DOWN)) {
        event.type              = LX_EVENT_TYPE_MOUSE;
        event.u.mouse.button    = LX_MOUSE_BUTTON_MIDDLE;
        event.u.mouse.code      = LX_MOUSE_SCROLL;
        lx_point_imake(&event.u.mouse.scroll, x, button == 3? y - 10 : y + 10);
        window->button            = LX_MOUSE_BUTTON_NONE;
    } else {
        window->button            = LX_MOUSE_BUTTON_NONE;
    }
    if (window->base.on_event) {
        window->base.on_event((lx_window_ref_t)window, &event);
    }
}

static lx_void_t lx_window_glut_motion(lx_int_t x, lx_int_t y) {
    lx_window_glut_t* window = lx_window_glut_get();
    lx_assert_and_check_return(window);

    lx_event_t event = {0};
    event.type           = LX_EVENT_TYPE_MOUSE;
    event.u.mouse.code   = LX_MOUSE_MOVE;
    event.u.mouse.button = window->button;
    lx_point_imake(&event.u.mouse.cursor, x, y);
    if (window->base.on_event) {
        window->base.on_event((lx_window_ref_t)window, &event);
    }
}

static lx_void_t lx_window_glut_visibility(lx_int_t state) {
    lx_window_glut_t* window = lx_window_glut_get();
    lx_assert_and_check_return(window && window->base.canvas);

    lx_event_t event = {0};
    event.type          = LX_EVENT_TYPE_ACTIVE;
    event.u.active.code = state == GLUT_VISIBLE? LX_ACTIVE_FOREGROUND : LX_ACTIVE_BACKGROUND;
    if (window->base.on_event) {
        window->base.on_event((lx_window_ref_t)window, &event);
    }
}

static lx_void_t lx_window_glut_timer(lx_int_t value) {
    lx_window_glut_t* window = g_windows[value];
    lx_assert_and_check_return(window);

    // post to draw it
    glutPostRedisplay();

    // compute delay for framerate
    lx_int_t  delay = 1;
    lx_hong_t time = lx_mclock();
    lx_int_t  dt = (lx_int_t)window->fps_drawtime;
    lx_int_t  fps_delay = 1000 / window->base.fps;
    if (fps_delay >= dt) {
        delay = fps_delay - dt;
    }

    // compute framerate
    if (window->base.flags & LX_WINDOW_FLAG_SHOW_FPS) {
        if (!window->fps_time) window->fps_time = time;
        window->fps_count++;
        if (time > window->fps_time + 1000) {
            lx_float_t framerate = (lx_float_t)(window->fps_count * 1000) / (lx_float_t)(time - window->fps_time);
            lx_char_t title[256];
            lx_snprintf(title, sizeof(title), "%s (%0.2f fps)", window->base.title, framerate);
            glutSetWindowTitle(title);
            window->fps_count = 0;
            window->fps_time = time;
        }
    }

    // next timer
    glutTimerFunc(delay, lx_window_glut_timer, value);
}

static lx_void_t lx_window_glut_reshape(lx_int_t width, lx_int_t height) {
    lx_window_glut_t* window = lx_window_glut_get();
    lx_assert_and_check_return(window);
    lx_check_return(width && height);

#if 0
    // the device
    lx_device_ref_t device = lx_canvas_device(window->base.canvas);
    lx_assert_and_check_return(device);

    // update the window width and height
    window->base.width   = width;
    window->base.height  = height;

    // resize the device
    lx_device_resize(device, width, height);
#endif

    lx_event_t event = {0};
    event.type = LX_EVENT_TYPE_ACTIVE;
    event.u.active.code = LX_ACTIVE_RESIZE_WINDOW;
    event.u.active.data[0] = (lx_size_t)width;
    event.u.active.data[1] = (lx_size_t)height;
    if (window->base.on_event) {
        window->base.on_event((lx_window_ref_t)window, &event);
    }
}

static lx_void_t lx_window_glut_fullscreen(lx_window_ref_t self, lx_bool_t is_fullscreen) {
    lx_window_glut_t* window = (lx_window_glut_t*)self;
    if (window) {
        if (is_fullscreen && !(window->base.flags & LX_WINDOW_FLAG_FULLSCREEN)) {
            window->normal_width  = window->base.width;
            window->normal_height = window->base.height;
            glutFullScreen();
            window->base.flags |= LX_WINDOW_FLAG_FULLSCREEN;
        } else if (window->base.flags & LX_WINDOW_FLAG_FULLSCREEN) {
            lx_size_t screen_width  = glutGet(GLUT_SCREEN_WIDTH);
            lx_size_t screen_height = glutGet(GLUT_SCREEN_HEIGHT);
            lx_assert(screen_width && screen_height && screen_width >= window->normal_width && screen_height >= window->normal_height);
            glutPositionWindow((screen_width - window->normal_width) >> 1, (screen_height - window->normal_height) >> 1);
            glutReshapeWindow(window->normal_width, window->normal_height);
            window->base.flags &= ~LX_WINDOW_FLAG_FULLSCREEN;
        }
    }
}

static lx_bool_t lx_window_glut_start(lx_window_glut_t* window) {
    lx_bool_t ok = lx_false;
    do {

        // init glut
        lx_int_t    argc = 1;
        lx_char_t*  argv[] = {"", lx_null};
        glutInit(&argc, argv);
        glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_STENCIL | GLUT_MULTISAMPLE);
        glutInitWindowPosition(0, 0);
        glutInitWindowSize(window->base.width, window->base.height);

        // init window
        window->id = glutCreateWindow(window->base.title? window->base.title : "lanox2d (GLUT)");
        lx_assert_and_check_break(window->id && window->id < lx_arrayn(g_windows));

        // save window
        g_windows[window->id] = window;

        // init window func
        glutDisplayFunc(lx_window_glut_display);
        glutReshapeFunc(lx_window_glut_reshape);
        glutKeyboardFunc(lx_window_glut_keyboard);
        glutSpecialFunc(lx_window_glut_special);
        glutKeyboardUpFunc(lx_window_glut_keyboard_up);
        glutSpecialUpFunc(lx_window_glut_special_up);
        glutMouseFunc(lx_window_glut_mouse);
        glutMotionFunc(lx_window_glut_motion);
        glutPassiveMotionFunc(lx_window_glut_motion);
        glutTimerFunc(1000 / window->base.fps, lx_window_glut_timer, window->id);
        glutVisibilityFunc(lx_window_glut_visibility);
#if defined(LX_CONFIG_OS_MACOSX) || defined(__FREEGLUT_EXT_H__)
        glutWMCloseFunc(lx_window_glut_close);
#endif

        // hide window cursor
        if (window->base.flags & LX_WINDOW_FLAG_HIDE_CURSOR) {
            glutSetCursor(GLUT_CURSOR_NONE);
        }

        // fullscreen
        if (window->base.flags & LX_WINDOW_FLAG_FULLSCREEN) {
            lx_window_glut_fullscreen((lx_window_ref_t)window, lx_true);
        }

        // init device
        window->base.device = lx_device_init_from_opengl((lx_window_ref_t)window);
        lx_assert_and_check_break(window->base.device);

        // init canvas
        window->base.canvas = lx_canvas_init(window->base.device);
        lx_assert_and_check_break(window->base.canvas);

        // ok
        ok = lx_true;
    } while (0);
    return ok;
}

static lx_void_t lx_window_glut_runloop(lx_window_ref_t self) {
    lx_window_glut_t* window = (lx_window_glut_t*)self;
    lx_assert_and_check_return(window);

    // start glut window
    if (!lx_window_glut_start(window)) {
        lx_trace_e("start glut window failed!");
        return ;
    }

    // run loop
#ifdef LX_CONFIG_OS_MACOSX
    while (!window->is_quit) {
        glutCheckLoop();
    }
#else
    glutMainLoop();
#endif
}

static lx_void_t lx_window_glut_show(lx_window_ref_t self, lx_bool_t is_show) {
    lx_window_glut_t* window = (lx_window_glut_t*)self;
    if (window) {
    }
}

static lx_void_t lx_window_glut_show_cursor(lx_window_ref_t self, lx_bool_t is_show) {
    glutSetCursor(is_show? GLUT_CURSOR_INHERIT : GLUT_CURSOR_NONE);
}

static lx_void_t lx_window_glut_exit(lx_window_ref_t self) {
    lx_window_glut_t* window = (lx_window_glut_t*)self;
    if (window) {
        if (window->base.canvas) {
            lx_canvas_exit(window->base.canvas);
            window->base.canvas = lx_null;
        }
        if (window->base.device) {
            lx_device_exit(window->base.device);
            window->base.device = lx_null;
        }
        if (window->id) {
            glutDestroyWindow(window->id);
            g_windows[window->id] = lx_null;
            window->id = 0;
        }
        lx_free(window);
    }
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
lx_window_ref_t lx_window_init_glut(lx_size_t width, lx_size_t height, lx_char_t const* title) {
    lx_bool_t         ok = lx_false;
    lx_window_glut_t* window = lx_null;
    do {

        // check
        lx_assert_and_check_break(width && height);

        // init window
        window = lx_malloc0_type(lx_window_glut_t);
        lx_assert_and_check_break(window);

        window->base.fps         = 60;
        window->base.width       = (lx_uint16_t)width;
        window->base.height      = (lx_uint16_t)height;
        window->base.title       = title;
        window->base.runloop     = lx_window_glut_runloop;
        window->base.quit        = lx_window_glut_quit;
        window->base.fullscreen  = lx_window_glut_fullscreen;
        window->base.show        = lx_window_glut_show;
        window->base.show_cursor = lx_window_glut_show_cursor;
        window->base.exit        = lx_window_glut_exit;

        // init pixfmt
        window->base.pixfmt = lx_quality() < LX_QUALITY_TOP? LX_PIXFMT_RGB565 : LX_PIXFMT_XRGB8888;

        // ok
        ok = lx_true;

    } while (0);

    if (!ok && window) {
        lx_window_exit((lx_window_ref_t)window);
        window = lx_null;
    }
    return (lx_window_ref_t)window;
}
