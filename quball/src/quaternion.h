#ifndef QUATERNION_H
#define QUATERNION_H

class Quaternion {
public:
  float w, x, y, z;

  static Quaternion from_angular_impulse(float wx, float wy, float wz);
  bool operator ==(const Quaternion& q) const;
  Quaternion operator *(const Quaternion& q) const;
  Quaternion operator +(const Quaternion& q) const;
  Quaternion operator -(const Quaternion& q) const;
  Quaternion conj() const;
};

#endif

