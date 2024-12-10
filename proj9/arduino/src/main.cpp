#include <Wire.h>
#include <MPU6050.h>
#include <Arduino_FreeRTOS.h>
#include <semphr.h> 

// Initialize the MPU6050 IMU object
MPU6050 mpu;


// set init and sim vals 
int16_t ax{0};
int16_t ay{0};
int16_t az{0};
int16_t gx{0};
int16_t gy{0};
int16_t gz{0};

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

}

void loop() {} // empty since we are using freeRTOS framework 

// Task to read data from IMU and calculate Roll, Pitch, and Yaw
void TaskReadIMU(void *pvParameters) {
    (void) pvParameters;

    while (1) {

        mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);

        // Convert gyroscope values from raw to degrees/second
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
        Serial.print(ax); Serial.print(',');
        Serial.print(ay); Serial.print(',');
        Serial.print(az); Serial.print(',');
        Serial.print(gx); Serial.print(',');
        Serial.print(gy); Serial.print(',');
        Serial.println(gz); 
        xSemaphoreGive(imuDataMutex);

        // Delay for a short interval before sending data again
        vTaskDelay(500 / portTICK_PERIOD_MS);
    }
}