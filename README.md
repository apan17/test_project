# IoT Smart Dustbin Monitor

This repository contains the complete source code for the "Smart Dustbin Monitor" IoT project. The system transforms a standard dustbin into a smart city asset by providing real-time monitoring of fill levels, hazardous gas detection, GPS location tracking, and automated touchless lid operation.

The project demonstrates a full end-to-end IoT solution, spanning from physical hardware sensors to cloud processing and a user-facing web dashboard.

## 📂 Project Structure

* `arduino_code/`: Contains the firmware for the Cytron Maker Feather S3 (ESP32-S3) microcontroller.
* `cloud_bridge/`: Contains the Python script that acts as a bridge between the MQTT broker and the Supabase database.
* `web_dashboard/`: Contains the HTML/JS code for the real-time web dashboard hosted on Vercel.

---

## 🏗️ System Architecture

The system follows a 5-layer IoT architecture, enabling robust data flow from the edge to the cloud.

![IoT Architecture Diagram](images/architecture.png)

1.  **Perception Layer:** ESP32-S3 reads data from Ultrasonic, Gas, PIR, and GPS sensors and controls a Servo and LCD.
2.  **Network Layer:** Data is transmitted via Wi-Fi using the lightweight MQTT protocol.
3.  **Processing Layer:** A Mosquitto MQTT broker on a Google Cloud VM receives data. A Python script processes this data and inserts it into a Supabase PostgreSQL database.
4.  **Application Layer:** A Vercel-hosted web dashboard subscribes to real-time database changes to visualize data for the user.
5.  **Business Layer:** Data is used for high-level decisions like route optimization and sanitary alerts.

---

## 🛠️ Hardware Checklist

The following components are required to build the physical prototype:

* **Microcontroller:** Cytron Maker Feather AIoT S3 (ESP32-S3)
* **Sensors:**
    * HC-SR04 Ultrasonic Sensor (Fill Level)
    * MQ-2 or MQ-135 Gas Sensor (Air Quality)
    * HC-SR501 PIR Motion Sensor (Presence Detection)
    * NEO-6M GPS Module (Location)
* **Actuators:**
    * SG90 or MG996R Servo Motor (Lid Mechanism)
    * I2C LCD 16x2 Display (Status Output)
* **Power:** USB Cable & Power Source (e.g., Power Bank)

---

## ⚙️ Setup & Installation

### 1. Hardware Setup (Arduino IDE)
1.  Navigate to the `arduino_code/` folder.
2.  Open `smart_dustbin.ino` in the Arduino IDE.
3.  Install required libraries via the Library Manager:
    * `PubSubClient` (for MQTT)
    * `TinyGPSPlus` (for GPS)
    * `ESP32Servo` (for Servo)
    * `LiquidCrystal_I2C` (for LCD)
4.  Update the `ssid`, `password`, and `mqtt_server` variables with your credentials.
5.  Select the correct board and port, then upload the code to your ESP32-S3.

### 2. Cloud Bridge Setup (Python)
1.  Navigate to the `cloud_bridge/` folder on your server (e.g., Google Cloud VM).
2.  Install the required Python dependencies:
    ```bash
    pip install -r requirements.txt
    ```
3.  Configure your script with your MQTT broker details and Supabase API keys.
4.  Run the script:
    ```bash
    python3 bridge.py
    ```

### 3. Web Dashboard Setup (Vercel)
1.  Navigate to the `web_dashboard/` folder.
2.  Open `index.html` and ensure your Supabase URL and Anon Key are correctly set in the JavaScript section.
3.  Deploy this folder to Vercel (or any static site host) by importing the Git repository or using the Vercel CLI.

---

## 📜 License
This project is open-source and available under the [MIT License](LICENSE).
