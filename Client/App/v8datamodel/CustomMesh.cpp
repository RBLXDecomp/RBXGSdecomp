#include "v8datamodel/CustomMesh.h"
#include "v8datamodel/PartInstance.h"
namespace RBX
{
	const char* sSpecialMesh = "SpecialMesh";
	static const Reflection::EnumPropDescriptor<SpecialShape, SpecialShape::MeshType> desc_meshType("MeshType", "Data", &SpecialShape::getMeshType, &SpecialShape::setMeshType, Reflection::EnumPropertyDescriptor::STANDARD);
	static const Reflection::PropDescriptor<SpecialShape, G3D::Vector3> desc_scale("Scale", "Data", &SpecialShape::getScale, &SpecialShape::setScale, Reflection::PropertyDescriptor::STANDARD);
	static const Reflection::PropDescriptor<SpecialShape, MeshId> desc_meshId("MeshId", "Data", &SpecialShape::getMeshId, &SpecialShape::setMeshId, Reflection::PropertyDescriptor::STANDARD);
	static const Reflection::PropDescriptor<SpecialShape, TextureId> desc_textureId("TextureId", "Data", &SpecialShape::getTextureId, &SpecialShape::setTextureId, Reflection::PropertyDescriptor::STANDARD);
	static const Reflection::PropDescriptor<SpecialShape, G3D::Vector3> desc_vertColor("VertexColor", "Data", &SpecialShape::getVertColor, &SpecialShape::setVertColor, Reflection::PropertyDescriptor::STANDARD);

	const float SpecialShape::getAlpha() const
	{
		if (getParent())
		{
			PartInstance* part = fastDynamicCast<PartInstance>(getParent());

			if (part)
			{
				return part->getTransparencyUi();
			}
		}
		return 0.0f;
	}

	SpecialShape::SpecialShape()
		: meshType(HEAD_MESH),
		scale(1.0f, 1.0f, 1.0f),
		textureId(),
		meshId(),
		vertColor(1.0f, 1.0f, 1.0f)
	{
		setName("Mesh");
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
			if (meshType != FILE_MESH)
			{
				meshType = FILE_MESH;
				raisePropertyChanged(desc_meshType);
			}
		}
	}

	void SpecialShape::setTextureId(const TextureId& value)
	{
		if (textureId != value)
		{
			textureId = value;
			raisePropertyChanged(desc_textureId);
			if (meshType != FILE_MESH)
			{
				meshType = FILE_MESH;
				raisePropertyChanged(desc_meshType);
			}
		}
	}

	bool SpecialShape::askSetParent(const Instance* instance) const
	{
		return fastDynamicCast<const PartInstance>(instance) != NULL;
	}
};