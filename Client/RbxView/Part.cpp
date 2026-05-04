#include "Part.h"
#include "v8tree/Instance.h"
#include "reflection/property.h"
#include "v8datamodel/Surfaces.h"
#include "v8datamodel/CustomMesh.h"
#include "v8datamodel/Workspace.h"
#include "v8datamodel/FaceInstance.h"
#include "v8datamodel/Decal.h"
#include <boost/signal.hpp>
#include <boost/function.hpp>
#include <boost/bind.hpp>
#include "RbxView/View.h"
#include "RenderLib/RenderScene.h"
#include "QuadVolume.h"
#include "BrickMesh.h"
#include "HeadMesh.h"
#include "CylinderMesh.h"
#include "FileMesh.h"
#include "SphereMesh.h"
#include "PBBMesh.h"
#include "WedgeMesh.h"
#include "TorsoMesh.h"
#include "MeshFactory.h"
#include "MaterialFactory.h"

namespace RBX {
	namespace View {
		const G3D::CoordinateFrame& PartChunk::cframe() 
		{
			coordinateFrame = partInstance->getCoordinateFrame();
			return coordinateFrame;
		}

		void PartChunk::onSpecialShapeChanged() 
		{
			invalidateMesh();
			invalidateMaterial();
		}

		G3D::ReferenceCountedPointer<RBX::Render::Mesh> PartChunk::getMesh() 
		{
			if (mesh.isNull())
				updateMesh();

			return mesh;
		}

		void PartChunk::onChildRemoved(boost::shared_ptr<RBX::Instance> child)
		{
			if (child.px == (Instance*)specialShape)
			{
				shapePropertyChangedConnection.disconnect();
				specialShape = NULL;
				invalidateMesh();
			}
		}

		void PartChunk::onPropertyChanged(const RBX::Reflection::PropertyDescriptor* descriptor)
		{
			if (descriptor == &RBX::PartInstance::prop_Size) 
			{
				invalidateMesh();
				invalidateMaterial();
				return;
			}
			
			//this could be an inline
			if (descriptor == &RBX::PartInstance::prop_shapeXml) {
				invalidateMesh();
				return;
			} else if (RBX::Surfaces::isSurfaceDescriptor(*descriptor)) {
				invalidateMesh();
			}
		}

		//97.33% matching
		void PartChunk::onChildAdded(boost::shared_ptr<RBX::Instance> child)
		{
			RBX::SpecialShape* shape = RBX::Instance::fastDynamicCast<SpecialShape>(child.get());
			if (shape) {
				specialShape = shape;
				//This line is the cause
				//If I move the boost::slot, that causes even more mismatches
				shapePropertyChangedConnection = RBX::Instance::event_propertyChanged.connect(specialShape, boost::slot<boost::function<void(const RBX::Reflection::PropertyDescriptor*)>>(boost::bind(&PartChunk::onSpecialShapeChanged, this)));
				invalidateMesh();
			}
		}

		//99.81% matching
		void PartChunk::onAncestorChanged(boost::shared_ptr<RBX::Instance> ancestor)
		{
			RBX::Workspace* workspace = RBX::Workspace::findWorkspace(partInstance.px);
			RBX::Instance* parent;

			//the issue seems to be lie with the parent = partInstance.get()->getParent(), however the parent check needs to be after the workspace check
			if (!workspace || (parent = partInstance.get()->getParent(), workspace == parent) || !parent || !parent->isDescendentOf(workspace))
			{
				view->sceneManager->removeModel(this);
			}
		}

		PartChunk::~PartChunk() {}

		void PartChunk::invalidateMesh()
		{
			mesh = NULL;
			view->sceneManager->invalidateModel(this, partInstance.get()->getCanAggregate());
		}

		void PartChunk::invalidateMaterial()
		{
			material = NULL;
			materialInvalid = true;
			view->sceneManager->invalidateModel(this, partInstance.get()->getCanAggregate());
		}

