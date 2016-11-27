#include <Wire.h>
#include <math.h>
#include "quaternion.h"

struct vec3 {
  float x;
  float y;
  float z;
};

void setup() {
  Serial.begin(9600);
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
  
  Wire.requestFrom(mpuAddress, len*2); //Request Gyro Registers (43 - 48)
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
  Serial.write((byte)(s & 0xFF));
  Serial.write((byte)((s >> 8) & 0xFF));
}
void serialWriteFloat(float f) {
  Serial.write((byte*)(void*)&f, 4);
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

unsigned long last_gyro_read_time = 0;
vec3 bias { 0, 0, 0 };
float gyration = 0;
int calibration_readings = 0;
Quaternion readNextGyroQuaternion() {
  vec3 v = recordGyroRegisters();
  float g = v.x*v.x + v.y*v.y + v.z*v.z;
  gyration = gyration * 0.9 + g * 0.1;
  if (gyration < 10000.0 && calibration_readings < 10000) {
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

  v.x -= bias.x;
  v.y -= bias.y;
  v.z -= bias.z;

  unsigned long t = micros();
  int dt = (int)(t - last_gyro_read_time);
  last_gyro_read_time = t;

  float dw = dt * 1000.0 * 3.14159265358979323846 / 180.0 / 1000.0 / 1000.0 / 256.0 / 128.0;
  return Quaternion::from_angular_impulse(v.x * dw, v.y * dw, v.z * dw);
}

float bumpiness;
vec3 readNextAccel() {
  vec3 v = mpuRequestAccel();
  float f = 16.0 / 256.0 / 128.0;
  v.x *= f;
  v.y *= f;
  v.z *= f;
  bumpiness += v.x*v.x + v.y*v.y + v.z*v.z;
  return v;
}

Quaternion pose;
void loop() {
  bumpiness = 0;
  pose = Quaternion { 1, 0, 0, 0 };
  for (int i = 0; i < 100; i++) {
    pose = pose * readNextGyroQuaternion();
    readNextAccel();
  }
  
  Serial.write(0xA9);
  serialWriteQuaternion(pose);
  serialWriteFloat(bumpiness);
  serialWriteFloat(0);
  serialWriteFloat(0);
}

