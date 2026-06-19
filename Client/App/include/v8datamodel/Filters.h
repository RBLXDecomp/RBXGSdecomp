#pragma once
#include "util/HitTestFilter.h"
#include "v8datamodel/ModelInstance.h"
#include "v8datamodel/PartInstance.h"
#include <boost/shared_ptr.hpp>

namespace RBX
{
	class Unlocked : public HitTestFilter
	{
	public:
		virtual HitTestFilter::Result filterResult(const Primitive* testMe) const
		{
			return unlocked(testMe) ? HitTestFilter::INCLUDE_PRIM : HitTestFilter::STOP_TEST;
		}

		static bool unlocked(const Primitive* testMe);
	};

	class PartByLocalCharacter : public HitTestFilter
	{
	protected:
		boost::shared_ptr<ModelInstance> character;
		boost::shared_ptr<PartInstance> head;

	public:
		PartByLocalCharacter(Instance* root);
		virtual HitTestFilter::Result filterResult(const Primitive* testMe) const;
	};

	class UnlockedPartByLocalCharacter : public PartByLocalCharacter
	{
	public:
		UnlockedPartByLocalCharacter(Instance*);
		virtual HitTestFilter::Result filterResult(const Primitive* testMe) const;
	};
}
