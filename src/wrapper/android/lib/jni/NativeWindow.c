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
 * @file        NativeWindow.c
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "lanox2d/lanox2d.h"
#include <jni.h>
#ifdef LX_CONFIG_DEVICE_HAVE_VULKAN
#   include <android/native_window.h>
#   include <android/native_window_jni.h>
#endif

/* //////////////////////////////////////////////////////////////////////////////////////
 * declaration
 */
lx_void_t lx_jni_javavm_set(JavaVM* jvm, lx_int_t jversion);

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */
JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* jvm, lx_pointer_t reserved) {
    lx_jni_javavm_set(jvm, JNI_VERSION_1_4);
    return JNI_VERSION_1_4;
}

JNIEXPORT jlong Java_io_lanox2d_lib_NativeWindow_window_1init(JNIEnv* env, jclass jthis, jint width, jint height, jobject surface) {
#ifdef LX_CONFIG_DEVICE_HAVE_VULKAN
    ANativeWindow* window = lx_null;
    if (surface) {
        window = ANativeWindow_fromSurface(env, surface);
    }
#else
    lx_pointer_t window = lx_null;
#endif
    return (jlong)lx_window_init(width, height, lx_null, window);
}

JNIEXPORT lx_void_t Java_io_lanox2d_lib_NativeWindow_window_1exit(JNIEnv* env, jclass jthis, jlong window) {
    if (window) {
        lx_window_exit((lx_window_ref_t)window);
    }
}

JNIEXPORT lx_void_t Java_io_lanox2d_lib_NativeWindow_window_1draw(JNIEnv* env, jclass jthis, jlong window) {
    if (window) {
        lx_window_draw((lx_window_ref_t)window);
    }
}

JNIEXPORT lx_void_t Java_io_lanox2d_lib_NativeWindow_window_1resize(JNIEnv* env, jclass jthis, jlong window, jint width, jint height) {
    if (window) {
        lx_window_resize((lx_window_ref_t)window, width, height);
    }
}

JNIEXPORT lx_void_t Java_io_lanox2d_lib_NativeWindow_window_1touchMove(JNIEnv* env, jclass jthis, jlong window, jfloat x, jfloat y) {
    if (window) {
        lx_event_t event = {0};
        lx_touch_t touches[1];
        event.type            = LX_EVENT_TYPE_TOUCH;
        event.u.touch.code    = LX_TOUCH_MOVED;
        event.u.touch.count   = 1;
        event.u.touch.touches = touches;
        lx_point_make(&touches[0].start, x, y);
        lx_point_make(&touches[0].prev,  x, y);
        lx_point_make(&touches[0].point, x, y);
        lx_window_notify((lx_window_ref_t)window, &event);
    }
}
