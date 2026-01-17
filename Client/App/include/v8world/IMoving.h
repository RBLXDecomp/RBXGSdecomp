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
		//IMoving(const IMoving&);
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
		//IMoving& operator=(const IMoving&);
	};

	class IMovingManager
	{
		friend class IMoving;

	private:
		std::set<IMoving*> moving;
		std::set<IMoving*>::iterator current;
	protected:
		void remove(IMoving* iMoving);
		void moved(IMoving* iMoving);
	public:
		//IMovingManager(const IMovingManager&);
		IMovingManager();
		virtual ~IMovingManager();
	public:
		void onMovingHeartbeat();
		//IMovingManager& operator=(const IMovingManager&);
	};
}
