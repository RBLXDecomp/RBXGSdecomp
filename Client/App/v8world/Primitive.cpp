#include "v8world/Primitive.h"
#include "v8world/Geometry.h"
#include "v8world/Assembly.h"
#include "v8world/Ball.h"
#include "v8world/Block.h"
#include "v8world/World.h"
#include <cmath>

namespace RBX 
{
	bool Primitive::ignoreBool = false;
	bool Primitive::disableSleep = false;

	#pragma warning (push)
	#pragma warning (disable : 4355) // warning C4355: 'this' : used in base member initializer list
	Primitive::Primitive(Geometry::GeometryType geometryType) :
		guidSetExternally(false),
		world(NULL),
		clump(NULL),
		spatialNodes(NULL),
		worldIndex(-1),
		clumpDepth(-1),
		traverseId(-1),
		fuzzyExtentsStateId(-2),
		geometry(newGeometry(geometryType)),
		body(new Body),
		elasticity(0.75f),
		friction(0.0f),
		myOwner(NULL),
		anchorObject(NULL),
		dragging(false),
		anchored(false),
		canCollide(true),
		canSleep(true),
		JointK(this, &Primitive::computeJointK),
		controller(NullController::getStaticNullController())
	{
		for(int i = 0; i < 6; i++) 
		{
			surfaceType[i] = NO_SURFACE;
			surfaceData[i] = NULL;
		}
	}
	#pragma warning (pop)

	Primitive::~Primitive()
	{
		Geometry::GeometryType type = geometry->getGeometryType();

		if(type != Geometry::GEOMETRY_NONE) {
			delete geometry;
			delete body;
		}

		for(int i = 0; i < 6; i++)
			delete surfaceData[i];

		RBXASSERT(!world);
		RBXASSERT(!clump);
	}

	G3D::Vector3 Primitive::clipToSafeSize(const G3D::Vector3& newSize)
	{
		G3D::Vector3 r = newSize.min(Vector3(512.0f, 512.0f, 512.0f));

		if(r.x * r.y * r.z > 1e+6f) 
		{
			r.y = floorf(1e+6f / (r.x * r.z));
		}

		return r;
	}

	float Primitive::computeJointK() const 
	{
		Geometry::GeometryType type = geometry->getGeometryType();
		return Constants::getJointK(geometry->getGridSize(), type == Geometry::GEOMETRY_BALL);
	}

	void Primitive::setGuid(const Guid& value) 
	{
		RBXASSERT(!world);
		guid.copyDataFrom(value);
		guidSetExternally = true;
	}

	void Primitive::setClump(Clump* set)
	{
		if(set != clump)
		{
			clump = set;
		}
	}

	const G3D::CoordinateFrame& Primitive::getCoordinateFrame() const
	{
		return body->getPV().position;
	}
	
	G3D::CoordinateFrame Primitive::getGridCorner() const
	{
		const G3D::CoordinateFrame& pos = body->getPV().position;
		G3D::Vector3 hVec = -(geometry->getGridSize() * 0.5f);

		return G3D::CoordinateFrame(pos.rotation, pos.pointToWorldSpace(hVec));
	}

	void Primitive::setGridCorner(const G3D::CoordinateFrame& gridCorner)
	{
		G3D::Vector3 hVec = geometry->getGridSize() * 0.5;
		setCoordinateFrame(G3D::CoordinateFrame(gridCorner.rotation, gridCorner.pointToWorldSpace(hVec)));
	}

	void Primitive::setVelocity(const Velocity& vel)
	{
		body->setVelocity(vel);
	}

	float Primitive::getPlanarSize() const
	{
		return Math::planarSize(geometry->getGridSize());
	}

	bool Primitive::hitTest(const G3D::Ray& worldRay, G3D::Vector3& worldHitPoint, bool& inside) 
	{
		G3D::Ray localRay = body->getPV().position.toObjectSpace(worldRay);
		G3D::Vector3 localHitPoint;

		if(geometry->hitTest(localRay, localHitPoint, inside))
		{
			worldHitPoint = body->getPV().position.pointToWorldSpace(localHitPoint);
			return true;
		}
		else return false;
	}

	Face Primitive::getFaceInObject(NormalId objectFace)
	{
		return Face::fromExtentsSide(getExtentsLocal(), objectFace);
	}

