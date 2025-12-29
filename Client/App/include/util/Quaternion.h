#pragma once
#include <G3D/Vector3.h>
#include <G3D/Matrix3.h>

namespace RBX {
	class Quaternion
	{
		public:
			float x;
			float y;
			float z;
			float w;
			Quaternion(const G3D::Matrix3& rot);
			Quaternion(): x(0), y(0), z(0), w(1) {}
			Quaternion(const G3D::Vector3& vec, float fNum):x(vec.x),y(vec.y),z(vec.z),w(fNum) {}
			Quaternion(float _x, float _y, float _z, float _w):x(_x),y(_y),z(_z),w(_w) {}
			Quaternion& operator=(const Quaternion& other);
			G3D::Vector3& imag();
			const G3D::Vector3& imag() const;
			void toRotationMatrix(G3D::Matrix3& rot) const;
			float dot(const Quaternion& other) const
			{
				return (other.w * w + other.z *z + other.y * y + other.x * x);
			}
			float magnitude() const { return sqrtf(x * x + y * y + z * z + w * w);}
			float maxComponent() const;
			float& operator[](int index) const;
			operator float *();
			operator const float *() const;
			Quaternion operator*(float fNum) const
			{
				return Quaternion(x * fNum, y * fNum, z * fNum, w * fNum);
			}
			Quaternion operator*(const Quaternion& other) const;
			Quaternion operator+(const Quaternion& other) const
			{
				return Quaternion(x + other.x, y + other.y, z + other.z, w + other.w);
			}
			Quaternion operator-(const Quaternion& other) const
			{
				return Quaternion(x - other.x, y - other.y, z - other.z, w - other.w);
			}
			Quaternion& operator*=(float fNum)
			{
				x = fNum * x;
				y = fNum * y;
				z = fNum * z;
				w = fNum * w;
				return *this;
			}
			Quaternion& operator+=(const Quaternion& other)
			{
				x = x + other.x;
				y = y + other.y;
				z = z + other.z;
				w = w + other.w;
				return *this;
			}
			void normalize();
	};
}