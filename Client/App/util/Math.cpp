#include "util/Math.h"
#include "util/Debug.h"
#include <boost/functional/hash/hash.hpp>
#include <limits>
#include <cmath>

#define LODWORD(x) (*(DWORD*)(&x))

namespace RBX
{
	const G3D::Matrix3& Math::matrixRotateY()
	{
		static G3D::Matrix3 rotateY(0, 0, 1, 0, 1, 0, -1, 0, 0);
		return rotateY;
	}

	const G3D::Matrix3& Math::matrixTiltZ()
	{
		static G3D::Matrix3 tiltZ(0, 1, 0, -1, 0, 0, 0, 0, 1);
		return tiltZ;
	}

	bool Math::isDenormal(float f)
	{
		return (LODWORD(f) & 0x7F800000) == 0 && (LODWORD(f) & 0x7FFFFF) != 0;
	}

	bool Math::isNanInfDenorm(float f)
	{
		return f == std::numeric_limits<float>::infinity()
			|| f == -std::numeric_limits<float>::infinity()
			|| f == std::numeric_limits<float>::quiet_NaN()
			|| f == std::numeric_limits<float>::signaling_NaN()
			|| Math::isDenormal(f);
	}

	bool Math::isNanInfDenormVector3(const G3D::Vector3& v)
	{
		return Math::isNanInfDenorm(v.x)
			|| Math::isNanInfDenorm(v.y)
			|| Math::isNanInfDenorm(v.z);
	}

	bool Math::fixDenorm(G3D::Vector3& v)
	{
		bool fixed = false;

		float x = v.x;
		if (Math::isDenormal(x))
		{
			v.x = 0;
			fixed = true;
		}

		float y = v.y; 
		if (Math::isDenormal(y))
		{
			v.y = 0;
			fixed = true;
		}

		float z = v.z;
		if (Math::isDenormal(z))
		{
			v.z = 0;
			fixed = true;
		}

		return fixed;
	}

	float Math::radWrap(float rad)
	{
		if (rad >= -pi() && rad < pi())
			return rad;

		float rot = 1/twoPi();
		float f = floor((rad + pi()) * rot);
		return rad - Math::iRound(f) * twoPi();
	}

	G3D::Vector3 Math::sortVector3(const G3D::Vector3& v)
	{
		G3D::Vector3 result = v;

		if ( result.z < result.y)
		{
			std::swap(result.z, result.y);
		}

		if ( result.y < result.x  )
		{
			std::swap(result.y, result.x);
		}

		if ( result.z < result.y  )
		{
			std::swap(result.z, result.y);
		}

		return result;
	}

	G3D::Matrix3 Math::momentToObjectSpace(const G3D::Matrix3& iWorld, const G3D::Matrix3& bodyRotation)
	{
		return bodyRotation.transpose() * iWorld * bodyRotation;
	}

	G3D::Matrix3 Math::momentToWorldSpace(const G3D::Matrix3& iBody, const G3D::Matrix3& bodyRotation)
	{
		return (bodyRotation * iBody) * bodyRotation.transpose();
	}

	G3D::Vector3 Math::toDiagonal(const G3D::Matrix3& m)
	{
		return G3D::Vector3(m[0][0],m[1][1],m[2][2]);
	}

	G3D::Matrix3 Math::fromDiagonal(const G3D::Vector3& v)
	{
		return G3D::Matrix3(v.x, 0, 0, 0, v.y, 0, 0, 0, v.z);
	}

	bool Math::lessThan(const G3D::Vector3& min, const G3D::Vector3& max)
	{
		return min.x < max.x && min.y < max.y && min.z < max.z;
	}

	bool Math::isAxisAligned(const G3D::Matrix3& matrix)
	{
		for (int i = 0; i < 3; ++i)
		{
			for (int j = 0; j < 3; ++j)
			{
				float f = matrix[i][j];
				if (f != 0 && f != 1 && f != -1)
					return false;
			}
		}

		return true;
	}

