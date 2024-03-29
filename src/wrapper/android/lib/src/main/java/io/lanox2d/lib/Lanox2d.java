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
 * @file        Lanox2d.java
 */
package io.lanox2d.lib;

import android.content.Context;

import io.lanox2d.lib.common.Logger;
import io.lanox2d.lib.internal.Lanox2dInternal;

public class Lanox2d {

    public static void init(Context context) {
        setLogger(new Logger.DefaultLogger());
        Lanox2dInternal.getInstance().init(context);
    }

    public static void setLogger(Logger.ILogger logger) {
        Logger.init(logger);
    }
}
