#pragma once

namespace RBX
{
	class Evaluator
	{
		virtual std::string evaluate(const std::string&) const = 0;
	};
}
