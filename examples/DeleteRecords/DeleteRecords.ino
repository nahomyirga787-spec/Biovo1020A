#include <SoftwareSerial.h>
#include <Biovo1020A.h>

SoftwareSerial sensorSerial(2, 3); // RX = Pin 2, TX = Pin 3
Biovo1020A finger(sensorSerial);

void printMenu() {
  Serial.println(F("\n--- Database Maintenance Menu ---"));
  Serial.println(F("1. Delete specific ID"));
  Serial.println(F("2. Delete ALL fingerprints"));
  Serial.println(F("3. Print total stored count"));
  Serial.print(F("Select option: "));
}

// Helper to reliably read an integer input from Serial
uint16_t readIDFromSerial() {
  while (Serial.available() == 0) {
    delay(10);
  }
  int id = Serial.parseInt();
  
  // Clear trailing newlines/spaces
  delay(50);
  while (Serial.available() > 0) {
    Serial.read();
  }
  return (id > 0 && id <= 100) ? (uint16_t)id : 0;
}

// Helper to clear residual characters from the input buffer
void flushSerialBuffer() {
  delay(50);
  while (Serial.available() > 0) {
    Serial.read();
  }
}

void setup() {
  Serial.begin(115200);
  sensorSerial.begin(19200);
  finger.begin();

  Serial.println(F("Biovo1020A Maintenance Utility Ready."));
  printMenu();
}

void loop() {
  if (!Serial.available()) return;

  char choice = Serial.read();
  
  // Ignore rogue line endings passed to menu selection
  if (choice == '\r' || choice == '\n') return;
  
  flushSerialBuffer();

  switch (choice) {
    case '1': {
      Serial.print(F("\nEnter ID to delete (1-100): "));
      uint16_t id = readIDFromSerial();
      
      if (id == 0) {
        Serial.println(F("Invalid ID range or timeout. Operation cancelled."));
        break;
      }

      if (finger.deleteUser(id)) {
        Serial.print(F("SUCCESS: ID #"));
        Serial.print(id);
        Serial.println(F(" removed."));
      } else {
        Serial.print(F("FAILED: Could not delete ID #"));
        Serial.print(id);
        Serial.println(F(" (ID may not exist)."));
      }
      break;
    }

    case '2': {
      Serial.println(F("\nWARNING: Erasing whole database! Type 'Y' to confirm: "));
      while (Serial.available() == 0) delay(10);
      char confirm = Serial.read();
      flushSerialBuffer();

      if (confirm == 'Y' || confirm == 'y') {
        if (finger.deleteAll()) {
          Serial.println(F("SUCCESS: All fingerprint records erased."));
        } else {
          Serial.println(F("FAILED: Erase command rejected."));
        }
      } else {
        Serial.println(F("Operation cancelled."));
      }
      break;
    }

    case '3': {
      int16_t count = finger.getCount();
      Serial.print(F("\nCurrent stored fingerprints: "));
      Serial.println(count);
      break;
    }

    default:
      Serial.println(F("Unknown option."));
      break;
  }

  printMenu();
}