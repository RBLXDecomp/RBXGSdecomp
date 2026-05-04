#pragma once

#include <G3D/Vector3.h>
#include "reflection/reflection.h"
#include "util/MeshId.h"
#include "util/TextureId.h"
#include "v8tree/Instance.h"

namespace RBX {
	extern const char* sSpecialMesh;

	class SpecialShape : public RBX::DescribedCreatable<SpecialShape, Instance, &sSpecialMesh>
	{
	public:
		enum MeshType
		{
			HEAD_MESH,
			TORSO_MESH,
			WEDGE_MESH,
			SPHERE_MESH,
			CYLINDER_MESH,
			FILE_MESH,
			BRICK_MESH
		};
		
	private:
		MeshType meshType;
		G3D::Vector3 scale;
		TextureId textureId;
		MeshId meshId;
		G3D::Vector3 vertColor;
	public:
		SpecialShape();
		const MeshType getMeshType() const
		{
			return meshType;
		}
		void setMeshType(MeshType value);
		const G3D::Vector3& getScale() const
		{
			return scale;
		}
		void setScale(const G3D::Vector3& value);
		const G3D::Vector3& getVertColor() const
		{
			return vertColor;
		}
		void setVertColor(const G3D::Vector3& value);
		const float getAlpha() const;

		const MeshId getMeshId() const
		{
			return meshId;
		}

		void setMeshId(const MeshId& value);
		const TextureId getTextureId() const
		{
			return textureId;
		}
		void setTextureId(const TextureId& value);
	protected:
		virtual bool askSetParent(const Instance*) const;
	};

};