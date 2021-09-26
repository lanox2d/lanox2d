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
 * @file        Lanox2dVkView.java
 */
package io.lanox2d.lib.vulkan;

import android.content.Context;
import android.util.AttributeSet;
import android.view.SurfaceView;

import io.lanox2d.lib.Lanox2d;

public class Lanox2dVkView extends SurfaceView {
    private static final String TAG = "Lanox2dVkView";

    public Lanox2dVkView(Context context, AttributeSet attrs) {
        super(context, attrs);

        // init lanox2d
        Lanox2d.init(context);
    }
}
