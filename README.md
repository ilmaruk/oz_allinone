# oz_allinone

A small ESP32 project that turns a MAX7219 display into a clock, countdown timer, and live scoreboard. The device connects to Wi‑Fi, syncs the time with NTP, and listens for MQTT messages to update the score or timer.

## What this project does

The firmware displays three kinds of information:

- current time
- a countdown/timer
- home and away score values

It is designed to be a compact digital board that can be controlled remotely through MQTT, without needing a local computer or manual button input.

## Main features

- ESP32-based firmware
- MAX7219 matrix display support
- Wi‑Fi connection with fallback networks
- NTP clock synchronization
- Countdown timer with start, pause, resume, and reset
- Scoreboard updates via MQTT
- Automatic reconnect handling for Wi‑Fi and MQTT

## Hardware used

- ESP32 development board
- MAX7219 LED display modules
- 5V power source for the display chain
- SPI wiring between ESP32 and MAX7219

Default GPIO mapping in `config.h`:

- CLK: GPIO 18
- DATA: GPIO 23
- CS: GPIO 5

## Dependencies

The project requires these Arduino libraries:

- `MD_Parola`
- `PubSubClient`
- `ArduinoJson`
- ESP32 board package in the Arduino IDE

## Repository layout

- `oz_allinone.ino` — main sketch
- `config.h` — pin definitions and MQTT defaults
- `secrets.h` — Wi‑Fi credentials
- `timer.h` — timer logic
- `mqtt.h` — MQTT handling and commands
- `oz_wifi.h` — Wi‑Fi helper functions
- `clock.h` — NTP + time formatting
- `clock_font.h` — display font data

## Setup

### 1. Install requirements

Open the project in the Arduino IDE and install the required libraries listed above. Also make sure the ESP32 board package is installed.

### 2. Configure the hardware

Edit `config.h` if your wiring or MQTT settings differ from the defaults.

### 3. Add your Wi‑Fi networks

Edit `secrets.h` and replace the sample credentials with your own.

Example:

```cpp
wifi_credentials wifi_networks[] = {
  {"MyNetwork", "MyPassword", 30000},
  {"BackupNetwork", "BackupPassword", 30000}
};
```

### 4. Upload the sketch

Compile and upload `oz_allinone.ino` to the ESP32.

> Important: `secrets.h` should not be committed to a public repository unless you intentionally want to share those Wi‑Fi credentials.

## MQTT protocol

The device listens for JSON messages on the configured MQTT topic.

### Score update

```json
{
  "home_id": "MNL",
  "away_id": "ROM",
  "home_score": 2,
  "away_score": 1
}
```

This updates the home and away score values shown on the display.

### Timer commands

```json
{
  "command": "SET",
  "duration": 90
}
```

Supported commands:

- `SET` — set a timer in seconds
- `START` — begin the countdown
- `STOP` — pause the countdown
- `RESUME` — continue the countdown

Example:

```json
{ "command": "START" }
```

## Runtime behavior

On boot, the board:

1. starts serial logging
2. sets up the display
3. configures time synchronization with NTP
4. connects to Wi‑Fi
5. connects to the MQTT broker
6. subscribes to the configured topic

During normal operation it keeps the connection alive and refreshes the display with the current time, timer, or score as needed.

## Notes

- The default timezone is set for Europe/Rome.
- The timer visually changes when it reaches zero.
- The code intentionally uses a blinking-colon effect for the clock display.

## License

See the license file in this repository for usage terms.
