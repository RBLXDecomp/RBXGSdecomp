#pragma once
#include <queue>
#include "v8tree/Instance.h"
#include "v8tree/Service.h"

namespace RBX
{
	class TimerService;

	extern const char* sDebrisService;

	class DebrisService : public DescribedCreatable<DebrisService, Instance, &sDebrisService>, public Service
	{
	private:
		std::queue<boost::weak_ptr<Instance>> queue;
		int maxItems;
		TimerService* timer;

	public:
		DebrisService();
		void addItem(boost::shared_ptr<Instance>, double);
		void setMaxItems(int);
		int getMaxItems() const;
	protected:
		virtual void onServiceProvider(const ServiceProvider*, const ServiceProvider*);
	private:
		void cleanup();
	};
}