		PartChunk::PartChunk(float polygonOffset, const boost::shared_ptr<RBX::PartInstance> &partInstance, RBX::View::View *view)
			: Chunk(polygonOffset),
			coordinateFrame(),
			childAddedConnection(),
			childRemovedConnection(),
			ancestorChangedConnection(),
			propertyChangedConnection(),
			shapePropertyChangedConnection(),
			partInstance(partInstance),
			material(),
			materialInvalid(true),
			mesh(),
			view(view),
			specialShape()
		{
			view->sceneManager->addModel(this);
			childAddedConnection = RBX::Instance::event_childAdded.connect(specialShape, boost::slot<boost::function<void(boost::shared_ptr<RBX::Instance>)>>(boost::bind(&PartChunk::onChildAdded, this, _1)));
			childRemovedConnection = RBX::Instance::event_childRemoved.connect(specialShape, boost::slot<boost::function<void(boost::shared_ptr<RBX::Instance>)>>(boost::bind(&PartChunk::onChildRemoved, this, _1)));
			ancestorChangedConnection = RBX::Instance::event_ancestryChanged.connect(specialShape, boost::slot<boost::function<void(boost::shared_ptr<RBX::Instance>)>>(boost::bind(&PartChunk::onAncestorChanged, this, _1)));
			propertyChangedConnection = RBX::Instance::event_propertyChanged.connect(specialShape, boost::slot<boost::function<void(const RBX::Reflection::PropertyDescriptor*)>>(boost::bind(&PartChunk::onPropertyChanged, this, _1)));
			//RBX::Instance::visitChildren call
		}

		bool Part::usesMegaTexture() const
		{
			if (!specialShape) 
			{
				RBX::PartInstance* px = partInstance.get();

				if (px->getPartType() != RBX::Part::BLOCK_PART)
					return false;

				G3D::Vector3 size = px->getPartSizeXml();
				return primaryComponent(size) < 30.0f;
			}

			return false;
		}
		
		void Part::onPropertyChanged(const RBX::Reflection::PropertyDescriptor *descriptor)
		{
			if (descriptor == &RBX::PartInstance::prop_Size)
			{
				invalidateMesh();
				invalidateMaterial();
			} else if (descriptor == &RBX::PartInstance::prop_shapeXml) 
			{
				invalidateMesh();
			} else if (RBX::Surfaces::isSurfaceDescriptor(*descriptor)) 
			{
				invalidateMesh();
			}

			if (descriptor == &RBX::PartInstance::prop_Color)
			{
				if (usesMegaTexture())
				{
					invalidateMesh();
					invalidateMaterial();
					return;
				}
			} else if (descriptor != &RBX::PartInstance::prop_Transparency && descriptor != &RBX::PartInstance::prop_Reflectance)
				return;

			invalidateMaterial();
		}

		void Part::onEvent(const RBX::PartInstance *source, RBX::CanAggregateChanged event)
		{
			//having to do with RBX::Notifier<RBX::Instance, RBX::DescendantAdded> in specialShape
		}

