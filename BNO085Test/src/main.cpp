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

//选择用途
#define Test true      // 用于测试改为true
#define Calibrate false // 用于校准改为true

//测试部分
#if Test
#include <Wire.h>

#include "SparkFun_BNO080_Arduino_Library.h" // Click here to get the library: http://librarymanager/All#SparkFun_BNO080
BNO080 myIMU;

void setup()
{
  Serial.begin(115200);
  Serial.println();
  Serial.println("BNO08X Read Example\n");

  // 在这里修改(SDA,ACL)
  Wire.begin(4, 5); // slime官方默认nodemcu接线
  // Wire.begin(D2, D3); // 磷桦板子

  Serial.printf("[INFO]  imu begin\n");

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
      Serial.printf("[INFO]  Wait 13 seconds to try again.\n");
      delay(13000);
    }
  }

  Wire.setClock(400000); // Increase I2C data rate to 400kHz
  Serial.printf("[INFO]  Increase I2C data rate to 400kHz\n");

  myIMU.enableAccelerometer(50); // Send data update every 50ms
  Serial.printf("[INFO]  enable Accelerometer\n");
  myIMU.enableGyro(50); // Send data update every 50ms
  Serial.printf("[INFO]  enable Gyro\n");
  myIMU.enableMagnetometer(50); // Send data update every 50ms
  Serial.printf("[INFO]  enable Magnetometer\n\n");
  myIMU.enableRotationVector(50); // Send data update every 50ms
  Serial.printf("[INFO]  enable RotationVector\n\n");
  myIMU.enableDebugging(Serial); // Output debug messages to the Serial port. Serial1, SerialUSB, etc is also allowed.

  // Serial.println(F("Output in form x, y, z, in m/s^2"));
  // Serial.println(F("Output in form x, y, z, in radians per second"));
  // Serial.println(F("Output in form x, y, z, in uTesla"));
  delay(2000);
}

void loop()
{
  // Look for reports from the IMU
  if (myIMU.dataAvailable() == true)
  {
    float ax = myIMU.getAccelX();
    float ay = myIMU.getAccelY();
    float az = myIMU.getAccelZ();

    float gx = myIMU.getGyroX();
    float gy = myIMU.getGyroY();
    float gz = myIMU.getGyroZ();

    float mx = myIMU.getMagX();
    float my = myIMU.getMagY();
    float mz = myIMU.getMagZ();

    float roll = (myIMU.getRoll()) * 180.0 / PI;   // Convert roll to degrees
    float pitch = (myIMU.getPitch()) * 180.0 / PI; // Convert pitch to degrees
    float yaw = (myIMU.getYaw()) * 180.0 / PI;     // Convert yaw / heading to degrees

    Serial.print(roll, 1);
    Serial.print(F(","));
    Serial.print(pitch, 1);
    Serial.print(F(","));
    Serial.print(yaw, 1);
    Serial.printf("加速度计Acceleration(m/s^2):[%5.3lf,%5.3lf,%5.3lf]\n", ax, ay, az);
    Serial.printf("陀螺仪Gyroscope(rad/s):     [%5.3lf,%5.3lf,%5.3lf]\n", gx, gy, gz);
    Serial.printf("磁力计Magnetometer(uTesla): [%5.3lf,%5.3lf,%5.3lf]\n", mx, my, mz);
    Serial.printf("欧拉角roll,pitch,yaw:       [%3lf,%3lf,%3lf]\n", roll, pitch, yaw);
    myIMU.printPacket();
    Serial.println();

    delay(250);
  }
}
#endif

#if CaCalibrate
/*
这后面的代码时用于085校准的，如果不是完全异常、几分钟就自旋了40度以上的085，否则不要轻易校准。
校准步骤请参阅手册
*/

#include <Wire.h>

#include "SparkFun_BNO080_Arduino_Library.h" // Click here to get the library: http://librarymanager/All#SparkFun_BNO080
BNO080 myIMU;

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
  Serial.println("BNO080 Read Example");

  // Wire.begin();

  // myIMU.begin();

  /**
   * 加上自己的部分
   * */
  // Wire.begin(4, 5); // 自己用的
  Wire.begin(D2, D1); // 赤灵用的
  // myIMU.begin(0x4A, Wire);//自己用的
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
      Serial.printf("[INFO]  Wait 13 seconds to try again.\n");
      delay(13000);
    }
  }

  Wire.setClock(400000); // Increase I2C data rate to 400kHz

  // Enable dynamic calibration for accel, gyro, and mag
  myIMU.calibrateAll(); // Turn on cal for Accel, Gyro, and Mag

  // Enable Game Rotation Vector output
  myIMU.enableGameRotationVector(100); // Send data update every 100ms

  // Enable Magnetic Field output
  myIMU.enableMagnetometer(100); // Send data update every 100ms

  // Once magnetic field is 2 or 3, run the Save DCD Now command
  Serial.println(F("Calibrating. Press 's' to save to flash"));
  Serial.println(F("Output in form x, y, z, in uTesla"));
}

void loop()
{
  if (Serial.available())
  {
    byte incoming = Serial.read();

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

      // myIMU.endCalibration(); //Turns off all calibration
      // In general, calibration should be left on at all times. The BNO080
      // auto-calibrates and auto-records cal data roughly every 5 minutes
    }
  }

  // Look for reports from the IMU
  if (myIMU.dataAvailable() == true)
  {
    float x = myIMU.getMagX();
    float y = myIMU.getMagY();
    float z = myIMU.getMagZ();
    byte accuracy = myIMU.getMagAccuracy();

    float quatI = myIMU.getQuatI();
    float quatJ = myIMU.getQuatJ();
    float quatK = myIMU.getQuatK();
    float quatReal = myIMU.getQuatReal();
    byte sensorAccuracy = myIMU.getQuatAccuracy();

    Serial.print(x, 2);
    Serial.print(F(","));
    Serial.print(y, 2);
    Serial.print(F(","));
    Serial.print(z, 2);
    Serial.print(F(","));
    printAccuracyLevel(accuracy);
    Serial.print(F(","));

    Serial.print("\t");

    Serial.print(quatI, 2);
    Serial.print(F(","));
    Serial.print(quatJ, 2);
    Serial.print(F(","));
    Serial.print(quatK, 2);
    Serial.print(F(","));
    Serial.print(quatReal, 2);
    Serial.print(F(","));
    printAccuracyLevel(sensorAccuracy);
    Serial.print(F(","));

    Serial.println();
  }
}
#endif
