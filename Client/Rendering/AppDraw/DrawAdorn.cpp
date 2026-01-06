#include "AppDraw/DrawAdorn.h"
#include "util/Math.h"
#include "G3D/Vector3.h"
#include "G3D/AABox.h"

namespace RBX
{
    // TODO: 97.36%
    // assignment of operator* result to rotation is screwing up somewhere
    void DrawAdorn::cylinder(Adorn* adorn, const G3D::CoordinateFrame& worldC, int axis, float length, float radius, const G3D::Color3& color)
    {
        G3D::Matrix3 rotation = worldC.rotation*Math::getAxisRotationMatrix(axis);
        G3D::CoordinateFrame rotatedWorldC(rotation, worldC.translation);

        adorn->setObjectToWorldMatrix(rotatedWorldC);
        adorn->cylinderAlongX(radius, length, color, G3D::Color4::clear());
    }
    
    // TOOD: 97.56%
    // minor control flow differences
    void DrawAdorn::surfaceBorder(Adorn* adorn, const G3D::Vector3& halfRealSize, float highlight, int surfaceId, const G3D::Color4& color)
    {
        // this is likely an inline from NormalId.h because of how cY and cZ are calculated
        int cX = surfaceId % 3;

        int polarity = (surfaceId < 3) ? 1 : -1;
        float direction = polarity * halfRealSize[cX];

        G3D::Vector3 p0;
        G3D::Vector3 p1;

        p0[cX] = direction - highlight;
        p1[cX] = direction + highlight;

        int cY = (cX + 1) % 3;
        int cZ = (cX + 2) % 3;

        for (int i = 0; i < 2; i++)
        {
            int c2 = i ? cY : cZ;
            int c3 = i ? cZ : cY;
            
            for (int polarity = -1; polarity <= 1; polarity += 2)
            {
                p0[c2] = polarity * halfRealSize[c2] - highlight;
                p1[c2] = polarity * halfRealSize[c2] + highlight;

                p0[c3] = -halfRealSize[c3] - highlight;
                p1[c3] = halfRealSize[c3] + highlight;
                
                adorn->box(G3D::AABox(p0, p1), color, G3D::Color4::clear());
            }
        }
    }

    void DrawAdorn::partSurface(const Part& part, int surfaceId, Adorn* adorn, const G3D::Color4& color)
    {
        adorn->setObjectToWorldMatrix(part.coordinateFrame);
        G3D::Vector3 halfRealSize = part.gridSize*0.5f;

        surfaceBorder(adorn, halfRealSize, 0.2f, surfaceId, color);
    }
}
