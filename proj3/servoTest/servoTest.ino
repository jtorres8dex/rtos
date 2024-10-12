#include <Servo.h>

Servo     ESC;              // servo object for ESC
int       potValue;         // reading from pot

uint8_t   potPin    = A0;   // analog pin location
uint8_t   ESCPin    = 9;   // ESC pin location

void setup() {

    Serial.begin(9600);

    int pin             = 9;
    int minPulsWidth    = 1000;
    int maxPulseWidth   = 2000;

    ESC.attach(ESCPin, minPulsWidth, maxPulseWidth);

}

void loop() {
  
    potValue = map(analogRead(potPin), 0, 1023, 0, 180);
    ESC.write(potValue);

    Serial.print("potValue: ");
    Serial.println(potValue);



}
