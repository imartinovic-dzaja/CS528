#include "BluetoothSerial.h"
#include <BLEAdvertisedDevice.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>


/* Check if Bluetooth configurations are enabled in the SDK */
/* If not, then you have to recompile the SDK */
#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

int scanTime = 5; //In seconds
BLEScan* pBLEScan;
BluetoothSerial SerialBT;


std::string convertToJSON(BLEAdvertisedDevice advertisedDevice) {

  std::string output = "{\n";
  output.append("\t\"module\":\"1\",\n");
  output.append("\t\"device\":\n");
  output.append("\t\t{\n");
  std::string address = advertisedDevice.getAddress().toString();
  output.append("\t\t\t\"address\":\"" + advertisedDevice.getAddress().toString() + "\",\n");
  if (advertisedDevice.haveAppearance()) {
    char temp_cstr[10];
    itoa(advertisedDevice.getAppearance(), temp_cstr, 10);    //Need this workaround since std::to_string() doesn't work
    output.append("\t\t\t\"appearance\":\"" + std::string(temp_cstr) + "\",\n");
  } else {
    output.append("\t\t\t\"appearance\":\"\",\n");
  }

  //  if (advertisedDevice.haveManufacturerData()){
  //    std::string man = advertisedDevice.getManufacturerData();
  //    char * cstr = new char [man.length()+1];
  //    strcpy(cstr, man.c_str());
  //    Serial.printf("%s\n", cstr);
  //    output.append("\t\t\t\"manufacturer\":\""+advertisedDevice.getManufacturerData()+"\",\n");
  //
  //  } else {
  //    output.append("\t\t\t\"manufacturer\":\"\",\n");
  //  }

  if (advertisedDevice.haveName()) {
    output.append("\t\t\t\"name\":\"" + advertisedDevice.getName() + "\",\n");

  } else {
    output.append("\t\t\t\"name\":\"\",\n");
  }

  if (advertisedDevice.haveRSSI()) {
    char temp_cstr[10];
    itoa(advertisedDevice.getRSSI(), temp_cstr, 10);    //Need this workaround since std::to_string() doesn't work
    output.append("\t\t\t\"RSSI\":\"" + std::string(temp_cstr) + "\",\n");
  } else {
    output.append("\t\t\t\"RSSI\":\"\",\n");
  }

  if (advertisedDevice.haveServiceUUID()) {
    output.append("\t\t\t\"UUID\":\"" + advertisedDevice.getServiceUUID().toString() + "\",\n");
  } else {
    output.append("\t\t\t\"UUID\":\"\",\n");
  }

  if (advertisedDevice.haveTXPower()) {
    char temp_cstr[10];
    itoa(advertisedDevice.getTXPower(), temp_cstr, 10);    //Need this workaround since std::to_string() doesn't work
    output.append("\t\t\t\"TxPower\":\"" + std::string(temp_cstr) + "\",\n");

  } else {
    output.append("\t\t\t\"TxPower\":\"\",\n");
  }

  output.append("\t\t}\n");
  output.append("}\n");

  return output;
}

class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
    void onResult(BLEAdvertisedDevice advertisedDevice) {
      std::string str = convertToJSON(advertisedDevice);
      char * cstr = new char [str.length() + 1];
      strcpy(cstr, str.c_str());
      Serial.printf("%s\n", cstr);

      int i = 0;
      while (cstr[i] != '\0') {
        SerialBT.write(cstr[i]);
        ++i;
      }
      SerialBT.write('\n');
      SerialBT.write('\0');
    }
};


void setup() {
  Serial.begin(115200);
  /* If no name is given, default 'ESP32' is applied */
  /* If you want to give your own name to ESP32 Bluetooth device, then */
  /* specify the name as an argument SerialBT.begin("myESP32Bluetooth"); */
  SerialBT.begin();
  Serial.println("Bluetooth Started! Ready to pair...");

  BLEDevice::init("Module 1");


  pBLEScan = BLEDevice::getScan(); //create new scan
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setActiveScan(true); //active scan uses more power, but get results faster
  pBLEScan->setInterval(100);
  pBLEScan->setWindow(99);  // less or equal setInterval value



}

void loop() {

  // put your main code here, to run repeatedly:
  BLEScanResults foundDevices = pBLEScan->start(scanTime, false);
  //Serial.print("Devices found: ");
  //Serial.println(foundDevices.getCount());
  Serial.println("Scan done!");
  pBLEScan->clearResults();   // delete results fromBLEScan buffer to release memory
  delay(2000);

  //if (SerialBT.available())
  //{
  //  Serial.write(SerialBT.read());
  //}
  delay(500);
}
