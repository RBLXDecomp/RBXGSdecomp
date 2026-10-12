#include "v8datamodel/GlobalSettings.h"

namespace RBX
{
	const char* const sGlobalSettings = "GlobalSettings";

	boost::recursive_mutex GlobalSettings::mutex;

	boost::shared_ptr<GlobalSettings> GlobalSettings::singleton()
	{
		boost::recursive_mutex::scoped_lock lock(mutex);

		static boost::shared_ptr<GlobalSettings> sing = Creatable::create<GlobalSettings>();
		return sing;
	}
}
