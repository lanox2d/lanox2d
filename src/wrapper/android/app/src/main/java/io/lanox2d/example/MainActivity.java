package io.lanox2d.example;

import androidx.appcompat.app.AppCompatActivity;

import android.os.Bundle;
import android.os.Handler;
import android.widget.TextView;

import java.io.File;
import java.io.FileOutputStream;
import java.io.InputStream;

import io.lanox2d.lib.NativeWindow;
import io.lanox2d.lib.vulkan.VkSurfaceView;

public class MainActivity extends AppCompatActivity implements Runnable {

    private TextView        infoView;
    private VkSurfaceView   surfaceView;
    private Handler         handler = new Handler();
    private float           fps = 0;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        infoView = (TextView)this.findViewById(R.id.InfoView);
        surfaceView = (VkSurfaceView)this.findViewById(R.id.Lanox2dView);

        final String imagePath = Utils.getAssetsFile(this.getApplicationContext(), "test.png").getAbsolutePath();
        NativeWindow.getInstance().setNativeWindowListener(new NativeWindow.NativeWindowListener() {

            private long fps_time = 0;
            private long fps_count = 0;

            @Override
            public void onInitWindow(int width, int height) {
                // load tests
                //NativeTest.getInstance().loadEmptyWindow();
                NativeTest.getInstance().loadShapeWindow("path");
                //NativeTest.getInstance().loadShapeWindow("path", imagePath);
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

    @Override
    public void onPause() {
        surfaceView.onPause();
        super.onPause();
    }

    @Override
    public void onResume() {
        surfaceView.onResume();
        super.onResume();
    }

    @Override
    public void onDestroy() {
        surfaceView.onDestroy();
        super.onDestroy();
    }
}