	G3D::Matrix3 Math::rotateAboutZ(const G3D::Matrix3& matrix, float radians)
	{
		G3D::Matrix3 m(G3D::Matrix3::identity());
		float sinR = sin(radians);
		float cosR = cos(radians);

		m.setColumn(0, G3D::Vector3(cosR, sinR, 0));
		m.setColumn(1, G3D::Vector3(-sinR, cosR, 0));

		return matrix * m;
	}

	void Math::rotateMatrixAboutY90(G3D::Matrix3& matrix, int times)
	{
		static G3D::Matrix3 y90(0, 0, 1, 0, 1, 0, -1, 0, 0);

		for (int i = 0; i < times; ++i)
		{
			matrix = y90 * matrix;
		}
	}

	float Math::getHeading(const G3D::Vector3& look)
	{
		return atan2(-look.x, -look.z);
	}

	float Math::getElevation(const G3D::Vector3& look)
	{
		return asin(look.y);
	}

	void Math::getHeadingElevation(const G3D::CoordinateFrame& c, float& heading, float& elevation)
	{
		G3D::Vector3 lookVector = c.getLookVector();

		heading = Math::getHeading(lookVector);
		elevation = Math::getElevation(lookVector);
	}

	void Math::setHeadingElevation(G3D::CoordinateFrame& c, float heading, float elevation)
	{
		float Y = sin(elevation);
		float unk = sqrtf(1 - Y * Y);
		float X = -(sin(heading) * unk);
		float Z = -(unk * cos(heading));
		c.lookAt(c.translation + G3D::Vector3(X, Y, Z).direction());
	}

	G3D::CoordinateFrame Math::getFocusSpace(const G3D::CoordinateFrame& focus)
	{
		G3D::Vector3 look = focus.lookVector();
		float heading = Math::getHeading(look);

		G3D::CoordinateFrame cf(focus.rotation);
		cf.translation = focus.translation;
		Math::setHeadingElevation(cf, heading, 0);
		return cf;
	}

	const G3D::Matrix3& Math::getAxisRotationMatrix(int face)
	{
		static G3D::Matrix3 y = G3D::Matrix3::fromEulerAnglesXYZ(0, 0, G3D::halfPi());
		static G3D::Matrix3 z = G3D::Matrix3::fromEulerAnglesXYZ(0, G3D::halfPi(), 0);
		static G3D::Matrix3 y_neg = G3D::Matrix3(y);
		static G3D::Matrix3 z_neg = G3D::Matrix3(z);

		switch (face)
		{
		case 1:
			return y;
		case 2:
			return z;
		case 4:
			return y_neg;
		case 5:
			return z_neg;
		default:
			return G3D::Matrix3::identity();
		}
	}

	G3D::Vector3 Math::vector3Abs(const G3D::Vector3& v)
	{
		return G3D::Vector3(fabs(v.x), fabs(v.y), fabs(v.z));
	}

	bool Math::isOrthonormal(const G3D::Matrix3& m)
	{
		return m.isOrthonormal();
	}

	bool Math::orthonormalizeIfNecessary(G3D::Matrix3& m)
	{
		if (!Math::isOrthonormal(m))
		{
			m.orthonormalize();
			return true;
		}

		return false;
	}

	size_t Math::hash(const G3D::Vector3& v)
	{
		const int magic = 1640531527;
		size_t h;

		h = (boost::hash_value(v.x) - magic);
		h = (boost::hash_value(v.y) - magic + (h >> 2) + (h << 6)) ^ h;
		h = (boost::hash_value(v.z) - magic + (h >> 2) + (h << 6)) ^ h;
		return h;
	}

	float Math::angle(const G3D::Vector3& v0, const G3D::Vector3& v1)
	{
		float f = v0.z * v1.z + v0.y * v1.y + v0.x * v1.x;
		if (f >= 1)
			return 0;
		if (f <= -1)
			return pi();
		return acos(f);
	}

	float Math::angleToE0(const G3D::Vector2& v)
	{
		G3D::Vector2 copy(v);
		copy.unitize();

		float f = acos(copy.x);
		if (copy.y < 0)
			return twoPi() - f;
		return f;
	}

	G3D::Vector3 Math::iRoundVector3(const G3D::Vector3& point)
	{
		return G3D::Vector3(Math::iRound(point.x), Math::iRound(point.y), Math::iRound(point.z));
	}

