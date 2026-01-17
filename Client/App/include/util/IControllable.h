#pragma once

namespace RBX
{
	class __declspec(novtable) IControllable
	{
	public:
		IControllable()
		{
		}
		virtual ~IControllable()
		{
		}

	public:
		virtual bool isControllable() const = 0;
	};
}
