#pragma once

namespace RBX
{
	template<typename T>
	class ScopedAssign
	{
		T* value;
		T oldValue;

	public:
		ScopedAssign(T& value, const T& oldValue)
			: value(&value),
			  oldValue(value)
		{
			*this->value = oldValue;
		}

		~ScopedAssign()
		{
			*this->value = oldValue;
		}
	};
}