		//73.90% matching
		//pretty messy
		//setSurfaceType for RenderSurfaceTypes will need to be defined for 100%
		//also the switch case could be apart of an inline? not too sure
		void Part::updateMesh()
		{
			RenderSurfaceType curSurface;

			RenderSurfaceTypes surfaceTypes;

			switch(partInstance.get()->getSurfaces()[NORM_X].getSurfaceType())
			{
			case STUDS:
				curSurface = STUDS;
				break;
			case INLET:
				curSurface = INLET;
				break;
			case WELD:
				curSurface = WELD;
				break;
			case GLUE:
				curSurface = GLUE;
				break;
			default:
				curSurface = FLAT;
				break;
			}
			surfaceTypes.setSurfaceType(NORM_X, curSurface);
			switch(partInstance.get()->getSurfaces()[NORM_Y].getSurfaceType())
			{
			case STUDS:
				curSurface = STUDS;
				break;
			case INLET:
				curSurface = INLET;
				break;
			case WELD:
				curSurface = WELD;
				break;
			case GLUE:
				curSurface = GLUE;
				break;
			default:
				curSurface = FLAT;
				break;
			}
			surfaceTypes.setSurfaceType(NORM_Y, curSurface);
			switch(partInstance.get()->getSurfaces()[NORM_Z].getSurfaceType())
			{
			case STUDS:
				curSurface = STUDS;
				break;
			case INLET:
				curSurface = INLET;
				break;
			case WELD:
				curSurface = WELD;
				break;
			case GLUE:
				curSurface = GLUE;
				break;
			default:
				curSurface = FLAT;
				break;
			}
			surfaceTypes.setSurfaceType(NORM_Z, curSurface);
			switch(partInstance.get()->getSurfaces()[NORM_X_NEG].getSurfaceType())
			{
			case STUDS:
				curSurface = STUDS;
				break;
			case INLET:
				curSurface = INLET;
				break;
			case WELD:
				curSurface = WELD;
				break;
			case GLUE:
				curSurface = GLUE;
				break;
			default:
				curSurface = FLAT;
				break;
			}
			surfaceTypes.setSurfaceType(NORM_X_NEG, curSurface);
			switch(partInstance.get()->getSurfaces()[NORM_Y_NEG].getSurfaceType())
			{
			case STUDS:
				curSurface = STUDS;
				break;
			case INLET:
				curSurface = INLET;
				break;
			case WELD:
				curSurface = WELD;
				break;
			case GLUE:
				curSurface = GLUE;
				break;
			default:
				curSurface = FLAT;
				break;
			}
			surfaceTypes.setSurfaceType(NORM_Y_NEG, curSurface);
			switch(partInstance.get()->getSurfaces()[NORM_Z_NEG].getSurfaceType())
			{
			case STUDS:
				curSurface = STUDS;
				break;
			case INLET:
				curSurface = INLET;
				break;
			case WELD:
				curSurface = WELD;
				break;
			case GLUE:
				curSurface = GLUE;
				break;
			default:
				curSurface = FLAT;
				break;
			}
			surfaceTypes.setSurfaceType(NORM_Z_NEG, curSurface);

			G3D::Vector3 size = partInstance.get()->getPartSizeXml();
			
			if (usesMegaTexture())
			{
				mesh = RBX::View::BrickMesh::create(size, surfaceTypes, partInstance.get()->getColor());
			}  else if (partInstance.get()->getPartType())
			{
				switch (partInstance.get()->getPartType())
				{
				case RBX::Part::CYLINDER_PART:
					mesh = RBX::View::MeshFactory<CylinderAlongXMesh, 1>::create(size, surfaceTypes);
					break;
				case RBX::Part::BLOCK_PART:
					mesh = RBX::View::MeshFactory<PBBMesh, 4>::create(size, surfaceTypes);
					break;
				case RBX::Part::BALL_PART:
					mesh = RBX::View::MeshFactory<SphereMesh, 1>::create(size, surfaceTypes);
					break;
				}
			} 
			
			if (specialShape) 
			{
				size *= specialShape->getScale();
				switch(specialShape->getMeshType())
				{
				case SpecialShape::WEDGE_MESH:
					mesh = RBX::View::MeshFactory<WedgeMesh, 1>::create(size, surfaceTypes);
					break;
				case SpecialShape::HEAD_MESH:
					mesh = RBX::View::MeshFactory<HeadMesh, 1>::create(size, surfaceTypes);
					break;
				case SpecialShape::TORSO_MESH:
					mesh = RBX::View::MeshFactory<TorsoMesh, 1>::create(size, surfaceTypes);
					break;
				case SpecialShape::SPHERE_MESH:
					mesh = RBX::View::MeshFactory<SphereMesh, 1>::create(size, surfaceTypes);
					break;
				case SpecialShape::CYLINDER_MESH:
					mesh = RBX::View::MeshFactory<CylinderAlongXMesh, 1>::create(size, surfaceTypes);
					break;
				case SpecialShape::BRICK_MESH:
					mesh = RBX::View::MeshFactory<PBBMesh, 4>::create(size, surfaceTypes);
					break;
				case SpecialShape::FILE_MESH:
					mesh = RBX::View::FileMesh::create(size, specialShape->getMeshId());
					break;
				}
			}

			radius = sqrt(2.0f) * 0.5 * primaryComponent(size);
		}

