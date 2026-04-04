#pragma once
#include <boost/shared_ptr.hpp>

namespace RBX
{
	class Instance;
	class InstanceHandle
	{
	private:
		boost::shared_ptr<Instance> target;
	  
	public:
		//InstanceHandle(const InstanceHandle&);
		InstanceHandle(boost::shared_ptr<Instance> target);
		InstanceHandle(Instance* target);
		InstanceHandle() 
		{
		}
	public:
		InstanceHandle& operator=(boost::shared_ptr<Instance> object)
		{
			linkTo(object);
			return *this;
		}
		//InstanceHandle& operator=(const InstanceHandle&);
		bool empty() const;
		boost::shared_ptr<Instance> getTarget() const;
		void linkTo(boost::shared_ptr<Instance> target);
		bool operator==(const InstanceHandle& other) const
		{
			return operatorEqual(other);
		}
		bool operator!=(const InstanceHandle& other) const
		{
			return !operatorEqual(other);
		}
		bool operator<(const InstanceHandle& other) const
		{
			return operatorLess(other);
		}
		bool operator>(const InstanceHandle& other) const
		{
			return operatorGreater(other);
		}
	protected:
		bool operatorEqual(const InstanceHandle& other) const;
		bool operatorLess(const InstanceHandle& other) const;
		bool operatorGreater(const InstanceHandle& other) const;
	};
}
