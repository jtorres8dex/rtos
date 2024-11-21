
// // test just a basic serial connection
// void setup()
// {
//     Serial.begin(9600);

// }

// void loop()
// {
//     while(1)
//     {
//         Serial.println("hello world");
//         delay(100);
//     }
// }

// // TEST I2C CONNECTION

// #include <Wire.h>

// void setup() {
//     Serial.begin(9600);
//     Wire.begin();
//     Serial.println("Scanning for I2C devices...");
// }

// void loop() {
//     byte error, address;
//     int nDevices = 0;

//     for (address = 1; address < 127; address++) {
//         Serial.println(1);
//         Wire.beginTransmission(address);
//         Serial.println(2);
//         error = Wire.endTransmission();
//         Serial.println(3);

//         if (error == 0) {
//             Serial.print("I2C device found at address 0x");
//             Serial.println(address, HEX);
//             nDevices++;
//         } else if (error == 4) {
//             Serial.print("Unknown error at address 0x");
//             Serial.println(address, HEX);
//         }
//     }

//     if (nDevices == 0) {
//         Serial.println("No I2C devices found");
//     } else {
//         Serial.println("done\n");
//     }

//     delay(5000);  // Wait 5 seconds before next scan
// }

#include <Wire.h>
#include <MPU6050.h>

MPU6050 mpu;

void setup() {
    Serial.begin(9600);
    Wire.begin();
    mpu.initialize();

    if (mpu.testConnection()) {
        Serial.println("MPU6050 connection successful.");
    } else {
        Serial.println("MPU6050 connection failed.");
        while (1);
    }
}

void loop() {
    int16_t ax, ay, az;
    int16_t gx, gy, gz;

    mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
    unsigned long currentTime = millis();
    float dt = (currentTime - previousTime) / 1000.0;
    previousTime = currentTime;

    // Calculate Roll and Pitch using accelerometer
    float accRoll = atan2(ay, az) * 180 / PI;
    float accPitch = atan2(-ax, sqrt(ay * ay + az * az)) * 180 / PI;

    // Integrate gyroscope data for Roll, Pitch, and Yaw
    roll += gyroXrate * dt;
    pitch += gyroYrate * dt;
    yaw += gyroZrate * dt;

    // Complementary filter to combine accelerometer and gyroscope data
    const float alpha = 0.98;
    roll = alpha * roll + (1 - alpha) * accRoll;
    pitch = alpha * pitch + (1 - alpha) * accPitch;

    // Output computed Roll, Pitch, and Yaw
    Serial.print("Roll: "); Serial.print(roll);
    Serial.print("\tPitch: "); Serial.print(pitch);
    Serial.print("\tYaw: "); Serial.println(yaw);


    delay(500);
}