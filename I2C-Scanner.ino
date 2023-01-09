/* 
* i2c_port_address_scanner
* Scans ports D0 to D7 on an ESP8266 and searches for I2C device. based on the original code
* available on Arduino.cc and later improved by user Krodal and Nick Gammon (www.gammon.com.au/forum/?id=10896)
* D8 throws exceptions thus it has been left out
*
*/

#include <Wire.h>

void setup() {
  Serial.begin(115200);
  while (!Serial);             // Leonardo: wait for serial monitor
  Serial.println("\n\nI2C Scanner to scan for devices on each port pair D0 to D7");
  scanPorts();
}

uint8_t portArray[] = {16, 5, 4, 0, 2, 14, 12, 13};
String portMap[] = {"D0", "D1", "D2", "D3", "D4", "D5", "D6", "D7"}; //for Wemos
//String portMap[] = {"GPIO16", "GPIO5", "GPIO4", "GPIO0", "GPIO2", "GPIO14", "GPIO12", "GPIO13"};

void scanPorts() { 
  for (uint8_t i = 0; i < sizeof(portArray); i++) {
    for (uint8_t j = 0; j < sizeof(portArray); j++) {
      if (i != j){
        Serial.print("Scanning (SDA : SCL) - " + portMap[i] + " : " + portMap[j] + " - ");
        Wire.begin(portArray[i], portArray[j]);
        #ifdef ESP8266
            Wire.setClockStretchLimit(1500);  //may need to increase this to 1500us for very slow parts
        #endif
        check_if_exist_I2C();
      }
    }
  }
}

void check_if_exist_I2C() {
  byte error, address;
  int nDevices;
  nDevices = 0;
  for (address = 8; address < 120; address++ )  {
    // The i2c_scanner uses the return value of
    // the Write.endTransmisstion to see if
    // a device did acknowledge to the address.
    Wire.beginTransmission(address);
    error = Wire.endTransmission();

    if (error == 0){
      Serial.print("I2C device found at address 0x");
      if (address < 16)
        Serial.print("0");
      Serial.print(address, HEX);
      Serial.println("  !");

      nDevices++;
      getName(address);

    } else if (error == 4) {
      Serial.print("Unknow error at address 0x");
      if (address < 16)
        Serial.print("0");
      Serial.println(address, HEX);
    }
  } //for loop
  if (nDevices == 0)
    Serial.println("No I2C devices found");
  else
    Serial.println("**********************************\n");
  //delay(1000);           // wait 1 seconds for next scan, did not find it necessary
}

void getName(int address)
{
  byte data1 = 0;
  byte error1 = 0;

  Serial.print(F("Possible Device: "));
  switch (address)
  {
  case 0x1D:
    Serial.println(F("ADXL345 digital accelerometer"));
    break;
  case 0x1E:
    Serial.println(F("HMC5883L 3-axis digital compass"));
    break;
  case 0x20:
  case 0x21:
  case 0x22:
  case 0x23:
  case 0x24:
  case 0x25:
  case 0x26:
    Serial.println("PCF8574 I/O expander");
    break;
  case 0x27:
    Serial.println("LCD with I2C backpack");
    Serial.println("PCF8574 I/O expander");
    break;
  case 0x38:
  case 0x39:
  case 0x3A:
  case 0x3B:
    break;
  case 0x3C:  /*SSD1306 OLED Shield*/
  case 0x3D:  /*SSD1306 OLED Shield*/
    Serial.println("SSD1306 OLED Shield");
    break;
  case 0x3E:
    Serial.println("PCF8574A I/O expander");
    break;
  case 0x3F:
    Serial.println("LCD with I2C backpack");
    Serial.println("PCF8574A I/O expander");
    break;
  case 0x40:
    Serial.println("HTU21D digital humidity and temperature sensor");
    break;
  case 0x48:
  case 0x49:
  case 0x4A:
  case 0x4B:
    Serial.println("ADS1113, ADS1114, ADS1115, ADS1013, ADS1014, ADS1015");
    break;
  case 0x50:
  case 0x51:
  case 0x52:
  case 0x54:
  case 0x55:
  case 0x56:
  case 0x57:
    Serial.println("AT24C32/64 Eeprom family");
    break;
  case 0x53:
    Serial.print("ADXL345 digital accelerometer");
    Serial.println(" or AT24C32/64 Eeprom family");
  case 0x68:
    Serial.print(" DS3231 or DS1307 Real Time Clock");
    Serial.print(" or MPU9250 gyroscope, accelerometer, magnetometer");
    Serial.println(" or L3G4200D gyroscope");
    break;
  case 0x69: // same device also on 0x68
             // also need to study pass-through mode of MPU9250
    Serial.print("MPU9250 gyroscope, accelerometer, magnetometer");
    Serial.println(" or L3G4200D gyroscope");
    break;
  case 0x76:
  case 0x77:
    Serial.println("BMP280 or BME280 or BME680 or MS5607,MS5611,MS5637");
    // note: address 0x77 may be BMP085,BMA180 and may not be MS5607 or MS5637 CHECK
    Wire.beginTransmission(address);
    // Select register
    Wire.write(0xD0); // 0xD0 hex address of chip_id
    // Stop I2C Transmission
    Wire.endTransmission();
    // Request 1 bytes of data
    Wire.requestFrom(address, 1);
    // Read 1 byte of data
    if (Wire.available() == 1)
    {
      data1 = Wire.read();
    } // end of if (Wire.available() == 3)

    Serial.print("Device ID=");
    Serial.print(data1, HEX);

    if (data1 == 0x58)
      Serial.println(" = BMP280");
    else if (data1 == 0x60)
      Serial.println(" = BME280");
    else if (data1 == 0x61)
      Serial.println(" = BME680");
    else
      Serial.println(" ID not in list");
    break;
  default:
    Serial.println("device not in list");
    break;
  }
}
  
void loop() {
}
