#include <SoftwareSerial.h>
#include <math.h>

#define PIN_BUTTON      8    // BUTTON
#define PIN_BUZZER      9    // PIEZO buzzer
#define PIN_LED         13   // LED
#define PIN_HDUS007_RX  10   // RX from ultra-sonic distance sensor

// HDUS-007 ultra-sonic distance sensor
SoftwareSerial hdus007(10, 11); // RX, TX
int hdus007_getDistance(void);

unsigned int midi2freq(uint8_t m) // m is 0 ~ 127
{
  double t = pow(2, m / 12);
  unsigned int hz = 440 * t;

  // Human can hear 20Hz ~ 20kHz
  // this converter makes 8Hz ~ 12.5kHz

  return hz;
}

void setup()
{
  pinMode(PIN_LED, OUTPUT);
  digitalWrite(PIN_LED, HIGH);

  pinMode(PIN_BUTTON, INPUT);
  digitalWrite(PIN_BUTTON, HIGH);

  pinMode(PIN_BUZZER, OUTPUT);
  digitalWrite(PIN_BUZZER, LOW);

  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  Serial.println("Goodnight moon!");

  // set the data rate for the SoftwareSerial port
  hdus007.begin(9600);
}

void loop() // run over and over
{
  if (digitalRead(PIN_BUTTON) == HIGH) {
    noTone(PIN_BUZZER);
    digitalWrite(PIN_LED, HIGH);
    delay(1);
    return;
  }

  digitalWrite(PIN_LED, LOW);
  // distance got 25 ~ 300mm
  int distance = hdus007_getDistance();
  if (distance > 0) {
    // human can hear 20 ~ 20,000Hz
    int pitch = map(distance, 20, 200, 120, 1500);
    tone(PIN_BUZZER, pitch);
  }
  delay(1);
}

int hdus007_getDistance(void)
{
  int distance = -1;
  if (hdus007.available() > 3 && hdus007.read() == 0xff) {
    byte distH = hdus007.read();
    byte distL = hdus007.read();
    byte checksum = (distH + distL + 0xff) & 0xff;
    if (hdus007.read() == checksum) {
      distance = (distH << 8) | distL;
    }
  }

  // HDUS-007 can measure 30mm ~ 2000mm
  if (distance > 0) {
    if (distance < 30) distance = 30;
    if (distance > 2000) distance = 2000;
  }

  return distance;
}
