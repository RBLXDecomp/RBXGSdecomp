#include "v8tree/Instance.h"
#include "util/TextureId.h"
#include "util/MeshId.h"
#include <G3D/Vector3.h>

namespace RBX
{
	extern char* sSpecialShape;

	class SpecialShape : public DescribedCreatable<SpecialShape, Instance, &sSpecialShape>
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
		const MeshId getMeshId() const;
		void setMeshId(const MeshId& value);
		const TextureId getTextureId() const;
		void setTextureId(const TextureId& value);
	protected:
		virtual bool askSetParent(const Instance* instance) const;
	};
}
