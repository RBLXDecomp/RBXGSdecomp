#include "v8datamodel/Seat.h"
#include "v8datamodel/PartInstance.h"
#include "v8datamodel/JointInstance.h"
#include "v8datamodel/Workspace.h"
#include "humanoid/Humanoid.h"
#include "Network/Players.h"

namespace RBX
{
	Seat::Seat()
		: sleepTime(0.0f)
	{
		setName("Seat");
	}

	void Seat::seatCharacter(Humanoid* h)
	{
		weld = Creatable::create<Weld>();

		PartInstance* torso = h->getTorso();
		RBXASSERT(torso);

		const G3D::CoordinateFrame& unused = torso->getCoordinateFrame();

		weld->setName("SeatWeld");
		weld->setPart0(this);
		weld->setPart1(torso);

		G3D::CoordinateFrame C0;
		C0.lookAt(G3D::Vector3(0.0f, -1.0f, 0.0f), G3D::Vector3(0.0f, 0.0f, -1.0f));

		C0.translation = G3D::Vector3(0.0f, 3.0f, 0.0f);

		G3D::CoordinateFrame C1;
		C1.lookAt(G3D::Vector3(0.0f, -1.0f, 0.0f), G3D::Vector3(0.0f, 0.0f, -1.0f));

		C1.translation = G3D::Vector3(0.0f, 0.6f, 0.0f);

		weld->setC0(C0);
		weld->setC1(C1);
		weld->setParent(this);
	}

	void Seat::unseatCharacter()
	{
		sleepTime = G3D::System::getLocalTime();

		if (weld)
		{
			weld->setParent(NULL);
			weld.reset();
		}
	}

	void Seat::onEvent_humanoidJumped(bool active)
	{
		unseatCharacter();
		humanoidJumped.disconnect();
	}

	void Seat::onServiceProvider(const ServiceProvider* oldProvider, const ServiceProvider* newProvider)
	{
		Instance::onServiceProvider(oldProvider, newProvider);

		if (!oldProvider)
		{
			boost::slot<boost::function<void(boost::shared_ptr<Instance>)>> slot(boost::bind(&Seat::onEvent_seatTouched, this, _1));
			seatTouched = PartInstance::event_Touched.connect(this, slot);
		}

		if (!newProvider)
		{
			seatTouched.disconnect();
		}
	}
}
