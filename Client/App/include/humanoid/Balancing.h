#pragma once
#include "humanoid/Humanoid.h"
#include "util/HitTestFilter.h"

namespace RBX
{
	class Balancing : public Humanoid::State, public HitTestFilter
	{
	private:
		const float kP;
		const float kD;
	protected:
		boost::shared_ptr<PartInstance> floorPart;
		G3D::Vector3 floorTouchInWorld;

	private:
		PartInstance* findFloor(bool);
		PartInstance* tryFloor(const G3D::Ray& ray, float maxDistance);
		void balance(const float, const G3D::Vector3&);
	protected:
		virtual void onComputeForce(const float);
		virtual Humanoid::State* onStep(const float stepDt, Controller& controller);
		virtual HitTestFilter::Result filterResult(const Primitive*) const;
		G3D::Vector3 getYAxis() const;
	public:
		Balancing(Humanoid* humanoid, const float kP, const float kD);

	protected:
		static const float maxTorqueComponent();
		static const float fallCutoff();
	};
}
