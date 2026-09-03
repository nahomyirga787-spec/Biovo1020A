#include <SoftwareSerial.h>
#include <Biovo1020A.h>

SoftwareSerial sensorSerial(2, 3); // RX = Pin 2, TX = Pin 3
Biovo1020A finger(sensorSerial);

void setup() {
  Serial.begin(115200);
  sensorSerial.begin(19200);
  finger.begin();

  Serial.println(F("========================================"));
  Serial.println(F("    Biovo1020A 1:N Verification Loop    "));
  Serial.println(F("========================================"));
  Serial.println(F("Place a registered finger on the sensor to identify...\n"));
}

void loop() {
  uint16_t matchedID = 0;
  uint8_t permission = 0;

  // Search database (5-second window per attempt)
  if (finger.search(matchedID, permission, 5000)) {
    Serial.println(F("****************************************"));
    Serial.print(F(" MATCH FOUND! ID: #"));
    Serial.println(matchedID);
    Serial.print(F(" Permission Level: "));
    Serial.println(permission);
    Serial.println(F("****************************************\n"));
    
    // Hold briefly to prevent rapid duplicate matches
    delay(2000);
  } else {
    // Check if failure was an execution error or just a missing match
    if (finger.getLastStatus() == BIOVO_ACK_FAIL) {
      Serial.println(F("Scan complete: No matching fingerprint in database."));
      delay(1000);
    }
  }
}