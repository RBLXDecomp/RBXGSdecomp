#pragma once
#include "reflection/reflection.h"
#include "v8tree/Instance.h"
#include "v8tree/Service.h"
#include "util/Object.h"
#include <boost/shared_ptr.hpp>
#include <boost/thread/mutex.hpp>

namespace RBX
{
	extern const char* const sGlobalSettings;

	class GlobalSettings : public DescribedNonCreatable<GlobalSettings, ServiceProvider, &sGlobalSettings>
	{
	public:
		class Item : public NonFactoryProduct<Instance, NULL>
		{
		protected:
			virtual bool askAddChild(const Instance* instance) const
			{
				return dynamic_cast<const GlobalSettings::Item*>(instance) != NULL;
			}
		};

	public:
		static boost::recursive_mutex mutex;

		GlobalSettings()
		{
			setName("Global Settings");
		}
		void loadState();
		void saveState();
		void eraseSettingsStore();

	public:
		static boost::shared_ptr<GlobalSettings> singleton();
	};

	template<typename Class, const char* const* ClassName>
	class GlobalSettingsItem : public DescribedCreatable<Class, GlobalSettings::Item, ClassName>, public Service
	{
	private:
		static GlobalSettingsItem* sing;

	protected:
		GlobalSettingsItem()
		{
			setName(*ClassName);
			sing = this;
		}
		~GlobalSettingsItem()
		{
			sing = NULL;
		}

	public:
		static Class& singleton()
		{
			boost::recursive_mutex::scoped_lock lock(GlobalSettings::mutex);

			if (!sing)
			{
				boost::shared_ptr<Class> s = Creatable::create<Class>();
				s->setParent(GlobalSettings::singleton().get());
			}

			return *static_cast<Class*>(sing);
		}
	};
}
