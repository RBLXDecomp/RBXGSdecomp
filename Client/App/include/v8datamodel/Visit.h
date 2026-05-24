#pragma once
#include "v8tree/Instance.h"
#include "v8tree/Service.h"

namespace RBX
{
	extern const char* sVisit;

	class Visit : public DescribedCreatable<Visit, Instance, &sVisit>, public Service
	{
	private:
		std::string uploadUrl;
		boost::scoped_ptr<worker_thread> pingThread;

	public:
		Visit();
		virtual ~Visit();
		void setPing(std::string, int);
		void setUploadUrl(std::string);
		std::string getUploadUrl();
		virtual XmlElement* write();

	public:
		static worker_thread::work_result ping(std::string, int);
	};
}