	const float& segSizeRadians()
	{
		// - 0.00001f is necessary to get the same value as the compiled binary
		static float ssr = (Math::pi() - 0.00001f) / 128;
		return ssr;
	}

	G3D::uint8 rotationToByteBase(float angle)
	{
		RBXASSERT(angle <= Math::pi());
		RBXASSERT(angle >= -Math::pi());

		float result = (angle + Math::pi()) / segSizeRadians();
		int resInt = G3D::iRound(result);
		
		RBXASSERT(resInt >= -1);
		RBXASSERT(resInt <= 256);

		resInt = std::max(0, resInt);
		resInt = std::min(255, resInt);

		G3D::uint8 byte = (G3D::uint8)resInt;

		RBXASSERT(byte <= 255);

		return byte;
	}

	G3D::uint8 Math::rotationToByte(float angle)
	{
		return rotationToByteBase(angle);
	}

	float Math::rotationFromByte(unsigned char byteAngle)
	{
		float byteAngleFloat = byteAngle;
		return byteAngleFloat * segSizeRadians() - pi();
	}

	G3D::Vector3 Math::toGrid(const G3D::Vector3& v, float grid)
	{
		return Math::toGrid(v, G3D::Vector3(grid, grid, grid));
	}

	G3D::Vector3 Math::toGrid(const G3D::Vector3& v, const G3D::Vector3& grid)
	{
		G3D::Vector3 units = v / grid;
		return grid * Math::iRoundVector3(units);
	}

	G3D::Matrix3 Math::getIWorldAtPoint(const G3D::Vector3& cofmPos, const G3D::Vector3& worldPos, const G3D::Matrix3& iWorldAtCofm, float mass)
	{
		G3D::Vector3 delta = worldPos - cofmPos;
		float deltaXSquared = delta.x * delta.x;
		float deltaYSquared = delta.y * delta.y;
		float deltaZSquared = delta.z * delta.z;
		float deltaYX = delta.y * delta.x;
		float deltaZX = delta.z * delta.x;
		float deltaZY = delta.z * delta.y;

		return iWorldAtCofm + mass * G3D::Matrix3(
				deltaZSquared + deltaYSquared,
				-deltaYX,
				-deltaZX,
				-deltaYX,
				deltaZSquared + deltaXSquared,
				-deltaZY,
				-deltaZX,
				-deltaZY,
				deltaYSquared + deltaXSquared
				);
	}

	bool Math::intersectRayPlane(const G3D::Ray& ray, const G3D::Plane& plane, G3D::Vector3& hit)
	{
		float dotProd = ray.direction.dot(plane.normal());
		if (plane.halfSpaceContains(ray.origin))
		{
			if (!(dotProd < 0.0f))
			{
				hit = G3D::Vector3::inf();
				return false;
			}
		}
		else if (!(dotProd > 0.0f))
		{
			hit = G3D::Vector3::inf();
			return false;
		}
		G3D::Line myLine = G3D::Line::fromPointAndDirection(ray.origin, ray.direction);
		hit = myLine.intersection(plane);
		return true;
	}

	int Math::getOrientId(const G3D::Matrix3& matrix)
	{
		NormalId ColumnNormId = Vector3ToNormalId(matrix.getColumn(0));
		return Vector3ToNormalId(matrix.getColumn(1)) + 6 * ColumnNormId;
	}

	float Math::maxAxisLength(const G3D::Vector3& v)
	{
		G3D::Vector3 vAbs = G3D::Vector3(fabs(v.x), fabs(v.y), fabs(v.z));
		return std::max(vAbs.x, std::max(vAbs.y, vAbs.z));
	}

	NormalId Math::getClosestObjectNormalId(const G3D::Vector3& worldV, const G3D::Matrix3& objectR)
	{
		G3D::Vector3 worldObjMul = worldV * objectR;
		float absX = fabs(worldObjMul.x);
		float absY = fabs(worldObjMul.y);
		float absZ = fabs(worldObjMul.z);
		if (absX > absY)
		{
			if (absX > absZ)
			{
				return worldObjMul.x > 0 ? NORM_X : NORM_X_NEG;
			}
			else
			{
				return worldObjMul.z > 0 ? NORM_Z : NORM_Z_NEG;
			}
		}
		else if (absY > absZ)
		{
			return worldObjMul.y > 0 ? NORM_Y : NORM_Y_NEG;
		}
		return worldObjMul.z > 0 ? NORM_Z : NORM_Z_NEG;
	}

