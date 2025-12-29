#pragma once
#include "AppDraw/Part.h"
#include "AppDraw/SelectState.h"
#include "v8datamodel/PVInstance.h"
#include "v8datamodel/BrickColor.h"
#include "v8datamodel/Surfaces.h"
#include "v8world/World.h"
#include "v8world/IMoving.h"
#include "util/ICameraSubject.h"
#include "util/IRenderable.h"
#include "util/ISelectable3d.h"

namespace RBX
{
	struct CanAggregateChanged
	{
	public:
		const bool canClump;
  
	public:
		//CanAggregateChanged(const CanAggregateChanged&);
		CanAggregateChanged(bool canAggregate)
			: canClump(canAggregate)
		{
		}
	};

	extern const char* sPart;
	class PartInstance : public DescribedCreatable<PartInstance, PVInstance, &sPart>,
						 public Notifier<PartInstance, CanAggregateChanged>,
						 public IMoving,
						 public IRenderable,
						 public virtual IPrimaryPart,
						 public virtual ICameraSubject,
						 public virtual ISelectable3d
	{
	public:
		enum FormFactor
		{
			SYMETRIC,
			BRICK,
			PLATE
		};

	private:
		Part::PartType partType;
		FormFactor formFactor;
		BrickColor color;
		float transparency;
		float reflectance;
		bool locked;
		mutable Surfaces surfaces;
		float renderImportance;
		boost::scoped_ptr<Primitive> primitive;
		World* myWorld;
		float alphaModifier;
		ComputeProp<Part, PartInstance> PersistentPart;
		ComputeProp<bool, PartInstance> SurfacesNeedAdorn;

	public:
		static bool highlightSleepParts;
		static bool highlightAwakeParts;
		static bool showAnchoredParts;
		static bool showPartCoord;
		static bool showUnalignedParts;
		static bool showSpanningTree;
		
		static const Reflection::PropDescriptor<PartInstance, float> prop_RenderImportance;
		static const Reflection::EnumPropDescriptor<PartInstance, Part::PartType> prop_shapeXml;
		static const Reflection::PropDescriptor<PartInstance, G3D::Color3> prop_Color;
		static const Reflection::PropDescriptor<PartInstance, BrickColor> prop_BrickColor;
		static const Reflection::PropDescriptor<PartInstance, float> prop_Transparency;
		static const Reflection::PropDescriptor<PartInstance, float> prop_Reflectance;
		static const Reflection::PropDescriptor<PartInstance, bool> prop_Locked;
		static const Reflection::PropDescriptor<PartInstance, bool> prop_CanCollide;
		static const Reflection::PropDescriptor<PartInstance, bool> prop_Anchored;
		static const Reflection::PropDescriptor<PartInstance, G3D::Vector3> prop_Size;
		static const Reflection::PropDescriptor<PartInstance, G3D::CoordinateFrame> prop_CFrame;
		static Reflection::SignalDesc<PartInstance, void(boost::shared_ptr<Instance>)> event_Touched;
	  
	private:
		G3D::Vector3 xmlToUiSize(const G3D::Vector3&) const;
		G3D::Vector3 uiToXmlSize(const G3D::Vector3&) const;
		Part computePersistentPart() const;
		bool computeSurfacesNeedAdorn() const;
		void safeMove();
	public:
		//PartInstance(const PartInstance&);
		PartInstance();
		virtual ~PartInstance();
	public:
		const Primitive* getPrimitive() const
		{
			return primitive.get();
		}
		Primitive* getPrimitive()
		{
			return primitive.get();
		}
		void setPartTypeUi(Part::PartType);
		void setPartSizeUi(const G3D::Vector3&);
		void setPartSizeUnjoined(const G3D::Vector3&);
		G3D::Vector3 getPartSizeUi() const;
		void setFormFactorUi(FormFactor);
		void setTranslationUi(const G3D::Vector3&);
		const G3D::Vector3& getTranslationUi() const;
		void setPartTypeXml(Part::PartType);
		Part::PartType getPartType() const
		{
			return partType;
		}
		void setPartSizeXml(const G3D::Vector3& value);
		const G3D::Vector3& getPartSizeXml() const;
		bool resize(NormalId, int);
		void writeResizeData(XmlState*);
		void setFormFactorXml(FormFactor value);
		FormFactor getFormFactor() const
		{
			return formFactor;
		}
		void setPhysics(const G3D::CoordinateFrame& value);
		void setPhysics(const G3D::CoordinateFrame& cf, const Velocity& vel);
		void setCoordinateFrame(const G3D::CoordinateFrame& value);
		const G3D::CoordinateFrame& getCoordinateFrame() const;
		float getMass();
		const Velocity& getVelocity() const;
		void setLinearVelocity(const G3D::Vector3&);
		const G3D::Vector3& getLinearVelocity() const;
		void setRotationalVelocity(const G3D::Vector3&);
		const G3D::Vector3& getRotationalVelocity() const;
		void setFriction(float friction);
		float getFriction() const;
		void setElasticity(float elasticity);
		float getElasticity() const;
		const Surfaces& getSurfaces() const;
		Surfaces& getSurfaces();
		Surface* getSurface(const G3D::Ray&, int&);
		bool getCanCollide() const;
		void setCanCollide(bool value);
		bool getAnchored() const;
		void setAnchored(bool value);
		bool getDragging() const;
		void setDragging(bool value);
		void join();
		void destroyJoints();
		float getRenderImportance() const
		{
			return renderImportance;
		}
		void setRenderImportance(float value);
		float getTransparencyXml() const
		{
			return transparency;
		}
		float getTransparencyUi() const
		{
			return 1.0f - (1.0f - transparency) * alphaModifier;
		}
		bool getIsTransparent() const
		{
			return getTransparencyUi() > 0.1f;
		}
		void setTransparency(float value);
		void setAlphaModifier(float value);
		float getReflectance() const
		{
			return reflectance;
		}
		void setReflectance(float value);
		BrickColor getColor() const
		{
			return color;
		}
		void setColor(BrickColor value);
		G3D::Color3 getColor3() const
		{
			return color.color3();
		}
		void setColor3(const G3D::Color3& value)
		{
			setColor(BrickColor::closest(value));
		}
		bool getPartLocked() const
		{
			return locked;
		}
		void setPartLocked(bool value);
		const Part& getPart() const;
		float alpha() const;
		bool lockedInPlace() const;
		bool aligned() const;
		G3D::CoordinateFrame worldSnapLocation() const;
		void onTouchThisStep(boost::shared_ptr<PartInstance> other);
		virtual void onCanAggregateChanged(bool canAggregate);
		virtual bool reportTouches() const;
		virtual void onAncestorChanged(const AncestorChanged&);
		virtual bool askSetParent(const Instance*) const;
		virtual void onChildAdded(Instance*);
		virtual void onGuidChanged();
		virtual const G3D::CoordinateFrame getLocation() const
		{
			return getCoordinateFrame();
		}
		virtual void onCameraNear(float);
		virtual void getCameraIgnorePrimitives(std::vector<const Primitive*>& primitives)
		{
			primitives.push_back(primitive.get());
		}
		virtual bool shouldRender3dAdorn() const;
		virtual void render3dAdorn(Adorn*);
		virtual void render3dSelect(Adorn*, SelectState);
		virtual bool isControllable() const;
		virtual PartInstance* getPrimaryPart()
		{
			return this;
		}
		virtual const PartInstance* getPrimaryPartConst() const
		{
			return this;
		}
		virtual void legacyTraverseState(const G3D::CoordinateFrame& parentState);
		virtual void onParentControllerChanged();
		virtual const Primitive* getBiggestPrimitive() const
		{
			return primitive.get();
		}
		virtual bool hitTest(const G3D::Ray&, G3D::Vector3&);
		virtual Extents getExtentsWorld() const;
		virtual Extents getExtentsLocal() const;
	private:
		void onSurfaceChanged(RBX::NormalId surfId);
		void writeSize(XmlState*);
		void raiseSurfacePropertyChanged(const RBX::Reflection::PropertyDescriptor&);
	public:
		//PartInstance& operator=(const PartInstance&);
	  
	public:
		static float plateHeight();
		static float brickHeight();
		static float cameraTransparentDistance();
		static float cameraTranslucentDistance();
		static bool nonNullInWorkspace(boost::shared_ptr<PartInstance> part);
		static PartInstance* fromPrimitive(Primitive*);
		static const RBX::PartInstance* fromPrimitiveConst(const Primitive*);
		static void primitivesToParts(const G3D::Array<Primitive*>&, std::vector<boost::weak_ptr<PartInstance>>&);
		static void primitivesToParts(const G3D::Array<Primitive*>&, G3D::Array<boost::shared_ptr<PartInstance>>&);
		static void findParts(Instance*, std::vector<boost::weak_ptr<RBX::PartInstance>>&);
		static bool getLocked(Instance* instance);
		static void setLocked(Instance*, bool);
	private:
		static float defaultFriction();
		static float defaultElasticity();
	};
}
