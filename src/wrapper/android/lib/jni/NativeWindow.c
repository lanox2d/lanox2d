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

JNIEXPORT jlong Java_io_lanox2d_lib_NativeWindow_window_1init(JNIEnv* env, jclass jthis, jint width, jint height) {
    return (jlong)lx_window_init(width, height, lx_null);
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
