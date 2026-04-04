#include "HeadMesh.h"

class CylinderTransform
{
	const float bevel;
	const int halfVertexCount;
public:
	CylinderTransform(float, int);
	void operator()(G3D::Vector3&, G3D::Vector3&, G3D::Vector2&, const G3D::Vector3&, const G3D::Vector2int16&);
};

class EndcapTransform
{
	const float bevel;
public:
	EndcapTransform(float bevel)
		: bevel(bevel)
	{
	}
	void operator()(G3D::Vector3&, G3D::Vector3&, G3D::Vector2&, const G3D::Vector3&, const G3D::Vector2int16&);
};
