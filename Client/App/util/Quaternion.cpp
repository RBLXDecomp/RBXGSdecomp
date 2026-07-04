#include "util/Quaternion.h"

namespace RBX
{
	// see: https://web.archive.org/web/20041019044742/http://www.wild-magic.com:80/Source/Math/WmlQuaternion.inl

	Quaternion::Quaternion(const G3D::Matrix3& rot)
	{
		float fTrace = rot[0][0] + rot[1][1] + rot[2][2];
		float fRoot;

		if (fTrace > 0.0f)
		{
			fRoot = sqrt(fTrace + 1);
			this->w = 0.5f * fRoot;
			fRoot = 0.5f / fRoot;
			this->x = (rot[2][1] - rot[1][2]) * fRoot;
			this->y = (rot[0][2] - rot[2][0]) * fRoot;
			this->z = (rot[1][0] - rot[0][1]) * fRoot;
		}
		else
		{
			if (rot[0][0] > rot[1][1] && rot[0][0] > rot[2][2])
			{
				fRoot = -sqrt(1.0f + rot[0][0] - rot[1][1] - rot[2][2]);
				this->x = 0.5f * fRoot;
				fRoot = 0.5f / fRoot;
				this->y = (rot[1][0] + rot[0][1]) * fRoot;
				this->z = (rot[2][0] + rot[0][2]) * fRoot;
				this->w = (rot[2][1] - rot[1][2]) * fRoot;
			}
			else
			{
				if (rot[1][1] > rot[2][2])
				{
					fRoot = -sqrt(1.0f + rot[1][1] - rot[0][0] - rot[2][2]);
					this->y = 0.5f * fRoot;
					fRoot = 0.5f / fRoot;
					this->x = (rot[1][0] + rot[0][1]) * fRoot;
					this->z = (rot[2][1] + rot[1][2]) * fRoot;
					this->w = (rot[0][2] - rot[2][0]) * fRoot;
				}
				else
				{
					fRoot = -sqrt(1.0f + rot[2][2] - rot[0][0] - rot[1][1]);
					this->z = 0.5f * fRoot;
					fRoot = 0.5f / fRoot;
					this->x = (rot[2][0] + rot[0][2]) * fRoot;
					this->y = (rot[2][1] + rot[1][2]) * fRoot;
					this->w = (rot[1][0] - rot[0][1]) * fRoot;
				}
			}
		}
	}

	Quaternion& Quaternion::operator=(const Quaternion& other)
	{
		this->w = other.w;
		this->x = other.x;
		this->y = other.y;
		this->z = other.z;
		return *this;
	}

	//needs to be moved to the header
	Quaternion Quaternion::operator*(const Quaternion& other) const
	{
		G3D::Vector3& thisConvert = *(G3D::Vector3*)this;
		G3D::Vector3& otherConvert = *(G3D::Vector3*)&other;
		float thisW = this->w;
		float otherW = other.w;

		return Quaternion(otherConvert * thisW + thisConvert * otherW + thisConvert.cross(otherConvert), thisW * otherW - thisConvert.dot(otherConvert));
	}

	float& Quaternion::operator[](int i) const
	{
		return ((float*)this)[i];
	}

	void Quaternion::normalize()
	{
		float sqY = G3D::square(this->y);
		float v1 = sqY + G3D::square(this->x) + G3D::square(this->z) + G3D::square(this->w);
		float v3 = G3D::rsqrt(v1);
		this->x = this->x * v3;
		this->y = this->y * v3;
		this->z = this->z * v3;
		this->w = this->w * v3;
	}

	void Quaternion::toRotationMatrix(G3D::Matrix3& rot) const
	{
		float fTx  = 2.0f * this->x;
		float fTy  = 2.0f * this->y;
		float fTz  = 2.0f * this->z;
		float fTxx = fTx * this->x;
		float fTxy = fTy * this->x;
		float fTxz = fTz * this->x;
		float fTwx = fTx * this->w;
		float fTwy = fTy * this->w;
		float fTwz = fTz * this->w;
		float fTyy = fTy * this->y;
		float fTyz = fTz * this->y;
		float fTzz = fTz * this->z;

		rot[0][0] = 1.0 - (fTyy + fTzz);
		rot[0][1] = fTxy - fTwz;
		rot[0][2] = fTxz + fTwy;
		rot[1][0] = fTxy + fTwz;
		rot[1][1] = 1.0 - (fTxx + fTzz);
		rot[1][2] = fTyz - fTwx;
		rot[2][0] = fTxz - fTwy;
		rot[2][1] = fTyz + fTwx;
		rot[2][2] = 1.0 - (fTxx + fTyy);
	}

	//needs to be moved to the header
	float Quaternion::maxComponent() const
	{
		Quaternion qAbs = Quaternion(fabs(w), fabs(z), fabs(y), fabs(x));
		return std::max(std::max(qAbs.w, qAbs.z), std::max(qAbs.y, qAbs.x));
	}
}