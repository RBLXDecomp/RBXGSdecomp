#pragma once
#include "v8tree/Service.h"
#include "util/Profiling.h"
#include "reflection/reflection.h"

namespace RBX
{
    namespace Stats
    {
        extern const char* sStatsItem;
        class Item : public DescribedNonCreatable<Item, Instance, &sStatsItem>
        {
        protected:
            double val;
            std::string sValue;
    
        protected:
            virtual void update();

        public:
            Item(const char*);
            Item();
            const std::string& getStringValue();
            double getValue();
            std::string getStringValue2();
            void setValue(double, const std::string&);
            void formatMem(size_t);
            void formatValue(double, const char*, ...);
            void formatPercent(double);
            Item* createChildItem(const char*);
            Item* createBoundChildItem(const Profiling::Profiler&);
            Item* createBoundMemChildItem(const char*, const size_t&);

        protected:
            virtual bool askAddChild(const Instance*) const;

        public:
            template<typename T>
            Item* createBoundChildItem(const char* name, const T& v);

            template<typename T>
            Item* createChildItem(const char* name, boost::function0<T> func);
        };

        template<typename T>
        class TypedStatsItem : public Item
        {
        protected: 
            const boost::function0<T> func;
  
        public:
            TypedStatsItem<T>(const T*);
            TypedStatsItem<T>(boost::function0<T>);
            virtual void update();
  
        private:
            static const bool& deref(const T*);
        };

        class TypedMemItem : public TypedStatsItem<size_t>
        {
        public:
            TypedMemItem(const size_t*);
            virtual void update();
        };

        extern const char *sStats;
        class StatsService : public DescribedNonCreatable<StatsService, Instance, &sStats>, 
                             public Service
        {
        public:
            StatsService();
        
        protected:
            virtual bool askAddChild(const Instance*) const;
            virtual XmlElement* write();
        };
    }
}
