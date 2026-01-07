#pragma once
#include "v8datamodel/JointInstance.h"

namespace RBX
{
    extern const char* sFeature;
    class Feature : public DescribedNonCreatable<Feature, Instance, &sFeature>,
                    public IRenderable
    {
    public:
        enum TopBottom
        {
            TOP = 0,
            CENTER_TB = 1,
            BOTTOM = 2,
        };

        enum LeftRight
        {
            LEFT = 0,
            CENTER_LR = 1,
            RIGHT = 2
        };

        enum InOut
        {
            EDGE = 0,
            INSET = 1,
            CENTER_IO = 2
        };

        enum InOutZ
        {
            Z_IN = 0,
            Z_OUT = 1
        };

    public:
        NormalId faceId;
        TopBottom topBottom;
        LeftRight leftRight;
        InOut inOut;

    private:
        virtual bool askSetParent(const Instance*) const;
        virtual bool shouldRender3dAdorn() const;
        virtual void render3dSelect(Adorn* adorn, SelectState selectState);

    protected:
        bool getRenderCoord(G3D::CoordinateFrame& c) const;
        virtual Feature::InOutZ getCoordOrientation() const;

    public:
        Feature();
        virtual ~Feature();

        NormalId getFaceId() const
        {
            return faceId;
        }
        void setFaceId(NormalId value);

        Feature::TopBottom getTopBottom() const
        {
            return topBottom;
        }
        void setTopBottom(TopBottom value);

        Feature::LeftRight getLeftRight() const
        {
            return leftRight;
        }
        void setLeftRight(LeftRight value);

        Feature::InOut getInOut() const
        {
            return inOut;
        }
        void setInOut(InOut value);

        G3D::CoordinateFrame computeLocalCoordinateFrame() const;
    };

    extern const char* sHole;
    class Hole : public DescribedCreatable<Hole, Feature, &sHole>
    {
    private:
        virtual void render3dAdorn(Adorn* adorn);
        virtual Feature::InOutZ getCoordOrientation() const;

    public:
        Hole();
        virtual ~Hole();
    };

    extern const char* sMotorFeature;
    class MotorFeature : public DescribedCreatable<MotorFeature, Feature, &sMotorFeature>
    {
    private:
        void otherFeatureChanged();
        virtual void render3dAdorn(Adorn* adorn);

    public:
        MotorFeature();
        virtual ~MotorFeature();

        static bool canJoin(Instance*, Instance*);
        static void join(Instance*, Instance*);
    };

    extern const char* sVelocityMotor;
    class VelocityMotor : public DescribedCreatable<VelocityMotor, JointInstance, &sVelocityMotor>
    {
    private:
        boost::shared_ptr<Hole> hole;
        boost::signals::scoped_connection holeAncestorChanged;

        const MotorJoint* motorJoint() const
        {
            return rbx_static_cast<MotorJoint*>(joint);
        }
        MotorJoint* motorJoint()
        {
            return rbx_static_cast<MotorJoint*>(joint);
        }

        void onEvent_HoleAncestorChanged();
        void setPart(int i, Feature* feature);

        virtual bool askSetParent(const Instance*) const;
        virtual void onAncestorChanged(const AncestorChanged& event);

    public:
        VelocityMotor();
        virtual ~VelocityMotor();

        Hole* getHole() const
        {
            return hole.get();
        }
        void setHole(Hole* value);

        float getMaxVelocity() const;
        void setMaxVelocity(float value);

        float getDesiredAngle() const;
        void setDesiredAngle(float value);

        float getCurrentAngle() const;
        void setCurrentAngle(float value);
    };
}
