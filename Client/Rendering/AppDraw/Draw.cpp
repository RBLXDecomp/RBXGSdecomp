#include "AppDraw/Draw.h"
#include "util/Math.h"

namespace RBX
{
	void Draw::spokes(float radius, float axis, Adorn* adorn)
    {
        float halfAxis = axis*0.5f;
        float subRadius = radius*0.7f;

        for (int i = 0; i < 2; i++)
        {
            float x = (i == 0) ? halfAxis : -halfAxis;
            float ccw = i ? -1.0f : 1.0f;

            for (int j = 0; j < 2; j++)
            {
                float z = (j == 0) ? subRadius : 0.1f;
                float y = (j == 0) ? 0.1f : subRadius;

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
            // TODO: wtf? logically this does the same thing but what
            // even with this rbxgs does 'add eax, -0x6' but i can only get it to do 'sub eax, 0x6'
            // if (part.surfaceType[i] <= NUM_SURF_TYPES-1)
            if (((unsigned int)part.surfaceType[i] - 6) <= 2)
            {
                Draw::constraint(part, adorn, i, controllerColor);
            }
        }
    }

    // TODO: 73.20%
    // in theory this is logically matching but there are
    // differences in expression somewhere
	void Draw::constraint(const Part& part, Adorn* adorn, int face, const G3D::Color3& controllerColor)
    {
        // (000938)  S_BPREL32: [FFFFFF8C], Type:             0x175D, halfSize
        // (000950)  S_BPREL32: [FFFFFF80], Type:             0x175D, relativeTranslation
        // (000970)  S_BPREL32: [FFFFFF9C], Type:             0x18E5, translation
        // (000988)  S_BPREL32: [FFFFFF7C], Type:     T_REAL32(0040), posNeg
        // (00099C)  S_BPREL32: [FFFFFFCC], Type:             0x18E5, newObject;

        int surfaceType = part.surfaceType[face]; 

        G3D::Vector3 halfSize = part.gridSize * 0.5;

        float posNeg = (face <= 2) ? 1.0f : -1.0f;

        G3D::Vector3 relativeTranslation;

        int axis = face % 3;
        relativeTranslation[axis] = halfSize[axis] * posNeg;

        G3D::CoordinateFrame translation(Math::getAxisRotationMatrix(face), relativeTranslation);
        G3D::CoordinateFrame newObject = translation*part.coordinateFrame;

        adorn->setObjectToWorldMatrix(newObject);
        adorn->cylinderAlongX(0.2f, 1.0f, G3D::Color3::yellow(), G3D::Color4::clear());        

        if (surfaceType == ROTATE_V || surfaceType == ROTATE_P)
        {
            adorn->cylinderAlongX(0.4f, 0.25f, controllerColor, G3D::Color4::clear());
        }
    }

    // TODO: 97.30%
    // it's pretty much correct but there are very subtle control flow differences
	void Draw::selectionBox(const Part& part, Adorn* adorn, const G3D::Color4& selectColor)
    {
        // on a probably related note i can't figure out where d2 and d3 can fit in
        // (000A88)  S_BPREL32: [FFFFFFD8:-0x28], Type:             0x175D, halfSize
        // (000AA0)  S_BPREL32: [00000004],       Type:       T_INT4(0074), d2
        // (000AB0)  S_BPREL32: [FFFFFF94:-0x6C], Type:       T_INT4(0074), d3
        // (000AC0)  S_BPREL32: [FFFFFFCC:-0x34], Type:             0x175D, p1
        // (000AD0)  S_BPREL32: [FFFFFFC0:-0x40], Type:             0x175D, p0;

        adorn->setObjectToWorldMatrix(part.coordinateFrame);

        G3D::Vector3 halfSize = part.gridSize*0.5;

        const float offset = 0.15f;

        for (int ix = 0; ix < 3; ix++)
        {
            int iy = (ix+1) % 3;
            int iz = (ix+2) % 3;

            for (int j = -1; j < 2; j+=2)
            {
                for (int k = -1; k < 2; k+=2)
                {
                    G3D::Vector3 p0;
                    G3D::Vector3 p1;

                    p0[ix] = -halfSize[ix] - offset;
                    p0[iy] = j * halfSize[iy] - offset;
                    p0[iz] = k * halfSize[iz] - offset;

                    p1[ix] = halfSize[ix] + offset;
                    p1[iy] = j * halfSize[iy] + offset;
                    p1[iz] = k * halfSize[iz] + offset;

                    adorn->box(G3D::AABox(p0, p1), selectColor, G3D::Color4::clear());
                }
            }
        }
    }
}
