#pragma once

namespace RBX
{
	class Evaluator
	{
	public:
		virtual std::string evaluate(const std::string&) const = 0;
	};
}
