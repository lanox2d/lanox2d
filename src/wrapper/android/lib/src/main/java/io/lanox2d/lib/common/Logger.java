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
 * @file        Logger.java
 *
 */
package io.lanox2d.lib.common;

import android.util.Log;

public class Logger {

    private static ILogger logger;
    private final static String TAG = "lanox2d";

    public final static int LEVEL_DEBUG = 3;
    public final static int LEVEL_INFO = 2;
    public final static int LEVEL_ERROR = 1;
    private static int level = LEVEL_INFO;

    public interface ILogger {
        void i(String tag, String msg);
        void d(String tag, String msg);
        void e(String tag, String msg);
    }

    public static class DefaultLogger implements ILogger {

        @Override
        public void i(String tag, String msg) {
            Log.i(TAG, tag != null ? "[" + tag + "]: " + msg : msg);
        }

        @Override
        public void d(String tag, String msg) {
            if (level >= LEVEL_DEBUG) {
                Log.d(TAG, tag != null ? "[" + tag + "]: " + msg : msg);
            }
        }

        @Override
        public void e(String tag, String msg) {
            if (level >= LEVEL_ERROR) {
                Log.e(TAG, tag != null ? "[" + tag + "]: " + msg : msg);
            }
        }
    }

    public static void setLevel(int level_) {
        level = level_;
    }

    public static void init(ILogger logger_) {
        logger = logger_;
    }

    public static void i(String msg) {
        i(null, msg);
    }

    public static void d(String msg) {
        d(null, msg);
    }

    public static void e(String msg) {
        e(null, msg);
    }

    public static void e(String msg, Throwable e) {
        e(null, msg, e);
    }

    public static void i(String tag, String msg) {
        if (logger != null) {
            logger.i(tag, msg);
        }
    }

    public static void d(String tag, String msg) {
        if (logger != null) {
            logger.d(tag, msg);
        }
    }

    public static void e(String tag, String msg) {
        if (logger != null) {
            logger.e(tag, msg);
        }
    }

    public static void e(String tag, String msg, Throwable e) {
        e(tag, msg + (e != null ? e.getMessage() : ""));
    }
}
