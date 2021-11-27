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
 * @file        VkSurfaceThread.java
 */
package io.lanox2d.lib.vulkan;

import java.util.concurrent.locks.Condition;
import java.util.concurrent.locks.ReentrantLock;

import io.lanox2d.lib.common.Logger;

public class VkSurfaceThread extends Thread {
    private static final String TAG = "VkSurfaceThread";

    private VkSurfaceRenderer renderer;
    private ReentrantLock lock = new ReentrantLock();
    private Condition lockCondition = lock.newCondition();
    private boolean shouldExit = false;
    private boolean exited = false;

    VkSurfaceThread(VkSurfaceRenderer renderer) {
        this.renderer = renderer;
    }

    private void threadExiting() {
        lock.lock();
        exited = true;
        lockCondition.signalAll();
        lock.unlock();
    }

    public void blockingExit() {
        lock.lock();
        shouldExit = true;
        lockCondition.signalAll();
        while (!exited) {
            try {
                Logger.i(TAG, "Waiting on exit for " + getName());
                lockCondition.await();
            } catch (InterruptedException e) {
                currentThread().interrupt();
            }
        }
        lock.unlock();
    }

    @Override
    public void run() {
        try {
            while (true) {
                lock.lock();
                while (true) {
                    if (shouldExit) {
                       return ;
                    }
                }
                lock.unlock();
            }
        } catch (Throwable e) {
            e.printStackTrace();
        } finally {
            threadExiting();
        }
    }
}
