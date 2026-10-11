#include "v8datamodel/custommesh.h"
#include "v8datamodel/PartInstance.h"

static RBX::Reflection::PropDescriptor<RBX::SpecialShape, G3D::Vector3> desc_scale("Scale", "Data", &RBX::SpecialShape::getScale, &RBX::SpecialShape::setScale, RBX::Reflection::PropertyDescriptor::STANDARD);
static RBX::Reflection::PropDescriptor<RBX::SpecialShape, RBX::MeshId> desc_meshId("MeshId", "Data", &RBX::SpecialShape::getMeshId, &RBX::SpecialShape::setMeshId, RBX::Reflection::PropertyDescriptor::STANDARD);
static RBX::Reflection::PropDescriptor<RBX::SpecialShape, RBX::TextureId> desc_textureId("TextureId", "Data", &RBX::SpecialShape::getTextureId, &RBX::SpecialShape::setTextureId, RBX::Reflection::PropertyDescriptor::STANDARD);
static RBX::Reflection::PropDescriptor<RBX::SpecialShape, G3D::Vector3> desc_vertColor("VertexColor", "Data", &RBX::SpecialShape::getVertColor, &RBX::SpecialShape::setVertColor, RBX::Reflection::PropertyDescriptor::STANDARD);

static RBX::Reflection::EnumPropDescriptor<RBX::SpecialShape, RBX::SpecialShape::MeshType> desc_meshType("MeshType", "Data", &RBX::SpecialShape::getMeshType, &RBX::SpecialShape::setMeshType, RBX::Reflection::PropertyDescriptor::STANDARD);

namespace RBX
{
	char* sSpecialShape = "SpecialMesh";

	SpecialShape::SpecialShape()
		: scale(1.0f, 1.0f, 1.0f),
		  vertColor(1.0f, 1.0f, 1.0f),
		  meshType(HEAD_MESH)
	{
		setName("Mesh");
	}

	bool SpecialShape::askSetParent(const Instance* instance) const
	{
		return dynamic_cast<const PartInstance*>(instance) != NULL;
	}

	const TextureId SpecialShape::getTextureId() const
	{
		return textureId;
	}

	const MeshId SpecialShape::getMeshId() const
	{
		return meshId;
	}

	const float SpecialShape::getAlpha() const
	{
		Instance* parent = getParent();
		if (!parent)
			return 0.0f;

		if (PartInstance* part = dynamic_cast<PartInstance*>(parent))
			return part->alpha();

		return 0.0f;
	}

	void SpecialShape::setMeshType(MeshType value)
	{
		if (meshType != value)
		{
			meshType = value;
			raisePropertyChanged(desc_meshType);
		}
	}

	void SpecialShape::setScale(const G3D::Vector3& value)
	{
		if (scale != value)
		{
			scale = value;
			raisePropertyChanged(desc_scale);
		}
	}

	void SpecialShape::setVertColor(const G3D::Vector3& value)
	{
		if (vertColor != value)
		{
			vertColor = value;
			raisePropertyChanged(desc_vertColor);
		}
	}

	void SpecialShape::setMeshId(const MeshId& value)
	{
		if (meshId != value)
		{
			meshId = value;
			raisePropertyChanged(desc_meshId);
			setMeshType(FILE_MESH);
		}
	}

	void SpecialShape::setTextureId(const TextureId& value)
	{
		if (textureId != value)
		{
			textureId = value;
			raisePropertyChanged(desc_textureId);
			setMeshType(FILE_MESH);
		}
	}

	namespace Reflection
	{
		EnumDesc<SpecialShape::MeshType>::EnumDesc()
			: EnumDescriptor("MeshType", typeid(SpecialShape::MeshType))
		{
			addPair(SpecialShape::HEAD_MESH, "Head");
			addPair(SpecialShape::TORSO_MESH, "Torso");
			addPair(SpecialShape::WEDGE_MESH, "Wedge");
			addPair(SpecialShape::BRICK_MESH, "Brick");
			addPair(SpecialShape::SPHERE_MESH, "Sphere");
			addPair(SpecialShape::CYLINDER_MESH, "Cylinder");
			addPair(SpecialShape::FILE_MESH, "FileMesh");
		}
	}
}
