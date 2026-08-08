#pragma once
#include "util/RunStateOwner.h"
#include <G3D/System.h>
#include <G3D/CoordinateFrame.h>

namespace RBX
{
	class PartInstance;

	namespace Network
	{
		class ClientPhysics : public Listener<RunService, Heartbeat>
		{
		private:
			G3D::RealTime lastUpdateTime;
			boost::weak_ptr<RunService> runService;
			PartInstance* part;
			G3D::CoordinateFrame lastPos;

		public:
			ClientPhysics(PartInstance* part);
			void update(const boost::shared_ptr<RunService>& runService);
			void sleep();
			virtual ~ClientPhysics();
		protected:
			virtual void onEvent(const RunService* source, Heartbeat event);
		};
	}
}
