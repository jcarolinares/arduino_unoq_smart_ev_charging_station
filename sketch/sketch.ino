// SPDX-FileCopyrightText: Copyright (C) ARDUINO SRL (http://www.arduino.cc)
//
// SPDX-License-Identifier: MPL-2.0
// Created by Julián Caro Linares for Arduino s.r.l using as a base the original example by Arduino "Video Object Detection"


#include <Arduino_LED_Matrix.h>
#include <Arduino_RouterBridge.h>
#include <Arduino_Modulino.h>


Arduino_LED_Matrix matrix;

ModulinoPixels leds;
int brightness = 25;

ModulinoBuzzer buzzer;
int frequency = 440;  // Frequency of the tone in Hz
int duration = 1000;  // Duration of the tone in milliseconds


ModulinoKnob knob;
int position = 0;
bool click = false;
int direction = 0;

ModulinoLatchRelay relay_car;
ModulinoLatchRelay relay_big_vehicle(0x09);


// Global Variables
int counter = 0;

// Frames
uint8_t counter_frame[104] = {
    0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,
};

void setup() {
  pinMode(LED_BUILTIN, OUTPUT); // We initiate the integrated LED to be used as visual Feedback
  
  // UNO Q Matrix Initialization
  matrix.begin();
  matrix.setGrayscaleBits(1);
  matrix.clear();

  // Bridge Initialization
  Serial.begin(9600);
  Bridge.begin();
  Bridge.provide("set_car_charging", set_car_charging);
  Bridge.provide("set_truck_charging", set_truck_charging);

  // Modulinos Inizialitation
  Modulino.begin(Wire1);
  leds.begin();
  buzzer.begin();
  knob.begin();
  relay_car.begin();
  relay_big_vehicle.begin();

  // Matrix Pixels starting as OFF
  for (int i = 0; i < 8; i++) {
      leds.set(i, RED, 0);
      leds.show();
  }

  // Rekay disconnected at the beggining as a safety feature
  relay_car.reset();
  relay_big_vehicle.reset();
  
}

void loop() {
  // Modulino Knob Reading
  position = knob.get();
  click = knob.isPressed();
  direction = knob.getDirection();


  if(click){
    // Serial.println("Clicked!");
    if(counter<104){
       counter++;
       counter_frame[counter-1] = 1; // row, column // This way matrix overloads jumping to next row
      
       // matrix.renderBitmap(counter_frame, 8, 12);
       matrix.draw(counter_frame);
       delay(250); // Quick anti rebound botton control
    }
  }
 else if(direction == 1){
    if(counter<104){
       counter++;
       counter_frame[counter -1] = 1; // row, column // This way matrix overloads jumping to next row
       matrix.draw(counter_frame);
    }
 }  
 else if(direction == -1){
    if(counter>0){
       counter--;
       counter_frame[counter] = 0; // row, column // This way matrix overloads jumping to next row
       matrix.draw(counter_frame);
    }
 }
  
  delay(10); // To control the speed of the reading of the sensors
}

void set_car_charging(bool state) {
    // LOW state means LED is ON
    digitalWrite(LED_BUILTIN, LOW);

    Serial.println("Car Standard Vehicle arduino detected");
  
    // Set all LEDs to RED
    for (int i = 0; i < 8; i++) {
        leds.set(i, RED, brightness);
        leds.show();
    }
    buzzer.tone(frequency, duration);  // Generate the tone
    delay(3000);
    buzzer.tone(0, duration);  // Stop the tone
    for (int i = 0; i < 8; i++) {
        leds.set(i, RED, 0);
        leds.show();
    }
  digitalWrite(LED_BUILTIN, HIGH);
  
  // Activating Charge
  relay_car.set();
  delay(10000); // Time to avoid multiple activations in different frames
}

void set_truck_charging(bool state) {
    // LOW state means LED is ON
    digitalWrite(LED_BUILTIN, LOW);

    Serial.println("Truck Vehicle arduino detected");
  
    // Set all LEDs to WHITE
    for (int i = 0; i < 8; i++) {
        leds.set(i, WHITE, brightness);
        leds.show();
    }
    buzzer.tone(220, duration);  // Generate the tone
    delay(3000);
    buzzer.tone(0, duration);  // Stop the tone
    for (int i = 0; i < 8; i++) {
        leds.set(i, WHITE, 0);
        leds.show();
    }
  digitalWrite(LED_BUILTIN, HIGH);

  // Activating Charge
  relay_big_vehicle.set();
  delay(10000); // Time to avoid multiple activations in different frames
}
