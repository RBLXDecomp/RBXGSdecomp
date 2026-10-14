#pragma once
#include "v8tree/Service.h"
#include "util/Profiling.h"
#include "reflection/reflection.h"

namespace RBX
{
    namespace Stats
    {
		template<typename T>
        class TypedStatsItem;

        extern char* sStatsItem;
        class Item : public DescribedNonCreatable<Item, Instance, &sStatsItem>
        {
        protected:
            double val;
            std::string sValue;
    
        protected:
            virtual void update()
			{
				return;
			}

        public:
            Item(const char* name)
				: Base(name)
			{
			}

            Item()
			{
			}

            const std::string& getStringValue();

            double getValue()
			{
				update();
				return val;
			}

            std::string getStringValue2()
			{
				update();
				return sValue;
			}

            void setValue(double value, const std::string& stringValue)
			{
				val = value;
				sValue = stringValue;
			}

            void formatMem(unsigned bytes);
            void formatValue(double val, const char* fmt, ...);
            void formatPercent(double);
            Item* createChildItem(const char* item);
            Item* createBoundChildItem(const Profiling::Profiler& p);
            Item* createBoundMemChildItem(const char*, const unsigned&);

        protected:
            virtual bool askAddChild(const Instance* instance) const
			{
				return dynamic_cast<const Stats::Item*>(instance) != NULL;
			}

        public:
            template<typename T>
            Item* createBoundChildItem(const char* name, const T& v)
			{
				boost::shared_ptr<Item> item = Creatable::create<TypedStatsItem<T>>(&v);
				item->setName(name);
				item->setParent(this);

				return item.get();
			}

            template<typename T>
            Item* createChildItem(const char* name, boost::function0<T> func)
			{
				boost::shared_ptr<Item> item = Creatable::create<TypedStatsItem<T>>(func);
				item->setName(name);
				item->setParent(this);

				return item.get();
			}

			template<typename T>
			void formatValue(const T& value);
        };

        template<typename T>
        class TypedStatsItem : public Item
        {
        protected: 
            const boost::function0<T> func;
  
        public:
            TypedStatsItem<T>(const T* value)
				: func(boost::bind(&deref, value))
			{
			}

            TypedStatsItem<T>(boost::function0<T> func)
				: func(func)
			{
			}

            virtual void update()
			{
				formatValue<T>(func());
			}
  
        private:
            static const T& deref(const T* value)
			{
				return *value;
			}
        };

        class TypedMemItem : public TypedStatsItem<unsigned>
        {
        public:
            TypedMemItem(const unsigned* value)
				: TypedStatsItem(value)
			{
			}

            virtual void update()
			{
				formatMem(func());
			}
        };

        extern char *sStats;
        class StatsService : public DescribedNonCreatable<StatsService, Instance, &sStats>, 
                             public Service
        {
        public:
            StatsService()
				: Base("Stats")
			{
			}
        
        protected:
            virtual bool askAddChild(const Instance* instance) const
			{
				return dynamic_cast<const Stats::Item*>(instance) != NULL;
			}

            virtual XmlElement* write()
			{
				return NULL;
			}
        };
    }
}

class ProfilingItem : public RBX::Stats::Item
{
private:
	const RBX::Profiling::Profiler& p;

public:
	ProfilingItem(const RBX::Profiling::Profiler* p);
	virtual void update();
};
