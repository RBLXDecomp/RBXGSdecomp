#pragma once
#include "RbxGraphics/Adorn.h"
#include "AppDraw/SelectState.h"
#include "util/IndexArray.h"
#include "AppDraw/SelectState.h"

namespace RBX 
{
	class IRenderableBucket;

	class __declspec(novtable) IRenderable
	{
		friend class IRenderableBucket;

	private:
		int index2d;
		int index3d;
		IRenderableBucket* bucket;

		int& indexFunc2d()
		{
			return index2d;
		}

		int& indexFunc3d()
		{
			return index3d;
		}

	protected:
		virtual bool shouldRender2d() const
		{
			return false;
		}

		virtual bool shouldRender3dAdorn() const
		{
			return false;
		}

	public:
		IRenderable() : index2d(-1), index3d(-1), bucket(NULL)
		{
		}

		~IRenderable();

		void shouldRenderSetDirty();

		virtual void render2d(Adorn* adorn);
		virtual void render3dAdorn(Adorn* adorn);
		virtual void render3dSelect(Adorn* adorn, SelectState selectState);
	};


	class IRenderableBucket
	{
		friend class IRenderable;

	private:
		IndexArray<IRenderable, &IRenderable::indexFunc2d> renderable2ds;
		IndexArray<IRenderable, &IRenderable::indexFunc3d> renderable3ds;
	  
	protected:
		void onAdded(IRenderable* iR);
		void onRemoving(IRenderable* iR);
		void recomputeShouldRender(IRenderable* iR);

	public:
		~IRenderableBucket();

		void render2dItems(Adorn* adorn);
		void render3dAdornItems(Adorn* adorn);
	};
}
