#include <TimerOne.h>

const double reference_temperature{ 77.0 };  // Reference temperature in Fahrenheit
const double R_reference_OHMS{ 10000 };      // Reference resistance in ohms (10k ohms)
const double beta{ 7035 };                   // Beta value 4035

double Vcc = 5.0;               // Assume 5V supply
double Vout;                    // Measured voltage across the thermistor
double thermistorResistance;    // Calculated thermistor resistance
double recordedTemperatureFar;  // Temperature in Fahrenheit
const double T0 = 298.15;       // Reference temperature in Kelvin (25°C = 298.15 K)

volatile bool tempReadFlag = false;  // Flag to signal temperature reading task

const bool DEBUG_MODE{ false };

double computeThermistorResistance(double V) {
  return R_reference_OHMS * (Vcc / V - 1);  // Voltage divider equation
}

double computeBetaEquation(double resistance) {

  double temperatureKelvin      = 1 / ((1 / T0) + (1 / beta) * log(resistance / R_reference_OHMS));
  double temperatureCelsius     = temperatureKelvin - 273.15;
  double temperatureFahrenheit  = (temperatureCelsius * 9.0 / 5.0) + 32.0;

  return temperatureFahrenheit;
}


void timerISR() {
  tempReadFlag = true;  // Set flag to true when the timer triggers
}

void setup() {
  Serial.begin(9600);                 // Initialize serial communication at 9600 baud rate
  pinMode(A1, INPUT);                 // Set the analog pin A0 as input

  Timer1.initialize(10000000);        // 10,000,000 microseconds = 10 seconds
  Timer1.attachInterrupt(timerISR);   // Attach the ISR to Timer1
}

void loop() {
  // Round-robin task 1: Temperature reading 
  if (tempReadFlag) {
    tempReadFlag = false;             // Reset the flag to prevent re-execution

    // Read the analog input pin (e.g., A0) and convert to voltage
    int sensorValue = analogRead(A1);
    if (DEBUG_MODE) {
      Serial.print("sensorValue: ");
      Serial.println(sensorValue);
    }
    Vout = sensorValue * (Vcc / 1023.0);  // Convert analog value to voltage

    if (DEBUG_MODE) {
      Serial.print("Vout: ");
      Serial.println(Vout);
      Serial.println(Vout);
      Serial.println(sensorValue);
    }

    // Compute thermistor resistance and temperature
    thermistorResistance      = computeThermistorResistance(Vout);
    recordedTemperatureFar    = computeBetaEquation(thermistorResistance);

    if (DEBUG_MODE) {
      Serial.print("Computed resistance: ");
      Serial.println(thermistorResistance);
    }

    // Get the current time in milliseconds
    unsigned long timeMillis = millis();

    // Transmit time and temperature in CSV format over Serial port
    Serial.print(timeMillis);
    Serial.print(", ");
    Serial.println(recordedTemperatureFar);

  }
  // Other Round Robin tasks would be added here
}
