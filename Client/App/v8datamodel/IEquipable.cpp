#include "v8datamodel/IEquipable.h"
#include "v8datamodel/JointInstance.h"

namespace RBX
{
	IEquipable::IEquipable()
		: workspace(NULL)
	{
	}

	IEquipable::~IEquipable()
	{
		RBXASSERT(!weld);
	}

	void IEquipable::buildWeld(PartInstance* humanoidPart, PartInstance* gadgetPart, const G3D::CoordinateFrame& humanoidCoord, const G3D::CoordinateFrame& gadgetCoord, const std::string& name)
	{
		G3D::CoordinateFrame attachmentCoordWorld = humanoidPart->getCoordinateFrame() * humanoidCoord;
		G3D::CoordinateFrame gadgetCoordWorld = attachmentCoordWorld * gadgetCoord.inverse();

		gadgetPart->setCoordinateFrame(gadgetCoordWorld);

		RBXASSERT(!weld);

		weld = Creatable<Instance>::create<Weld>();

		weld->setName(name);
		weld->setPart0(humanoidPart);
		weld->setPart1(gadgetPart);
		weld->setC0(humanoidCoord);
		weld->setC1(gadgetCoord);
		weld->setParent(humanoidPart);
	}
}
