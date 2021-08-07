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
 * @file        Lanox2dViewRenderer.java
 */
package io.lanox2d.lib;

import android.opengl.GLSurfaceView;

import javax.microedition.khronos.opengles.GL10;
import javax.microedition.khronos.egl.EGLConfig;

import io.lanox2d.lib.common.Logger;

public class Lanox2dViewRenderer implements GLSurfaceView.Renderer {
    private static final String TAG = "Lanox2dViewRenderer";
    private Lanox2dView view;

    public Lanox2dViewRenderer(Lanox2dView view)  {
        this.view = view;
    }

    @Override
    public void onSurfaceCreated(GL10 gl, EGLConfig eglConfig) {

    }

    @Override
    public void onDrawFrame(GL10 gl) {
    }

    @Override
    public void onSurfaceChanged(GL10 gl, int width, int height)
    {
    }
}
