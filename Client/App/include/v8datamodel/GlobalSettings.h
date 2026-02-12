#include "reflection/reflection.h"
#include "v8tree/Instance.h"
#include "v8tree/Service.h"
#include "util/Object.h"
#include <boost/shared_ptr.hpp>
#include <boost/thread/mutex.hpp>

namespace RBX
{
	extern const char* sGlobalSettings;

	class GlobalSettings : public DescribedNonCreatable<GlobalSettings, ServiceProvider, &sGlobalSettings>
	{
	public:
		class Item : public NonFactoryProduct<Instance, NULL>
		{
		protected:
			virtual bool askAddChild(const Instance*) const;
		};

	public:
		static boost::recursive_mutex mutex;

	public:
		GlobalSettings();
		void loadState();
		void saveState();
		void eraseSettingsStore();

	public:
		static boost::shared_ptr<GlobalSettings> singleton();
	};

	template<typename Class, const char** ClassName>
	class GlobalSettingsItem : public DescribedCreatable<Class, GlobalSettings::Item, ClassName>, public Service
	{
	private:
		static GlobalSettingsItem* sing;

	protected:
		GlobalSettingsItem();
		~GlobalSettingsItem();

	public:
		static Class& singleton();
	};
}