	bool Math::fuzzyAxisAligned(const G3D::Matrix3& m0, const G3D::Matrix3& m1, float radTolerance)
	{
		G3D::Vector3 vectors[3] = {G3D::Vector3(), G3D::Vector3(), G3D::Vector3()};

		for (int i = 0; i < 3; i++)
		{
			vectors[i] = m1.getColumn(i);
		}

		for (int i = 0; i < 3; i++)
		{
			G3D::Vector3 m0Column = m0.getColumn(i);
			G3D::Vector3* vector = vectors; 
			int count = 0;
			while (count < 3)
			{
				if (m0Column.cross(*vector).magnitude() < radTolerance)
					break;
				count++;
				vector++;
				if (count >= 3)
					return false;
			}
		}
		return true;
	}

	void Math::idToMatrix3(int orientId, G3D::Matrix3& matrix)
	{
		NormalId norm1 = intToNormalId(orientId / 6);
		NormalId norm2 = intToNormalId(orientId % 6);
		G3D::Vector3 norm1Vec3 = normalIdToVector3(norm1);
		G3D::Vector3 norm2Vec3 = normalIdToVector3(norm2);
		G3D::Vector3 cross = norm1Vec3.cross(norm2Vec3);
		matrix.setColumn(0, norm1Vec3);
		matrix.setColumn(1, norm2Vec3);
		matrix.setColumn(2, cross);
	}

	bool Math::legalCameraCoord(const G3D::CoordinateFrame& c)
	{
		int i = 0;
		const float* transMatArr = (const float*)c.translation;
		const float* rotMatArr = (const float*)c.rotation;
		for (; i < 3; i++, transMatArr++, rotMatArr += 4)
		{
			float rotValue = *rotMatArr;
			for (int j = 0; j < 3; j++)
			{
				if (!(rotValue > -1.2f && rotValue < 1.2f))
					return false;
			}
			float transValue = *transMatArr;
			if (!(transValue > -1000000.0f && transValue < 1000000.0f))
				return false;
		}
		return true;
	}

	G3D::Matrix3 Math::snapToAxes(const G3D::Matrix3& align)
	{
		G3D::Matrix3 unkMat;
		float aBest = 0.0f;
		int second = -1;
		int bestV = -1;
		
		for (int i = 0; i < 3; i++)
		{
			G3D::Vector3 loopColumn = align.getColumn(i);
			for (int j = 0; j < 3; j++)
			{
				float dotProd = G3D::Matrix3::identity().getColumn(j).dot(loopColumn);
				unkMat[i][j] = dotProd;
				if (fabs(dotProd) > fabs(aBest))
				{
					bestV = i;
					second = j;
					aBest = dotProd;
				}
			}
		}
		
		G3D::Vector3 myColumn = G3D::Matrix3::identity().getColumn(second);
		if (aBest < 0.0f)
		{
			myColumn *= -1.0f;
		}

		int v10 = -1;
		int v11 = -1;
		float v9 = 0.0f;
		
		for (int i = 0; i < 3; i++)
		{
			if (i != bestV)
			{
				for (int j = 0; j < 3; j++)
				{
					if (j != second)
					{
						if (fabs(unkMat[i][j]) > fabs(v9))
						{
							v9 = unkMat[i][j];
							v10 = i;
							v11 = j;
						}
					}
				}
			}
		}

		G3D::Vector3 myColumn2 = G3D::Matrix3::identity().getColumn(v11);
		if (v9 < 0.0f)
		{
			myColumn2 *= -1.0f;
		}

		int v15 = 3 - v10 - bestV;
		int calcColmTemp = 3 - v11 - second;
		G3D::Vector3 myColumn3 = G3D::Matrix3::identity().getColumn(calcColmTemp);
		if (unkMat[v15][v11] < 0.0f)
		{
			myColumn3 *= -1.0f;
		}

		G3D::Matrix3 result;
		result.setColumn(bestV, myColumn);
		result.setColumn(v10, myColumn2);
		result.setColumn(v15, myColumn3);
		return result;
	}

