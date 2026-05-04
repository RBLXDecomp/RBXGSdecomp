#include "util/NormalId.h"
#include "util/Debug.h"

using namespace G3D;

namespace RBX
{
	NormalId intToNormalId(int num)
	{
		return (NormalId)num;
	}

	bool validNormalId(NormalId id)
	{
		return id >= NORM_X && id <= NORM_Z_NEG;
	}

	NormalId RBX::normalIdOpposite(NormalId normalId)
	{
		return (NormalId)((normalId + NORM_X_NEG) % NORM_UNDEFINED);
	}

	//35% match, too lazy to do better right now
	const Vector3& normalIdToVector3(NormalId normalId)
	{
		switch (normalId)
		{
			case NORM_X:
				{
				static Vector3 x_0(1.0f, 0.0f, 0.0f);
				return x_0;
				}
			case NORM_Y:
				{
				static Vector3 y_0(0.0f, 1.0f, 0.0f);
				return y_0;
				}
			case NORM_Z:
				{
				static Vector3 z_0(0.0f, 0.0f, 1.0f);
				return z_0;
				}
			case NORM_X_NEG:
				{
				static Vector3 xn_0(-1.0f, 0.0f, 0.0f);
				return xn_0;
				}
			case NORM_Y_NEG:
				{
				static Vector3 yn_0(0.0f, -1.0f, 0.0f);
				return yn_0;
				}
			case NORM_Z_NEG:
				{
				static Vector3 zn_0(0.0f, 0.0f, -1.0f);
				return zn_0;
				}
		}
		RBXASSERT(0);
		return G3D::Vector3::zero();
	}

	NormalId Vector3ToNormalId(const G3D::Vector3& v)
	{
		RBXASSERT((v == Vector3::unitX()) || (v == Vector3::unitY()) || (v == Vector3::unitZ()) || (v == -Vector3::unitX()) || (v == -Vector3::unitY()) || (v == -Vector3::unitZ()));
		if (v.x == 1.0f)
			return NORM_X;
		if (v.y == 1.0f)
			return NORM_Y;
		if (v.z == 1.0f)
			return NORM_Z;
		if (v.x == -1.0f)
			return NORM_X_NEG;
		if (v.y == -1.0f)
			return NORM_Y_NEG;
		if (v.z == -1.0f)
			return NORM_Z_NEG;
		RBXASSERT(0);
		return NORM_UNDEFINED;
	}

	NormalId Matrix3ToNormalId(const G3D::Matrix3& m)
	{
		return Vector3ToNormalId(m.getColumn(2));
	}

	NormalId inlinedFunction1(NormalId id)
	{
		switch (id)
		{
		case NORM_X:
			return NORM_Z_NEG;
		case NORM_Y:
			return NORM_X_NEG;
		case NORM_Z:
			return NORM_X;
		case NORM_X_NEG:
			return NORM_Z;
		case NORM_Y_NEG:
			return NORM_X;
		case NORM_Z_NEG:
			return NORM_X_NEG;
		default:
			RBXASSERT(0);
			return NORM_Y;
		}
	}

	NormalId inlinedFunction2(NormalId id)
	{
		switch (id)
		{
		case NORM_X:
			return NORM_Z;
		case NORM_Y:
			return NORM_Y;
		case NORM_Z:
			return NORM_Z;
		case NORM_X_NEG:
			return NORM_Z;
		case NORM_Y_NEG:
			return NORM_Y;
		case NORM_Z_NEG:
			return NORM_Z;
		default:
			RBXASSERT(0);
			return NORM_Y;
		}
	}

