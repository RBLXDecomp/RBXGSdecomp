#pragma once
#include <G3D/Vector3.h>
#include "v8kernel/Body.h"

namespace RBX {
	class Point : RBX::KernelIndex
	{
	public: // this is meant to be private
		int numOwners;
	protected:
		RBX::Body *body;
		G3D::Vector3 localPos;
		G3D::Vector3 worldPos;
		G3D::Vector3 force;
	public: // this is meant to be private
		int& getKernelIndex() {return this->kernelIndex;}
	public:
		Point(Body* _body); // TODO: the ctor is supposed to be private but Kernel::newPoint uses these ctors
		Point(const Point& _point);
		virtual ~Point() {}

		void step();
		void accumulateForce(const G3D::Vector3& _force) {this->force += _force;}
		void forceToBody();
		void setLocalPos(const G3D::Vector3& _localPos);
		void setWorldPos(const G3D::Vector3& _worldPos);
		void setBody(RBX::Body* _body) { this->body = _body; }
		RBX::Body* getBody() {return this->body;}
		const G3D::Vector3& getWorldPos() {return worldPos;}
		//Point& operator=(const Point&);

		static bool Point::sameBodyAndOffset(const RBX::Point& p1, const RBX::Point& p2)
		{
			return p1.body == p2.body && p1.localPos == p2.localPos;
		}
	};
}