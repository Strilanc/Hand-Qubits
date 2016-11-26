#include "quaternion.h"
#include <math.h>

Quaternion Quaternion::from_angular_impulse(float wx, float wy, float wz) {
    float n = sqrtf(wx*wx + wy*wy + wz*wz);
	if (n < 0.00001) {
		return Quaternion{ 1, 0, 0, 0 };
	}
    float hangle = n / 2;
    float c = cosf(hangle);
    float s = sinf(hangle) / n;
    return Quaternion{ c, s*wx, s*wy, s*wz };
}

bool Quaternion::operator==(const Quaternion& q) const {
	return w == q.w && x == q.x && y == q.y && z == q.z;
}

Quaternion Quaternion::operator*(const Quaternion& q) const {
	return Quaternion{
		w*q.w - x*q.x - y*q.y - z*q.z,
		w*q.x + x*q.w + y*q.z - z*q.y,
		w*q.y - x*q.z + y*q.w + z*q.x,
		w*q.z + x*q.y - y*q.x + z*q.w 
	};
}

Quaternion Quaternion::operator+(const Quaternion& q) const {
	return Quaternion{
		w + q.w,
		x + q.x,
		y + q.y,
		z + q.z
	};
}

Quaternion Quaternion::operator-(const Quaternion& q) const {
	return Quaternion{
		w - q.w,
		x - q.x,
		y - q.y,
		z - q.z
	};
}

 Quaternion Quaternion::conj() const {
	return Quaternion{ w, -x, -y, -z };
}
