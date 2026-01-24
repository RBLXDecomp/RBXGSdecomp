#pragma once

namespace RBX
{
	class PartInstance;
	class __declspec(novtable) IPrimaryPart
	{
	public:
		virtual PartInstance* getPrimaryPart() = 0;
		virtual const PartInstance* getPrimaryPartConst() const = 0;
	};
}