		//34.14% matching
		//also messy
		G3D::ReferenceCountedPointer<RBX::Render::Material> Part::getMaterial()
		{
			if (materialInvalid)
			{
				RBX::PartInstance* partInstancePX = partInstance.get();
				RBX::View::MaterialFactory::Attributes attributes = {partInstancePX->getColor(), partInstancePX->getTransparencyUi(), partInstancePX->getReflectance()};
				if (usesMegaTexture())
				{
					material = view->materialFactory->getMegaMaterial(attributes);
				} else if (!specialShape || specialShape->getMeshType() != SpecialShape::FILE_MESH)
				{
					material = view->materialFactory->getMaterial(attributes);
					return material;
				}
				
				std::string texFile;
				if (RBX::ContentProvider::singleton().requestContentFile(specialShape->getTextureId(), texFile))
				{
					G3D::Color3 vertColor = specialShape->getVertColor();
					float alpha = specialShape->getAlpha();
					
					material = new RBX::Render::Material();
					RBX::Render::TextureProxy* texProxy = new RBX::Render::TextureProxy(*view->textureManager.get(), texFile, false);
					material.getPointer()->appendLevel(texProxy, vertColor, 0.0f, 0.1f, 0.0f, partInstance.get()->getTransparencyUi());
				} else 
				{
					material = view->materialFactory->getMaterial(attributes);
				}
				materialInvalid = false;
				return material;
			}
			return material;
		}

		//78.70% matching
		Part::~Part() {
			//has Notifier remove listener
		}

		void Decal::onDecalPropertyChanged(const RBX::Reflection::PropertyDescriptor* descriptor)
		{
			if (descriptor == &FaceInstance::prop_Face)
			{
				invalidateMesh();
			} else if (descriptor == &RBX::Decal::prop_Texture || descriptor == &RBX::Decal::prop_Specular || descriptor == &RBX::Decal::prop_Shiny)
			{
				invalidateMaterial();
			}
		}

		
		//99.88% matching
		void Decal::onDecalAncestorChanged(boost::shared_ptr<RBX::Instance> ancestor)
		{
			RBX::Workspace* workspace = RBX::Workspace::findWorkspace(decal.get());
			RBX::Instance* parent;

			//same error as the PartChunk variant
			if (!workspace || (parent = decal.get()->getParent(), workspace == parent) || !parent || !parent->isDescendentOf(workspace))
			{
				view->sceneManager->removeModel(this);
			}
		}

		//38.85% matching
		//would probably be way higher if it weren't for the fact that the part type switch case is above where it should be
		//i legit don't know why it isn't in its proper position
		void Decal::updateMesh()
		{
			if (partInstance.get())
			{
				G3D::Vector3 size = partInstance->getPartSizeXml();
				if (specialShape)
				{
					size *= specialShape->getScale();
				}
			
				if (partInstance->getPartType())
				{
					switch (partInstance->getPartType())
					{
					case RBX::Part::CYLINDER_PART:
						mesh = RBX::View::MeshFactory<CylinderAlongXMesh, 1>::createDecal(size, decal->getFace());
						break;
					case RBX::Part::BLOCK_PART:
						mesh = RBX::View::MeshFactory<PBBMesh, 4>::createDecal(size, decal->getFace());
						break;
					case RBX::Part::BALL_PART:
						mesh = RBX::View::MeshFactory<SphereMesh, 1>::createDecal(size, decal->getFace());
						break;
					}
				}

				if (specialShape)
				{
					switch(specialShape->getMeshType())
					{
					case SpecialShape::WEDGE_MESH:
						mesh = RBX::View::MeshFactory<WedgeMesh, 1>::createDecal(size, decal->getFace());
						break;
					case SpecialShape::HEAD_MESH:
						mesh = RBX::View::MeshFactory<HeadMesh, 1>::createDecal(size, decal->getFace());
						break;
					case SpecialShape::TORSO_MESH:
						mesh = RBX::View::MeshFactory<TorsoMesh, 1>::createDecal(size, decal->getFace());
						break;
					case SpecialShape::SPHERE_MESH:
						mesh = RBX::View::MeshFactory<SphereMesh, 1>::createDecal(size, decal->getFace());
						break;
					case SpecialShape::CYLINDER_MESH:
						mesh = RBX::View::MeshFactory<CylinderAlongXMesh, 1>::createDecal(size, decal->getFace());
						break;
					case SpecialShape::BRICK_MESH:
						mesh = RBX::View::MeshFactory<PBBMesh, 4>::createDecal(size, decal->getFace());
						break;
					default:
						break;
					}
				}
				radius = sqrt(2.0f) * 0.5 * primaryComponent(size);
			}
		}
		
