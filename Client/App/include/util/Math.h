#pragma once
#include <G3D/Vector3.h>
#include <G3D/Matrix3.h>
#include <G3D/CoordinateFrame.h>
#include <G3D/GCamera.h>
#include <G3D/Line.h>
#include <G3D/Plane.h>
#include <G3D/Ray.h>
#include "util/NormalId.h"
#include "util/Velocity.h"

namespace RBX 
{
	class Math { /* Size=0x1 */
		public:
			static const int maxOrientationId;
			static const int minOrientationId;

			static const float pi() {return 3.1415927f;}
			static const float piHalf() {return 1.5707964f;}
			static const float twoPi() {return 6.283185f;}
			static const float& inf()
			{
				static float inf = std::numeric_limits<float>::infinity();
				return inf;
			}
			static inline int iRound(float fvalue) {return G3D::iRound(fvalue);}
			static int iFloor(float);
			static float polarity(float);
			static float sign(float fValue)
			{
				// G3D::sign but it supports floats
				if (fValue > 0.0f) {
					return 1.0f;
				}

				if (fValue < 0.0f) {
					return -1.0f;
				}

				return 0.0f;
			}
			static bool isDenormal(float f);
			static bool isNanInfDenorm(float f);
			static bool isNanInfDenormVector3(const G3D::Vector3& v);
			static bool isNanInfDenormMatrix3(const G3D::Matrix3&);
			static bool fixDenorm(G3D::Vector3& v);
			static bool fixDenorm(float&);
			static bool fuzzyEq(const G3D::CoordinateFrame& c0, const G3D::CoordinateFrame& c1, float epsT, float epsR);
			static bool fuzzyEq(const G3D::Matrix3& m0, const G3D::Matrix3& m1, float epsilon);
			static bool fuzzyEq(const G3D::Vector3& v0, const G3D::Vector3& v1, float epsilon);
			static bool fuzzyEq(float, float, float);
			static bool fuzzyAxisAligned(const G3D::Matrix3& m0, const G3D::Matrix3& m1, float radTolerance);
			static bool isEven(int);
			static bool isOdd(int);
			static int nextEven(int);
			static int nextOdd(int);
			static float angleToE0(const G3D::Vector2& v);
			static size_t hash(const G3D::Vector3& v);
			static bool isIntegerVector3(const G3D::Vector3&);
			static G3D::Vector3 iRoundVector3(const G3D::Vector3& point);
			static float angle(const G3D::Vector3& v0, const G3D::Vector3& v1);
			static float smallAngle(const G3D::Vector3&, const G3D::Vector3&);
			static float elevationAngle(const G3D::Vector3&);
			static G3D::Vector3 vector3Abs(const G3D::Vector3& v);
			static float volume(const G3D::Vector3&);
			static float maxAxisLength(const G3D::Vector3& v);
			static G3D::Vector3 sortVector3(const G3D::Vector3& v);
			static RBX::Velocity calcTrajectory(const G3D::Vector3&, const G3D::Vector3&, float);
			static G3D::Vector3 toGrid(const G3D::Vector3& v, float grid);
			static G3D::Vector3 toGrid(const G3D::Vector3& v, const G3D::Vector3& grid);
			static bool lessThan(const G3D::Vector3& min, const G3D::Vector3& max);
			static __forceinline float longestVector3Component(const G3D::Vector3& v)
			{
				return std::max(fabs(v.x), std::max(fabs(v.y), fabs(v.z)));
			}
			static float planarSize(const G3D::Vector3& size)
			{
				if (size.x < size.y)
				{
					if (size.x < size.z)
						return size.y * size.z;
					else
						return size.y * size.x;
				}
				else
				{
					if (size.y < size.z)
						return size.x * size.z;
					else
						return size.x * size.y;
				}
			};
			static float taxiCabMagnitude(const G3D::Vector3& v) {return fabs(v.x) + fabs(v.y) + fabs(v.z);}
			static const G3D::Plane& yPlane();
			static G3D::Vector3 closestPointOnRay(const G3D::Ray&, const G3D::Ray&);
			static bool cameraSeesPoint(const G3D::Vector3&, const G3D::GCamera&);
			static bool legalCameraCoord(const G3D::CoordinateFrame& c);
			static G3D::Vector3 toSmallAngles(const G3D::Matrix3&);
			static G3D::Matrix3 snapToAxes(const G3D::Matrix3& align);
			static bool isOrthonormal(const G3D::Matrix3& m);
			static bool orthonormalizeIfNecessary(G3D::Matrix3& m);
			static G3D::Vector3 toFocusSpace(const G3D::Vector3&, const G3D::CoordinateFrame&);
			static G3D::Vector3 fromFocusSpace(const G3D::Vector3&, const G3D::CoordinateFrame&);
			static G3D::Vector3 toDiagonal(const G3D::Matrix3& m);
			static G3D::Matrix3 fromDiagonal(const G3D::Vector3& v);
			static G3D::Vector3 getColumn(const G3D::Matrix3& m, int c)
			{
				return G3D::Vector3(m[0][c], m[1][c], m[2][c]);
			}
			static unsigned char rotationToByte(float angle);
			static float rotationFromByte(unsigned char byteAngle);
			static bool isAxisAligned(const G3D::Matrix3& matrix);
			static int getOrientId(const G3D::Matrix3& matrix);
			static void idToMatrix3(int orientId, G3D::Matrix3& matrix);
			static const G3D::Matrix3& matrixRotateY();
			static const G3D::Matrix3& matrixRotateNegativeY();
			static const G3D::Matrix3& matrixTiltZ();
			static const G3D::Matrix3& matrixTiltNegativeZ();
			static const G3D::Matrix3 matrixTiltQuadrant(int);
			static void rotateMatrixAboutX90(G3D::Matrix3&, int);
			static void rotateMatrixAboutY90(G3D::Matrix3& matrix, int times);
			static void rotateMatrixAboutZ90(G3D::Matrix3&);
			static G3D::Matrix3 rotateAboutZ(const G3D::Matrix3& matrix, float radians);
			static G3D::Matrix3 momentToObjectSpace(const G3D::Matrix3& iWorld, const G3D::Matrix3& bodyRotation);
			static G3D::Matrix3 momentToWorldSpace(const G3D::Matrix3& iBody, const G3D::Matrix3& bodyRotation);
			static G3D::Matrix3 getIWorldAtPoint(const G3D::Vector3& cofmPos, const G3D::Vector3& worldPos, const G3D::Matrix3& iWorldAtCofm, float mass);
			static void rotateAboutYLocal(G3D::CoordinateFrame&, float);
			static void rotateAboutYGlobal(G3D::CoordinateFrame&, float);
			static G3D::CoordinateFrame snapToGrid(const G3D::CoordinateFrame& snap, const G3D::Vector3& grid);
			static G3D::CoordinateFrame snapToGrid(const G3D::CoordinateFrame& snap, float grid);
			static float zAxisAngle(const G3D::CoordinateFrame&);
			static void pan(const G3D::Vector3&, G3D::CoordinateFrame&, float);
			static int radiansToQuadrant(float);
			static int radiansToOctant(float);
			static float radiansToDegrees(float);
			static float degreesToRadians(float);
			static float getHeading(const G3D::Vector3& look);
			static float getElevation(const G3D::Vector3& look);
			static void getHeadingElevation(const G3D::CoordinateFrame& c, float& heading, float& elevation);
			static void setHeadingElevation(G3D::CoordinateFrame& c, float heading, float elevation);
			static G3D::CoordinateFrame getFocusSpace(const G3D::CoordinateFrame& focus);
			static int toYAxisQuadrant(const G3D::CoordinateFrame&);
			static G3D::Matrix3 alignAxesClosest(const G3D::Matrix3&, const G3D::Matrix3&);
			static RBX::NormalId getClosestObjectNormalId(const G3D::Vector3& worldV, const G3D::Matrix3& objectR);
			static G3D::Vector3 getWorldNormal(RBX::NormalId id, const G3D::CoordinateFrame& coord)
			{
				return (1 - (id / 3) * 2) * Math::getColumn(coord.rotation, id % 3);
			}
			static G3D::Vector3 getWorldNormal(RBX::NormalId id, const G3D::Matrix3& mat)
			{
				return(1 - (id / 3) * 2) * Math::getColumn(mat, id % 3);
			}
			static float radWrap(float rad);
			static const G3D::Matrix3& getAxisRotationMatrix(int face);
			static G3D::Vector3 vectorToObjectSpace(const G3D::Vector3& _vec, const G3D::Matrix3& _mat)
			{
				return _vec * _mat;
			}
			static bool clipRay(G3D::Vector3&, G3D::Vector3&, G3D::Vector3*, G3D::Vector3&);
			static bool intersectLinePlane(const G3D::Line&, const G3D::Plane&, G3D::Vector3&);
			static bool intersectRayPlane(const G3D::Ray& ray, const G3D::Plane& plane, G3D::Vector3& hit);
			static float computeLaunchAngle(float, float, float, float);
	};
}