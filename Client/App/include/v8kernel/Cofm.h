#pragma once
#include <G3D/Vector3.h>
#include <G3D/Matrix3.h>
//#include "v8kernel/Body.h"

namespace RBX {
	class Body;
	class Cofm
	{
		private:
			Body *body;
			mutable bool dirty;
			mutable G3D::Vector3 cofmInBody;
			mutable float mass;
			mutable G3D::Matrix3 moment;
			void updateIfDirty() const;
		public:
			Cofm::Cofm(Body* body);
			const G3D::Vector3& getCofmInBody() const;
			bool getIsDirty() const {return dirty;}
			void makeDirty() {dirty = true;}
			const float getMass() const;
			const G3D::Matrix3& getMoment() const;
	};
}