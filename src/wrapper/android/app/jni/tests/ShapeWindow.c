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
 * @file        ShapeWindow.c
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "lanox2d/lanox2d.h"
#include <jni.h>
#include "../../../../../examples/shape.c"

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */
JNIEXPORT lx_void_t Java_io_lanox2d_example_NativeTest_load_1shapeWindow(JNIEnv* env, jclass jthis, jlong window, jstring testName, jstring imagePath) {
    lx_char_t const* testName_cstr = (*env)->GetStringUTFChars(env, testName, lx_null);
    lx_char_t const* imagePath_cstr = imagePath? (*env)->GetStringUTFChars(env, imagePath, lx_null) : lx_null;
    lx_trace_i("load_shapeWindow: %s %s", testName_cstr, imagePath_cstr);
    if (window) {
        int argc = 2;
        static lx_char_t* argv[] = {"", lx_null, lx_null, lx_null};
        argv[1] = (lx_char_t*)testName_cstr;
        if (imagePath_cstr) {
            argv[2] = (lx_char_t*)imagePath_cstr;
            argc++;
        }
        window_init((lx_window_ref_t)window, argc, argv);
        (lx_void_t)window_exit;
    }
    (*env)->ReleaseStringUTFChars(env, testName, testName_cstr);
    if (imagePath) {
        (*env)->ReleaseStringUTFChars(env, imagePath, imagePath_cstr);
    }
}

