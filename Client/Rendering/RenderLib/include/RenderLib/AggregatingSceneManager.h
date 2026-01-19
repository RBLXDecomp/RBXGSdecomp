#include "RenderLib/RenderScene.h"
#include <boost/noncopyable.hpp>
#include <map>
#include <set>

namespace RBX
{
	namespace Render
	{
		class AggregatingSceneManager : public SceneManager
		{
			class Bucket : public G3D::ReferenceCountedObject, public boost::noncopyable
			{
			private:
				std::vector<G3D::ReferenceCountedPointer<Chunk>> queue;
			public:
				bool dequeueSleepingChunk(const G3D::ReferenceCountedPointer<Chunk>& chunk);
				void addToQueue(const G3D::ReferenceCountedPointer<Chunk>&);
				void clear();
				void optimize(AggregatingSceneManager&, double);
			};

			struct BucketKey
			{
				bool castsShadows;
				bool cullable;
				float polygonOffset;
				G3D::ReferenceCountedPointer<Material> material;

				BucketKey(const G3D::ReferenceCountedPointer<Chunk>& that);

				bool operator<(const BucketKey&) const;
			};

		private:
			std::map<BucketKey, G3D::ReferenceCountedPointer<Bucket>> buckets;
			std::set<G3D::WeakReferenceCountedPointer<Chunk>> invalidSleepingModels;
			std::set<G3D::WeakReferenceCountedPointer<Chunk>> invalidMovingModels;
		public:
			static size_t aggregationSize;

		public:
			AggregatingSceneManager(RenderScene*);
			virtual ~AggregatingSceneManager();

			virtual void invalidateModel(const G3D::ReferenceCountedPointer<Chunk>& chunk, bool isSleeping);
			virtual void addModel(const G3D::ReferenceCountedPointer<Chunk>& chunk);
			virtual void removeModel(const G3D::ReferenceCountedPointer<Chunk>& chunk);
			virtual void clear();
			virtual void setSleeping(const G3D::ReferenceCountedPointer<Chunk>& chunk, bool sleeping);
			virtual void prerender(double);
		private:
			void dequeueSleepingChunk(const G3D::ReferenceCountedPointer<Chunk>&);
			void queueSleepingChunk(const G3D::ReferenceCountedPointer<Chunk>&);
			void deconstructAggregate(G3D::ReferenceCountedPointer<AggregateChunk>);
		};
	}
}
