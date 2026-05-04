#pragma once

#include "ViewBase.h"
#include <GLG3D/Sky.h>
#include <GLG3D/TextureManager.h>
#include <boost/signals/connection.hpp>
#include <boost/shared_ptr.hpp>
#include <map>
#include <memory>

class RenderStats;

namespace RBX
{
	class DataModel;

	namespace Render {
		//Really, Really, REALLY weird class (or struct? idk)
		//There is like NO INFO about it except the std::map below
		//No header, no pdb data, NOTHING
		//Your only guess into how this classed worked is probably through the use of models
		class Model;

		class SceneManager;
		class RenderScene;
	};

	namespace View 
	{
		class MaterialFactory;

		class View : public ViewBase
		{
		private:
			G3D::ReferenceCountedPointer<G3D::Sky> sky;
			boost::shared_ptr<RBX::DataModel> dataModel;
			std::map<RBX::Instance, RBX::Render::Model> models;
			boost::signals::scoped_connection lightingChangedConnection;
			boost::signals::scoped_connection workspaceDescendentAddedConnection;
			bool lightingValid;
		public:
			std::auto_ptr<RBX::Render::SceneManager> sceneManager;
			std::auto_ptr<RBX::Render::RenderScene> renderScene;
			std::auto_ptr<G3D::TextureManager> textureManager;
			std::auto_ptr<MaterialFactory> materialFactory;
			
			View(boost::shared_ptr<RBX::DataModel> dataModel);
			virtual ~View();
			virtual void render(void* rd);
			G3D::ReferenceCountedPointer<RBX::Render::Material> getMaterial(G3D::ReferenceCountedPointer<RBX::Render::Material>);
			virtual float getShadingQuality() const;
			virtual float getMeshDetail() const;
			virtual void updateSettings(float shadingQuality, float meshDetail, bool shadows, float cameraDistance);
			virtual void suppressSkybox();
			virtual RBX::Instance* getWorkspace();
			virtual RenderStats& getRenderStats();
		private:
			virtual void onWorkspaceDescendentAdded(boost::shared_ptr<RBX::Instance> descendent);
			virtual void updateLighting();
			virtual void invalidateLighting(bool updateSkybox);
		};
	};
};