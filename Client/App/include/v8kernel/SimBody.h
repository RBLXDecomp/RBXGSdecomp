#pragma once
#include <G3D/Vector3.h>
#include "util/Quaternion.h"
#include "util/PV.h"
//#include "v8kernel/Body.h"

namespace RBX {
	class Body;

	class SimBody
	{
		private:
		  void clearAccumulators();
		  void update();
		  void updateIfDirty()
		  {
			if (dirty)
				update();
		  }
		public:
			Body *body;
			bool dirty;
			PV pv;
			Quaternion qOrientation;
			G3D::Vector3 angMomentum;
			G3D::Vector3 momentRecip;
			float massRecip;
			float constantForceY;
			G3D::Vector3 force;
			G3D::Vector3 torque;
			//funcs
			__forceinline void SimBody::unkSimInline(float fNum);
			//void SimBody(const RBX::SimBody&);
			SimBody(RBX::Body* body);
			~SimBody();
			void step(float dt);
			void makeDirty() {dirty = true;};
			bool getDirty() const {return dirty;};
			PV getOwnerPV();
			const PV& getPV() {return pv;}
			void accumulateForceCofm(const G3D::Vector3& _force)
			{
				updateIfDirty();
				force += _force;	
			};
			inline static G3D::Vector3 accumulateInline(const G3D::Vector3& _force, const G3D::Vector3& pvWorld, const G3D::Vector3& worldPos)
			{
				G3D::Vector3 temp = worldPos - pvWorld;
				return temp.cross(_force);
			}
			void accumulateForce(const G3D::Vector3& _force, const G3D::Vector3& worldPos)
			{
				updateIfDirty();
				force += _force;
				torque += accumulateInline(_force, pv.position.translation, worldPos);
			}
			void accumulateTorque(const G3D::Vector3& _torque)
			{
				updateIfDirty();
				torque += _torque;	
			}
			void SimBody::resetAccumulators()
			{
				updateIfDirty();
				force = G3D::Vector3(0.0f, constantForceY, 0.0f);
				torque = G3D::Vector3(0.0f, 0.0f, 0.0f);
			}
			const G3D::Vector3& getForce() const {return force;}
			const G3D::Vector3& getTorque() const {return torque;}
			void matchDummy();
	};
}