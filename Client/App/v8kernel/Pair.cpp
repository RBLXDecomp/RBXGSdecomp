#include "v8kernel/Pair.h"
#include "util/Math.h"
#include "util/Debug.h"

namespace RBX
{
	GeoPair::GeoPair()
	{
		this->body0 = NULL;
		this->body1 = NULL;
		this->offset0 = NULL;
		this->offset1 = NULL;
	}

	void GeoPair::computeNormalPerpVel(float& normalVel, G3D::Vector3& perpVel, const PairParams& _params)
	{
		G3D::Vector3 body0Velocity = this->body0->getPV().linearVelocityAtPoint(_params.position);
		G3D::Vector3 body1Velocity = this->body1->getPV().linearVelocityAtPoint(_params.position);
		body1Velocity -= body0Velocity;

		float body1VelDot = _params.normal.dot(body1Velocity);
		normalVel = body1VelDot;
		G3D::Vector3 normalVelMul = _params.normal * body1VelDot;
		perpVel = body1Velocity - normalVelMul;
	}

	void GeoPair::forceToBodies(const G3D::Vector3& _force, const G3D::Vector3& _position)
	{
		this->body0->accumulateForce(-_force, _position);
		this->body1->accumulateForce(_force, _position);
		RBXASSERT(fabs(_force.x) < Math::inf());
	}

	void GeoPair::computeBallBall(PairParams& _params)
	{
		const PV& body0PV = this->body0->getPV();
		const PV& body1PV = this->body1->getPV();

		const G3D::Vector3& body0Trans = body0PV.position.translation;
		const G3D::Vector3& body1Trans = body1PV.position.translation;

		_params.normal = body1Trans - body0Trans;
		_params.length = _params.normal.unitize() - this->pairData.radiusSum;
		_params.position = _params.normal * this->pairData.radius0 + body0Trans;
	}

	void GeoPair::computeBallPoint(PairParams& _params)
	{
		const G3D::Vector3& body0Trans = this->body0->getPV().position.translation;
		const G3D::CoordinateFrame& body1Pos = this->body1->getPV().position;

		const G3D::Vector3& myoffset1 = *this->offset1;
		_params.position = body1Pos.pointToWorldSpace(myoffset1);
		_params.normal = _params.position - body0Trans;
		_params.length = _params.normal.unitize() - this->pairData.radius0;
	}

	G3D::Vector3 somemaththing(const G3D::Vector3& normal, const G3D::Vector3& trans)
	{
		return trans - normal * normal.dot(trans);
	}

	void GeoPair::computeBallEdge(PairParams& _params)
	{
		const G3D::Vector3& body0PV = this->body0->getPV().position.translation;
		const G3D::CoordinateFrame& body1PV = this->body1->getPV().position;
		const G3D::Vector3& myoffset1 = *this->offset1;
		const G3D::Vector3& worldPoint = body1PV.pointToWorldSpace(myoffset1);

		const G3D::Vector3& normal = Math::getWorldNormal(this->pairData.normalID1, body1PV);

		const G3D::Vector3& temp = worldPoint - body0PV;
		_params.normal = somemaththing(normal, temp);
		_params.position = _params.normal + body0PV;
		_params.length = _params.normal.unitize() - this->pairData.radius0;
	}

	void GeoPair::computeBallPlane(RBX::PairParams& _params)
	{
		const G3D::Vector3& body0PV = this->body0->getPV().position.translation;
		const G3D::CoordinateFrame& body1PV = this->body1->getPV().position;
		const G3D::Vector3& myoffset1 = *this->offset1;
		const G3D::Vector3& worldPoint = body1PV.pointToWorldSpace(myoffset1);

		const G3D::Vector3& normal = -Math::getWorldNormal(this->pairData.normalID1, body1PV);

		_params.normal = normal;

		const G3D::Vector3& temp = worldPoint - body0PV;
		G3D::Vector3 someMathThing = normal.dot(temp) * normal;
		_params.position = body0PV + someMathThing;
		_params.length = someMathThing.length() - this->pairData.radius0;
	}

