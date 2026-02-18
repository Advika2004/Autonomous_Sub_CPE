#include "ping1d.h"
#include <SoftwareSerial.h>

static const uint8_t arduinoRxPin = 9;   //goes to sonar TX (white)
static const uint8_t arduinoTxPin = 10;  //goes to sonar RX (green)

SoftwareSerial pingSerial(arduinoRxPin, arduinoTxPin);
static Ping1D ping{ pingSerial };

void setup() {
  Serial.begin(115200);
  pingSerial.begin(9600);

  Serial.println("simple test");

  while (!ping.initialize()) {
    Serial.println("Sonar failed to initialize. Check wiring (white->9, green->10) and power.");
    delay(1000);
  }
  Serial.println("Sonar initialized!");
}

//set a limit so it doesn't go forever
const uint8_t MAX_READS = 30;
uint8_t readCount = 0;

void loop() {
  if (readCount >= MAX_READS) {
    Serial.println("Done. Stopping program.");
    while (1) {
      //nothing
    }
  }

  if (ping.update()) {
    float distance_in = ping.distance() / 25.4f; //converting to inches for better estimation
    Serial.print("Distance(in): ");
    Serial.print(distance_in, 2);   //2 decimals
    Serial.print("\tConfidence: ");
    Serial.println(ping.confidence());
    readCount++;
  } else {
    Serial.println("No update received!");
  }

  //print every one second
  delay(1000);
}

