#pragma once
#include <vector>

namespace RBX
{
	// NOTE: this class uses templated functions: add those in manually
	template<typename Associated>
	class Association
	{
	public:
		class Item
		{
		public:
			virtual ~Item();
		};

		template<typename T>
		class TItem : public Item
		{
		public:
			T t;
  
		public:
			TItem();
		};

	private:
		std::vector<Item*> items;
  
	public:
		~Association()
		{
			std::vector<Item*>::iterator it = items.begin();
			std::vector<Item*>::iterator end = items.end();

			while(it != end)
			{
				delete (*it);
				it++;
			}
		}
  
	private:
		static size_t& count();

	public:
		template<typename T>
		bool contains() const;

		template<typename T>
		T& get();

		template<typename T>
		void remove();
	};
}
