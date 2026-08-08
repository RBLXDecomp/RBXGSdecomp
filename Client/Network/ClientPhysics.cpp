#include "ClientPhysics.h"
#include "NetworkSettings.h"
#include "v8datamodel/PartInstance.h"

//99.98% match
bool areClose(const G3D::CoordinateFrame& c1, const G3D::CoordinateFrame& c2)
{
	if (fabs(c1.translation.x - c2.translation.x) > 0.2f)
		return false;

	if (fabs(c1.translation.y - c2.translation.y) > 0.2f)
		return false;

	if (fabs(c1.translation.z - c2.translation.z) > 0.2f)
		return false;

	for (int i = 0; i < 3; i++)
	{
		G3D::Vector3 v1, v2;

		v1.x = c1.rotation[0][i];
		v1.y = c1.rotation[1][i];
		v1.z = c1.rotation[2][i];
		v2.x = c2.rotation[0][i];
		v2.y = c2.rotation[1][i];
		v2.z = c2.rotation[2][i];

		if ((v1 - v2).squaredMagnitude() > 0.010000001f)
			return false;
	}

	return true;
}

namespace RBX
{
	namespace Network
	{
		ClientPhysics::ClientPhysics(PartInstance* part)
			: part(part)
		{
		}

		ClientPhysics::~ClientPhysics()
		{
			Notifier<RunService, Heartbeat>::disconnect(runService.lock(), this);
		}

		void ClientPhysics::sleep()
		{
			Notifier<RunService, Heartbeat>::disconnect(runService.lock(), this);
			runService.reset();

			lastPos = part->getCoordinateFrame();
		}

		void ClientPhysics::onEvent(const RunService* source, Heartbeat event)
		{
			G3D::RealTime currentTime = G3D::System::getLocalTime();

			if (NetworkSettings::singleton().clientPhysicsLifetime + lastUpdateTime < currentTime)
				sleep();
			else
				RBXASSERT(0);
		}

		void ClientPhysics::update(const boost::shared_ptr<RunService>& runService)
		{
			lastUpdateTime = G3D::System::getLocalTime();

			const G3D::CoordinateFrame& cframe = part->getCoordinateFrame();

			if (areClose(lastPos, cframe))
			{
				sleep();
				return;
			}

			lastPos = cframe;

			if (this->runService.lock() != runService)
			{
				Notifier<RunService, Heartbeat>::disconnect(this->runService.lock(), this);

				this->runService = runService;

				Notifier<RunService, Heartbeat>::connect(this->runService.lock(), this);
			}
		}
	}
}
