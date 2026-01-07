#include "v8world/RotateJoint.h"
#include "v8world/Primitive.h"
#include "v8world/SurfaceData.h"
#include "v8world/Tolerance.h"
#include "v8world/World.h"
#include "v8kernel/Constants.h"
#include "v8kernel/Body.h"
#include "util/Face.h"

namespace RBX
{
	// TODO: check which of these need to go into the header

	RotateJoint::RotateJoint(
		Primitive* axlePrim,
		Primitive* holePrim,
		const G3D::CoordinateFrame& c0,
		const G3D::CoordinateFrame& c1)
		: MultiJoint(axlePrim, holePrim, c0, c1, 2),
		  rotateConnector(NULL)
	{
	}

	RotateJoint::~RotateJoint()
	{
		RBXASSERT(!rotateConnector);
	}

	void RotateJoint::removeFromKernel()
	{
		RBXASSERT(getKernel());
		rotateConnector = NULL;
		MultiJoint::removeFromKernel();
	}

	float RotateJoint::getChannelValue(int uiStepId)
	{
		RBXASSERT(getAxlePrim());
		const SurfaceData& axleSurface = getAxlePrim()->getSurfaceData(getAxleId());

		Controller* controller = getAxlePrim()->getController();
		RBXASSERT(controller);

		float value = controller->getValue(axleSurface.inputType);
		float paramA = axleSurface.paramA;
		float paramB = axleSurface.paramB;

		float dt = Constants::uiDt() * uiStepId;

		switch (axleSurface.inputType)
		{
		case Controller::NO_INPUT:
			return 0.0;
		case Controller::CONSTANT_INPUT:
			return paramB;
		case Controller::SIN_INPUT:
			return sin(dt * paramB) * paramA;
		case Controller::LEFT_TRACK_INPUT:
			if (value < -0.1)
				return value * fabs(paramA);
			else if (value > 0.1)
				return value * fabs(paramB);
			return 0.0;
		case Controller::RIGHT_TRACK_INPUT:
			if (value < -0.1)
				return -(value * fabs(paramA));
			else if (value > 0.1)
				return -(value * fabs(paramB));
			return 0.0;
		default:
			if (value < -0.1)
				return -(value * paramA);
			else if (value > 0.1)
				return value * paramB;
			return 0.0;
		}
	}

	RotateJoint* RotateJoint::surfaceTypeToJoint(
		SurfaceType surfaceType,
		Primitive* axlePrim,
		Primitive* holePrim,
		const G3D::CoordinateFrame& c0,
		const G3D::CoordinateFrame& c1)
	{
		switch (surfaceType)
		{
		case ROTATE:
			return new RotateJoint(axlePrim, holePrim, c0, c1);
		case ROTATE_P:
			return new RotatePJoint(axlePrim, holePrim, c0, c1);
		case ROTATE_V:
			return new RotateVJoint(axlePrim, holePrim, c0, c1);
		default:
			RBXASSERT(0);
			return NULL;
		}
	}

	RotateJoint* RotateJoint::canBuildJoint(Primitive* p0, Primitive* p1, NormalId nId0, NormalId nId1)
	{
		SurfaceType s0 = p0->getSurfaceType(nId0);
		SurfaceType s1 = p1->getSurfaceType(nId1);
		if (!(s0 >= ROTATE || s1 >= ROTATE))
			return NULL;

		const G3D::CoordinateFrame& c0 = p0->getCoordinateFrame();
		const G3D::CoordinateFrame& c1 = p1->getCoordinateFrame();

		G3D::Vector3 n0 = Math::getWorldNormal(nId0, c0);
		G3D::Vector3 n1 = Math::getWorldNormal(nId1, c1);

		G3D::Vector3 holePtWorld = -n1;
		if (Math::angle(n0, holePtWorld) <= 0.025f)
		{
			Face f0 = p0->getFaceInWorld(nId0);
			Face f1 = p1->getFaceInWorld(nId1);

			if (Face::hasOverlap(f0, f1, 0.35f) && Face::overlapWithinPlanes(f0, f1, 0.05f))
			{
				G3D::Vector3 center0 = f0.center();
				G3D::Vector3 center1 = f1.center();

				bool inExtrusion0 = f0.fuzzyContainsInExtrusion(center0, 0.05f);
				bool inExtrusion1 = f1.fuzzyContainsInExtrusion(center1, 0.05f);
				bool cond0 = s0 >= ROTATE && inExtrusion0;
				bool cond1 = s1 >= ROTATE && inExtrusion1;

				if (!cond0 && !cond1)
					return NULL;

				if (!cond0 || (cond1 && p0->getGridSize().sum() > p1->getGridSize().sum()))
				{
					std::swap(nId0, nId1);
					std::swap(s0, s1);
					std::swap(center0, center1);
					std::swap(n0, n1);
					std::swap(p0, p1);
				}

				G3D::CoordinateFrame axleInP0 = p0->getFaceCoordInObject(nId0);
				G3D::Vector3 axlePtWorld = p0->getCoordinateFrame().pointToWorldSpace(axleInP0.translation);
				G3D::Vector3 axlePtInP1 = p1->getCoordinateFrame().pointToObjectSpace(axlePtWorld);
				
				NormalId oppNId1 = normalIdOpposite(nId1);
				G3D::Vector3 byAtLeast = Math::toGrid(axlePtInP1, 0.1f);

				G3D::CoordinateFrame holeInP1(normalIdToMatrix3(oppNId1), byAtLeast);
				G3D::Vector3 holePtWorld = p1->getCoordinateFrame().pointToWorldSpace(holeInP1.translation);
				if (!Tolerance::pointsUnaligned(axlePtWorld, holePtWorld))
				{
					for (int i = 0; i < 2; i++)
					{
						float multiplier = i == 0 ? 1.0f : -1.0f;

						G3D::Vector3 ref0 = axlePtWorld - n0 * multiplier;
						G3D::Vector3 ref1 = holePtWorld + n1 * multiplier;
						if (Tolerance::pointsUnaligned(ref0, ref1))
							return NULL;
					}

					return surfaceTypeToJoint(s0, p0, p1, axleInP0, holeInP1);
				}
			}
		}

		return NULL;
	}

