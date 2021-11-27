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

import android.view.SurfaceHolder;

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
    private boolean resumed = false;
    private boolean rendererResumed = false;
    private boolean rendererInitialized = false;
    private boolean hasSurface = false;
    private boolean surfaceChanged = false;
    private int surfaceWidth = 0;
    private int surfaceHeight = 0;

    VkSurfaceThread(VkSurfaceRenderer renderer) {
        this.renderer = renderer;
    }

    private boolean readyToDraw() {
        return hasSurface && resumed;
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

    public void onPause() {
        lock.lock();
        resumed = false;
        lockCondition.signalAll();
        lock.unlock();
    }

    public void onResume() {
        lock.lock();
        resumed = true;
        lockCondition.signalAll();
        lock.unlock();
    }

    public void onSurfaceCreated() {
    }

    public void onSurfaceDestroyed() {
        lock.lock();
        hasSurface = false;
        lockCondition.signalAll();
        lock.unlock();
    }

    public void onSurfaceChanged(int width, int height) {
        lock.lock();
        hasSurface = true;
        surfaceChanged = true;
        surfaceWidth = width;
        surfaceHeight = height;
        lockCondition.signalAll();
        lock.unlock();
    }

    @Override
    public void run() {
        try {
            boolean broken = false;
            while (true) {
                lock.lock();
                while (true) {
                    if (shouldExit) {
                        renderer.onDestroy();
                        broken = true;
                        break;
                    }
                    if (readyToDraw()) {
                        if (!rendererResumed) {
                            rendererResumed = true;
                            renderer.onResume();
                            if (!rendererInitialized) {
                                rendererInitialized = true;
                                renderer.onSurfaceCreated();
                            }
                        }
                        if (surfaceChanged) {
                            renderer.onSurfaceChanged(surfaceWidth, surfaceHeight);
                            surfaceChanged = false;
                        }
                        break;
                    } else if (rendererResumed) {
                        rendererResumed = false;
                        renderer.onPause();
                    }
                    lockCondition.await();
                }
                lock.unlock();
                if (broken) {
                    break;
                }
                renderer.onDrawFrame();
            }
        } catch (Throwable e) {
            e.printStackTrace();
        } finally {
            threadExiting();
        }
    }
}
