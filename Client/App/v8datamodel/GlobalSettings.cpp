#include "v8datamodel/GlobalSettings.h"

namespace RBX
{
	boost::shared_ptr<GlobalSettings> GlobalSettings::singleton()
	{
		boost::recursive_mutex::scoped_lock lock(mutex);

		static boost::shared_ptr<GlobalSettings> sing = Creatable::create<GlobalSettings>();

		return sing;
	}
}
