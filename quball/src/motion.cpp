#include <Arduino.h>
#include <math.h>
#include <Wire.h>

#include "motion.h"
#include "contact.h"
#include "quaternion.h"

struct vec3 {
    float x;
    float y;
    float z;
};

#define mpuAddress 0b1101000
#define ACCEL_SCALE_FACTOR ((float)(1.0 / 16.0 / 256.0 / 128.0))
#define GYRO_TIME_SCALE_FACTOR ((float)(3.14159265358979323846f / 180 / 1000 / 256 / 128))

static uint64_t last_gyro_read_time;
static vec3 bias;
static float gyration;
static int calibration_readings;
static Quaternion pose;
static vec3 accel;
static int accumulate_count;
static float bumpiness;
static int request_state;
static int request_state_buf[6];

void setupMPU();

void motion_reset() {
    request_state = 0;
    last_gyro_read_time = 0;
    bias = { 0, 0, 0 };
    gyration = 0;
    calibration_readings = 0;
    pose = { 1, 0, 0, 0 };
    accel = { 0, 0, 0 };
    accumulate_count = 0;
    bumpiness = 0;
}

void motion_setup() {
    motion_reset();

    Wire.begin();
    Wire.setClock(400000L); // max supported by MPU-6050
    setupMPU();
}

void mpuSendPair(int a, int b) {
    Wire.beginTransmission(mpuAddress);
    Wire.write(a);
    Wire.write(b);
    Wire.endTransmission();
}

void setupMPU() {
    //Accessing the register 6B - Power Management (Sec. 4.28)
    //Setting SLEEP register to 0. (Required; see Note on p. 9)
    mpuSendPair(0x6B, 0);

    // Set gyro range.
    int REG_GYRO_RANGE = 0x1B;
    int FLAG_GYRO_RANGE_DEG_PER_SEC_250 = 0x00;
    int FLAG_GYRO_RANGE_DEG_PER_SEC_500 = 0x08;
    int FLAG_GYRO_RANGE_DEG_PER_SEC_1000 = 0x10;
    int FLAG_GYRO_RANGE_DEG_PER_SEC_2000 = 0x18;
    mpuSendPair(REG_GYRO_RANGE, FLAG_GYRO_RANGE_DEG_PER_SEC_1000);

    //Accessing the register 1C - Acccelerometer Configuration (Sec. 4.5) 
    //Setting the accel to +/- 16g
    mpuSendPair(0x1C, 0x18);
}

bool mpuTryMotionRequest(vec3& accel, vec3& gyro) {
    switch (request_state) {
    case 0:
        // Request accelerometer data.
        Wire.beginTransmission(mpuAddress);
        Wire.write(0x3B);
        Wire.endTransmission();
        Wire.requestFrom(mpuAddress, 6);
        request_state = 1;
    case 1:
        if (Wire.available() < 6) {
            return false;
        }
        for (int i = 0; i < 3; i++) {
            request_state_buf[i] = Wire.read() << 8;
            request_state_buf[i] |= Wire.read();
        }
        request_state = 2;
    case 2:
        // Request gyrometer data.
        Wire.beginTransmission(mpuAddress);
        Wire.write(0x43);
        Wire.endTransmission();
        Wire.requestFrom(mpuAddress, 6);
        request_state = 3;
    default:
        if (Wire.available() < 6) {
            return false;
        }
        for (int i = 0; i < 3; i++) {
            request_state_buf[i+3] = Wire.read() << 8;
            request_state_buf[i+3] |= Wire.read();
        }

        // Dump into values.
        accel.x = (float)request_state_buf[0] * ACCEL_SCALE_FACTOR;
        accel.y = (float)request_state_buf[1] * ACCEL_SCALE_FACTOR;
        accel.z = (float)request_state_buf[2] * ACCEL_SCALE_FACTOR;
        gyro.x = (float)request_state_buf[3];
        gyro.y = (float)request_state_buf[4];
        gyro.z = (float)request_state_buf[5];
        request_state = 0;
        return true;
    }
}

void serialWriteInt(int s) {
    bluetoothSerial.write((byte)(s & 0xFF));
    bluetoothSerial.write((byte)((s >> 8) & 0xFF));
}
void serialWriteFloat(float f) {
    bluetoothSerial.write((byte*)(void*)&f, 4);
}
void serialWriteVec3(vec3 v) {
    serialWriteFloat(v.x);
    serialWriteFloat(v.y);
    serialWriteFloat(v.z);
}
void serialWriteQuaternion(Quaternion q) {
    serialWriteFloat(q.w);
    serialWriteFloat(q.x);
    serialWriteFloat(q.y);
    serialWriteFloat(q.z);
}

Quaternion angularVelocitiesToQuaternion(vec3 v) {
    // If still early, build up average to calibrate zero bias.
    float g = v.x*v.x + v.y*v.y + v.z*v.z;
    gyration = gyration * 0.9f + g * 0.1f;
    if (gyration < 100000.0 && calibration_readings < 10000) {
        calibration_readings++;
        float a = 0.999f;
        float b = 1 - a;
        bias.x *= a;
        bias.y *= a;
        bias.z *= a;
        bias.x += b * v.x;
        bias.y += b * v.y;
        bias.z += b * v.z;
    }

    // Cancel out estimated bias.
    v.x -= bias.x;
    v.y -= bias.y;
    v.z -= bias.z;

    // Duration to integrate over.
    uint64_t t = micros();
    int dt = (int)(t - last_gyro_read_time);
    last_gyro_read_time = t;

    // Estimated rotation over sample duration.
    float dw = dt * GYRO_TIME_SCALE_FACTOR;
    return Quaternion::from_angular_impulse(v.x * dw, v.y * dw, v.z * dw);
}

void send_accumulated_motion_and_reset() {
    bluetoothSerial.write(0xA9);
    bluetoothSerial.write(0x42);
    bluetoothSerial.write(contact_get_byte_to_send());
    bluetoothSerial.write(contact_get_current_other_message());
    serialWriteQuaternion(pose);
    serialWriteVec3(accel);
    serialWriteFloat(bumpiness);

    bumpiness = 0;
    accel.x = 0;
    accel.y = 0;
    accel.z = 0;
    pose = Quaternion{ 1, 0, 0, 0 };
}

void motion_loop() {
    vec3 acc, gyr;
    if (!mpuTryMotionRequest(acc, gyr)) {
        return;
    }

    // Sending motion data constantly is too expensive. Build it up for 100 loops before doing that.
    accumulate_count++;
    if (accumulate_count >= 100) {
        send_accumulated_motion_and_reset();
        accumulate_count = 0;
    }

    // Accumulate rotations.
    pose = pose * angularVelocitiesToQuaternion(gyr);

    // Accumulate estimate of whether we're slamming into a table or not.
    accel.x += acc.x;
    accel.y += acc.y;
    accel.z += acc.z;
    bumpiness += acc.x*acc.x + acc.y*acc.y + acc.z*acc.z;
}
