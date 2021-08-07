package io.lanox2d.example;

import androidx.appcompat.app.AppCompatActivity;

import android.os.Bundle;

import io.lanox2d.lib.Lanox2d;
import io.lanox2d.lib.Lanox2dView;

public class MainActivity extends AppCompatActivity {

    private Lanox2dView lanox2dView = null;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        // init lanox2d view
        lanox2dView = (Lanox2dView)this.findViewById(R.id.Lanox2dView);
    }
}