#pragma once
#include <G3D/Vector3.h>
#include <G3D/Matrix3.h>
#include <G3D/CoordinateFrame.h>
#include "util/indexarray.h"
#include "v8kernel/SimBody.h"
#include "v8kernel/Link.h"
#include "util/PV.h"
#include "v8kernel/KernelIndex.h"
#include "v8kernel/Cofm.h"
#include "util/math.h"

namespace RBX {
	//class SimBody;
	class Body : KernelIndex
	{
		private:
			int& getIndex() {return index;};
			Body *root;
			Body *parent;
			int index;
			IndexArray<RBX::Body, &getIndex> children;
			Cofm *cofm;
			SimBody *simBody;
			bool canThrottle;
			RBX::Link *link;
			G3D::CoordinateFrame meInParent;
			G3D::Matrix3 moment;
			float mass;
			mutable int stateIndex;
			mutable PV pv;
			//int& getKernelIndex();
			const RBX::SimBody* getRootSimBody() {return getRoot()->simBody;};
			void resetRoot(RBX::Body* newRoot);
			bool validateParentCofmDirty();
			const G3D::CoordinateFrame& getMeInParent() const
			{
				RBXASSERT(getParent());
				return getLink() ? getLink()->getChildInParent() : meInParent;
			}
			void updatePV() const;
			void onChildAdded(RBX::Body* child);
			void onChildRemoved(RBX::Body* child);
			const RBX::Body* calcRootConst() const;
			__declspec(noinline) Body* Body::calcRoot()
			{
				return (getParent() ? getParent()->calcRoot() : this);
			}
			void matchDummy();
		public:
			int& getKernelIndex() {return kernelIndex;}
			Body();
			~Body();
			void step(float, bool);
			bool cofmIsClean();
			void makeCofmDirty();
			void advanceStateIndex();
			void makeStateDirty();
			int getStateIndex() const
			{
				updatePV();
				return stateIndex;
			}
			int numChildren() const {return children.size();}
			RBX::Body* getChild(int index) {return children[index];} //suppose to be const
			inline RBX::Body* getParent() const
			{
				return parent;
			}
			RBX::Link* getLink() const {return link;}
			const RBX::Body* getRootConst() const {return root;}
			RBX::Body* getRoot() {return root;}
			const G3D::Vector3& getCofmOffset() const
			{
				if (cofm)
				{
					return cofm->getCofmInBody();
				}
				else
				{
					return G3D::Vector3::zero();
				}
			}

			G3D::CoordinateFrame getMeInDescendant(const RBX::Body* descendant) const;
			G3D::CoordinateFrame getMeInRoot() const;
			float getMass() const {return mass;}
			G3D::Matrix3 getIBody() const
			{
				updatePV();
				return moment;
			}
			G3D::Vector3 getIBodyV3() const;
			G3D::Matrix3 getIWorld() const
			{
				return Math::momentToWorldSpace(getIBody(), pv.position.rotation);
			}
			G3D::Matrix3 getIWorldAtPoint(const G3D::Vector3& point) const;
			float getBranchMass() const
			{
				return (cofm ? cofm->getMass() : mass);
			}
			G3D::Matrix3 getBranchIBody() const
			{
				return cofm ? cofm->getMoment() : moment;
			}
			G3D::Vector3 getBranchIBodyV3() const
			{
				return Math::toDiagonal(getBranchIBody());
			}
			G3D::Matrix3 getBranchIWorld() const
			{
				updatePV();
				return Math::momentToWorldSpace(getBranchIBody(), pv.position.rotation);
			}
			G3D::Matrix3 getBranchIWorldAtPoint(const G3D::Vector3& point) const;
			G3D::Vector3 getBranchCofmPos() const;
			G3D::CoordinateFrame getBranchCofmCoordinateFrame() const;
			const G3D::Vector3& getPos() const
			{
				updatePV();
				return cofm->getCofmInBody();
			}
			const G3D::CoordinateFrame& getCoordinateFrame() const
			{
				updatePV();
				return pv.position;
			}
			const RBX::Velocity& getVelocity() const
			{
				updatePV();
				return pv.velocity;
			}
			const RBX::PV& getPV() const
			{
				updatePV();
				return pv;
			}
			const bool getCanThrottle() const {return this->canThrottle;}
			void accumulateForceAtCofm(const G3D::Vector3&);
			void accumulateForceAtBranchCofm(const G3D::Vector3& force)
			{
				RBXASSERT(root == this);

				if (root->simBody)
					root->simBody->accumulateForceCofm(force);
			}
			void accumulateForce(const G3D::Vector3& force, const G3D::Vector3& worldPos)
			{
				SimBody* simBody = root->simBody;
				if (simBody)
					simBody->accumulateForce(force, worldPos);
			}
			void accumulateTorque(const G3D::Vector3& torque)
			{
				if (root->simBody)
					root->simBody->accumulateTorque(torque);
			}
			void resetAccumulators()
			{
				if (root->simBody)
					root->simBody->resetAccumulators();
			}
			const G3D::Vector3& getBranchForce() const
			{
				if (root->simBody)
					return root->simBody->force;
				else
					return G3D::Vector3::zero();
			}
			const G3D::Vector3& getBranchTorque() const
			{
				if (root->simBody)
					return root->simBody->torque;
				else
					return G3D::Vector3::zero();
			}
			void setParent(Body* newParent);
			void setMeInParent(RBX::Link*);
			void setMeInParent(const G3D::CoordinateFrame& _meInParent);
			void setMass(float _mass);
			__declspec(noinline) void setMoment(const G3D::Matrix3& _momentInBody) // noinline for Primitive::setGridSize
			{
				if (moment != _momentInBody)
				{
					makeCofmDirty();
					moment = _momentInBody;
				}
			}
			void setPv(const PV& _pv);
			void setCoordinateFrame(const G3D::CoordinateFrame& worldCord);
			void setVelocity(const RBX::Velocity& worldVelocity);
			void setCanThrottle(bool);
			float kineticEnergy() const;
			float potentialEnergy() const;
			static int getNextStateIndex();
			static Body* getWorldBody();
	};
}