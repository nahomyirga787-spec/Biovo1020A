#include <SoftwareSerial.h>
#include <Biovo1020A.h>

SoftwareSerial sensorSerial(2, 3); // RX = Pin 2, TX = Pin 3
Biovo1020A finger(sensorSerial);

uint16_t getIDFromSerial() {
  Serial.println(F("\nEnter ID to enroll (1-100): "));
  
  // Wait until user actually enters a number
  while (Serial.available() == 0) {
    delay(10);
  }

  int id = Serial.parseInt();

  // Clear ALL residual bytes (\r, \n, extra spaces) from buffer
  delay(50); // Small buffer settle delay
  while (Serial.available() > 0) {
    Serial.read();
  }

  if (id < 1 || id > 100) {
    Serial.println(F("Invalid ID range. Cancelled."));
    return 0;
  }
  return (uint16_t)id;
}

void setup() {
  Serial.begin(115200);
  sensorSerial.begin(19200);
  finger.begin();

  Serial.println(F("========================================"));
  Serial.println(F("     Biovo1020A Enrollment Example     "));
  Serial.println(F("========================================"));

  int16_t count = finger.getCount();
  if (count < 0) {
    Serial.println(F("Error: Fingerprint sensor not responding."));
    while (1);
  }
  
  Serial.print(F("Sensor connected. Current enrolled count: "));
  Serial.println(count);
}

void loop() {
  uint16_t id = getIDFromSerial();
  if (id == 0) return;

  Serial.print(F("Starting enrollment for ID #"));
  Serial.println(id);

  // Step 1
  Serial.println(F("[Step 1/3] Place finger on sensor..."));
  if (!finger.enroll(id, 1, 30000)) {
    Serial.println(F("Step 1 failed or timed out."));
    return;
  }
  Serial.println(F("Step 1 complete. Remove finger."));
  delay(1500);

  // Step 2
  Serial.println(F("[Step 2/3] Place the SAME finger again..."));
  if (!finger.enroll(id, 2, 30000)) {
    Serial.println(F("Step 2 failed or timed out."));
    return;
  }
  Serial.println(F("Step 2 complete. Remove finger."));
  delay(1500);

  // Step 3
  Serial.println(F("[Step 3/3] Place the SAME finger one final time..."));
  if (!finger.enroll(id, 3, 30000)) {
    Serial.println(F("Step 3 failed or timed out."));
    return;
  }

  Serial.println(F("========================================"));
  Serial.print(F(" SUCCESS: Fingerprint enrolled under ID #"));
  Serial.println(id);
  Serial.println(F("========================================"));
}