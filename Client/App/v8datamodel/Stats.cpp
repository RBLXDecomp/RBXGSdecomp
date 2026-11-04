#include "v8datamodel/Stats.h"
#include "util/Log.h"

static RBX::Reflection::BoundFuncDesc<RBX::Stats::Item, double(void), 0> func_value(&RBX::Stats::Item::getValue, "GetValue", RBX::Reflection::FunctionDescriptor::NeedTrustedCaller);
static RBX::Reflection::BoundFuncDesc<RBX::Stats::Item, std::string(void), 0> func_values(&RBX::Stats::Item::getStringValue2, "GetValueString", RBX::Reflection::FunctionDescriptor::NeedTrustedCaller);

namespace RBX
{
	namespace Stats
	{
		void Item::formatMem(unsigned bytes)
		{
			setValue(bytes, Log::formatMem(bytes));
		}

		void Item::formatValue(double val, const char* fmt, ...)
		{
			va_list args;
			va_start(args, fmt);
			setValue(val, G3D::vformat(fmt, args));
			va_end(args);
		}

		Item* Item::createChildItem(const char* name)
		{
			boost::shared_ptr<Item> item = Creatable::create<Item>();
			item->setName(name);
			item->setParent(this);

			return item.get();
		}

		template<>
		void Item::formatValue<double>(const double& value)
		{
			formatValue(value, "%.3g", value);
		}

		template<>
		void Item::formatValue<int>(const int& value)
		{
			formatValue((double)value, "%d", value);
		}

		template<>
		void Item::formatValue<unsigned>(const unsigned& value)
		{
			formatValue((double)value, "%d", value);
		}

		template<>
		void Item::formatValue<bool>(const bool& value)
		{
			sValue = value ? "true" : "false";
			val = (double)(value ? 1 : 0);
		}

		Item* Item::createBoundChildItem(const Profiling::Profiler& p)
		{
			boost::shared_ptr<Item> item = Creatable::create<ProfilingItem>(&p);
			item->setName(p.name.c_str());
			item->setParent(this);

			return item.get();
		}

		Item* Item::createBoundMemChildItem(const char* name, const unsigned& v)
		{
			boost::shared_ptr<Item> item = Creatable::create<TypedMemItem>(&v);
			item->setName(name);
			item->setParent(this);

			return item.get();
		}
	}
}

ProfilingItem::ProfilingItem(const RBX::Profiling::Profiler* p)
	: p(*p)
{
}

void ProfilingItem::update()
{
	RBX::Profiling::Bucket b = p.getData(5.0);

	if (b.sampleTimeSpan > 0.0)
	{
		val = b.getTotalTime() / b.sampleTimeSpan;

		char buffer[256];
		if (b.frames > 0)
		{
			std::string t = RBX::Log::formatTime(b.getFrameTime());
			sprintf(buffer, "%s, %.3gfps (%.3g%%)", t.c_str(), b.getNominalFPS(), val * 100.0);
		}
		else
		{
			sprintf(buffer, "%.3g%%", val * 100.0);
		}

		sValue = buffer;
	}
	else
	{
		val = 0.0;
		sValue = "?";
	}
}
