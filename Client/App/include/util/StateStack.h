#pragma once
#include "util/Name.h"
#include <vector>
#include <string>

namespace RBX
{
	template<class T>
	class StateStack
	{
		struct StateEntry
		{
		public:
			std::string name;
			T* undoState;
			T* redoState;
		};

	private:
		int currentState;
		std::vector<StateEntry> stack;
	public:
		StateStack()
		{
			currentState = -1; // see DataModel constructor
		}

		~StateStack()
		{
			clearStackAfter(-1);
			currentState = -1;
		}

		void pushState(const Name&, T*, T*);
		void pushState(std::string, T*, T*);
		void clearStack();
		bool canUndo(std::string&) const;
		T* undo();
		bool canRedo(std::string&) const;
		T* redo();
	protected:
		size_t getStackSize() const
		{
			return stack.size();
		}
		virtual bool isStackTooBig() const
		{
			return false;
		}
	private:
		void clearStackAfter(int index)
		{
			for (int i = stack.size() - 1; i > index; i--)
			{
				delete stack[i].redoState;
				delete stack[i].undoState;
			}

			stack.resize(index + 1);
		}
	};
}
