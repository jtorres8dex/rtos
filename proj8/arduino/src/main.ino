#include <Wire.h>
#include <MPU6050.h>
#include <Arduino_FreeRTOS.h>
#include <semphr.h> 

// Initialize the MPU6050 IMU object
MPU6050 mpu;


// set init and sim vals 
float roll{0.0};
float pitch{0.0};
float yaw{0.0};
// arbitrarily assuming 100 Hz
float timeStep = 0.01;
unsigned long previousTime{0};

// Mutex for safely sharing data between tasks
SemaphoreHandle_t imuDataMutex;

// Task handles
void TaskReadIMU(void *pvParameters);
void TaskSendData(void *pvParameters);

void setup() {
    
    // baud rate
    Serial.begin(9600);
    
    // calibrate (must be stationary)
    mpu.CalibrateGyro();
    
    // wire library (an I2C communication library)
    Wire.begin();
    Serial.println("Initializing IMU...");
    
    // imu init
    mpu.initialize();

    if (mpu.testConnection()) {
        Serial.println("MPU6050 connection successful.");
    } else {
        Serial.println("IMU connection failed.");
        while (1);
    }
    Serial.println("DONE");

    imuDataMutex = xSemaphoreCreateMutex();

    //////////////////// Create tasks ////////////////////
    // xtaskCreate( task function pointer, 
    //              description, 
    //              stack size (words not bytes),
    //              parameters to task,
    //              priority,
    //              passes a handle to created task out of this function)
    
    xTaskCreate(TaskReadIMU, "IMU Read", 128, NULL, 2, NULL);
    xTaskCreate(TaskSendData, "Data Send", 128, NULL, 1, NULL);

    Serial.println("Done setup function");
}

void loop() {} // empty since we are using freeRTOS framework 

// Task to read data from IMU and calculate Roll, Pitch, and Yaw
void TaskReadIMU(void *pvParameters) {
    (void) pvParameters;

    while (1) {
        int16_t ax, ay, az;
        int16_t gx, gy, gz;

        mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);

        // Convert gyroscope values from raw to degrees/second
        float gyroXrate = gx / 131.0; // Assuming sensitivity is ±250 degrees/sec
        float gyroYrate = gy / 131.0;
        float gyroZrate = gz / 131.0;

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
        pitch = alpha * pitch + (1 - alpha) * accPitch + 1;
        // Store the calculated Roll, Pitch, Yaw values in shared variables
        // handle nan case
        if (isnan(pitch)){pitch=0.0;}
        xSemaphoreGive(imuDataMutex);

        // Delay to allow other tasks to run
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}

// Task to send Roll, Pitch, and Yaw data over Serial
void TaskSendData(void *pvParameters) {
    (void) pvParameters;

    while (1) {
        // Take the mutex to safely write Roll, Pitch, and Yaw data
        xSemaphoreTake(imuDataMutex, portMAX_DELAY);
        Serial.print(roll); Serial.print(',');
        Serial.print(pitch); Serial.print(',');
        Serial.println(yaw);
        xSemaphoreGive(imuDataMutex);

        // Delay for a short interval before sending data again
        vTaskDelay(500 / portTICK_PERIOD_MS);
    }
}