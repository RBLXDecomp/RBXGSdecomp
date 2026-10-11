#include "gui/EquationDisplay.h"
#include "util/Evaluator.h"

namespace RBX
{
	EquationDisplay::EquationDisplay(const std::string& label, const std::string& equation)
		: TextDisplay(label, label),
		  equation(equation)
	{
	}

	std::string EquationDisplay::getLabel() const
	{
		const Instance* root = Instance::getRootAncestor(this);
		if (const Evaluator* evaluator = dynamic_cast<const Evaluator*>(root))
		{
			std::string evaluatedEquation = evaluator->evaluate(equation);
			return label + " " + evaluatedEquation;
		}
		else
		{
			return label;
		}
	}
}
