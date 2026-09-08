# Biovo1020A

Arduino library for interfacing with **BIOVO 1020A / FPC1020A capacitive fingerprint sensors** over UART.

The library communicates directly with the sensor using its native **0xF5 UART packet protocol** and is designed to be lightweight enough for resource-constrained Arduino boards.

## Features

- UART communication with BIOVO 1020A fingerprint sensors
- Read the number of stored fingerprints
- 3-step fingerprint enrollment
- 1:N fingerprint search and verification
- Delete individual fingerprint records
- Delete all fingerprint records
- UART packet framing and checksum validation
- Configurable command timeouts
- Fixed-size buffers and no dynamic memory allocation
- Uses Arduino `Stream` for serial communication

## Supported Operations

| Operation | Function |
|---|---|
| Get stored fingerprint count | `getCount()` |
| Enrollment | `enroll(id, step)` |
| Search fingerprint | `search()` |
| Delete fingerprint | `deleteUser(id)` |
| Delete all fingerprints | `deleteAll()` |
| Read last status | `getLastStatus()` |
| Read last response packet | `getLastRxPacket()` |

## Hardware

### Sensor Pinout

| Pin | Function |
|---:|---|
| 1 | GND |
| 2 | UART RX |
| 3 | UART TX |
| 4 | VCC |
| 5 | TOUCH OUT |
| 6 | V_TOUCH |

> **Important:** Check the voltage requirements of your specific BIOVO 1020A module before connecting VCC.

### Arduino Uno Example

| BIOVO 1020A | Arduino Uno |
|---|---|
| GND | GND |
| UART RX | D3 (TX) |
| UART TX | D2 (RX) |
| VCC | Appropriate module supply |
| TOUCH OUT | Not used |
| V_TOUCH | Not used |

The included examples use:

```cpp
SoftwareSerial sensorSerial(2, 3); // RX, TX
```

Sensor UART:

```text
19200 baud
8 data bits
No parity
1 stop bit
```

## Installation

### Arduino IDE

1. Download or clone this repository.
2. Open Arduino IDE.
3. Select **Sketch → Include Library → Add .ZIP Library...**
4. Select the library ZIP file.
5. Open an example from **File → Examples → Biovo1020A**.

### Manual Installation

Copy the library folder into:

```text
Documents/Arduino/libraries/
```

The structure should look like:

```text
Biovo1020A/
├── src/
│   ├── Biovo1020A.cpp
│   └── Biovo1020A.h
├── examples/
│   ├── BasicUsage/
│   ├── DeleteRecords/
│   ├── EnrollUser/
│   └── SearchAndVerify/
├── library.properties
├── LICENSE
└── README.md
```

## Basic Usage

```cpp
#include <SoftwareSerial.h>
#include <Biovo1020A.h>

SoftwareSerial sensorSerial(2, 3); // RX, TX
Biovo1020A finger(sensorSerial);

void setup() {
  Serial.begin(115200);
  sensorSerial.begin(19200);

  finger.begin();

  int16_t count = finger.getCount();

  if (count >= 0) {
    Serial.print("Stored fingerprints: ");
    Serial.println(count);
  } else {
    Serial.println("Communication failed.");
  }
}

void loop() {
}
```

## Fingerprint Enrollment

Enrollment is performed in three steps:

```cpp
uint16_t id = 1;

if (finger.enroll(id, 1)) {
  Serial.println("Enrollment step 1 complete.");
}

if (finger.enroll(id, 2)) {
  Serial.println("Enrollment step 2 complete.");
}

if (finger.enroll(id, 3)) {
  Serial.println("Fingerprint enrolled successfully.");
}
```

Use the same finger for all enrollment steps.

The included example accepts IDs from **1–100**.

## Searching for a Fingerprint

```cpp
uint16_t matchedID;
uint8_t permission;

if (finger.search(matchedID, permission)) {
  Serial.print("Match found. ID: ");
  Serial.println(matchedID);

  Serial.print("Permission: ");
  Serial.println(permission);
}
```

If no fingerprint matches, the function returns `false`.

## Deleting a Fingerprint

```cpp
if (finger.deleteUser(1)) {
  Serial.println("Fingerprint deleted.");
}
```

## Deleting All Fingerprints

```cpp
if (finger.deleteAll()) {
  Serial.println("All fingerprints deleted.");
}
```

> **Warning:** `deleteAll()` permanently removes all stored fingerprint records.

## Checking Stored Fingerprints

