package io.lanox2d.example;

import android.content.Context;

import io.lanox2d.lib.NativeWindow;
import io.lanox2d.lib.common.Logger;
import io.lanox2d.lib.internal.Lanox2dInternal;

public class NativeTest {
    private static final String TAG = "NativeTest";
    private boolean loaded = false;

    // the singleton holder
    private static class NativeTestHolder {
        private static final NativeTest instance = new NativeTest();
    }

    // get instance
    public static NativeTest getInstance() {
        return NativeTest.NativeTestHolder.instance;
    }

    public NativeTest() {
        load();
    }

    public void loadEmptyWindow() {
        load_emptyWindow(NativeWindow.getInstance().getNativeWindowPtr());
    }

    public void loadShapeWindow(String testName) {
        load_shapeWindow(NativeWindow.getInstance().getNativeWindowPtr(), testName);
    }

    // load the native library
    private boolean load() {

        // is loaded?
        if (loaded) return true;

        // attempt to load library
        try {
            System.loadLibrary("test_jni");
            loaded = true;
        } catch (Throwable e) {
            e.printStackTrace();
            Context context = Lanox2dInternal.getInstance().getContext();
            if (context != null) {
                try {
                    System.load(context.getFilesDir().getParent() + "/lib/test_jni.so");
                } catch (Throwable ex) {
                    ex.printStackTrace();
                }
            }
        }
        Logger.i(TAG, String.format("load %s", loaded? "ok" : "failed"));
        return loaded;
    }

    public static native void load_emptyWindow(long nativeWindowPtr);
    public static native void load_shapeWindow(long nativeWindowPtr, String testName);
}
