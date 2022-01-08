#include <SoftwareSerial.h>

// Min voltage from the FSR required to activate the device
// 600/1024 = 3 V
#define MIN_PRESSURE 600

// Create the Bluetooth communication device connected on pins 2 and 3
SoftwareSerial HM10(2, 3);

// Set up variables for managing the button presses
int p8_pressed = 0;
int p7_pressed = 0;
int p6_pressed = 0;

void setup() {
  // Begin the bluetooth communication at 9600 bits/s
  HM10.begin(9600);

  // Digital pins
  pinMode(6, INPUT);
  pinMode(7, INPUT);
  pinMode(8, INPUT);

  // Analog pins
  pinMode(A0, INPUT);
  pinMode(A1, INPUT);
}

void loop() {
  if (analogRead(A1) >= MIN_PRESSURE) { // only activate when fsr is held
    
    if (digitalRead(8)) { // sw1 is held
      if (!p8_pressed) { // sw1 was just pressed this loop iteration
        HM10.print("TEXT 1\n"); // send a "TEXT 1" command across bluetooth
        p8_pressed = 1; // set this to 1 so we don't run this again next loop iteration
      }
    } else {
      p8_pressed = 0;
    }
  
    if (digitalRead(7)) {
      if (!p7_pressed) {
        HM10.print("TEXT 2\n");
        p7_pressed = 1;
      }
    } else {
      p7_pressed = 0;
    }
  
    if (digitalRead(6)) {
      if (!p6_pressed) {
        // read the mic amplitude and send it over bluetooth
        int micval = analogRead(A0);
        HM10.print("VOICE ");
        HM10.print(micval);
        HM10.print("\n");
        p6_pressed = 1;
      }
    } else {
      p6_pressed = 0;
    }
  
  }

  delay(100);
}
