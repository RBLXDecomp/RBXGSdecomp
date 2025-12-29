#pragma once
#include "reflection/property.h"
#include "v8world/Controller.h"
#include "v8xml/XmlElement.h"
#include "util/SurfaceType.h"
#include "util/NormalId.h"

namespace RBX
{
	class PartInstance;

	class Surface
	{
	private:
		PartInstance* partInstance;
		NormalId surfId;
	  
	public:
		void flat();
	public:
		Surface(PartInstance*, NormalId);
	public:
		PartInstance* getPartInstance() const;
		NormalId getNormalId() const;
		bool isControllable() const;
		SurfaceType getSurfaceType() const;
		void setSurfaceType(SurfaceType);
		Controller::InputType getInput() const;
		void setSurfaceInput(Controller::InputType);
		float getParamA() const;
		float getParamB() const;
		void setParamA(float);
		void setParamB(float);
		void toggleSurfaceType();
		void writeValue(XmlElement*) const;
		void readValue(const XmlElement*, IReferenceBinder&);
	};
}
