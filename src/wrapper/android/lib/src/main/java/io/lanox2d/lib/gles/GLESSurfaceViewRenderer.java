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
 * @file        GLESSurfaceViewRenderer.java
 */
package io.lanox2d.lib.gles;

import android.opengl.GLSurfaceView;

import javax.microedition.khronos.opengles.GL10;
import javax.microedition.khronos.egl.EGLConfig;

import io.lanox2d.lib.NativeWindow;

public class GLESSurfaceViewRenderer implements GLSurfaceView.Renderer {
    private static final String TAG = "Lanox2dGLViewRenderer";
    private boolean started = false;
    private GLESSurfaceView view;
    private NativeWindow nativeWindow;

    public GLESSurfaceViewRenderer(GLESSurfaceView view)  {
        this.view = view;
        this.nativeWindow = NativeWindow.getInstance();
    }

    @Override
    public void onSurfaceCreated(GL10 gl, EGLConfig eglConfig) {
        if (nativeWindow.initWindow(view.getWidth(), view.getHeight(), null)) {
            started = true;
        }
    }

    @Override
    public void onDrawFrame(GL10 gl) {
        if (started) {
            nativeWindow.drawWindow();
        }
    }

    @Override
    public void onSurfaceChanged(GL10 gl, int width, int height) {
        if (started) {
            nativeWindow.resizeWindow(view.getWidth(), view.getHeight());
        }
    }

}
