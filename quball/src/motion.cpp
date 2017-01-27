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

static unsigned long last_gyro_read_time = 0;
static vec3 bias { 0, 0, 0 };
static float gyration = 0;
static int calibration_readings = 0;
static Quaternion pose { 1, 0, 0, 0 };
static int accumulate_count = 0;
static float bumpiness;

void setupMPU();

void motion_setup() {
  Wire.begin();
  Wire.setClock(400000L); // max supported by MPU-6050
  setupMPU();
}

int mpuAddress = 0b1101000;

void mpuSendPair(int a, int b) {
  Wire.beginTransmission(mpuAddress);
  Wire.write(a);
  Wire.write(b);
  Wire.endTransmission();  
}

void setupMPU(){
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

void mpuRequest(int reg, int len, int* dst) {
  Wire.beginTransmission(mpuAddress);
  Wire.write(reg);
  Wire.endTransmission();
  
  Wire.requestFrom(mpuAddress, len*2);
  while(Wire.available() < len*2) {
    // wait
  }
  for (int i = 0; i < len; i++) {
    dst[i] = Wire.read()<<8;
    dst[i] |= Wire.read();
  }
}

struct vec3 mpuRequestAccel() {
  int buf[3];
  mpuRequest(0x3B, 3, buf);
  struct vec3 r;
  r.x = buf[0];
  r.y = buf[1];
  r.z = buf[2];
  return r;
}

struct vec3 recordGyroRegisters() {
  int buf[3];
  mpuRequest(0x43, 3, buf);
  struct vec3 r;
  r.x = buf[0];
  r.y = buf[1];
  r.z = buf[2];
  return r;
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

Quaternion readNextGyroQuaternion() {
  // Grab angular velocities.
  vec3 v = recordGyroRegisters();

  // If still early, build up average to calibrate zero bias.
  float g = v.x*v.x + v.y*v.y + v.z*v.z;
  gyration = gyration * 0.9 + g * 0.1;
  if (gyration < 100000.0 && calibration_readings < 10000) {
    calibration_readings++;
    float a = 0.999;
    float b = 1.0 - a;
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
  unsigned long t = micros();
  int dt = (int)(t - last_gyro_read_time);
  last_gyro_read_time = t;

  // Estimated rotation over sample duration.
  float dw = dt * 3.14159265358979323846 / 180.0 / 1000.0 / 256.0 / 128.0;
  return Quaternion::from_angular_impulse(v.x * dw, v.y * dw, v.z * dw);
}

vec3 readNextAccel() {
  vec3 v = mpuRequestAccel();
  float f = 16.0 / 256.0 / 128.0;
  v.x *= f;
  v.y *= f;
  v.z *= f;
  return v;
}

void send_accumulated_motion_and_reset() {
  bluetoothSerial.write(0xA9);
  serialWriteQuaternion(pose);
  serialWriteFloat(bumpiness);
  bluetoothSerial.write(contact_get_byte_to_send());
  bluetoothSerial.write(contact_get_current_other_message());

  bumpiness = 0;
  pose = Quaternion { 1, 0, 0, 0 };
}

void motion_loop() {
  // Sending motion data constantly is too expensive. Build it up for 100 loops before doing that.
  if (accumulate_count >= 100) {
    send_accumulated_motion_and_reset();
    accumulate_count = 0;
  }
  accumulate_count++;

  // Accumulate rotations.
  pose = pose * readNextGyroQuaternion();

  // Accumulate estimate of whether we're slamming into a table or not.
  vec3 v = readNextAccel();
  bumpiness += v.x*v.x + v.y*v.y + v.z*v.z;
}
