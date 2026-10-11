#pragma once

namespace RBX
{
	class __declspec(novtable) Evaluator
	{
	public:
		virtual std::string evaluate(const std::string& valueName) const = 0;
	};
}