	float RotateJoint::getTorqueArmLength()
	{
		Primitive* axlePrim = getAxlePrim();
		Primitive* holePrim = getHolePrim();

		G3D::Vector3 ownerSize = axlePrim->getGridSize();
		G3D::Vector3 otherSize = holePrim->getGridSize();

		NormalId ownerId = getAxleId();
		NormalId otherId = normalIdOpposite(getHoleId());

		float ownerMax = G3D::max(ownerSize[(2 + (int)ownerId) % 3], ownerSize[(1 + (int)ownerId) % 3]);
		float otherMax = G3D::max(otherSize[(2 + (int)otherId) % 3], otherSize[(1 + (int)otherId) % 3]);
		float min = G3D::min(ownerMax, otherMax);

		return min * 0.1f;
	}

	void RotateJoint::putInKernel(Kernel* _kernel)
	{
		MultiJoint::putInKernel(_kernel);

		Body* b0 = getAxlePrim()->getBody();
		Body* b1 = getHolePrim()->getBody();

		G3D::CoordinateFrame cAxleWorld = b0->getCoordinateFrame() * jointCoord0;
		G3D::CoordinateFrame cHoleWorld = b1->getCoordinateFrame() * jointCoord1;

		G3D::Vector3 vAxleWorld = cAxleWorld.rotation.getColumn(2);
		G3D::Vector3 vHoleWorld = cHoleWorld.rotation.getColumn(2);

		for (int i = 0; i < 2; i++)
		{
			float multiplier = i == 0 ? -1.0f : 1.0f;

			G3D::Vector3 pAxleWorldPos = vAxleWorld * multiplier + cAxleWorld.translation;
			G3D::Vector3 pHoleWorldPos = vHoleWorld * multiplier + cHoleWorld.translation;

			Point* pAxleWorld = getKernel()->newPoint(b0, pAxleWorldPos);
			Point* pHoleWorld = getKernel()->newPoint(b1, pHoleWorldPos);

			PointToPointBreakConnector* connector = new PointToPointBreakConnector(pAxleWorld, pHoleWorld, getJointK(), Math::inf());
			addToMultiJoint(pAxleWorld, pHoleWorld, connector);
		}

		if (getJointType() == ROTATE_P_JOINT || getJointType() == ROTATE_V_JOINT)
		{
			G3D::Vector3 center0 = cAxleWorld.translation;
			G3D::Vector3 perp = cAxleWorld.rotation.getColumn(0);
			G3D::Vector3 marker0 = perp * 10.0f + center0;

			Point* pMarker0 = getKernel()->newPoint(b0, marker0);
			Point* pMarker1 = getKernel()->newPoint(b1, marker0);

			RBXASSERT(!rotateConnector);
			RotateConnector* connector = new RotateConnector(getPoint(0), getPoint(2), pMarker0, pMarker1, getJointK(), getTorqueArmLength());
			rotateConnector = connector;
			addToMultiJoint(pMarker0, pMarker1, connector);
		}
	}

	void RotateVJoint::stepUi(int uiStepId)
	{
		float value = getChannelValue(uiStepId);

		RotateConnector* connector = rotateConnector;
		if (connector)
		{
			float newValue = value / (float)Constants::kernelStepsPerUiStep();
			connector->getKernelInput()->setDelta(newValue);
		}

		if (value != 0.0f)
		{
			World* world = getWorld();
			world->ticklePrimitive(getAxlePrim());
			world->ticklePrimitive(getHolePrim());
		}
	}

	inline float calculateIncrementRotatePJoint(float value, float currentGoal, int steps)
	{
		return (value - currentGoal) / steps;
	}

	void RotatePJoint::stepUi(int uiStepId)
	{
		float value = getChannelValue(uiStepId);

		RotateConnector* connector = rotateConnector;
		if (connector)
		{
			connector->getKernelInput()->setGoalCalc(value, calculateIncrementRotatePJoint);
		}

		if (value != 0.0f)
		{
			World* world = getWorld();
			world->ticklePrimitive(getAxlePrim());
			world->ticklePrimitive(getHolePrim());
		}
	}
}