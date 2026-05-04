#pragma once

#include "RenderLib/Chunk.h"
#include "v8datamodel/PartInstance.h"
#include "reflection/reflection.h"
#include <G3D/CoordinateFrame.h>
#include <boost/signals/connection.hpp>
#include <boost/shared_ptr.hpp>

namespace RBX 
{
	class SpecialShape;
	class Instance;
	class Decal;
	class Texture;

	namespace View 
	{
		class View;

		class __declspec(novtable) PartChunk : public RBX::Render::Chunk 
		{
		private:
			G3D::CoordinateFrame coordinateFrame;
			boost::signals::scoped_connection childAddedConnection;
			boost::signals::scoped_connection childRemovedConnection;
			boost::signals::scoped_connection ancestorChangedConnection;
			boost::signals::scoped_connection propertyChangedConnection;
			boost::signals::scoped_connection shapePropertyChangedConnection;
		protected:
			boost::shared_ptr<RBX::PartInstance> partInstance;
			G3D::ReferenceCountedPointer<RBX::Render::Material> material;
			bool materialInvalid;
			G3D::ReferenceCountedPointer<RBX::Render::Mesh> mesh;
			View* view;
			RBX::SpecialShape* specialShape;
		private:
			virtual void updateMesh() = 0;
		public:
			virtual ~PartChunk(); //May not actually exist
			virtual bool castShadows() const
			{
				return true;
			}
			virtual bool cullable() const
			{
				return true;
			}
		protected:
			PartChunk(float polygonOffset, const boost::shared_ptr<RBX::PartInstance>& partInstance, View* view);
			void invalidateMaterial();
			void invalidateMesh();
			virtual void onPropertyChanged(const RBX::Reflection::PropertyDescriptor* descriptor);
			virtual G3D::ReferenceCountedPointer<RBX::Render::Mesh> getMesh();
			virtual const G3D::CoordinateFrame& cframe();
		private:
			void onAncestorChanged(boost::shared_ptr<RBX::Instance>);
			void onChildAdded(boost::shared_ptr<RBX::Instance>);
			void onChildRemoved(boost::shared_ptr<RBX::Instance>);
			void onSpecialShapeChanged();
		};

		class Part : public PartChunk, public RBX::Listener<RBX::PartInstance, RBX::CanAggregateChanged>
		{
		public:
			Part(const boost::shared_ptr<RBX::PartInstance>& partInstance, RBX::View::View* view);
			virtual ~Part();
			virtual G3D::ReferenceCountedPointer<RBX::Render::Material> getMaterial();
		protected:
			virtual void onPropertyChanged(const RBX::Reflection::PropertyDescriptor* descriptor);
			virtual void onEvent(const RBX::PartInstance* source, RBX::CanAggregateChanged event);
		private:
			bool usesMegaTexture() const;
			virtual void updateMesh();
		};

		class Decal : public PartChunk
		{
		private:
			boost::shared_ptr<RBX::Decal> decal;
			boost::signals::scoped_connection decalAncestorChangedConnection;
			boost::signals::scoped_connection decalPropertyChangedConnection;
		public:
			Decal(RBX::Decal& decal, RBX::PartInstance& partInstance, RBX::View::View* view);
			virtual ~Decal();
			virtual G3D::ReferenceCountedPointer<RBX::Render::Material> getMaterial();
		protected:
			void onDecalPropertyChanged(const RBX::Reflection::PropertyDescriptor* descriptor);
			void onDecalAncestorChanged(boost::shared_ptr<RBX::Instance> ancestor);
		private:
			virtual void updateMesh();
		};

		class Texture : public PartChunk
		{
		private:
			boost::shared_ptr<RBX::Texture> texture;
			boost::signals::scoped_connection textureAncestorChangedConnection;
			boost::signals::scoped_connection texturePropertyChangedConnection;
		public:
			Texture(RBX::Texture& decal, RBX::PartInstance& parentPart, RBX::View::View* view);
			virtual ~Texture();
			virtual G3D::ReferenceCountedPointer<RBX::Render::Material> getMaterial();
		protected:
			void onTexturePropertyChanged(const RBX::Reflection::PropertyDescriptor* descriptor);
			void onTextureAncestorChanged(boost::shared_ptr<RBX::Instance> ancestor);
		private:
			virtual void updateMesh();
		};

		float primaryComponent(const G3D::Vector3& v);
	};
};