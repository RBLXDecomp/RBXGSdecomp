#include "v8kernel/Connector.h"
#include "util/Debug.h"

namespace RBX
{
	void ContactConnector::computeForce(const float dt, bool throttling)
	{
		RBXASSERT(!throttling || !this->canThrottle());

		PairParams params;
		this->geoPair.computeLengthNormalPosition(params);

		if (params.length < 0.f)
		{
			float normalVel;
			G3D::Vector3 perpVel;
			this->geoPair.computeNormalPerpVel(normalVel, perpVel, params);
			this->frictionOffset += perpVel * dt;

			float v8 = this->k * 0.2f;
			float mag = this->frictionOffset.magnitude() * v8;
			float v10 = this->kFriction * this->forceMagLast;
			if (v10 < mag && mag > 0.00000001)
			{
				float v14 = v10 / mag;
				this->frictionOffset *= v14; 
			}

			float v18 = this->frictionOffset.dot(params.normal);
			this->frictionOffset -= params.normal * v18;

			float newThreshold;
			if (this->threshold != 0)
			{
				newThreshold = (this->threshold + 0.01f) * 0.999f - 0.01f;
			}
			else
			{
				if (this->firstApproach == 0)
					newThreshold = 0;
				else
					newThreshold = params.length;
			}
			this->threshold = newThreshold;

			float newFirstApproach;
			if (this->firstApproach != 0)
			{
				newFirstApproach = (this->firstApproach + 0.01f) * 0.999f - 0.01f;
			}
			else
			{
				newFirstApproach = params.length;
			}
			this->firstApproach = newFirstApproach;

			float kApplied = perpVel.x < 0 ? this->k : this->kNeg;
			float v27 = (this->firstApproach - params.length) * kApplied;
			this->forceMagLast = v27;
			this->forceMagLast = this->threshold <= params.length ? 0.f : this->forceMagLast;

			G3D::Vector3 v31 = this->frictionOffset * v8;
			G3D::Vector3 force = (params.normal * this->forceMagLast) - v31;
			this->geoPair.forceToBodies(force, params.position); 
		}
		else
		{
			this->firstApproach = 0;
			this->threshold = 0;
			this->forceMagLast = 0;
		}
	}

	bool ContactConnector::canThrottle() const
	{
		return this->geoPair.getBody(0)->getCanThrottle() && this->geoPair.getBody(1)->getCanThrottle();
	}

	void PointToPointBreakConnector::forceToPoints(const G3D::Vector3& force)
	{
		this->point0->accumulateForce(-force);
		this->point1->accumulateForce(force);
		RBXASSERT(force.magnitude() < Math::inf());
	}

	void PointToPointBreakConnector::computeForce(const float dt, bool throttling)
	{
		if (!this->broken)
		{
			G3D::Vector3 force = (this->point1->getWorldPos() - this->point0->getWorldPos()) * -this->k;

			float taxi = Math::taxiCabMagnitude(force);
			this->broken = taxi > this->breakForce;
			this->forceToPoints(force);
		}
	}

	float PointToPointBreakConnector::potentialEnergy()
	{
		G3D::Vector3 diff = this->point1->getWorldPos() - this->point0->getWorldPos();
		float mag = diff.length();
		return this->k * mag * mag * 0.5f;
	}

	RotateConnector::RotateConnector(Point* base0, Point* ray0, Point* ref0, Point* ref1, float kValue, float armLength)
		: base0(base0),
		  ray0(ray0),
		  ref0(ref0),
		  ref1(ref1),
  		  k(kValue * armLength * armLength),
		  lastRotation(0),
		  windings(0),
		  kernelInput()
	{}

	void RotateConnector::computeForce(const float dt, bool throttling)
	{
		G3D::Vector3 normal;
		float rotation;
		float rotVel;
		this->computeParams(normal, rotation, rotVel);

		this->kernelInput.setGoal(rotation);

		float lastGoal = 0;
		float currentGoal;
		float increment;
		this->kernelInput.get(lastGoal, currentGoal, increment);

		//float v1 = (currentGoal - rotation) * this->k;
		G3D::Vector3 v2 = normal * ((currentGoal - rotation) * this->k);

		this->ref0->getBody()->accumulateTorque(-v2);
		this->ref1->getBody()->accumulateTorque(v2);
	}

	void RotateConnector::computeParams(G3D::Vector3& normal, float& rotation, float& rotVel)
	{
		G3D::Vector3 ray0base0_delta = this->ray0->getWorldPos() - this->base0->getWorldPos();
		normal = ray0base0_delta;
		normal.unitize();
		G3D::Vector3 ref0base0_delta = this->ref0->getWorldPos() - this->base0->getWorldPos();
		G3D::Vector3 ref1base0_delta = this->ref1->getWorldPos() - this->base0->getWorldPos();

		float funnyCalculation1 = normal.dot(ref0base0_delta);
		float funnyCalculation2 = normal.dot(ref1base0_delta);

		G3D::Vector3 ref0base0_delta_funny = ref0base0_delta - (normal * funnyCalculation1);
		G3D::Vector3 ref1base0_delta_funny = ref1base0_delta - (normal * funnyCalculation2);

		int oldWindings = this->windings;

		G3D::Vector3 cross = ref0base0_delta_funny.cross(ref1base0_delta_funny);
		float garbage2 = ref1base0_delta_funny.dot(ref0base0_delta_funny);

		float result = atan2(cross.z * cross.z + cross.y * cross.y + cross.x * cross.x, garbage2);

		if (this->lastRotation > G3D::halfPi())
		{
			if (result < -G3D::halfPi()) //line numbers show that this is an actual line
				this->windings = oldWindings + 1; // these could be very likely temp values 
		}									   //that are later written to actual class variable according to original asm
		else if (result < -G3D::halfPi())
		{
			if (result < G3D::halfPi())
				this->windings = oldWindings - 1;
		}

		rotation = this->windings * Math::twoPi() + result;// line 69 and line 70
		rotVel = rotation - (Math::twoPi() * oldWindings + this->lastRotation);
		this->lastRotation = result; //line 71
	}

	void NormalBreakConnector::computeForce(const float dt, bool throttling)
	{
		if ( !this->broken )
		{
			const G3D::Vector3& worldNormal = Math::getWorldNormal(this->normalIdBody0, this->point0->getBody()->getPV().position);
			const G3D::Vector3& force = -this->k * (this->point1->getWorldPos() - this->point0->getWorldPos());
			
			float forceDot = force.dot(worldNormal);
			this->broken = -forceDot > this->breakForce;
			this->forceToPoints(force);
		}
	}
}