	const G3D::Matrix3& normalIdToMatrix3(NormalId normalId)
	{
		switch (normalId)
		{
		case NORM_X:
			{
				static const G3D::Matrix3 x = normalIdToMatrix3Internal(NORM_X);
				return x;
			}
		case NORM_Y:
			{
				static const G3D::Matrix3 y = normalIdToMatrix3Internal(NORM_Y);
				return y;
			}
		case NORM_Z:
			{
				static const G3D::Matrix3 z = normalIdToMatrix3Internal(NORM_Z);
				return z;
			}
		case NORM_X_NEG:
			{
				static const G3D::Matrix3 xn = normalIdToMatrix3Internal(NORM_X_NEG);
				return xn;
			}
		case NORM_Y_NEG:
			{
				static const G3D::Matrix3 yn = normalIdToMatrix3Internal(NORM_Y_NEG);
				return yn;
			}
		case NORM_Z_NEG:
			{
				static const G3D::Matrix3 zn = normalIdToMatrix3Internal(NORM_Z_NEG);
				return zn;
			}
		default:
			{
				RBXASSERT(0);
				return G3D::Matrix3::identity();
			}
		}
	}

	G3D::Matrix3 normalIdToMatrix3Internal(NormalId normalId)
	{
		G3D::Vector3 uInObject = uvwToObject(G3D::Vector3::unitX(), normalId);
		G3D::Vector3 vInObject = uvwToObject(G3D::Vector3::unitY(), normalId);
		G3D::Vector3 wInObject = uvwToObject(G3D::Vector3::unitZ(), normalId);

		RBXASSERT(uInObject == uvwToObject(objectToUvw(uInObject, normalId), normalId));
		RBXASSERT(vInObject == uvwToObject(objectToUvw(vInObject, normalId), normalId));
		RBXASSERT(wInObject == uvwToObject(objectToUvw(wInObject, normalId), normalId));

		RBXASSERT(uInObject == normalIdToVector3(inlinedFunction1(normalId)));
		RBXASSERT(vInObject == normalIdToVector3(inlinedFunction2(normalId)));
		RBXASSERT(wInObject == normalIdToVector3(normalId));

		return G3D::Matrix3(
			uInObject.x, vInObject.x, wInObject.x,
			uInObject.y, vInObject.y, wInObject.y,
			uInObject.z, vInObject.z, wInObject.z
		);
	}

	template<NormalId faceId>
	G3D::Vector3 uvwToObject(const G3D::Vector3& v)
	{
		return uvwToObject(v, faceId);
	}

	G3D::Vector3 uvwToObject(const G3D::Vector3& uvwPt, NormalId faceId)
	{
		switch (faceId)
		{
		case NORM_X:
			return G3D::Vector3(uvwPt.z, uvwPt.y, -uvwPt.x);
		case NORM_Y:
			return G3D::Vector3(-uvwPt.x, uvwPt.z, uvwPt.y);
		case NORM_Z:
			return G3D::Vector3(uvwPt.x, uvwPt.y, uvwPt.z);
		case NORM_X_NEG:
			return G3D::Vector3(-uvwPt.z, uvwPt.y, uvwPt.x);
		case NORM_Y_NEG:
			return G3D::Vector3(uvwPt.x, -uvwPt.z, uvwPt.y);
		case NORM_Z_NEG:
			return G3D::Vector3(-uvwPt.x, uvwPt.y, -uvwPt.z);
		default:
			return G3D::Vector3::unitX();
		}
	}

	G3D::Vector3 objectToUvw(const G3D::Vector3& objectPt, NormalId faceId)
	{
		switch (faceId)
		{
		case NORM_X:
			return G3D::Vector3(-objectPt.z, objectPt.y, objectPt.x);
		case NORM_Y:
			return G3D::Vector3(-objectPt.x, objectPt.z, objectPt.y);
		case NORM_Z:
			return G3D::Vector3(objectPt.x, objectPt.y, objectPt.z);
		case NORM_X_NEG:
			return G3D::Vector3(objectPt.z, objectPt.y, -objectPt.x);
		case NORM_Y_NEG:
			return G3D::Vector3(objectPt.x, objectPt.z, -objectPt.y);
		case NORM_Z_NEG:
			return G3D::Vector3(-objectPt.x, objectPt.y, -objectPt.z);
		default:
			return G3D::Vector3::unitX();
		}
	}

	G3D::Vector3 mapToUvw_Legacy(const G3D::Vector3& ptInObject, NormalId faceId)
	{
		return uvwToObject(ptInObject, faceId);
	}
}