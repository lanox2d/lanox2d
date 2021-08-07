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
 * @file        Lanox2dView.java
 */
package io.lanox2d.lib;

import android.app.ActivityManager;
import android.content.Context;
import android.content.pm.ConfigurationInfo;
import android.opengl.GLSurfaceView;
import android.util.AttributeSet;

import io.lanox2d.lib.common.Logger;

public class Lanox2dView extends GLSurfaceView {
    private static final String TAG = "Lanox2dView";

    public Lanox2dView(Context context, AttributeSet attrs) {
        super(context, attrs);

        // init lanox2d
        Lanox2d.init(context);

        // init opengl version, gles2.0 or gles3.0?
        ActivityManager am = (ActivityManager) context.getSystemService(Context.ACTIVITY_SERVICE);
        ConfigurationInfo info = am.getDeviceConfigurationInfo();
        Logger.i(TAG, String.format("reqGlEsVersion: %x", info.reqGlEsVersion));
        if (info.reqGlEsVersion >= 0x30000) {
            setEGLContextClientVersion(3);
        } else if (info.reqGlEsVersion >= 0x20000) {
            setEGLContextClientVersion(2);
        }

        // init config chooser, no multisample
        setEGLConfigChooser(8, 8, 8, 8, 16, 4);

        // init render
        setRenderer(new Lanox2dViewRenderer(this));

        // init focus
        requestFocus();
        setFocusableInTouchMode(true);
    }
}
