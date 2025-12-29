#pragma once
#include "v8datamodel/PVInstance.h"
#include "v8datamodel/ICameraOwner.h"
#include "util/IRenderable.h"
#include "util/ICameraSubject.h"
#include "util/ISelectable3d.h"
#include "util/Extents.h"

namespace RBX
{
	class Primitive;
	class PartInstance;

	extern const char* sModel;
	class ModelInstance : public DescribedCreatable<ModelInstance, PVInstance, &sModel>,
						  public IRenderable,
						  public virtual IPrimaryPart,
						  public virtual ICameraSubject,
						  public virtual ISelectable3d
	{
	private:
		mutable G3D::CoordinateFrame modelInPrimary;
		mutable PartInstance* primaryPart;
		boost::shared_ptr<PartInstance> candidatePrimaryPart;
		ComputeProp<float, ModelInstance> FlagHeight;
		ComputeProp<Extents, ModelInstance> LocalGridExtents;
		ComputeProp<Extents, ModelInstance> WorldGridExtents;

	public:
		static bool showModelCoord;

	private:
		void updatePrimaryPart(PartInstance* set) const;
		float computeFlagHeight() const;
		Extents computeLocalGridExtents() const;
		Extents computeWorldGridExtents() const;
		void dirtyAll() const;
		virtual bool shouldRender3dAdorn() const;
		virtual void render3dAdorn(Adorn* adorn);
		virtual void render3dSelect(Adorn* adorn, SelectState selectState);
	protected:
		virtual void onDescendentAdded(Instance* instance);
		virtual void onDescendentRemoving(const boost::shared_ptr<Instance>& instance);
		virtual bool askSetParent(const Instance* instance) const;
	private:
		virtual void onLastChildRemoved()
		{
		}
	public:
		//ModelInstance(const ModelInstance&);
		ModelInstance();
		virtual ~ModelInstance();
	public:
		PartInstance* getPrimaryPartInternal() const;
		void setPrimaryPart(PartInstance* set);
		const G3D::CoordinateFrame& getModelInPrimary() const
		{
			return modelInPrimary;
		}
		void setModelInPrimary(const G3D::CoordinateFrame& set)
		{
			modelInPrimary = set;
		}
		virtual void onExtentsChanged() const;
		virtual const Primitive* getBiggestPrimitive() const;
		virtual bool hitTest(const G3D::Ray& worldRay, G3D::Vector3& worldHitPoint);
		virtual Extents getExtentsWorld() const
		{
			return WorldGridExtents;
		}
		virtual Extents getExtentsLocal() const
		{
			return LocalGridExtents;
		}
		virtual const G3D::CoordinateFrame getLocation() const;
		virtual void onCameraNear(float distance);
		virtual void getCameraIgnorePrimitives(std::vector<const Primitive*>& primitives);
		virtual PartInstance* getPrimaryPart()
		{
			return getPrimaryPartInternal();
		}
		virtual const PartInstance* getPrimaryPartConst() const
		{
			return getPrimaryPartInternal();
		}
		void setFrontAndTop(const G3D::Vector3&);
		void destroyIfNoParts(const ModelInstance*);
		void makeJoints();
		void breakJoints();
		float computeTotalMass() const;
		float computeLargestMoment() const;
	protected:
		virtual void legacyTraverseState(const G3D::CoordinateFrame& parentState);
	public:
		//ModelInstance& operator=(const ModelInstance&);
	};
}

