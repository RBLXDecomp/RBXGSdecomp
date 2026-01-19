#pragma once
#include "RenderLib/EffectSettings.h"
#include "RenderLib/RenderSurface.h"
#include "RenderLib/Chunk.h"
#include <GLG3D/Sky.h>

namespace RBX
{
	namespace Render
	{
		class RenderScene
		{
			friend class SceneManager;

		private:
			G3D::Array<RenderSurface> proxyArray;
			G3D::Array<RenderSurface*> diffuseProxyArray;
			G3D::Array<RenderSurface*> reflectProxyArray;
			G3D::Array<RenderSurface*> transparentProxyArray;
			G3D::Array<RenderSurface> shadowProxyArray;
			G3D::Array<G3D::ReferenceCountedPointer<Chunk>> shadowCachingChunkArray;
			G3D::VAR shadowVAR;
			G3D::ReferenceCountedPointer<G3D::VARArea> shadowVARArea;
			EffectSettings effectSettings;
			G3D::ReferenceCountedPointer<G3D::Lighting> desiredLighting;
			G3D::ReferenceCountedPointer<G3D::Lighting> lighting;

			float shadingQuality;
			float meshDetail;
			bool shadows;
			float cameraDistance;
			G3D::Array<G3D::ReferenceCountedPointer<Chunk>> chunkArray;
			G3D::Array<unsigned> shadowIndexArray;
		public:
			RenderStats renderStats;
			bool debugShadowVolumes;
			G3D::ReferenceCountedPointer<G3D::Sky> sky;
			G3D::Color4 colorClearValue;
			G3D::LightingParameters desiredSkyParameters;
			G3D::LightingParameters skyParameters;

		private:
			void updateShadowVAR(const G3D::Array<G3D::Vector3>& shadowVertex);
			void computeShadowVolumeGeometry(G3D::Array<unsigned>& indexArray, G3D::Array<G3D::Vector3>& shadowVertex, const G3D::GLight& light, bool generateLightCap, float shadowVertexDistance) const;
			void clearProxyArrays();
			void allocateProxies(G3D::RenderDevice*, const G3D::GCamera&);
			void classifyProxies();
			void sortProxies();
			void computeProxyArrays(G3D::RenderDevice* rd, const G3D::GCamera& camera);
			void sendDiffuseProxyMeshGeometry(G3D::RenderDevice* rd) const;
			void markStencilShadows(G3D::RenderDevice*, const G3D::GCamera&, const G3D::GLight&);
			void sendShadowProxyMeshGeometry(G3D::RenderDevice*, const G3D::GLight&) const;
			void turnOnLights(G3D::RenderDevice* rd, bool allLights) const;
			void diffusePass(G3D::RenderDevice*);
			void reflectionPass(G3D::RenderDevice* rd);
			void transparentPass(G3D::RenderDevice* rd);
			void debugShowTextures(G3D::RenderDevice*, const G3D::GCamera&);
			void renderShadowVolumeGeometry(G3D::RenderDevice* rd, const G3D::GLight& light, bool caps, float shadowVertexDistance);
		public:
			//RenderScene(const RenderScene&);
			RenderScene();
			~RenderScene();

			void presetLighting(G3D::ReferenceCountedPointer<G3D::Sky> sky, G3D::LightingParameters skyParameters, G3D::Color3 ambientTop, G3D::Color3 ambientBottom);
			void setLighting(const G3D::ReferenceCountedPointer<G3D::Lighting>& L);
			void setThrottle(float t, float m, bool s, float c);
			float getShadingQuality() const;
			float getMeshDetail() const;
			void render(G3D::RenderDevice*, const G3D::GCamera&);
			//RenderScene& operator=(const RenderScene&);
		};

		class SceneManager
		{
		public:
			RenderScene* renderScene;
		public:
			virtual void invalidateModel(const G3D::ReferenceCountedPointer<Chunk>&, bool) = 0;
			virtual void addModel(const G3D::ReferenceCountedPointer<Chunk>&) = 0;
			virtual void removeModel(const G3D::ReferenceCountedPointer<Chunk>&) = 0;
			virtual void clear() = 0;
			virtual void setSleeping(const G3D::ReferenceCountedPointer<Chunk>&, bool) = 0;
			virtual void prerender(double) = 0;
			virtual ~SceneManager() {}
			//SceneManager(const SceneManager&);
		protected:
			SceneManager(RenderScene* renderScene)
				: renderScene(renderScene)
			{
			}
			void clearScene();
			void addToScene(const G3D::ReferenceCountedPointer<Chunk>& chunk)
			{
				renderScene->chunkArray.push_back(chunk);
			}
			void removeFromScene(const G3D::ReferenceCountedPointer<Chunk>& chunk)
			{
				renderScene->chunkArray.fastRemove(renderScene->chunkArray.findIndex(chunk));
			}
		};
	}
}
