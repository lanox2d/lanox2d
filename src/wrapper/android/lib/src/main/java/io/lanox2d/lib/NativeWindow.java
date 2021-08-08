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
 * @file        NativeWindow.java
 */
package io.lanox2d.lib;

import android.content.Context;
import android.opengl.GLSurfaceView;

import javax.microedition.khronos.opengles.GL10;
import javax.microedition.khronos.egl.EGLConfig;

import io.lanox2d.lib.common.Logger;
import io.lanox2d.lib.internal.Lanox2dInternal;

public class NativeWindow {
    private static final String TAG = "NativeWindow";
    private boolean loaded = false;

    // the singleton holder
    private static class NativeWindowHolder {
        private static final NativeWindow instance = new NativeWindow();
    }

    // get instance
    public static NativeWindow getInstance() {
        return NativeWindow.NativeWindowHolder.instance;
    }

    public NativeWindow() {
        load();
    }

    // load the native library
    private boolean load() {

        // is loaded?
        if (loaded) return true;

        // attempt to load library
        try {
            System.loadLibrary("lanox2d");
            loaded = true;
        } catch (Throwable e) {
            Context context = Lanox2dInternal.getInstance().getContext();
            if (context == null) {
                e.printStackTrace();
            } else {
                try {
                    System.load(context.getFilesDir().getParent() + "/lib/lanox2d.so");
                } catch (Throwable ex) {
                    ex.printStackTrace();
                }
            }
        }
        Logger.i(TAG, String.format("load %s", loaded? "ok" : "failed"));
        return loaded;
    }

    public boolean initWindow(int width, int height) {
        if (!loaded) {
            return false;
        }
        try {
            return window_init(width, height);
        } catch (Throwable e) {
            e.printStackTrace();
        }
        return false;
    }

    public void exitWindow() {
        try {
            if (loaded) {
                window_exit();
            }
        } catch (Throwable e) {
            e.printStackTrace();
        }
    }

    public void drawWindow() {
        try {
            if (loaded) {
                window_draw();
            }
        } catch (Throwable e) {
            e.printStackTrace();
        }
    }

    public void resizeWindow(int width, int height) {
        try {
            if (loaded) {
                window_resize(width, height);
            }
        } catch (Throwable e) {
            e.printStackTrace();
        }
    }

    private static native boolean window_init(int width, int height);
    private static native void    window_exit();
    private static native void    window_draw();
    private static native void    window_resize(int width, int height);
}
