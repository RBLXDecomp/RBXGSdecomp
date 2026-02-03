#include "util/RunStateOwner.h"

namespace RBX
{
	extern const char* sTimerService;

	class TimerService : public DescribedCreatable<TimerService, Instance, &sTimerService>, public Service, public Listener<RunService, Heartbeat>
	{
		class Item
		{
		public:
			G3D::RealTime time;
			boost::function0<void> func;
		};

	private:
		boost::shared_ptr<RunService> runService;
		std::list<Item> items;
	public:
		TimerService();
		void delay(boost::function0<void>, double);
	protected:
		virtual void onServiceProvider(const ServiceProvider*, const ServiceProvider*);
		virtual void onEvent(const RunService*, Heartbeat);
	};
}
