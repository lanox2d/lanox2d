package io.lanox2d.example;

import androidx.appcompat.app.AppCompatActivity;

import android.os.Bundle;
import android.os.Handler;
import android.widget.TextView;

import java.io.File;
import java.io.FileOutputStream;
import java.io.InputStream;

import io.lanox2d.lib.gles.Lanox2dGLView;
import io.lanox2d.lib.NativeWindow;

public class MainActivity extends AppCompatActivity implements Runnable {

    private TextView        infoView;
    private Lanox2dGLView   lanox2dView;
    private Handler         handler = new Handler();
    private float           fps = 0;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        infoView = (TextView)this.findViewById(R.id.InfoView);
        lanox2dView = (Lanox2dGLView)this.findViewById(R.id.Lanox2dView);

        NativeWindow.getInstance().setNativeWindowListener(new NativeWindow.NativeWindowListener() {

            private long fps_time = 0;
            private long fps_count = 0;

            @Override
            public void onInitWindow(int width, int height) {
                // load tests
                //NativeTest.getInstance().loadEmptyWindow();
                //NativeTest.getInstance().loadShapeWindow("path");
                NativeTest.getInstance().loadShapeWindow("path", getAssetsFile("test.png").getAbsolutePath());
            }

            @Override
            public void onDrawWindow() {

                // compute fps
                long time = System.currentTimeMillis();
                if (fps_time == 0) fps_time = time;
                else fps_count++;
                if (time > fps_time + 1000) {
                    fps = (float)(fps_count * 1000) / (float)(time - fps_time);
                    fps_count = 0;
                    fps_time = time;
                }
            }

            @Override
            public void onResizeWindow(int width, int height) {

            }
        });
        handler.postDelayed(this, 1000);
    }

    public void run() {
        if (infoView != null) {
            infoView.setText(String.format("%f fps", fps));
        }
        handler.postDelayed(this, 1000);
    }

    public File getAssetsFile(String name) {
        try {
            File outputDir = this.getApplicationContext().getCacheDir();
            File outputFile = File.createTempFile("assets", ".tmp", outputDir);
            InputStream is = this.getApplicationContext().getAssets().open(name);
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
