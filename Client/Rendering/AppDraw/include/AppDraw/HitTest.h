#pragma once
#include "AppDraw/Part.h"
#include "G3D/Ray.h"
#include "G3D/Vector3.h"

namespace RBX
{
    class HitTest
    {
    private:
        static bool hitTestBox(const Part& part, G3D::Ray& rayInPartCoords, G3D::Vector3& hitPointInPartCoords, float gridToReal);
        static bool hitTestBall(const Part& part, G3D::Ray& rayInPartCoords, G3D::Vector3& hitPointInPartCoords, float gridToReal);
        static bool hitTestCylinder(const Part& part, G3D::Ray& rayInPartCoords, G3D::Vector3& hitPointInPartCoords, float gridToReal);

    public:
        static bool hitTest(const Part& part, G3D::Ray& rayInPartCoords, G3D::Vector3& hitPointInPartCoords, float gridToReal);
        static const float handleOffset();
        static float distanceToHandleRadius(const G3D::Vector3&, const G3D::Vector3&);
    };
}
