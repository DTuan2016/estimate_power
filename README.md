# Raspberry Pi 4 Power Measurement

## 1. Overall Circuit Diagram
I am using the following circuit to measure the power consumption of a Raspberry Pi 4. The devices used are:

- A microcontroller such as ESP32, Arduino, etc. Here, I am using ESP8266 because it is available.
- An AC module: PZEM-004T
- Power outlet and plug

Below is my wiring setup:  
![Circuit Diagram](img/sodomach.png)

---

## 2. Compiling and Uploading Code to ESP8266

### 2.1. If you are using Windows
- Download the Arduino IDE (I used version 2.3.4).  
- Follow the ESP8266 board installation guide: [ESP8266 Arduino IDE Setup](http://arduino.vn/bai-viet/1172-lap-trinh-esp8266-bang-arduino-ide)  
- Install the PZEM-004T library in Arduino IDE.  
- Select the correct COM port to upload code → Choose the correct board (ESP8266, NodeMCU 1.0).  
- Clone my `.ino` code, edit your Wi-Fi SSID and password, then compile and upload.

### 2.2. If you are using Linux
- Download Arduino CLI:
```bash
curl -fsSL https://raw.githubusercontent.com/arduino/arduino-cli/master/install.sh | sh
```
- Move `arduino-cli` from your personal directory to a system-wide directory:
```bash
sudo mv ~/bin/arduino-cli /usr/local/bin/
```
+ Add your user to the `dialout` group to access serial ports:
```bash
sudo usermod -a -G dialout $USER
```
+ Initialize Arduino CLI configuration:
```bash
arduino-cli config init
```
+ Add the URL so Arduino CLI can fetch the ESP8266 Board package:
```bash
arduino-cli config set board_manager.additional_urls https://arduino.esp8266.com/stable/package_esp8266com_index.json
```
+ Update the board and core index:
```bash
arduino-cli core update-index
```
+ Install ESP8266 Arduino Core (includes toolchain, libraries, compiler):
```bash
arduino-cli core install esp8266:esp8266
```
+ List all installed cores (ESP8266 should appear here):
```bash
arduino-cli core list
```
+ List all connected boards:
```bash
arduino-cli board list
```
+ Install the library for the PZEM-004T module:
```bash
arduino-cli lib install "PZEM004Tv30"
```
- Enter your Wi-Fi SSID and password, and fill in the IP address of the server you are hosting (where `server.py` runs) in the `estimate_power.ino` file.

+ Then, compile file .ino in your folder:
```bash
arduino-cli compile --fqbn esp8266:esp8266:nodemcuv2 <folder-containing-.ino>
```
+ Upload the compiled code
```bash
arduino-cli upload -p /dev/ttyUSB0 --fqbn esp8266:esp8266:nodemcuv2 <folder-containing-.ino>
```
+ Open the serial monitor to view data from ESP8266:
```bash
arduino-cli monitor -p /dev/ttyUSB0 -c baudrate=115200
```
## 3. Run with Server
```bash
python3 server.py
```

After running, the power information of the Raspberry Pi 4 will be sent to the server continuously, with updates occurring approximately once per second.

## NOTE: The folder name must match the .ino file name inside that folder.
