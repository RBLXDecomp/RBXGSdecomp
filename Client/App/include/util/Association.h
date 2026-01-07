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
			//Item(const Item&);
			Item();
		public:
			//Item& operator=(const Item&);
		};

		template<typename T>
		class TItem : public Item
		{
		public:
			T t;
  
		public:
			//TItem(const TItem&);
			TItem();
			virtual ~TItem();
		public:
			//TItem& operator=(const TItem&);
		};

	private:
		std::vector<Item*> items;
  
	public:
		~Association();
		//Association(const Association&);
		Association() {}
	public:
		//Association& operator=(const Association&);
  
	private:
		static size_t& count();
	};
}