		//75.61% matching
		G3D::ReferenceCountedPointer<RBX::Render::Material> Decal::getMaterial()
		{
			if (materialInvalid)
			{
				std::string textureFile;
				if (RBX::ContentProvider::singleton().requestContentFile(decal->getTexture(), textureFile))
				{
					RBX::Render::Material* newMat = new RBX::Render::Material();
					material = newMat;
					RBX::Render::TextureProxy* texProxy = new RBX::Render::TextureProxy(*view->textureManager.get(), textureFile, false);
					material->appendLevel(texProxy, G3D::Color3::WHITE, decal->getSpecular(), decal->getShiny(), 0.0f, 0.0f);
					materialInvalid = false;
				}
			}

			return material;
		}
		
		Decal::~Decal() {}

		//75.92% matching
		//more boost connection problems
		Decal::Decal(RBX::Decal &decal, RBX::PartInstance &partInstance, RBX::View::View *view)
			: PartChunk(RBX::Render::Chunk::DECAL_OFFSET, RBX::shared_from<RBX::PartInstance>(&partInstance), view),
			decal(RBX::shared_from<RBX::Decal>(&decal)),
			decalAncestorChangedConnection(),
			decalPropertyChangedConnection()
		{
			decalAncestorChangedConnection = RBX::Instance::event_ancestryChanged.connect(&decal, boost::slot<boost::function<void(boost::shared_ptr<RBX::Instance>)>>(boost::bind(&Decal::onDecalAncestorChanged, this, _1)));
			decalPropertyChangedConnection = RBX::Instance::event_propertyChanged.connect(&decal, boost::slot<boost::function<void(const RBX::Reflection::PropertyDescriptor*)>>(boost::bind(&Decal::onDecalPropertyChanged, this, _1)));
		}

		//99.88% matching
		void Texture::onTextureAncestorChanged(boost::shared_ptr<RBX::Instance> ancestor)
		{
			RBX::Workspace* workspace = RBX::Workspace::findWorkspace(texture.get());
			RBX::Instance* parent;

			//same error as the PartChunk variant
			if (!workspace || (parent = texture.get()->getParent(), workspace == parent) || !parent || !parent->isDescendentOf(workspace))
			{
				view->sceneManager->removeModel(this);
			}
		}

		//99.71% matching
		//Branching :pray:
		void Texture::onTexturePropertyChanged(const RBX::Reflection::PropertyDescriptor *descriptor)
		{
			if (descriptor != &FaceInstance::prop_Face)
			{
				if (descriptor == &RBX::Decal::prop_Texture || descriptor == &RBX::Decal::prop_Specular || descriptor == &RBX::Decal::prop_Shiny)
				{
					invalidateMaterial();
					return;
				}
			}
			if (descriptor != &RBX::Texture::prop_StudsPerTileU && descriptor != &RBX::Texture::prop_StudsPerTileV)
			{
				return;
			}
			invalidateMesh();
		}

