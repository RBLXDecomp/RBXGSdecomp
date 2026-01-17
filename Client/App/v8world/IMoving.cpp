#include "v8world/IMoving.h"
#include "util/Debug.h"

namespace RBX
{
	IMoving::IMoving()
		: iMovingManager(NULL),
		  stepsToSleep(0)
	{
	}

	void IMoving::makeMoving()
	{
		RBXASSERT(stepsToSleep > 0);
		onCanAggregateChanged(false);
		if (iMovingManager)
		{
			iMovingManager->moved(this);
		}
	}

	void IMoving::setMovingManager(IMovingManager* _iMovingManager)
	{
		if (iMovingManager)
			iMovingManager->remove(this);

		iMovingManager = _iMovingManager;
		if (_iMovingManager)
		{
			if (stepsToSleep > 0)
				makeMoving();
		}
	}

	bool IMoving::checkSleep()
	{
		RBXASSERT(stepsToSleep > 0);
		
		if (stepsToSleep > 1)
		{
			stepsToSleep -= 1;
			return false;
		}

		RBXASSERT(stepsToSleep == 1);
		stepsToSleep = 0;
		onCanAggregateChanged(false);
		return true;
	}

	void IMoving::onCanAggregateChanged(bool canAggregate)
	{
	}

	bool IMoving::reportTouches() const
	{
		return false;
	}

	void IMoving::notifyMoved()
	{
		if (stepsToSleep == 0)
		{
			stepsToSleep = 30;
			makeMoving();
		}
	}

	IMovingManager::IMovingManager()
	{
		current = moving.end();
	}

	IMovingManager::~IMovingManager()
	{
		RBXASSERT(current == moving.end());
		RBXASSERT(moving.empty());
	}

	void IMovingManager::remove(IMoving* iMoving)
	{
		if (current != moving.end() && *current == iMoving)
			current = moving.erase(current);
		else
			moving.erase(iMoving);
	}

	void IMovingManager::moved(IMoving* iMoving)
	{
		moving.insert(iMoving);
	}

	void IMovingManager::onMovingHeartbeat()
	{
		RBXASSERT(current == moving.end());

		current = moving.begin();
		while (current != moving.end())
		{
			IMoving* iMoving = *current;

			if (iMoving->checkSleep())
				remove(iMoving);
			else
				current++;
		}
	}
}
