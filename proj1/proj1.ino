const int ledPin = 13;        // LED connected to digital pin 13
const int dotDuration = 200;  // Duration of a dot in milliseconds
const int dashDuration = 600; // Duration of a dash in milliseconds
const int letterSpace = 600;  // Space between letters
const int wordSpace = 1400;   // Space between words

volatile bool stopExecution = false;  // Interrupt flag to stop execution

// using const char* rather than String saved 7% of dynamic memory
const char* morseTable[36] = {
  ".-", "-...", "-.-.", "-..", ".", "..-.", "--.", "....", "..", ".---", 
  "-.-", ".-..", "--", "-.", "---", ".--.", "--.-", ".-.", "...", "-", 
  "..-", "...-", ".--", "-..-", "-.--", "--..", "-----", ".----", "..---", 
  "...--", "....-", ".....", "-....", "--...", "---..", "----." 
};

void setup() {
  pinMode(ledPin, OUTPUT);
  Serial.begin(9600);
  Serial.println("Enter a string to display in Morse Code (enter '~' to exit):");
}

void loop() {
  if (Serial.available()) {
    String input = Serial.readStringUntil('\n');

    // Check for '~' to trigger stop
    if (input.indexOf('~') != -1) {
      Serial.println("Interrupt received, stopping...");
      stopExecution = true;  // Set the flag to stop further execution
      return;  // Exit the loop
    }

    // Reset the stop flag before processing input
    stopExecution = false;

    Serial.println("Displaying in Morse Code:");
    for (int i = 0; i < input.length(); i++) {
      // Check if interrupt is triggered while processing
      if (Serial.available()) {
        String interruptCheck = Serial.readStringUntil('\n');
        if (interruptCheck.indexOf('~') != -1) {
          Serial.println("Interrupt received during processing, stopping...");
          stopExecution = true;
          break;  // Break out of the loop
        }
      }

      if (stopExecution) {
        Serial.println("Execution stopped");
        return;
      }

      char c = toupper(input[i]);
      if (c == ' ') {
        nonBlockingDelay(wordSpace);
      } else {
        int index = c - 'A';
        if (index >= 0 && index < 26) {
          displayMorse(morseTable[index]);
        } else if (c >= '0' && c <= '9') {
          displayMorse(morseTable[c - '0' + 26]);
        }
        nonBlockingDelay(letterSpace);
      }
    }
  }
}

void displayMorse(const char* morse) {
  for (int i = 0; morse[i] != '\0'; i++) {
    // Check for '~' during Morse code output
    if (Serial.available()) {
      String interruptCheck = Serial.readStringUntil('\n');
      if (interruptCheck.indexOf('~') != -1) {
        Serial.println("Interrupt received during Morse code output, stopping...");
        stopExecution = true;
        return;
      }
    }

    if (stopExecution) {
      Serial.println("Execution stopped during Morse code output");
      return;
    }

    if (morse[i] == '.') {
      digitalWrite(ledPin, HIGH);
      nonBlockingDelay(dotDuration);
    } else if (morse[i] == '-') {
      digitalWrite(ledPin, HIGH);
      nonBlockingDelay(dashDuration);
    }
    digitalWrite(ledPin, LOW);
    nonBlockingDelay(dotDuration);  // Space between dots and dashes
  }
}

// Non-blocking delay function using millis()
void nonBlockingDelay(int duration) {
  unsigned long startTime = millis();
  while (millis() - startTime < duration) {
    // During this time, check for interrupts
    if (Serial.available()) {
      String interruptCheck = Serial.readStringUntil('\n');
      if (interruptCheck.indexOf('~') != -1) {
        Serial.println("Interrupt received during delay, stopping...");
        stopExecution = true;
        return;
      }
    }

    if (stopExecution) {
      return;
    }
  }
}
