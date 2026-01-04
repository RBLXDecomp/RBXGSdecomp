#include "v8kernel/Body.h"
#include "v8kernel/SimBody.h"
#include "util/Debug.h"
using namespace RBX;

Body::Body()
	:index(-1),
	canThrottle(true),
	cofm(NULL),
	root(NULL),
	parent(NULL),
	link(NULL),
	simBody(NULL),
	stateIndex(getNextStateIndex()),
	moment(G3D::Matrix3::identity()),
	mass(0.0f)
{
	root = this;
	simBody = new SimBody(this);
}

Body::~Body()
{
	RBXASSERT(numChildren() == 0);
	if (parent)
		setParent(NULL);
	RBXASSERT(!parent);
	RBXASSERT(!link);
	RBXASSERT(index == -1);
	RBXASSERT(simBody);
	delete simBody;
	simBody = NULL;
	RBXASSERT(!cofm);
	RBXASSERT(root == this);
}

void Body::updatePV() const
{
	RBXASSERT((parent != NULL) || (getRootConst() == this));

	if (parent && this->stateIndex != getRootConst()->getStateIndex())
	{
		Body* tempParent = getParent();
		tempParent->updatePV();
		PV& tempPV = tempParent->pv;
		this->pv = tempPV.pvAtLocalCoord(getMeInParent());
		this->stateIndex = root->getStateIndex();
	}
}

void Body::resetRoot(RBX::Body* newRoot)
{
	RBXASSERT(newRoot == calcRoot());
	root = newRoot;
	for (int i = 0; i < children.size(); i++)
	{
		children[i]->resetRoot(newRoot);
	}
}

float Body::kineticEnergy() const
{
	float calc = (pv.velocity.rotational.dot(getIWorld() * pv.velocity.rotational) + pv.velocity.linear.dot(pv.velocity.linear) * mass) / 2;
	return calc;

}

int Body::getNextStateIndex()
{
	static int p;
	if (++p == INT_MAX)
		p = 1;
	return p;
}

void Body::advanceStateIndex()
{
	stateIndex = getNextStateIndex();
}

bool Body::validateParentCofmDirty()
{
	RBXASSERT(cofm);
	RBXASSERT(cofm->getIsDirty());
	if (getParent())
		getParent()->validateParentCofmDirty();
	return true;
}

void Body::makeCofmDirty()
{
	if (cofm && cofm->getIsDirty())
	{
		// TODO: this match is super fake, i dont know roblox managed to create this cursed assertion
		#if defined(_DEBUG) || defined(_RELEASEASSERT)
		if (Debugable::assertAction == Debugable::CrashOnAssert)
		{
			if (parent)
				parent->validateParentCofmDirty();
			RBXASSERT(getRootSimBody()->getDirty());
		}
		#endif
	}
	else
	{
		if (getParent())
		{
			RBXASSERT(!simBody);
			getParent()->makeCofmDirty();
		}
		else
		{
			RBXASSERT(root == this);
			if (simBody)
				simBody->makeDirty();
		}
		if (cofm)
		{
			cofm->makeDirty();
			RBXASSERT(numChildren() > 0);
		}
		else
		{
			RBXASSERT(!numChildren());
		}
	}
}

void Body::onChildAdded(RBX::Body* child)
{
	makeCofmDirty();
	children.fastAppend(child);
	if (!cofm)
	{
		RBXASSERT(numChildren() == 1);
		cofm = new Cofm(this);
	}
}

void Body::onChildRemoved(RBX::Body* child)
{
	RBXASSERT(cofm);
	children.fastRemove(child);
	if (numChildren() == 0)
	{
		delete cofm;
		cofm = NULL;
	}
	makeCofmDirty();
}

void Body::setMass(float _mass)
{
	if (mass != _mass)
	{
		makeCofmDirty();
		mass = _mass;
	}
}

void Body::step(float dt, bool throttling)
{
	RBXASSERT(!getParent());
	RBXASSERT(simBody);

	if (throttling && canThrottle)
	{
		simBody->resetAccumulators();
	}
	else
	{
		simBody->step(dt);

		pv = cofm == NULL ? simBody->pv : simBody->getOwnerPV();
		advanceStateIndex();
	}
}

void Body::setVelocity(const Velocity& worldVelocity)
{
	if (!getParent())
	{
		pv.velocity = worldVelocity;
		advanceStateIndex();
		if (simBody)
			simBody->makeDirty();
	}
}