```cpp
int16_t count = finger.getCount();

if (count >= 0) {
  Serial.print("Stored fingerprints: ");
  Serial.println(count);
}
```

A negative return value indicates a communication or command failure.

## API Reference

### `begin()`

```cpp
void begin();
```

Initializes the library and clears existing data from the serial receive buffer.

### `getCount()`

```cpp
int16_t getCount(uint32_t timeout = 5000);
```

Returns the number of stored fingerprint records.

### `enroll()`

```cpp
bool enroll(uint16_t id, uint8_t step, uint32_t timeout = 30000);
```

Performs one enrollment step.

- `id` — fingerprint database ID
- `step` — enrollment step (`1`, `2`, or `3`)
- `timeout` — timeout in milliseconds

### `search()`

```cpp
bool search(
    uint16_t &matchedID,
    uint8_t &permission,
    uint32_t timeout = 30000
);
```

Searches the fingerprint database and returns the matched ID and permission level.

### `deleteUser()`

```cpp
bool deleteUser(uint16_t id, uint32_t timeout = 5000);
```

Deletes a fingerprint record.

### `deleteAll()`

```cpp
bool deleteAll(uint32_t timeout = 5000);
```

Deletes all fingerprint records.

### `getLastStatus()`

```cpp
uint8_t getLastStatus() const;
```

Returns the status byte from the most recent valid response.

### `getLastRxPacket()`

```cpp
const uint8_t* getLastRxPacket() const;
```

Returns the most recently received response packet.

## Communication Protocol

The library communicates using an 8-byte UART frame.

### Command Frame

```text
F5 | CMD | D1 | D2 | D3 | 00 | CHECKSUM | F5
```

The checksum is calculated using XOR:

```text
CMD XOR D1 XOR D2 XOR D3 XOR 00
```

The library validates the packet framing and checksum before accepting a response.

## Command Definitions

| Command | Value | Function |
|---|---:|---|
| `BIOVO_CMD_ENROLL_1` | `0x01` | Enrollment step 1 |
| `BIOVO_CMD_ENROLL_2` | `0x02` | Enrollment step 2 |
| `BIOVO_CMD_ENROLL_3` | `0x03` | Enrollment step 3 |
| `BIOVO_CMD_DELETE` | `0x04` | Delete individual record |
| `BIOVO_CMD_DELETE_ALL` | `0x05` | Delete all records |
| `BIOVO_CMD_COUNT` | `0x09` | Get stored record count |
| `BIOVO_CMD_SEARCH` | `0x0C` | Search fingerprint database |

## Status Codes

| Status | Value | Meaning |
|---|---:|---|
| `BIOVO_ACK_SUCCESS` | `0x00` | Command successful |
| `BIOVO_ACK_FAIL` | `0x01` | Command failed / no match |
| `BIOVO_ACK_FULL` | `0x04` | Database full |
| Timeout | `0xFF` | No valid response received |

## Included Examples

### BasicUsage

Checks communication with the sensor and displays the number of stored fingerprints.

### EnrollUser

Interactive serial interface for enrolling fingerprints into IDs `1–100`.

### SearchAndVerify

Continuously searches for fingerprints and reports the matched ID and permission level.

### DeleteRecords

Interactive menu for:

1. Deleting a specific fingerprint
2. Deleting all fingerprints
3. Checking the number of stored fingerprints

## Serial Monitor

For the included examples, use:

```text
115200 baud
```

The sensor UART remains:

```text
19200 baud
```

These are separate serial connections.

## Memory Considerations

The library uses:

- Fixed-size packet buffers
- No dynamic memory allocation
- Arduino `Stream`
- Small command and response frames

This makes it suitable for resource-constrained boards such as:

- Arduino Uno
- Arduino Nano
- Arduino Mega

The library declares:

```text
architectures=*
```

in `library.properties`. Compatibility with other architectures should be tested before deployment.

## Troubleshooting

### Sensor does not respond

Check:

1. VCC and GND.
2. Sensor baud rate: **19200**.
3. UART wiring.
4. RX/TX direction.

For the included Uno example:

```text
Sensor TX → Arduino RX
Sensor RX → Arduino TX
```

### Communication fails

Check:

- Power supply
- RX/TX wiring
- Baud rate
- Serial port configuration
- Sensor response

If necessary, verify the RX/TX connections by swapping them.

### Enrollment fails

Make sure:

- The same finger is used for all three steps.
- The finger is properly positioned.
- The selected ID is valid.
- The sensor has enough time to complete each operation.


