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
 * @file        VkSurfaceRenderer.java
 */
package io.lanox2d.lib.vulkan;

import io.lanox2d.lib.NativeWindow;

public class VkSurfaceRenderer {

    private VkSurfaceView surfaceView;
    private NativeWindow nativeWindow;
    private boolean started = false;

    VkSurfaceRenderer(VkSurfaceView surfaceView) {
        this.surfaceView = surfaceView;
        this.nativeWindow = NativeWindow.getInstance();
    }

    public void onPause() {
    }

    public void onResume() {
    }

    public void onSurfaceCreated() {
    }

    public void onSurfaceDestroyed() {
    }

    public void onSurfaceChanged(int width, int height) {
        if (!started && nativeWindow.initWindow(width, height, surfaceView.getHolder().getSurface())) {
            started = true;
        }
    }

    public void onDrawFrame() {
    }

    public void onDestroy() {
    }
}
