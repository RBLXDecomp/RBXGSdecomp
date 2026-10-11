#pragma once
#include "gui/Gui.h"

namespace RBX
{
	class EquationDisplay : public TextDisplay
	{
	private:
		std::string equation;

	protected:
		virtual std::string getLabel() const;
	public:
		EquationDisplay(const std::string&, const std::string&, const std::string&);
		EquationDisplay(const std::string& label, const std::string& equation);
	};
}
