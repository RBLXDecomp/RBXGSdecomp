#pragma once
#include <boost/shared_ptr.hpp>
#include <G3D/CoordinateFrame.h>
#include <string>

namespace RBX
{
	class Weld;
	class PartInstance;
	class Workspace;

	class IEquipable
	{
	protected:
		boost::shared_ptr<Weld> weld;
		Workspace* workspace;

	protected:
		void buildWeld(PartInstance*, PartInstance*, const G3D::CoordinateFrame&, const G3D::CoordinateFrame&, const std::string&);
	public:
		//IEquipable(const IEquipable&);
	protected:
		IEquipable();
		virtual ~IEquipable();
	public:
		//IEquipable& operator=(const IEquipable&);
	};
}
