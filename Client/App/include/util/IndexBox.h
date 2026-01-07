#pragma once
#include "G3D/Vector3.h"
#include "G3D/Plane.h"

namespace RBX
{
    class IndexBox
    {
    private:
        G3D::Vector3 corner[8];

    public:
        static const int INDEXBOX_FACE_TO_VERTEX[6][4];
        static const float INDEXBOX_FACE_TO_NORMAL[6][3];
        static const int INDEXBOX_FACE_EDGE_TO_NORMAL[6][4];
        static const int INDEXBOX_EDGE_TO_VERTEX_AND_NORMALS[12][4];

    public:
        IndexBox(const G3D::Vector3& min, const G3D::Vector3& max);
        IndexBox();

        G3D::Vector3 getCenter() const;
        G3D::Vector3 getCorner(int i) const
        {
            return corner[i];
        }
        G3D::Vector3 getFaceNormal(int f) const
        {
            return G3D::Vector3(
                INDEXBOX_FACE_TO_NORMAL[f][0],
                INDEXBOX_FACE_TO_NORMAL[f][1],
                INDEXBOX_FACE_TO_NORMAL[f][2]
            );
        }
        G3D::Vector3 getEdgeNormal(int, int) const;

        void getFaceCorners(int f, G3D::Vector3& v0, G3D::Vector3& v1, G3D::Vector3& v2, G3D::Vector3& v3) const;
        void getEdge(int, G3D::Vector4&, G3D::Vector4&, G3D::Vector3&, G3D::Vector3&) const;
        void getEdge(int, G3D::Vector3&, G3D::Vector3&, G3D::Vector3&, G3D::Vector3&) const;

        bool culledBy(const G3D::Plane*, int) const;
        bool contains(const G3D::Vector3&) const;

        // RBX::IndexBox& operator=(const RBX::IndexBox&);

        static void getTextureCornersCentered(int, const G3D::Vector3&, G3D::Vector2&, G3D::Vector2&, G3D::Vector2&, G3D::Vector2&);
        static void getTextureCornersGrid(int, const G3D::Vector3&, G3D::Vector2&, G3D::Vector2&, G3D::Vector2&, G3D::Vector2&);
    };
}
