#pragma once
#include <G3D/CoordinateFrame.h>
//#include "v8kernel/Body.h"

namespace RBX
{
	class Body;
	class Link
	{
		protected:
			RBX::Body *body;
			G3D::CoordinateFrame parentCoord;
			G3D::CoordinateFrame childCoord;
			G3D::CoordinateFrame childCoordInverse;
			mutable G3D::CoordinateFrame childInParent;
			mutable int stateIndex;
			virtual void computeChildInParent(G3D::CoordinateFrame& answer) const = 0;
			void dirty();
			//void setBody(RBX::Body* _body){ body = _body;}
		public:
			Link();
			//~Link();
			const G3D::CoordinateFrame& getChildInParent() const;
			Body* getBody() const;
			void reset(const G3D::CoordinateFrame&, const G3D::CoordinateFrame&);
			void setBody(RBX::Body* _body){ body = _body;} // this isn't suppose to be in public but then i can't access it in Body::setParent()
	};

	class RevoluteLink : public Link
	{
	private:
		float jointAngle;
  
	private:
		virtual void computeChildInParent(G3D::CoordinateFrame& answer) const;
	public:
		//RevoluteLink(const RevoluteLink&);
		RevoluteLink() : jointAngle(0.0f) {}
	public:
		void setJointAngle(float value)
		{
			jointAngle = value;
			dirty();
		}
	public:
		~RevoluteLink() {}
		//RBX::RevoluteLink& operator=(const RevoluteLink&);
	};
}