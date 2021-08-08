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
 * @file        Lanox2dInternal.java
 */
package io.lanox2d.lib.internal;

import android.content.Context;

import io.lanox2d.lib.common.Logger;

public class Lanox2dInternal {
    private static final String TAG = "Lanox2dInternal";

    // basic
    private boolean inited = false;
    private Context context = null;

    // the singleton holder
    private static class Lanox2dInternalHolder {
        private static final Lanox2dInternal instance = new Lanox2dInternal();
    }

    // get instance
    public static Lanox2dInternal getInstance() {
        return Lanox2dInternal.Lanox2dInternalHolder.instance;
    }

    // init lanox2d
    public synchronized void init(Context context_) {
        if (inited) {
            Logger.i(TAG,"have been inited already!");
            return;
        }
        try {
            if (context_ != null) {
                context = context_.getApplicationContext();
            }
            if (context != null) {
                inited = true;
            }
        } catch (Throwable e) {
            context = null;
        }
        Logger.i(TAG,"init " + (inited ? "ok" : "failed"));
    }

    // get context
    public synchronized Context getContext() {
        return context;
    }
}