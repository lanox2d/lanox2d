package io.lanox2d.example;

import android.content.Context;

import java.io.File;
import java.io.FileOutputStream;
import java.io.InputStream;

public class Utils {
    public static File getAssetsFile(Context context, String name) {
        try {
            File outputDir = context.getCacheDir();
            File outputFile = File.createTempFile("assets", ".tmp", outputDir);
            InputStream is = context.getAssets().open(name);
            FileOutputStream fos = new FileOutputStream(outputFile);
            byte[] buffer = new byte[8192];
            int byteCount = 0;
            while ((byteCount = is.read(buffer)) != -1) {
                fos.write(buffer, 0, byteCount);
            }
            fos.flush();
            is.close();
            fos.close();
            return outputFile;
        } catch (Throwable e) {
            e.printStackTrace();
        }
        return null;
    }
}
