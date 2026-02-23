#pragma once
#include <boost/shared_ptr.hpp>
#include "RenderLib/RenderStats.h"

namespace RBX
{
	class Instance;
	class ViewBase
	{
	public:
		//ViewBase(const ViewBase&);
		ViewBase()
		{
		}
		virtual ~ViewBase()
		{
		}
	public:
		virtual void render(void* rd) = 0;
		virtual float getShadingQuality() const = 0;
		virtual float getMeshDetail() const = 0;
		virtual void updateSettings(float shadingQuality, float meshDetail, bool shadows, float cameraDistance) = 0;
		virtual void suppressSkybox() = 0;
		virtual Instance* getWorkspace() = 0;
		virtual RenderStats& getRenderStats() = 0;
	private:
		virtual void onWorkspaceDescendentAdded(boost::shared_ptr<Instance> descendent) = 0;
		virtual void updateLighting() = 0;
		virtual void invalidateLighting(bool updateSkybox) = 0;
	public:
		//ViewBase& operator=(const ViewBase&);
	  
	public:
		static bool& getShadowsEnabled()
		{
			static bool _shadowsEnabled = false;
			return _shadowsEnabled;
		}
	};
}
