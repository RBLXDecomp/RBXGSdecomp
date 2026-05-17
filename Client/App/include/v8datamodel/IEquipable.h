#pragma once
#include <boost/shared_ptr.hpp>
#include <G3D/CoordinateFrame.h>
#include <string>

namespace RBX
{
	class Weld;
	class PartInstance;
	class Workspace;

	class __declspec(novtable) IEquipable
	{
	protected:
		boost::shared_ptr<Weld> weld;
		Workspace* workspace;

	protected:
		void buildWeld(PartInstance* humanoidPart, PartInstance* gadgetPart, const G3D::CoordinateFrame& humanoidCoord, const G3D::CoordinateFrame& gadgetCoord, const std::string& name);
	protected:
		IEquipable();
		virtual ~IEquipable();
	};
}