void Body::setParent(Body* newParent)
{
	RBXASSERT(!newParent || newParent->getParent() != this);
	RBXASSERT(newParent != this);

	if (parent != newParent)
	{
		if (link)
		{
			link->setBody(NULL);
			link = NULL;
		}

		if (getParent())
		{
			RBXASSERT(root == getParent()->getRoot());
			RBXASSERT(!simBody);
			parent->onChildRemoved(this);
		}
		else
		{
			RBXASSERT(root = this);
			RBXASSERT(simBody);
			delete simBody;
			simBody = NULL;
		}

		parent = newParent;

		if (newParent)
		{
			newParent->onChildAdded(this);
		}
		else
		{
			simBody = new SimBody(this);
		}
		Body* myRoot = getParent() ? getParent()->calcRoot() : this;
		myRoot->advanceStateIndex();
		resetRoot(myRoot);
	}
}

Body* Body::getWorldBody()
{
	static Body b = Body();
	return &b;
}

G3D::CoordinateFrame Body::getMeInDescendant(const Body* descendant) const
{
	if (descendant == this)
	{
		return G3D::CoordinateFrame();
	}
	else if (descendant == parent)
	{
		RBXASSERT(parent);
		return getLink() ? getLink()->getChildInParent() : meInParent;
	}
	else
	{
		RBXASSERT(parent);
		return parent->getMeInDescendant(descendant) * meInParent;
	}
}

void Body::setPv(const PV& _pv)
{
	if (parent)
	{
		RBXASSERT(0);
	}
	else
	{
		pv = _pv;
		if (simBody)
			simBody->makeDirty();
		advanceStateIndex();
	}
	RBXASSERT(Math::longestVector3Component(pv.position.translation) < 1000000.0);
	RBXASSERT(Math::isOrthonormal(pv.position.rotation));
	RBXASSERT(!Math::isNanInfDenormVector3(pv.position.translation));
	RBXASSERT(!Math::isNanInfDenormVector3(pv.velocity.linear));
	RBXASSERT(!Math::isNanInfDenormVector3(pv.velocity.rotational));
}

void Body::setCoordinateFrame(const G3D::CoordinateFrame& worldCord)
{
	updatePV();
	setPv(PV::PV(worldCord, pv.velocity));
}

void Body::setMeInParent(const G3D::CoordinateFrame& _meInParent)
{
	RBXASSERT(Math::longestVector3Component(_meInParent.translation) < 1000000.0);

	if (getLink())
	{
		RBXASSERT(0);
		getLink()->setBody(NULL);
		link = NULL;
	}

	if (getParent())
	{
		meInParent = _meInParent;
		makeCofmDirty();
		root->advanceStateIndex();
	}
	else
	{
		RBXASSERT(0);
	}
}

G3D::Vector3 Body::getBranchCofmPos() const
{
	return cofm ? pv.position.pointToObjectSpace(getPos()) : getCoordinateFrame().translation;
}

G3D::CoordinateFrame Body::getBranchCofmCoordinateFrame() const
{
	return G3D::CoordinateFrame(getPV().position.rotation, getBranchCofmPos());
}

G3D::Matrix3 Body::getIWorldAtPoint(const G3D::Vector3& point) const
{
	float _mass = mass;
	updatePV(); //supposed to be inlined probably
	G3D::Matrix3 iBody = moment;
	G3D::Matrix3& iWorldAtCofm = Math::momentToWorldSpace(iBody, pv.position.rotation);
	return Math::getIWorldAtPoint(getPV().position.translation, point, iWorldAtCofm, _mass);
}

G3D::Matrix3 Body::getBranchIWorldAtPoint(const G3D::Vector3& point) const
{
	float _mass = getBranchMass();
	updatePV(); //more probably inlined updatePV calls
	G3D::Matrix3 _moment = cofm ? cofm->getMoment() : moment;
	G3D::Matrix3& iWorldAtCofm = Math::momentToWorldSpace(_moment, pv.position.rotation);
	return Math::getIWorldAtPoint(getBranchCofmPos(), point, iWorldAtCofm, _mass);
}

void Body::matchDummy()
{
	accumulateForceAtBranchCofm(G3D::Vector3(1.3f,1.2f,1.7f));
	accumulateTorque(G3D::Vector3(6.3f,4.2f,9.7f));
	setMoment(G3D::Matrix3());
	resetAccumulators();
	mass = getBranchMass();
	index = getBranchForce().z + getBranchTorque().y + getBranchIBodyV3().x;
}