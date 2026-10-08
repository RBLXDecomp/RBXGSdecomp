#pragma once
#include <set>
#include "util/Debug.h"

namespace RBX
{
	class IMovingManager;
	class __declspec(novtable) IMoving
	{
		friend class IMovingManager;

	private:
		IMovingManager* iMovingManager;
		int stepsToSleep;
  
	private:
		void makeMoving();
	protected:
		virtual void onCanAggregateChanged(bool canAggregate) = 0;
		void setMovingManager(IMovingManager* _iMovingManager);
		bool checkSleep();
	public:
		IMoving();
		~IMoving()
		{
			RBXASSERT(!iMovingManager);
		}
	public:
		void notifyMoved();
		virtual bool reportTouches() const = 0;
		bool getCanAggregate() const
		{
			return stepsToSleep == 0;
		}
	};

	class __declspec(novtable) IMovingManager
	{
		friend class IMoving;

	private:
		std::set<IMoving*> moving;
		std::set<IMoving*>::iterator current;
	protected:
		void remove(IMoving* iMoving);
		void moved(IMoving* iMoving);
	public:
		IMovingManager();
		virtual ~IMovingManager();
	public:
		void onMovingHeartbeat();
	};
}
