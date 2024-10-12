int PhotoReaderPin_a = A1;
int measuredLight;

void setup() {
  Serial.begin(9600);


}

void loop() {
  measuredLight = analogRead(PhotoReaderPin_a);

  Serial.print("measuredLight: ");
  Serial.println(measuredLight);
}
