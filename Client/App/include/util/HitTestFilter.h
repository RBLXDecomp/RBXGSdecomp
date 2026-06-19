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
		virtual Result filterResult(const Primitive* testMe) const = 0;
	};
}
