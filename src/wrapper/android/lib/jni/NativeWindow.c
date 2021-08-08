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
 * interfaces
 */
JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* jvm, lx_pointer_t reserved) {
    return JNI_VERSION_1_4;
}

JNIEXPORT jboolean Java_io_lanox2d_lib_NativeWindow_window_1init(JNIEnv* env, jclass jthis, jint width, jint height) {
    lx_trace_i("window_init: %dx%d", width, height);
    return lx_true;
}

JNIEXPORT lx_void_t Java_io_lanox2d_lib_NativeWindow_window_1exit(JNIEnv* env, jclass jthis) {
    lx_trace_i("window_exit");
}

JNIEXPORT lx_void_t Java_io_lanox2d_lib_NativeWindow_window_1draw(JNIEnv* env, jclass jthis) {
    lx_trace_i("window_draw");
}

JNIEXPORT lx_void_t Java_io_lanox2d_lib_NativeWindow_window_1resize(JNIEnv* env, jclass jthis, jint width, jint height) {
    lx_trace_i("window_resize: %dx%d", width, height);
}
