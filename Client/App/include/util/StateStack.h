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

		~StateStack();

		void pushState(const Name&, T*, T*);
		void pushState(std::string, T*, T*);
		void clearStack();
		bool canUndo(std::string&) const;
		T* undo();
		bool canRedo(std::string&) const;
		T* redo();
	protected:
		size_t getStackSize() const;
		virtual bool isStackTooBig() const;
	private:
		void clearStackAfter(int);
	};
}