	Face Primitive::getFaceInWorld(NormalId objectFace)
	{
		return getFaceInObject(objectFace).toWorldSpace(body->getPV().position);
	}

	G3D::CoordinateFrame Primitive::getFaceCoordInObject(NormalId objectFace)
	{
		return G3D::CoordinateFrame(normalIdToMatrix3(objectFace), normalIdToVector3(objectFace) * geometry->getGridSize());
	}

	void Primitive::setSurfaceType(NormalId id, SurfaceType newSurfaceType)
	{
		if(surfaceType[id] != newSurfaceType)
			surfaceType[id] = newSurfaceType;
	}

	void Primitive::setSurfaceData(NormalId id, const SurfaceData& newSurfaceData)
	{
		if(!surfaceData[id] && newSurfaceData.isEmpty())
			return;

		if(surfaceData[id] && *surfaceData[id] == newSurfaceData)
			return;

		if(newSurfaceData.isEmpty())
		{
			RBXASSERT(surfaceData[id]);
			delete surfaceData[id];
			surfaceData[id] = NULL;
			return;
		}
		else if(!surfaceData[id])
		{
			surfaceData[id] = new SurfaceData;
		}

		*surfaceData[id] = newSurfaceData;
	}

	Edge* Primitive::getFirstEdge() const
	{
		return joints.first ? joints.first : contacts.first;
	}

	Joint* Primitive::getFirstJoint() const
	{
		return rbx_static_cast<Joint*>(joints.first);
	}

	Contact* Primitive::getFirstContact()
	{
		return rbx_static_cast<Contact*>(contacts.first);
	}

	RigidJoint* Primitive::getFirstRigid()
	{
		return getFirstRigidAt(getFirstEdge());
	}

	RigidJoint* Primitive::getNextRigid(RigidJoint* prev)
	{
		return getFirstRigidAt(getNextEdge(prev));
	}

	Geometry* Primitive::newGeometry(Geometry::GeometryType geometryType)
	{
		switch(geometryType)
		{
			case Geometry::GEOMETRY_BLOCK: return new Block;
			case Geometry::GEOMETRY_BALL: return new Ball;
			default: return Geometry::nullGeometry();
		}
	}

	void newTouch(Primitive* touch, Primitive* touchOther)
	{
		Clump* clumpTouch = touch->getClump();
		Clump* clumpOTouch = touchOther->getClump();

		RBXASSERT(clumpTouch);
		RBXASSERT(clumpOTouch);

		if(touch->getOwner()->reportTouches() && 
			((!clumpTouch->getAnchored() && clumpTouch->getSleepStatus() == Sim::AWAKE) ||
			(!clumpOTouch->getAnchored() && clumpOTouch->getSleepStatus() == Sim::AWAKE)))
		{
			touch->getWorld()->onPrimitiveTouched(touch, touchOther);
		}
	};

	void Primitive::onNewTouch(Primitive* p0, Primitive* p1)
	{
		newTouch(p0, p1);
		newTouch(p1, p0);
	}

	Assembly* Primitive::getAssembly() const 
	{
		return clump ? clump->getAssembly() : NULL;
	}

	void Primitive::setPrimitiveType(Geometry::GeometryType geometryType)
	{
		RBXASSERT(geometry->getGeometryType() != Geometry::GEOMETRY_NONE);
		RBXASSERT(geometryType != Geometry::GEOMETRY_NONE);

		if(geometry->getGeometryType() != geometryType)
		{
			G3D::Vector3 oldSize = geometry->getGridSize();
			if(geometry)
				delete geometry;

			geometry = newGeometry(geometryType);

			if(world)
				world->onPrimitiveGeometryTypeChanged(this);

			setGridSize(oldSize);
			JointK.setDirty();
		}
	}

	void Primitive::setCoordinateFrame(const G3D::CoordinateFrame& value)
	{
		if(value != body->getPV().position)
		{
			Assembly* assembly = getAssembly();
			if(!assembly)
			{
				body->setCoordinateFrame(value);
				myOwner->notifyMoved();
				if(world)
					world->onPrimitiveExtentsChanged(this);
			}
			else
			{
				RBXASSERT(world);
				if(assembly->getMainPrimitive() == this)
				{
					body->setCoordinateFrame(value);
					assembly->notifyMoved();
					world->onAssemblyExtentsChanged(assembly);
					if(!anchored)
						world->ticklePrimitive(this);
				}
			}
		}
	}

