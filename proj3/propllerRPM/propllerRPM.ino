#include <Servo.h>
#include <TimerOne.h>

// Motor vars
Servo ESC;                       // Servo object for ESC

// Potentiometer vars     
uint8_t potPin = A0;             // Analog pin location for potentiometer
int potValue;                    // Reading from potentiometer
      
// Photoresistor vars     
int PhotoReaderPin_a = A1;       // Pin for photoresistor
int measuredLight;               // Analog read from photoresistor
      
// Interrupt and RPM vars     
volatile int rpmCount = 0;       // Count of light interruptions
volatile long timeold = 0;       // Previous time for RPM calculation

volatile const int irPin = 2;    // Pin for photoresistor interrupt
volatile int rpm = 0;            // Measured RPM

long debugTime = 0;

// Enum to represent tasks in the round-robin scheduler
enum TaskState { READ_POT, READ_LIGHT, CALC_RPM };
TaskState currentTask = READ_POT;

// Interrupt Service Routine (ISR) for counting RPM
void rpmISR() {
  rpmCount++;
}

void setup() {
  Serial.begin(9600);

  // Set up ESC
  int pin = 9;
  int minPulsWidth = 1000;
  int maxPulsWidth = 2000;
  ESC.attach(pin, minPulsWidth, maxPulsWidth);  // Attach the ESC

  // Set up interrupt on irPin
  attachInterrupt(digitalPinToInterrupt(irPin), rpmISR, FALLING);  

  // Initialize timing variables
  timeold = millis();
  debugTime = timeold;
}

void loop() {
  // Round-robin task scheduling
  switch (currentTask) {

    case READ_POT:
      // Potentiometer task
      potValue = analogRead(potPin);
      potValue = map(potValue, 0, 1023, 1000, 2000);  // Map to ESC PWM range
      ESC.write(potValue);                            // Set motor speed based on potValue
      currentTask = READ_LIGHT;
      break;

    case READ_LIGHT:
      // Light sensor task
      measuredLight = analogRead(PhotoReaderPin_a);
      if (millis() - debugTime >= 100) {
        Serial.print("measuredLight,");
        Serial.println(measuredLight);
        debugTime = millis();                         // Reset debug timer
      }
      currentTask = CALC_RPM;
      break;

    case CALC_RPM:
      // RPM calculation task
      if (millis() - timeold >= 1000) {
        // Calculate RPM: (rpmCount * 60) / Number of blades (assuming 1 blade)
        rpm = abs((rpmCount * 60)) / 2;  // Multiply by 60 to convert to RPM (/2 for 2 blades)
        Serial.print("RPM,");
        Serial.println(rpm);

        // Reset for next interval
        timeold = millis();
        rpmCount = 0;
      }
      currentTask = READ_POT;
      break;
  }
}