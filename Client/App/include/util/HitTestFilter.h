#pragma once

namespace RBX
{
	class Primitive;

	class HitTestFilter
	{
	public:
		enum Result
		{
			STOP_TEST,
			IGNORE_PRIM,
			INCLUDE_PRIM
		};

	public:
		virtual Result filterResult(const Primitive*) const = 0;
	public:
		//HitTestFilter(const HitTestFilter&);
		HitTestFilter();
	public:
		//HitTestFilter& operator=(const HitTestFilter&);
	};
}
