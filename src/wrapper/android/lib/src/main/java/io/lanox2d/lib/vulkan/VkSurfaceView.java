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
 * @file        VkSurfaceView.java
 */
package io.lanox2d.lib.vulkan;

import android.content.Context;
import android.util.AttributeSet;
import android.view.SurfaceHolder;
import android.view.SurfaceView;

import io.lanox2d.lib.Lanox2d;
import io.lanox2d.lib.NativeWindow;
import io.lanox2d.lib.common.Logger;

public class VkSurfaceView extends SurfaceView implements SurfaceHolder.Callback {
    private static final String TAG = "Lanox2dVkView";
    private VkSurfaceThread thread;

    public VkSurfaceView(Context context, AttributeSet attrs) {
        super(context, attrs);

        // init lanox2d
        Lanox2d.init(context);

        // bind holder
		this.getHolder().addCallback(this);

        // start renderer
        startRenderer(new VkSurfaceRenderer(this));
    }

    // start renderer
    public void startRenderer(VkSurfaceRenderer renderer) {
        thread = new VkSurfaceThread(renderer);
        thread.start();
    }

    public void onDestroy() {
        if (thread != null) {
            thread.blockingExit();
        }
    }

    public void onPause() {
        if (thread != null) {
            thread.onPause();
        }
    }

    public void onResume() {
        if (thread != null) {
            thread.onResume();
        }
    }

    @Override
    public void surfaceCreated(SurfaceHolder holder) {
        if (thread != null) {
            thread.onSurfaceCreated();
        }
    }

    @Override
    public void surfaceDestroyed(SurfaceHolder holder) {
        if (thread != null) {
            thread.onSurfaceDestroyed();
        }
    }

    @Override
    public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {
        if (thread != null) {
            thread.onSurfaceChanged(width, height);
        }
    }
}
