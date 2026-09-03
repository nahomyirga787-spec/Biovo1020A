#include <SoftwareSerial.h>
#include <Biovo1020A.h>

SoftwareSerial sensorSerial(2, 3); // RX = Pin 2, TX = Pin 3
Biovo1020A finger(sensorSerial);

void setup() {
  Serial.begin(115200);
  sensorSerial.begin(19200);
  finger.begin();

  Serial.println(F("Initializing BIOVO 1020A..."));

  int16_t count = finger.getCount();
  if (count >= 0) {
    Serial.print(F("Sensor online. Stored fingerprints: "));
    Serial.println(count);
  } else {
    Serial.println(F("Communication failed. Check power and connections."));
  }
}

void loop() {
  // Call finger.search(id, permission), finger.enroll(id, step), etc.
}