	void Primitive::setDragging(bool _dragging)
	{
		if(dragging != _dragging)
		{
			bool oldDrag = !dragging && canCollide;
			dragging = _dragging;

			setAnchor(anchored);
			if(world)
			{
				bool newDrag = !dragging && canCollide;
				if(newDrag != oldDrag)
					world->onPrimitiveCanCollideChanged(this);
			}
		}
	}

	void Primitive::setAnchor(bool _anchored)
	{
		anchored = _anchored;

		bool exists = getAnchor();

		if(_anchored || dragging)
			_anchored = true;

		if(_anchored == exists)
			return;

		if(_anchored)
		{
			RBXASSERT(!anchorObject);
			anchorObject = new Anchor(this);
			if(world)
				world->onPrimitiveAddedAnchor(this);
		}
		else
		{
			RBXASSERT(anchorObject);
			if(world)
				world->onPrimitiveRemovingAnchor(this);
			
			if(anchorObject)
				delete anchorObject;

			anchorObject = NULL;
		}
	}

	void Primitive::setCanCollide(bool value)
	{
		bool oldDrag = !dragging && canCollide;

		if(canCollide != value)
		{
			canCollide = value;
			if(world)
			{
				bool newDrag = !dragging && value;
				if(oldDrag != newDrag)
					world->onPrimitiveCanCollideChanged(this);
			}
		}
	}

	void Primitive::setCanSleep(bool _canSleep)
	{
		if(_canSleep != canSleep)
		{
			canSleep = _canSleep;
			if(world)
				world->onPrimitiveCanSleepChanged(this);
		}
	}

	void Primitive::setFriction(float _friction)
	{
		if(_friction != friction)
		{
			friction = _friction;
			if(world)
				world->onPrimitiveContactParametersChanged(this);
		}
	}

	void Primitive::setElasticity(float _elasticity)
	{
		if(_elasticity != elasticity)
		{
			elasticity = _elasticity;
			if(world)
				world->onPrimitiveContactParametersChanged(this);
		}
	}

	void Primitive::setGridSize(const G3D::Vector3& gridSize) // 100% match when Body::setMoment is set as __declspec(noinline)
	{
		G3D::Vector3 protectedSize = clipToSafeSize(gridSize);

		if(protectedSize != geometry->getGridSize())
		{
			fuzzyExtentsStateId = -2;
			geometry->setGridSize(protectedSize);

			float newSize = geometry->getGridVolume();
			body->setMass(newSize);
			body->setMoment(geometry->getMoment(newSize));
			JointK.setDirty();

			if(world)
				world->onPrimitiveExtentsChanged(this);

			JointK.setDirty();
		}
	}

	bool Primitive::aaBoxCollide(const Primitive& p0, const Primitive& p1)
	{
		G3D::Vector3 delta = p0.getCoordinateFrame().translation - p1.getCoordinateFrame().translation;
		float r0 = p0.getRadius();
		float r1 = p1.getRadius();
		
		if (r0 + r1 < Math::longestVector3Component(delta))
			return false;

		const Extents& f1 = p1.getFastFuzzyExtents();
		const Extents& f0 = p0.getFastFuzzyExtents();
		return f0.overlapsOrTouches(f1);
	}

	Extents Primitive::computeFuzzyExtents() const
	{
		Extents ext = Extents::fromCenterCorner(body->getPV().position.translation, geometry->getCenterToCorner(body->getPV().position.rotation));
		ext.expand(0.01f);

		return ext;
	}

	void Primitive::setController(Controller* _controller)
	{
		if(!_controller)
			_controller = NullController::getStaticNullController();

		if(controller != _controller)
			controller = _controller;
	}

	const Extents& Primitive::getFastFuzzyExtents() const
	{
		if(fuzzyExtentsStateId != body->getStateIndex())
		{
			fuzzyExtents = computeFuzzyExtents();
			fuzzyExtentsStateId = body->getStateIndex();
		}

		return fuzzyExtents;
	}

	Joint* Primitive::getNextJoint(Joint* prev) const
	{
		return rbx_static_cast<Joint*>(prev->getNext(this));
	}

