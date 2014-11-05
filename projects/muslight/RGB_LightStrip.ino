/*
*Arduino Music Spectrum Analyser and Moodlight RGB LightStrip
*By: Luis Flores
*
*Based on David Wang's Instructable here:
*http://www.instructables.com/id/Blinking-LEDs-to-the-Frequency-of-Musi/
*
*Find schematics and pictures here:
*http://lflores.me/musicstrip.html
*/

#define FADESPEED 10

//switch pin for moodlight mode and music mode functionality
int functionPin = 5;
int function = LOW;

// LED pins connected to the PWM pins on the Arduino
int ledPinG = 11;
int ledPinR = 10;
int ledPinB = 9;
int statusLED = 13;

int analogPin = 0; // MSGEQ7 OUT
int strobePin = 7; // MSGEQ7 STROBE
int resetPin = 8; // MSGEQ7 RESET
int spectrumValue[7];

// MSGEQ7 OUT pin produces values around 50-80
// when there is no input, so use this value to
// filter out a lot of the chaff.
int filterValue = 120;

void setup()
{
  //Read from function pin
  pinMode(functionPin, INPUT);

  // Read from MSGEQ7 OUT
  pinMode(analogPin, INPUT);
  // Write to MSGEQ7 STROBE and RESET
  pinMode(strobePin, OUTPUT);
  pinMode(resetPin, OUTPUT);

  // Set analogPin's reference voltage
  analogReference(DEFAULT); // 5V

  // Set startup values for pins
  digitalWrite(resetPin, LOW);
  digitalWrite(strobePin, HIGH);
  //turns on on board LED to make sure we set up right
  digitalWrite(statusLED, HIGH);
}

void musicMode(void) {
  // Set reset pin low to enable strobe
  digitalWrite(resetPin, HIGH);
  digitalWrite(resetPin, LOW);

  // Get all 7 spectrum values from the MSGEQ7
  for (int i = 0; i < 7; i++)
  {
    digitalWrite(strobePin, LOW);
    delayMicroseconds(20); // Allow output to settle

    spectrumValue[i] = analogRead(analogPin);

    // Constrain any value above 1023 or below filterValue
    spectrumValue[i] = constrain(spectrumValue[i], filterValue, 1023);

    // Remap the value to a number between 0 and 255
    spectrumValue[i] = map(spectrumValue[i], filterValue, 1023, 0, 255);

    digitalWrite(strobePin, HIGH);
  }

  // Write the PWM values to the LEDs
  // I find that with three LEDs, these three spectrum values work the best
  analogWrite(ledPinR, spectrumValue[1]);
  analogWrite(ledPinG, spectrumValue[4]);
  analogWrite(ledPinB, spectrumValue[6]);
}

void moodlightMode(void) {
  //this particular mode slowly fades the blue LEDs in and out
  int b;
  analogWrite(ledPinR, 0);
  analogWrite(ledPinG, 0);

  // fade from blue to violet
  for (b = 10; b < 256; b++) {
    analogWrite(ledPinB, b);
    delay(FADESPEED);
  }
  for (b = 255; b > 10; b--) {
    analogWrite(ledPinB, b);
    delay(FADESPEED);
  }
}
void loop()
{
  if (digitalRead(functionPin)) {
    musicMode();
  }
  else {
    moodlightMode();
  }
}
