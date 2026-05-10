#pragma once
#include "v8world/Controller.h"
#include "v8xml/XmlElement.h"
#include "util/SurfaceType.h"
#include "util/NormalId.h"

namespace RBX
{
	class PartInstance;
	class IReferenceBinder;

	class Surface
	{
	private:
		PartInstance* partInstance;
		NormalId surfId;
	  
	public:
		void flat();
	public:
		Surface(PartInstance* partInstance, NormalId surfId);
	public:
		PartInstance* getPartInstance() const;
		NormalId getNormalId() const;
		bool isControllable() const;
		SurfaceType getSurfaceType() const;
		void setSurfaceType(SurfaceType type);
		Controller::InputType getInput() const;
		void setSurfaceInput(Controller::InputType value);
		float getParamA() const;
		float getParamB() const;
		void setParamA(float value);
		void setParamB(float value);
		void toggleSurfaceType();
		void writeValue(XmlElement*) const;
		void readValue(const XmlElement*, IReferenceBinder&);
	};
}
