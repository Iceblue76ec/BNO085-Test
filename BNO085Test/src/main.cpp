/*
  Using the BNO080 IMU

  Example : Tare function
  By: Sofian Audry
  Date: March 2nd, 2022

  Based on: Example9-Calibrate
  By: Nathan Seidle
  SparkFun Electronics
  Date: December 21st, 2017
  SparkFun code, firmware, and software is released under the MIT License.
  Please see LICENSE.md for further details.

  Feel like supporting our work? Buy a board from SparkFun!
  https://www.sparkfun.com/products/14586

  This example shows how to use the tare functionalities.
  It is best to run a calibration before using the tare functions.

  It takes about 1ms at 400kHz I2C to read a record from the sensor, but we are polling the sensor continually
  between updates from the sensor. Use the interrupt pin on the BNO080 breakout to avoid polling.

  Hardware Connections:
  Attach the Qwiic Shield to your Arduino/Photon/ESP32 or other
  Plug the sensor onto the shield
  Serial.print it out at 115200 baud to serial monitor.
*/

/*
这里的的代码时用于085校准的，如果不是完全异常、几分钟就自旋了40度以上的085，否则不要轻易校准。
校准步骤请参阅手册
*/

#include <Wire.h>
#include "SparkFun_BNO080_Arduino_Library.h" // Click here to get the library: http://librarymanager/All#SparkFun_BNO080
BNO080 myIMU;

// 程序时钟
const unsigned long interval = 150; // 串口打印输出间隔为150毫秒
unsigned long previousMillis = 0;   // 上一次输出的时间

// Given a accuracy number, print what it means
void printAccuracyLevel(byte accuracyNumber)
{
  if (accuracyNumber == 0)
    Serial.print(F("Unreliable"));
  else if (accuracyNumber == 1)
    Serial.print(F("Low"));
  else if (accuracyNumber == 2)
    Serial.print(F("Medium"));
  else if (accuracyNumber == 3)
    Serial.print(F("High"));
}

void setup()
{

  Serial.begin(115200);
  Serial.println();

  // 在这里修改你板子对应的的SDA,SCL引脚
  // Wire.begin(4, 5); // 自用
  //  Wire.begin(D2, D1); // 赤灵用的
  // Wire.begin(D2, D3); // 磷桦PCB

  while (1)
  {
    if (myIMU.begin(0x4A, Wire))
    {
      Serial.printf("[INFO]  BNO08X at 0X4A Successful initialization.\n");
      break;
    }
    else if (myIMU.begin(0x4B, Wire))
    {
      Serial.printf("[INFO]  BNO08X at 0X4B Successful initialization.\n");
      break;
    }
    else
    {
      Serial.printf("[ERR!]  BNO08X at 0X4A/0X4B initialization failure.\n");
      Serial.printf("[INFO]  Wait 10 seconds to try again.\n");
      Serial.printf("[INFO]  启动失败，请手动断电重启085\n");
      delay(10000);
      // ESP.restart();没啥效果
    }
  }

  Wire.setClock(400000); // Increase I2C data rate to 400kHz

  myIMU.enableAccelerometer(50); // Send data update every 50ms
  Serial.printf("[INFO]  enable Accelerometer\n");

  myIMU.enableGyro(50); // Send data update every 50ms
  Serial.printf("[INFO]  enable Gyro\n");

  myIMU.enableMagnetometer(50); // Send data update every 50ms
  Serial.printf("[INFO]  enable Magnetometer\n\n");

  myIMU.enableRotationVector(50); // Send data update every 50ms
  Serial.printf("[INFO]  enable RotationVector\n\n");

  myIMU.calibrateAll();

  // Enable Game Rotation Vector output
  myIMU.enableGameRotationVector(100); // Send data update every 100ms

  // Serial.println("---------注意事项：-------------");
  // Serial.println("校准过程中，请仔细观察每个部件的精度值。");
  // Serial.println("全部校准到hign后,输入 s 保存校准数据到085芯片内");
  // Serial.println("保存后将关闭自动校准");
  // Serial.println("结束后请刷回原本SlimeVR固件");

}

void loop()
{
  unsigned long currentMillis = millis(); // 获取当前时间

  if (Serial.available())
  {
    byte incoming = Serial.read();

    if (incoming == 'c')
    {
      // Enable dynamic calibration for accel, gyro, and mag
      myIMU.calibrateAll(); // Turn on cal for Accel, Gyro, and Mag
      Serial.println("重新启动自动校准");
      delay(1000);
    }

    if (incoming == 's')
    {
      myIMU.saveCalibration();          // Saves the current dynamic calibration data (DCD) to memory
      myIMU.requestCalibrationStatus(); // Sends command to get the latest calibration status

      // Wait for calibration response, timeout if no response
      int counter = 100;
      while (1)
      {
        if (--counter == 0)
          break;
        if (myIMU.dataAvailable() == true)
        {
          // The IMU can report many different things. We must wait
          // for the ME Calibration Response Status byte to go to zero
          if (myIMU.calibrationComplete() == true)
          {
            Serial.println("Calibration data successfully stored");
            delay(1000);
            break;
          }
        }

        delay(1);
      }
      if (counter == 0)
      {
        Serial.println("Calibration data failed to store. Please try again.");
      }

      myIMU.endCalibration(); // Turns off all calibration
      Serial.println("关闭自动校准");
      delay(1000);
      // In general, calibration should be left on at all times. The BNO080
      // auto-calibrates and auto-records cal data roughly every 5 minutes
    }
  }

  // Look for reports from the IMU
  if (myIMU.dataAvailable() == true)
  {
    float ax = myIMU.getAccelX();
    float ay = myIMU.getAccelY();
    float az = myIMU.getAccelZ();
    byte AccelAccuracy = myIMU.getAccelAccuracy();

    float gx = myIMU.getGyroX();
    float gy = myIMU.getGyroY();
    float gz = myIMU.getGyroZ();
    byte GyroAccuracy = myIMU.getGyroAccuracy();

    float mx = myIMU.getMagX();
    float my = myIMU.getMagY();
    float mz = myIMU.getMagZ();
    byte MagAccuracy = myIMU.getMagAccuracy();

    float quatI = myIMU.getQuatI();
    float quatJ = myIMU.getQuatJ();
    float quatK = myIMU.getQuatK();
    float quatReal = myIMU.getQuatReal();
    byte sensorAccuracy = myIMU.getQuatAccuracy();

    if (currentMillis - previousMillis >= interval)
    {                                 // 判断是否满足输出条件
      previousMillis = currentMillis; // 更新上一次输出的时间

      // 每秒20次打印串口信息
      Serial.println("---BNO08X---------原始数据----------精度-------");
      Serial.printf("Accel加速度计:  [%.2f,%.2f,%.2f] ,", ax, ay, az);
      printAccuracyLevel(AccelAccuracy);
      Serial.println();
      Serial.printf("Gyro陀螺仪:     [%.2f,%.2f,%.2f] ,", gx, gy, gz);
      printAccuracyLevel(GyroAccuracy);
      Serial.println();
      Serial.printf("Mag磁力计:      [%.2f,%.2f,%.2f] ,", mx, my, mz);
      printAccuracyLevel(MagAccuracy);
      Serial.println();
      Serial.printf("Quat欧拉角:      [%.2f,%.2f,%.2f,%.2f] ,", quatI, quatJ, quatK, quatReal);
      printAccuracyLevel(sensorAccuracy);
      Serial.println();
      Serial.println();
      Serial.println();
    }
  }
}