#pragma once
#include "RenderLib/DepthBlur.h"
#include <GLG3D/ToneMap.h>
#include <GLG3D/LightingParameters.h>

namespace RBX
{
	namespace Render
	{
		class EffectSettings
		{
		private:
			bool _stencilShadows;
			bool _alphaBlendShadowLights;
			bool _useAllLightsInUnshadowedPass;
			bool _skyBox;
			bool _toneMap;
			bool _hemisphereLighting;
			bool _depthBlur;
			float _farCullZ;
			float _groundLOD;
			float _LODShift;
		public:
			G3D::ReferenceCountedPointer<G3D::Lighting> update(
				float,
				float,
				bool,
				float,
				const G3D::ReferenceCountedPointer<G3D::Lighting>&,
				const G3D::LightingParameters&,
				G3D::LightingParameters&
			);

			bool stencilShadows() const;
			bool hemisphereLighting() const
			{
				return _hemisphereLighting;
			}
			bool skyBox() const;
			float farCullZ() const;
			bool alphaBlendShadowLights() const;
			bool useAllLightsInUnshadowedPass() const;
			float groundLOD() const;
			float LODShift() const;
			bool applyToneMap() const;
			bool applyDepthBlur() const;

			static G3D::ToneMap* getToneMap();
			static DepthBlur* getDepthBlur();
		};
	}
}
