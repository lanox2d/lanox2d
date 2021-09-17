package io.lanox2d.example;

import androidx.appcompat.app.AppCompatActivity;

import android.os.Bundle;
import android.os.Handler;
import android.widget.TextView;

import io.lanox2d.lib.Lanox2d;
import io.lanox2d.lib.Lanox2dView;
import io.lanox2d.lib.NativeWindow;
import io.lanox2d.lib.common.Logger;

public class MainActivity extends AppCompatActivity implements Runnable {

    private TextView    infoView;
    private Lanox2dView lanox2dView;
    private Handler     handler = new Handler();
    private float       fps = 0;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        infoView = (TextView)this.findViewById(R.id.InfoView);
        lanox2dView = (Lanox2dView)this.findViewById(R.id.Lanox2dView);

        NativeWindow.getInstance().setNativeWindowListener(new NativeWindow.NativeWindowListener() {

            private long fps_time = 0;
            private long fps_count = 0;

            @Override
            public void onInitWindow(int width, int height) {
                // load tests
                //NativeTest.getInstance().loadEmptyWindow();
                NativeTest.getInstance().loadShapeWindow("path");
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
}
