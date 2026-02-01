#pragma once
#include "reflection/descriptor.h"
#include <iterator>
#include <vector>

namespace RBX
{
	namespace Reflection
	{
		class DescribedBase;
		class ClassDescriptor;

		class MemberDescriptor : public Descriptor
		{
		public:
			const Name& category;
			const ClassDescriptor& owner;
		  
		protected:
			MemberDescriptor(const ClassDescriptor& owner, const char* name, const char* category)
				: Descriptor(name),
				  category(Name::declare(category, -1)),
				  owner(owner)
			{
			}
			virtual ~MemberDescriptor();

		public:
			bool isMemberOf(const DescribedBase*) const;
			bool isMemberOf(const ClassDescriptor&) const;
		};

		template<typename TheDescriptor>
		class MemberDescriptorContainer
		{
		public:
			typedef std::vector<TheDescriptor*> CollectionType;
			class Collection : public CollectionType
			{
			};

		public:
			class Iterator : public std::iterator<std::forward_iterator_tag, typename TheDescriptor::Describing, void>
			{
			private:
				DescribedBase* instance;
				typename CollectionType::const_iterator iter;
			  
			public:
				Iterator(const typename CollectionType::const_iterator& iter, DescribedBase* instance)
					: instance(instance),
					  iter(iter)
				{
				}

			public:
				typename TheDescriptor::Describing operator*() const;
				bool operator==(const Iterator& other) const
				{
					return iter == other.iter;
				}
				bool operator!=(const Iterator& other) const
				{
					return iter != other.iter;
				}
				Iterator operator++(int);
				Iterator& operator++();
			};

		public:
			class ConstIterator : public std::iterator<std::forward_iterator_tag, typename TheDescriptor::Describing, void>
			{
			private:
				const DescribedBase* instance;
				typename CollectionType::const_iterator iter;
		  
			public:
				ConstIterator(const typename CollectionType::const_iterator& iter, const DescribedBase* instance)
					: instance(instance),
					  iter(iter)
				{
				}
			public:
				typename TheDescriptor::Describing operator*() const;
				bool operator==(const ConstIterator& other) const
				{
					return iter == other.iter;
				}
				bool operator!=(const ConstIterator& other) const
				{
					return iter != other.iter;
				}
				ConstIterator operator++(int);
				ConstIterator& operator++();
				const TheDescriptor& getDescriptor() const;
			};

		protected:
			Collection descriptors;
			std::vector<MemberDescriptorContainer*> derivedContainers;
			MemberDescriptorContainer* base;

		protected:
			// TODO: check if matches
			MemberDescriptorContainer(MemberDescriptorContainer* base)
				: descriptors(),
				  derivedContainers(),
				  base(base)
			{
				if (base)
				{
					boost::recursive_mutex::scoped_lock lock(sync_0());
					mergeMembers(base);
					base->derivedContainers.push_back(this);
				}
			}

		public:
			void declare(TheDescriptor*);

			typename CollectionType::const_iterator descriptors_begin() const
			{
				return descriptors.begin();
			}
			typename CollectionType::const_iterator descriptors_end() const
			{
				return descriptors.end();
			}
			typename CollectionType::const_iterator findDescriptor(const Name& name) const
			{
				size_t posStart = 0;
				size_t posEnd = descriptors.size();

				if (posEnd)
				{
					size_t i;

					while (true)
					{
						i = (posStart+posEnd)/2;

						const Name& descName = descriptors[i]->name;

						if (name == descName)
							break;
						
						int compare = name.compare(descName);

						if (compare < 0)
						{
							posEnd = i;	
						}
						else
						{
							if (compare <= 0)
								break;

							posStart = i+1;
						}

						// TODO: can we get rid of this jump? it's not obvious to me how
						if (posStart >= posEnd)
							goto notFound;
					}

					return descriptors.begin()+i;
				}

			notFound:
				return descriptors.end();
			}

			Iterator findMember(const Name& name, DescribedBase* instance) const
			{
				return Iterator::Iterator(findDescriptor(name), instance);
			}
			ConstIterator findConstMember(const Name& name, const DescribedBase* instance) const
			{
				return ConstIterator::ConstIterator(findDescriptor(name), instance);
			}

			Iterator members_begin(DescribedBase* instance) const
			{
				return Iterator::Iterator(descriptors_begin(), instance);
			}
			ConstIterator members_begin(const DescribedBase* instance) const
			{
				return ConstIterator::ConstIterator(descriptors_begin(), instance);
			}

			Iterator members_end(DescribedBase* instance) const
			{
				return Iterator::Iterator(descriptors_end(), instance);
			}
			ConstIterator members_end(const DescribedBase* instance) const
			{
				return ConstIterator::ConstIterator(descriptors_end(), instance);
			}

			void mergeMembers(const MemberDescriptorContainer*);

		public:
			static bool compare(const TheDescriptor*, const TheDescriptor*);
		};
	}
}
