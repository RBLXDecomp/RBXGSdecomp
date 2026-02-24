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
			virtual bool askAddChild(const Instance* instance) const
			{
				return fastDynamicCast<const GlobalSettings::Item>(instance) != NULL;
			}
		public:
			//Item(const Item&);
			Item();
			~Item();
			//Item& operator=(const Item&);
		};

		static boost::recursive_mutex mutex;

		//GlobalSettings(const GlobalSettings&);
		GlobalSettings()
		{
			setName("Global Settings");
		}
		void loadState();
		void saveState();
		void eraseSettingsStore();
		~GlobalSettings();
		//GlobalSettings& operator=(const GlobalSettings&);

		static boost::shared_ptr<GlobalSettings> singleton();
	};

	template<typename Class, const char** ClassName>
	class GlobalSettingsItem : public DescribedCreatable<Class, GlobalSettings::Item, ClassName>, public Service
	{
	private:
		static GlobalSettingsItem* sing;
	public:
		//GlobalSettingsItem(const GlobalSettingsItem&);
	protected:
		GlobalSettingsItem();
		~GlobalSettingsItem();
	public:
		//GlobalSettingsItem& operator=(const GlobalSettingsItem&);

		static Class& singleton();
	};
}
