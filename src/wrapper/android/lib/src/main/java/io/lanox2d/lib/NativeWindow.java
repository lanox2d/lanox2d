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
import android.view.Surface;
import android.view.SurfaceControl;

import javax.microedition.khronos.opengles.GL10;
import javax.microedition.khronos.egl.EGLConfig;

import io.lanox2d.lib.common.Logger;
import io.lanox2d.lib.internal.Lanox2dInternal;

public class NativeWindow {
    private static final String TAG = "NativeWindow";
    private boolean loaded = false;
    private long nativeWindowPtr = 0;
    private NativeWindowListener nativeWindowListener;

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
            System.loadLibrary("lanox2d_jni");
            loaded = true;
        } catch (Throwable e) {
            e.printStackTrace();
            Context context = Lanox2dInternal.getInstance().getContext();
            if (context != null) {
                try {
                    System.load(context.getFilesDir().getParent() + "/lib/lanox2d_jni.so");
                } catch (Throwable ex) {
                    ex.printStackTrace();
                }
            }
        }
        Logger.i(TAG, String.format("load %s", loaded? "ok" : "failed"));
        return loaded;
    }

    public boolean initWindow(int width, int height, Surface surface) {
        if (!loaded) {
            return false;
        }
        try {
            if (nativeWindowPtr == 0) {
                nativeWindowPtr = window_init(width, height, surface);
            }
            if (nativeWindowListener != null) {
                nativeWindowListener.onInitWindow(width, height);
            }
        } catch (Throwable e) {
            e.printStackTrace();
        }
        return nativeWindowPtr != 0;
    }

    public void exitWindow() {
        try {
            if (loaded && nativeWindowPtr != 0) {
                window_exit(nativeWindowPtr);
                nativeWindowPtr = 0;
            }
        } catch (Throwable e) {
            e.printStackTrace();
        }
    }

    public void drawWindow() {
        try {
            if (loaded && nativeWindowPtr != 0) {
                window_draw(nativeWindowPtr);
                if (nativeWindowListener != null) {
                    nativeWindowListener.onDrawWindow();
                }
            }
        } catch (Throwable e) {
            e.printStackTrace();
        }
    }

    public void resizeWindow(int width, int height) {
        try {
            if (loaded && nativeWindowPtr != 0) {
                window_resize(nativeWindowPtr, width, height);
                if (nativeWindowListener != null) {
                    nativeWindowListener.onResizeWindow(width, height);
                }
            }
        } catch (Throwable e) {
            e.printStackTrace();
        }
    }

    public void touchMove(float x, float y) {
        try {
            if (loaded && nativeWindowPtr != 0) {
                window_touchMove(nativeWindowPtr, x, y);
            }
        } catch (Throwable e) {
            e.printStackTrace();
        }
    }

    public long getNativeWindowPtr() {
        return nativeWindowPtr;
    }

    public void setNativeWindowListener(NativeWindowListener nativeWindowListener) {
        this.nativeWindowListener = nativeWindowListener;
    }

    public interface NativeWindowListener {
        public void onInitWindow(int width, int height);
        public void onDrawWindow();
        public void onResizeWindow(int width, int height);
    }

    private static native long window_init(int width, int height, Surface surface);
    private static native void window_exit(long nativeWindowPtr);
    private static native void window_draw(long nativeWindowPtr);
    private static native void window_resize(long nativeWindowPtr, int width, int height);
    private static native void window_touchMove(long nativeWindowPtr, float x, float y);
}
