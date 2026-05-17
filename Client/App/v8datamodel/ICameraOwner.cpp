#include "v8datamodel/ICameraOwner.h"
#include "v8datamodel/PartInstance.h"

namespace RBX
{
	void ICameraOwner::getCameraIgnorePrimitives(std::vector<const Primitive*>& primitives)
	{
		for (size_t i = 0; i < cameraIgnoreParts.size(); i++)
		{
			if (!cameraIgnoreParts[i].expired())
			{
				PartInstance* part = cameraIgnoreParts[i].lock().get();
				Primitive* prim = part->getPrimitive();

				primitives.push_back(prim);
			}
		}
	}

	void ICameraOwner::setCameraIgnoreParts(PartInstance* _set)
	{
		std::vector<PartInstance*> temp;
		temp.push_back(_set);

		setCameraIgnoreParts(temp);
	}

	void ICameraOwner::setCameraIgnoreParts(const std::vector<PartInstance*>& _set)
	{
		cameraIgnoreParts.clear();

		for (size_t i = 0; i < _set.size(); i++)
		{
			PartInstance* part = _set[i];
			boost::weak_ptr<PartInstance> ptr = shared_from(part);

			cameraIgnoreParts.push_back(ptr);
		}
	}
}