	Contact* Primitive::getNextContact(Contact* prev)
	{
		return rbx_static_cast<Contact*>(prev->getNext(this));
	}

	Edge* Primitive::getNextEdge(Edge* e) const
	{
		Edge* next = e->getNext(this);

		if(next)
			return next;
		else
			return Joint::isJoint(e) ? contacts.first : NULL;
	}

	Joint* Primitive::getJoint(Primitive* p0, Primitive* p1)
	{
		Primitive* least = p0->getNumJoints2() < p1->getNumJoints2() ? p0 : p1;

		for(Joint* current = least->getFirstJoint(); current != NULL; current = least->getNextJoint(current))
		{
			if(p0 == current->getPrimitive(0) && p1 == current->getPrimitive(1) || p0 == current->getPrimitive(1) && p1 == current->getPrimitive(0))
			   return current;
		}
		return NULL;
	}

	Contact* Primitive::getContact(Primitive* p0, Primitive* p1)
	{
		Primitive* least = p0->getNumContacts() < p1->getNumContacts() ? p0 : p1;

		for(Contact* current = least->getFirstContact(); current != NULL; current = least->getNextContact(current))
		{
			if(p0 == current->getPrimitive(0) && p1 == current->getPrimitive(1) || p0 == current->getPrimitive(1) && p1 == current->getPrimitive(0))
			   return current;
		}
		return NULL;
	}

	RigidJoint* Primitive::getFirstRigidAt(Edge* start)
	{
		for(Edge* current = start; current != NULL; current = getNextEdge(current))
		{
			if(RigidJoint::isRigidJoint(current))
				return rbx_static_cast<RigidJoint*>(current);
		}
		return NULL;
	}

	int Primitive::countNumJoints() const
	{
		int counter = 0;
		for(Joint* current = getFirstJoint(); current != NULL; current = getNextJoint(current))
		{
			Joint::JointType type = current->getJointType();
			if(type != Joint::FREE_JOINT && type != Joint::ANCHOR_JOINT)
				counter++;
		}
		return counter;
	}

	void Primitive::insertEdge(Edge* e)
	{
		Primitive* p0 = e->getPrimitive(0);
		Primitive* p1 = e->getPrimitive(1);

		if(Joint::isJoint(e))
		{
			EdgeList::insertEdge(p0, e, e->getPrimitive(0)->joints);

			if(p1)
				EdgeList::insertEdge(p1, e, e->getPrimitive(1)->joints);
			else
				RBXASSERT(rbx_static_cast<Joint*>(e)->getJointType() == Joint::ANCHOR_JOINT || rbx_static_cast<Joint*>(e)->getJointType() == Joint::FREE_JOINT);
		}
		else
		{
			EdgeList::insertEdge(p0, e, e->getPrimitive(0)->contacts);
			EdgeList::insertEdge(p1, e, e->getPrimitive(1)->contacts);
		}
	}

	void Primitive::removeEdge(Edge* e)
	{
		Primitive* p0 = e->getPrimitive(0);
		Primitive* p1 = e->getPrimitive(1);

		if(Joint::isJoint(e))
		{
			EdgeList::removeEdge(p0, e, e->getPrimitive(0)->joints);

			if(p1)
				EdgeList::removeEdge(p1, e, e->getPrimitive(1)->joints);
			else
				RBXASSERT(rbx_static_cast<Joint*>(e)->getJointType() == Joint::ANCHOR_JOINT || rbx_static_cast<Joint*>(e)->getJointType() == Joint::FREE_JOINT);
		}
		else
		{
			EdgeList::removeEdge(p0, e, e->getPrimitive(0)->contacts);
			EdgeList::removeEdge(p1, e, e->getPrimitive(1)->contacts);
		}
	}

	void EdgeList::insertEdge(Primitive* p, Edge* e, EdgeList& list)
	{
		e->setNext(p, list.first);
		list.first = e;
		list.num++;
	}

	void EdgeList::removeEdge(Primitive* p, Edge* e, EdgeList& list)
	{
		if(list.first == e)
		{
			list.first = list.first->getNext(p);
		}
		else
		{
			Edge* currentEdge = list.first;

			while(currentEdge->getNext(p) != e)
			{
				currentEdge = currentEdge->getNext(p);
			}

			currentEdge->setNext(p, e->getNext(p));
		}

		list.num--;
	}
}