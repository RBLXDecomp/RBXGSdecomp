#pragma once
#include <string>

namespace RBX
{
	class IMetric
	{
	public:
		IMetric()
		{
		}
		virtual ~IMetric()
		{
		}
	public:
		virtual std::string getMetric(const std::string&) const = 0;
	};
}