		//75.62% matching
		G3D::ReferenceCountedPointer<RBX::Render::Material> Texture::getMaterial()
		{
			if (materialInvalid)
			{
				std::string textureFile;
				if (RBX::ContentProvider::singleton().requestContentFile(texture->getTexture(), textureFile))
				{
					RBX::Render::Material* newMat = new RBX::Render::Material();
					material = newMat;
					RBX::Render::TextureProxy* texProxy = new RBX::Render::TextureProxy(*view->textureManager.get(), textureFile, true);
					material->appendLevel(texProxy, G3D::Color3::WHITE, texture->getSpecular(), texture->getShiny(), 0.0f, 0.0f);
					materialInvalid = false;
				}
			}

			return material;
		}
		
		Texture::~Texture()
		{
		}

		//38.85% matching
		//just like decals
		void Texture::updateMesh()
		{
			if (partInstance.get())
			{
				G3D::Vector3 size = partInstance->getPartSizeXml();
				if (specialShape)
				{
					size *= specialShape->getScale();
				}
			
				if (partInstance->getPartType())
				{
					switch (partInstance->getPartType())
					{
					case RBX::Part::CYLINDER_PART:
						mesh = RBX::View::MeshFactory<CylinderAlongXMesh, 1>::createTexture(size, texture->getFace(), texture->getStudsPerTile());
						break;
					case RBX::Part::BLOCK_PART:
						mesh = RBX::View::MeshFactory<PBBMesh, 4>::createTexture(size, texture->getFace(), texture->getStudsPerTile());
						break;
					case RBX::Part::BALL_PART:
						mesh = RBX::View::MeshFactory<SphereMesh, 1>::createTexture(size, texture->getFace(), texture->getStudsPerTile());
						break;
					}
				}

				if (specialShape)
				{
					switch(specialShape->getMeshType())
					{
					case SpecialShape::WEDGE_MESH:
						mesh = RBX::View::MeshFactory<WedgeMesh, 1>::createTexture(size, texture->getFace(), texture->getStudsPerTile());
						break;
					case SpecialShape::HEAD_MESH:
						mesh = RBX::View::MeshFactory<HeadMesh, 1>::createTexture(size, texture->getFace(), texture->getStudsPerTile());
						break;
					case SpecialShape::TORSO_MESH:
						mesh = RBX::View::MeshFactory<TorsoMesh, 1>::createTexture(size, texture->getFace(), texture->getStudsPerTile());
						break;
					case SpecialShape::SPHERE_MESH:
						mesh = RBX::View::MeshFactory<SphereMesh, 1>::createTexture(size, texture->getFace(), texture->getStudsPerTile());
						break;
					case SpecialShape::CYLINDER_MESH:
						mesh = RBX::View::MeshFactory<CylinderAlongXMesh, 1>::createTexture(size, texture->getFace(), texture->getStudsPerTile());
						break;
					case SpecialShape::BRICK_MESH:
						mesh = RBX::View::MeshFactory<PBBMesh, 4>::createTexture(size, texture->getFace(), texture->getStudsPerTile());
						break;
					default:
						break;
					}
				}
				radius = sqrt(2.0f) * 0.5 * primaryComponent(size);
			}
		}
		

		//75.92%
		//similar to decal's
		Texture::Texture(RBX::Texture &texture, RBX::PartInstance &parentPart, RBX::View::View *view)
			: PartChunk(RBX::Render::Chunk::TEXTURE_OFFSET, RBX::shared_from<RBX::PartInstance>(&parentPart), view),
			texture(RBX::shared_from<RBX::Texture>(&texture)),
			textureAncestorChangedConnection(),
			texturePropertyChangedConnection()
		{
			textureAncestorChangedConnection = RBX::Instance::event_ancestryChanged.connect(&texture, boost::slot<boost::function<void(boost::shared_ptr<RBX::Instance>)>>(boost::bind(&Texture::onTextureAncestorChanged, this, _1)));
			texturePropertyChangedConnection = RBX::Instance::event_propertyChanged.connect(&texture, boost::slot<boost::function<void(const RBX::Reflection::PropertyDescriptor*)>>(boost::bind(&Texture::onTexturePropertyChanged, this, _1)));
		}

	};
};