	void GeoPair::computePointPlane(RBX::PairParams& _params)
	{
		_params.position = this->body0->getPV().position.pointToWorldSpace(*offset0);
		const G3D::CoordinateFrame& body1PV = this->body1->getPV().position;
		G3D::Vector3 body1worldSpace = body1PV.pointToWorldSpace(*offset1);
		_params.normal = -Math::getWorldNormal(this->pairData.normalID1, body1PV);
		_params.length = _params.normal.dot(body1worldSpace - _params.position);
	}

	// is this actually an inline?
	__forceinline float unkClampInline(float d, float max)
	{
		if (fabs(d) > max)
			return max * Math::sign(d);

		return d;
	}
	
	void GeoPair::computeEdgeEdgePlane(RBX::PairParams& _params)
	{
		const G3D::CoordinateFrame& body0PV = this->body0->getPV().position;
		const G3D::CoordinateFrame& body1PV = this->body1->getPV().position;

		//world space defines
		G3D::Vector3 body0worldSpace = body0PV.pointToWorldSpace(*this->offset0);
		G3D::Vector3 body1worldSpace = body1PV.pointToWorldSpace(*this->offset1);
		//normals
		G3D::Vector3 body0Normal = Math::getWorldNormal(this->pairData.normalID0, body0PV);
		G3D::Vector3 body1Normal = Math::getWorldNormal(this->pairData.normalID1, body1PV);
		//calcs
		G3D::Vector3 bodyWorldSpaceDelta = body1worldSpace - body0worldSpace;
		float bodyNormalDot = body0Normal.dot(body1Normal);
		float compareResult = 1.0f - bodyNormalDot * bodyNormalDot;
		_params.normal = -Math::getWorldNormal(this->pairData.planeID, body1PV.rotation);
		if ( compareResult > 0.00001 )
		{
			float body1Dot = -bodyWorldSpaceDelta.dot(body1Normal);
			float body0Dot = bodyWorldSpaceDelta.dot(body0Normal);
			float theSinner = (bodyNormalDot + (body1Dot * body0Dot)) / compareResult;
			theSinner = unkClampInline(theSinner, 6.0f);
			_params.position = body0Normal * theSinner + body0worldSpace;
			_params.length = _params.normal.dot(body1worldSpace - _params.position);
		}
		else
		{
			_params.position = body0worldSpace;
			_params.length = 0.0;
		}
	}

	void GeoPair::computeEdgeEdge(RBX::PairParams& _params)
	{
		const G3D::CoordinateFrame& body0coord = this->body0->getPV().position;
		const G3D::CoordinateFrame& body1coord = this->body1->getPV().position;

		//world space defines
		G3D::Vector3 body0worldSpace = body0coord.pointToWorldSpace(*this->offset0);
		G3D::Vector3 body1worldSpace = body1coord.pointToWorldSpace(*this->offset1);
		//normals
		G3D::Vector3 body0Normal = Math::getWorldNormal(this->pairData.normalID0, body0coord);
		G3D::Vector3 body1Normal = Math::getWorldNormal(this->pairData.normalID1, body1coord);
		//calcs
		G3D::Vector3 bodyWorldSpaceDelta = body1worldSpace - body0worldSpace;
		float bodyNormalDot = body0Normal.dot(body1Normal);
		float body0Dot = bodyWorldSpaceDelta.dot(body0Normal);
		float body1Dot = -bodyWorldSpaceDelta.dot(body1Normal);
		float compareResult = 1.0f - bodyNormalDot * bodyNormalDot;
		if ( compareResult > 0.00001f )
		{
			float bodyDotCalc1 = (body0Dot + body1Dot * bodyNormalDot) * (1.0f / compareResult);
			float bodyDotCalc2 = (body1Dot + body0Dot * bodyNormalDot) * (1.0f / compareResult);
			const G3D::Vector3 body0NormalDotCalc = body0worldSpace + (body0Normal * bodyDotCalc1);
			const G3D::Vector3 body1NormalDotCalc = body1worldSpace + (body1Normal * bodyDotCalc2);
			_params.position = (body1NormalDotCalc + body0NormalDotCalc) * 0.5f;
			_params.normal = body0NormalDotCalc - body1NormalDotCalc;
			_params.length = -_params.normal.unitize();

			
		}
		else
		{
			_params.position = body0worldSpace;
			_params.normal = body0Normal;
			_params.length = 0.0f;
		}
	}
}
