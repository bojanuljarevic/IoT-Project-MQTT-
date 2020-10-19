package com.bignerdranch.android.homeautomationuserapp;

import androidx.appcompat.app.AppCompatActivity;

import android.graphics.Color;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;

import org.eclipse.paho.client.mqttv3.IMqttDeliveryToken;
import org.eclipse.paho.client.mqttv3.MqttCallback;
import org.eclipse.paho.client.mqttv3.MqttClient;
import org.eclipse.paho.client.mqttv3.MqttException;
import org.eclipse.paho.client.mqttv3.MqttMessage;
import org.eclipse.paho.client.mqttv3.persist.MemoryPersistence;

public class MainActivity extends AppCompatActivity implements MqttCallback {

    private static final String TAG = "Main Activity";

    private Button buttonLight;
    private Button buttonTemp;
    private Button buttonWindow;
    private Button buttonPlant;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        buttonLight = findViewById(R.id.buttonLight);
        buttonTemp = findViewById(R.id.buttonTemp);
        buttonWindow = findViewById(R.id.buttonWindow);
        buttonPlant = findViewById(R.id.buttonPlant);

        final MqttClient client;

        try {
            client = new MqttClient("tcp://192.168.4.1:1883", "UserApp", new MemoryPersistence());
            client.setCallback(this);
            client.connect();
            Log.d(TAG, "Connection successful");


            final boolean[] OnOff = {true, true, true, true};
            final String topicLight = "/on-off/light";
            final String topicTemp = "/on-off/temp";
            final String topicWindow = "/on-off/window";
            final String topicPlant = "/on-off/plant";
            final String offStr = "0";
            final String onStr = "1";

            buttonLight.setOnClickListener(new View.OnClickListener() {
                @Override
                public void onClick(View v) {
                    try {
                        if(OnOff[0] == true) {
                            client.publish(topicLight, new MqttMessage(offStr.getBytes()));
                            OnOff[0] = false;
                            buttonLight.setBackgroundColor(Color.WHITE);
                        } else {
                            client.publish(topicLight, new MqttMessage(onStr.getBytes()));
                            OnOff[0] = true;
                            buttonLight.setBackgroundColor(Color.YELLOW);
                        }
                    } catch (MqttException e) {
                        e.printStackTrace();
                    }
                }
            });

            buttonTemp.setOnClickListener(new View.OnClickListener() {
                @Override
                public void onClick(View v) {
                    try {
                        if(OnOff[1] == true) {
                            client.publish(topicTemp, new MqttMessage(offStr.getBytes()));
                            OnOff[1] = false;
                            buttonTemp.setBackgroundColor(Color.WHITE);
                        } else {
                            client.publish(topicTemp, new MqttMessage(onStr.getBytes()));
                            OnOff[1] = true;
                            buttonTemp.setBackgroundColor(Color.RED);
                        }
                    } catch (MqttException e) {
                        e.printStackTrace();
                    }
                }
            });

            buttonWindow.setOnClickListener(new View.OnClickListener() {
                @Override
                public void onClick(View v) {
                    try {
                        if(OnOff[2] == true) {
                            client.publish(topicWindow, new MqttMessage(offStr.getBytes()));
                            OnOff[2] = false;
                            buttonWindow.setBackgroundColor(Color.WHITE);
                        } else {
                            client.publish(topicWindow, new MqttMessage(onStr.getBytes()));
                            OnOff[2] = true;
                            buttonWindow.setBackgroundColor(Color.BLUE);
                        }
                    } catch (MqttException e) {
                        e.printStackTrace();
                    }
                }
            });

            buttonPlant.setOnClickListener(new View.OnClickListener() {
                @Override
                public void onClick(View v) {
                    try {
                        if(OnOff[3] == true) {
                            client.publish(topicPlant, new MqttMessage(offStr.getBytes()));
                            OnOff[3] = false;
                            buttonPlant.setBackgroundColor(Color.WHITE);
                        } else {
                            client.publish(topicPlant, new MqttMessage(onStr.getBytes()));
                            OnOff[3] = true;
                            buttonPlant.setBackgroundColor(Color.GREEN);
                        }
                    } catch (MqttException e) {
                        e.printStackTrace();
                    }
                }
            });


        } catch (MqttException e) {
            Log.d(TAG, "Connection failed");
          e.printStackTrace();
        }



    }

    @Override
    public void connectionLost(Throwable cause) {
        Log.d(TAG, "connectionLost");
    }

    @Override
    public void messageArrived(String topic, MqttMessage message) throws Exception {
        String payload = new String(message.getPayload());
        Log.d(TAG, payload);
    }

    @Override
    public void deliveryComplete(IMqttDeliveryToken token) {
        Log.d(TAG, "deliveryComplete");
    }

}
