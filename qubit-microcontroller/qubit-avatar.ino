#include <Wire.h>

struct vec3 {
  float x;
  float y;
  float z;
};

void setup() {
  Serial.begin(9600);
  Wire.begin();
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

  //Accessing the register 1B - Gyroscope Configuration (Sec. 4.4) 
  //Setting the gyro to full scale +/- 250deg./s 
  mpuSendPair(0x1B, 0);

  //Accessing the register 1C - Acccelerometer Configuration (Sec. 4.5) 
  //Setting the accel to +/- 2g
  mpuSendPair(0x1C, 0);
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
    dst[i] += Wire.read();
  }
}

struct vec3 mpuRequestAccel() {
  int buf[3];
  mpuRequest(0x3B, 3, buf);
  struct vec3 r;
  r.x = buf[0] / 16384.0;
  r.y = buf[1] / 16384.0; 
  r.z = buf[2] / 16384.0;
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

void printData(struct vec3 rot, struct vec3 acc) {
  Serial.print("Gyro X:");
  Serial.println(rot.x);
  Serial.print("Gyro Y:");
  Serial.println(rot.y);
  Serial.print("Gyro Z:");
  Serial.println(rot.z);

  Serial.print("Accel X:");
  Serial.println(acc.x);
  Serial.print("Accel Y:");
  Serial.println(acc.y);
  Serial.print("Accel Z:");
  Serial.println(acc.z);
}

void loop() {
  vec3 acc = mpuRequestAccel();
  vec3 rot = recordGyroRegisters();
  printData(rot, acc);
  //delay(100);
}