	bool Math::fuzzyEq(float f0, float f1, float epsilon)
	{
		float num1 = fabs(f0) + 1;
		float num2 = fabs(f0 - f1);

		return (f0 == f1 || num2 <= num1 * epsilon);
	}

	bool Math::fuzzyEq(const G3D::Vector3& v0, const G3D::Vector3& v1, float epsilon)
	{
		for (int i = 0; i < 3; i++)
		{
			if (!Math::fuzzyEq(v0[i], v1[i], epsilon))
				return false;
		}

		return true;
	}

	bool Math::fuzzyEq(const G3D::Matrix3& m0, const G3D::Matrix3& m1, float epsilon)
	{
		for (int i = 0; i < 3; i++)
		{
			for (int j = 0; j < 3; j++)
			{
				if (!Math::fuzzyEq(m0[i][j], m1[i][j], epsilon))
					return false;
			}
		}

		return true;
	}

	bool Math::fuzzyEq(const G3D::CoordinateFrame& c0, const G3D::CoordinateFrame& c1, float epsT, float epsR)
	{
		bool tRes = Math::fuzzyEq(c0.translation, c1.translation, epsT);
		if (!tRes)
			return false;

		bool rRes = Math::fuzzyEq(c0.rotation, c1.rotation, epsR);
		return rRes ? true : false;
	}

	G3D::CoordinateFrame Math::snapToGrid(const G3D::CoordinateFrame& snap, const G3D::Vector3& grid)
	{
		return G3D::CoordinateFrame(Math::snapToAxes(snap.rotation), Math::toGrid(snap.translation, grid));
	}

	G3D::CoordinateFrame Math::snapToGrid(const G3D::CoordinateFrame& snap, float grid)
	{
		return G3D::CoordinateFrame(Math::snapToAxes(snap.rotation), Math::toGrid(snap.translation, grid));
	}

	G3D::Matrix3 Math::alignAxesClosest(const G3D::Matrix3& align, const G3D::Matrix3& target)
	{
		float dots[3][3];
		int maxI = -1, maxJ = -1;
		float maxDot = 0.0f;

		for(int i = 0; i < 3; i++)
		{
			G3D::Vector3 aAxis = align.getColumn(i);
			for(int j = 0; j < 3; j++)
			{
				G3D::Vector3 tAxis = target.getColumn(j);
				float atDot = aAxis.dot(tAxis);
				dots[i][j] = atDot;

				if(fabs(atDot) > fabs(maxDot))
				{
					maxI = i;
					maxJ = j;
					maxDot = atDot;
				}
			}
		}

		float mult = (maxDot >= 0.0f) ? 1.0f : -1.0f;
		G3D::Vector3 bestV = align.getColumn(maxI) * mult;

		float maxDot1 = 0.0f;
		int maxI1 = -1, maxJ1 = -1;

		for(int i = 0; i < 3; i++)
		{
			if(i != maxI)
				for(int j = 0; j < 3; j++)
				{
					if(j != maxJ && fabs(dots[i][j]) > fabs(maxDot1))
					{
						maxDot1 = dots[i][j];
						maxI1 = i;
						maxJ1 = j;
					}
				}
		}
		
		float mult1 = (maxDot1 >= 0.0f) ? 1.0f : -1.0f;
		G3D::Vector3 secondV = align.getColumn(maxI1) * mult1;

		int maxI2 = (3 - maxI1) - maxI;
		int maxJ2 = (3 - maxJ1) - maxJ;

		float maxDot2 = dots[maxI2][maxJ2];
		float mult2 = (maxDot2 >= 0.0f) ? 1.0f : -1.0f;

		G3D::Vector3 thirdV = align.getColumn(maxI2) * mult2;

		G3D::Matrix3 result;
		result.setColumn(maxJ, bestV);
		result.setColumn(maxJ1, secondV);
		result.setColumn(maxJ2, thirdV);

		return result;
	}
}
