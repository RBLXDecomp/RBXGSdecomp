#pragma once

template <typename T> 
class Vector6
{
private:
	T data[6];
public:
	Vector6(const T& value)
	{
		for (int i = 5; i >= 0; i--)
		{
			data[i] = value;
		}
	}

	Vector6() // see PartInstance constructor
	{
	}

	T& operator[](int i)
	{
		return data[i];
	}

	const T& operator[](int i) const
	{
		return data[i];
	}
};
