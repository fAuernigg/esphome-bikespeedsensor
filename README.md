# bikespeedsensor 🚲

Esphome component to measure speed of a bike using a simple reed sensor


A simple **ESPHome external component** that measures bike speed using a **reed switch sensor**  
and optionally controls other ESPHome components (e.g. a relay) based on speed.

Supports configuration of **wheel size in inches (Zoll)** directly in YAML.  
Works on ESP32 and ESP8266.

---

## ✨ Features

- Calculates **real-time speed** in km/h from reed switch pulses  
- Adjustable **wheel size** (inches → automatic circumference calculation)  
- Designed for **low-power & high reliability**  
- Integrates seamlessly into **Home Assistant** via ESPHome  
- Works great for e-bikes, DIY speedometers, parking triggers, etc.

---

## 📦 Installation

### 1️⃣ Add as an external component

Add this block to your ESPHome YAML:

```yaml
external_components:
  - source: github://fAuernigg/esphome-bikespeedsensor
