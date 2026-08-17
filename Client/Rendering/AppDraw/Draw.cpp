#include "AppDraw/Draw.h"
#include "util/Math.h"

namespace RBX
{
	void Draw::spokes(float radius, float axis, Adorn* adorn)
    {
        // no idea where the 0.51 comes from lol
        float halfAxis = axis*0.51f;
        float subRadius = radius*0.7f;

        for (int i = 0; i < 2; i++)
        {
            float x = (i == 0) ? halfAxis : -halfAxis;
            float ccw = i ? -1.0f : 1.0f;

            for (int j = 0; j < 2; j++)
            {
                float y = (j == 0) ? subRadius : 0.1f;
                float z = (j == 0) ? 0.1f : subRadius;

                adorn->quad(
                    G3D::Vector3(x, y * ccw, z),
                    G3D::Vector3(x, -y * ccw, z),
                    G3D::Vector3(x, -y * ccw, -z),
                    G3D::Vector3(x, y * ccw, -z),
                    G3D::Color4(0.5f, 0.5f, 0.5f, 1.0f)
                );
            }
        }
    }

	void Draw::partAdorn(const Part& part, Adorn* adorn, const G3D::Color3& controllerColor)
    {
        adornSurfaces(part, adorn, controllerColor);

        if (part.type == Part::CYLINDER_PART)
        {
            adorn->setObjectToWorldMatrix(part.coordinateFrame);

            G3D::Vector3 bleh = part.gridSize;
            spokes(bleh.z * 0.5, bleh.x, adorn);
        }
    }

    const G3D::Color3& Draw::selectColor()
    {
        static G3D::Color3 c(0.1f, 0.6f, 1.0f);
        return c;
    }

	void Draw::selectionBox(const Part& part, Adorn* adorn, SelectState selectState)
    {
        Draw::selectionBox(part, adorn, (selectState == SELECT_NORMAL) ? Draw::selectColor() : G3D::Color3(0.2f, 0.7f, 1.0f));
    }

	void Draw::adornSurfaces(const Part& part, Adorn* adorn, const G3D::Color3& controllerColor)
    {
        for (int i = 0; i < 6; i++)
        {
			SurfaceType type = part.surfaceType[i];
            if (type >= ROTATE && type <= ROTATE_P) // ROTATE, ROTATE_V, ROTATE_P
            {
                Draw::constraint(part, adorn, i, controllerColor);
            }
        }
    }

    // TODO: 94.56% match
	// classic case of MSVC optimizer being really inconsistent
	void Draw::constraint(const Part& part, Adorn* adorn, int face, const G3D::Color3& controllerColor)
    {
		SurfaceType surfaceType = part.surfaceType[face];

        G3D::Vector3 halfSize = part.gridSize * 0.5f;

        const G3D::Matrix3& rot = Math::getAxisRotationMatrix(face);

        G3D::Vector3 relativeTranslation;

        int axis = face % 3;
        float posNeg = (face > 2) ? -1.0f : 1.0f;

        relativeTranslation[axis] = halfSize[axis]*posNeg;

        G3D::CoordinateFrame translation(rot, relativeTranslation);
        G3D::CoordinateFrame newObject = part.coordinateFrame*translation;

        adorn->setObjectToWorldMatrix(newObject);
        adorn->cylinderAlongX(0.2f, 1.0f, G3D::Color3::yellow(), G3D::Color4::clear());

        if (surfaceType == ROTATE_V || surfaceType == ROTATE_P)
        {
            adorn->cylinderAlongX(0.4f, 0.25f, controllerColor, G3D::Color4::clear());
        }
    }

	void Draw::selectionBox(const Part& part, Adorn* adorn, const G3D::Color4& selectColor)
    {
        adorn->setObjectToWorldMatrix(part.coordinateFrame);

        G3D::Vector3 halfSize = part.gridSize*0.5;

        const float offset = 0.15f;

        for (int ix = 0; ix < 3; ix++)
        {
            int iy = (ix+1) % 3;
            int iz = (ix+2) % 3;

            for (int d2 = -1; d2 < 2; d2+=2)
            {
                for (int d3 = -1; d3 < 2; d3+=2)
                {
                    G3D::Vector3 p0;
                    G3D::Vector3 p1;

                    p0[ix] = -halfSize[ix] - offset;
                    p0[iy] = d2 * halfSize[iy] - offset;
                    p0[iz] = d3 * halfSize[iz] - offset;

                    p1[ix] = halfSize[ix] + offset;
                    p1[iy] = d2 * halfSize[iy] + offset;
                    p1[iz] = d3 * halfSize[iz] + offset;

                    adorn->box(G3D::AABox(p0, p1), selectColor, G3D::Color4::clear());
                }
            }
        }
    }
}
