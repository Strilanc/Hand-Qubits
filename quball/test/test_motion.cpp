#include <stdio.h>
#include "TestUtils.h"
#include "ArduinoTestHarness.h"
#include "Wire.h"
#include "motion.h"

SoftwareSerial bluetoothSerial;

int16_t gyro_x;
int16_t gyro_y;
int16_t gyro_z;
int16_t accel_x;
int16_t accel_y;
int16_t accel_z;

void push_int_to_read(int16_t i) {
    uint16_t u = (uint16_t)i;
    uint8_t* b = (uint8_t*)(void*)&u;
    Wire.to_read.push_back(b[1]);
    Wire.to_read.push_back(b[0]);
}

uint8_t read_bluetooth_byte() {
    assert(!bluetoothSerial.test_harness_written.empty());
    uint8_t r = bluetoothSerial.test_harness_written.front();
    bluetoothSerial.test_harness_written.pop_front();
    return r;
}
    
float read_bluetooth_float() {
    uint8_t buf[4];
    for (int i = 0; i < 4; i++) {
        buf[i] = read_bluetooth_byte();
    }
    return *(float*)buf;
}

struct Report {
    uint8_t id;
    uint8_t peer_id;
    float qw;
    float qx;
    float qy;
    float qz;
    float ax;
    float ay;
    float az;
    float bumpiness;
};
Report read_bluetooth_motion_report() {
    assert(read_bluetooth_byte() == 0xA9);
    assert(read_bluetooth_byte() == 0x42);
    uint8_t id = read_bluetooth_byte();
    uint8_t peer_id = read_bluetooth_byte();
    float qw = read_bluetooth_float();
    float qx = read_bluetooth_float();
    float qy = read_bluetooth_float();
    float qz = read_bluetooth_float();
    float ax = read_bluetooth_float();
    float ay = read_bluetooth_float();
    float az = read_bluetooth_float();
    float bumpiness = read_bluetooth_float();
    return{ id, peer_id, qw, qx, qy, qz, ax, ay, az, bumpiness };
}

void handleMpu6050Requests() {
    while (Wire.written.size() > 0) {
        uint8_t b = Wire.written.front();
        Wire.written.pop_front();
        switch (b) {
        case 0x3B:
            push_int_to_read(accel_x);
            push_int_to_read(accel_y);
            push_int_to_read(accel_z);
            break;
        case 0x43:
            push_int_to_read(gyro_x);
            push_int_to_read(gyro_y);
            push_int_to_read(gyro_z);
            break;
        case 0x1B:
        case 0x1C:
        case 0x6B:
            // Also ignore the payload byte for these pair messages.
            Wire.written.pop_front();
        }
    }
}

void motion_loops_read_cycle() {
    motion_loop();
    handleMpu6050Requests();
    motion_loop();
    handleMpu6050Requests();
    motion_loop();
}

void motion_main() {

    test("reports_accumulated_motion", []() {
        motion_setup();

        // Calibrate.
        gyro_x = 0;
        gyro_y = 0;
        gyro_z = 0;
        accel_x = 0;
        accel_y = 0;
        accel_z = 1000;
        for (int i = 0; i < 10000; i++) {
            test_harness_advance_time(100);
            motion_loops_read_cycle();
            bluetoothSerial.test_harness_written.clear();
        }

        // Do a reading.
        gyro_x = 0;
        gyro_y = 0;
        gyro_z = 10000;
        accel_x = 0;
        accel_y = 0;
        accel_z = 10000;
        for (int i = 0; i < 100; i++) {
            test_harness_advance_time(100);
            motion_loops_read_cycle();
        }

        Report r = read_bluetooth_motion_report();

        assert(r.qw < 0.9999);
        assert(r.qx == 0);
        assert(r.qy == 0);
        assert(r.qz > 0.01);

        assert(r.ax == 0);
        assert(r.ay == 0);
        assert(r.az > 0);
    });

    printf("MOTION PASS\n");
}
