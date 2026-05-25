#include "v8datamodel/Visit.h"
#include "util/Http.h"
#include "util/standardout.h"
#include <G3D/format.h>
#include <boost/thread.hpp>

namespace RBX
{
	Visit::Visit()
		: uploadUrl("NoVisit")
	{
	}

	Visit::~Visit() {}

	void Visit::setPing(std::string url, int interval)
	{
		if (url != "")
		{
			pingThread.reset(new worker_thread(boost::bind(&Visit::ping, url, interval), "rbx_visit"));
		}
		else
		{
			pingThread.reset();
		}
	}

	worker_thread::work_result Visit::ping(std::string url, int interval)
	{
		try
		{
			std::string message;
			Http(url).get(message);
		}
		catch (std::exception& exp)
		{
			std::string failMessage = G3D::format("ping %s failed: %s", url.c_str(), exp.what());
			StandardOut::singleton()->print(MESSAGE_WARNING, failMessage.c_str());
		}

		boost::xtime xt;
		boost::xtime_get(&xt, boost::TIME_UTC);

		xt.sec += interval;

		boost::thread::sleep(xt);

		return worker_thread::more;
	}
}
