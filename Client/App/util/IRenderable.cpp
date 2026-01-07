#include "util/IRenderable.h"

namespace RBX 
{
	IRenderable::~IRenderable()
	{
		RBXASSERT(index2d == -1);
		RBXASSERT(index3d == -1);
		RBXASSERT(bucket == NULL);
	}

	void IRenderable::shouldRenderSetDirty()
	{
		if (bucket)
			bucket->recomputeShouldRender(this);
	}

	IRenderableBucket::~IRenderableBucket()
	{
		RBXASSERT(renderable2ds.size() == 0);
		RBXASSERT(renderable3ds.size() == 0);
	}

	void IRenderableBucket::onAdded(IRenderable* iR)
	{
		RBXASSERT(iR->index2d == -1);
		RBXASSERT(iR->index3d == -1);
		RBXASSERT(iR->bucket == NULL);

		iR->bucket = this;
		recomputeShouldRender(iR);
	}

	void IRenderableBucket::onRemoving(IRenderable* iR)
	{
		RBXASSERT(iR->bucket == this);

		if (iR->index2d >= 0)
			renderable2ds.fastRemove(iR);

		if (iR->index3d >= 0)
			renderable3ds.fastRemove(iR);

		iR->bucket = NULL;

		RBXASSERT(iR->index2d == -1);
		RBXASSERT(iR->index3d == -1);
	}

	void IRenderableBucket::recomputeShouldRender(IRenderable* iR)
	{
		RBXASSERT(iR->bucket == this);

		if (iR->shouldRender2d())
		{
			if (iR->index2d < 0)
				renderable2ds.fastAppend(iR);
		}
		else if (iR->index2d >= 0)
		{
			renderable2ds.fastRemove(iR);
		}

		if (iR->shouldRender3dAdorn())
		{
			if (iR->index3d < 0)
				renderable3ds.fastAppend(iR);
		}
		else if (iR->index3d >= 0)
		{
			renderable3ds.fastRemove(iR);
		}
	}

	void IRenderableBucket::render2dItems(Adorn* adorn)
	{
		for (int i = 0; i < renderable2ds.size(); ++i)
			renderable2ds[i]->render2d(adorn);
	}

	void IRenderableBucket::render3dAdornItems(Adorn* adorn)
	{
		for (int i = 0; i < renderable3ds.size(); ++i)
			renderable3ds[i]->render3dAdorn(adorn);
	}
}
