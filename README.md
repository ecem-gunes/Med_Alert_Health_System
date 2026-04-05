# 🏥 Med-Alert Health System
### A Smart Emergency Notification System

> **Yaşar University — COMP 3328: Embedded Systems Project**  
> **Author:** Ecem Güneş (22070001039)  
> **Advisor:** Assist. Prof. (PhD) Ömer Çetin  
> **Date:** May 2025

---

## 📌 Overview

The **Med-Alert Health System** is an AVR C-based embedded system designed to support elderly individuals or patients who may experience medical emergencies while alone — especially in home care or assisted living settings.

Users can select an **emergency contact** (Nurse, Doctor, or Family) and an **urgency level** (Low, Medium, High) via a keypad. The system provides immediate multi-modal feedback and logs all events to a microSD card.

---

## 🔗 Wokwi Simulation

▶️ **[Click here to open the live simulation on Wokwi](https://wokwi.com/projects/431842178076233729)**

---

## ⚙️ Hardware Components

| Component | Purpose | Est. Cost |
|---|---|---|
| Arduino Mega | Main microcontroller (ATmega2560) | $35.00 |
| 16x2 LCD Display | Shows recipient, urgency level, confirmation | $9.99 |
| 4x4 Membrane Keypad | User input for recipient & urgency selection | $10.99 |
| 10-Segment LED Bar Graph | Visual urgency scale (3, 6, or 9 LEDs) | $3.00 |
| 3× Discrete LEDs | Individual Low / Medium / High indicators | $1.50 |
| Buzzer | Audible alert for Nurse + Medium/High urgency | $1.50 |
| MicroSD Card Module | Logs all notification events via SPI | $4.50 |
| Jumper Wires | — | $3.99 |
| **TOTAL** | | **$70.47** |

---

## 📁 Project Structure

```
med-alert-health-system/
├── src/
│   └── sketch.ino         # Main AVR C source code (Arduino)
├── wokwi/
│   ├── diagram.json       # Wokwi circuit diagram
│   ├── libraries.txt      # Wokwi library list
│   └── wokwi-project.txt  # Wokwi project reference
├── docs/
│   └── Final_Report.pdf   # Full project report
└── README.md
```

---

## 🧠 How It Works

1. **Power on** → LCD displays `"Select Recipient"`
2. Press **`1`** (Nurse), **`2`** (Doctor), or **`3`** (Family) to select contact
3. Press **`4`** (Low), **`5`** (Med), or **`6`** (High) to select urgency level
4. System responds simultaneously:
   - ✅ LCD updates with recipient + level confirmation
   - ✅ LED bar graph lights up (3 / 6 / 9 segments)
   - ✅ Urgency LEDs activate (1 / 2 / 3 LEDs)
   - ✅ Buzzer sounds if **Nurse** selected with **Medium or High** urgency (1–2 sec)
   - ✅ Event is **logged to SD card** (e.g. `To:Nurse, Level:High`)

---

## 💻 Software Details

- **Language:** AVR C (`avr/io.h`, `util/delay.h`, `stdio.h`, `string.h`)
- **Platform:** Arduino Mega (ATmega2560, 16 MHz)
- **Simulation:** [Wokwi Emulator](https://wokwi.com)
- **Communication:** SPI (for microSD card)
- **LCD Mode:** 4-bit parallel

### Key Functions

| Function | Description |
|---|---|
| `lcd_init()` | Initializes LCD in 4-bit mode |
| `lcd_print(str)` | Prints a string to the LCD |
| `keypad_read()` | Scans 4x4 keypad matrix for key press |
| `buzzer_beep_ms(d)` | Activates buzzer for `d` milliseconds |
| `ledbar_set_level(l)` | Lights up `l` LEDs on the bar graph |
| `urgency_leds_set(c)` | Sets 1–3 urgency indicator LEDs |
| `spi_init()` | Initializes SPI communication |
| `sd_init()` | Initializes the microSD card |
| `sd_write_block(txt)` | Writes a 512-byte block to SD card |
| `get_log_message(r, u)` | Formats log string from recipient + urgency |

### Pin Assignments

| Component | Pins Used |
|---|---|
| LCD (RS, E, D4–D7) | PB6, PB5, PB4, PH6, PH5, PH4 |
| Keypad Rows | PC5, PC4, PC3, PC2 |
| Keypad Columns | PC1, PC0, PD7, PG2 |
| LED Bar Graph | PA0–PA7, PC6, PC7 |
| Urgency LEDs | PE3, PE5, PG5 |
| Buzzer | PB7 |
| SD Card (SPI) | PB0 (CS), PB1 (SCK), PB2 (MOSI), PB3 (MISO) |

---

## 🚀 Future Work

- 📡 Wireless alerts via Wi-Fi or Bluetooth module
- 💓 Auto-trigger using heart rate / motion sensors
- 🔋 Low-power mode for portable/wearable use
- 📱 Mobile app for caregivers to monitor and track alerts

---

## 📚 References

- [AVR C Programming — Microchip App Note](https://ww1.microchip.com/downloads/en/Appnotes/AVR1000b-Getting-Started-Writing-C-Code-for-AVR-DS90003262B.pdf)
- [Wokwi Documentation](https://docs.wokwi.com/)
- [Matrix Keypad with AVR — Microchip App Note](https://ww1.microchip.com/downloads/en/Appnotes/00003407A.pdf)
- [LED Bar Graph with Arduino](https://www.teachmemicro.com/how-to-use-a-led-bar-graph-with-arduino/)

---

*Yaşar University — Engineering Faculty, Computer Engineering Program*
