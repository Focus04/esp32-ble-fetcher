# BLE API Fetcher

This ESP32 project connects to the PIA mobile app via Bluetooth Low Energy (BLE) and fetches character data from "The Wolf Among Us" API endpoints to send to the connected device.

## Key Features

- BLE connectivity with custom service and characteristics
- WiFi network scanning and connection handling
- Two API endpoints integration:
  - Character list fetching
  - Detailed character information
- JSON data parsing and transmission
- Team identification (Team A35) in all responses

## Hardware Requirements

- ESP32 development board
- USB cable for programming and power
- Mobile device with BLE support (for the PIA app)

## Software Requirements

- Arduino IDE with ESP32 support or VSC with the PlatformIO extension
- Required libraries:
  - `WiFi.h`
  - `HTTPClient.h`
  - `ArduinoJson.h`
  - `BLEDevice.h`
  - `BLEUtils.h`
  - `BLEServer.h`

## Installation

- Clone this repository
- Open `main.cpp` in Arduino IDE or PlatformIO
- Install required libraries listed above
- Upload to your ESP32

## BLE Service Structure

- **Device Name:** `API_Fetcher`
- **Service UUID:** `1b2e4970-7e52-43ed-93ee-7dd01880e8b3`
- **Characteristic UUID:** `bd104210-8851-42d2-a610-3548f79dad88`
- **Properties:** `READ`, `WRITE`, `NOTIFY`

## API Endpoints

- Character list: `http://proiectia.bogdanflorea.ro/api/the-wolf-among-us/characters`
- Character details (append character ID): `http://proiectia.bogdanflorea.ro/api/the-wolf-among-us/character?id=`

## Supported BLE Commands

The ESP32 responds to the following JSON actions received via BLE:
- `getNetworks` - Scans and returns available WiFi networks
- `connect` - Connects to specified WiFi network (requires ssid and password)
- `getData` - Fetches and returns character list
- `getDetails` - Fetches detailed character information (requires character id)

## Usage Flow

- ESP32 boots and starts BLE advertising
- Mobile app connects to device `API_Fetcher`
- App can
  - Request WiFi networks list
  - Send WiFi credentials to connect to a network
  - Request character data
  - Request specific character details
- ESP32 processes requests and sends responses via BLE notifications

## Code Structure

The main functionality is implemented in `main.cpp` with:
- WiFi connection handling
- BLE server setup with callbacks
- Characteristic callback handler for processing requests
- HTTP client for API communication
- JSON parsing and response generation

## Response Examples

### WiFI Network Response
```json
{
  "ssid": "MyWiFi",
  "strength": -65,
  "encryption": 3,
  "teamId": "A35"
}
```

### Character Data Response
```json
{
  "id": "28",
  "name": "Nerissa",
  "image": "/path/to/image.jpg",
  "teamId": "A35"
}
```

### Character Details Response:
```json
{
  "name": "Nerissa",
  "image": "/path/to/image.jpg",
  "description": "Nerissa's story...",
  "teamId": "A35"
}
```

## Troubleshooting
- Connection issues: Ensure ESP32 is powered properly and in range
- WiFi problems: Verify credentials and network availability
- API failures: Check if endpoints are accessible
- Serial monitor: Use 115200 baud for debug output

## Team Members
- [Focus04](https://github.com/Focus04)
- [Lucas](https://github.com/lookasdev)