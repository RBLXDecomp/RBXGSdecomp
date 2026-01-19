#include "RenderLib/AggregatingSceneManager.h"

namespace RBX
{
	namespace Render
	{
		AggregatingSceneManager::AggregatingSceneManager(RenderScene* renderScene)
			: SceneManager(renderScene)
		{
		}

		AggregatingSceneManager::~AggregatingSceneManager() {}

		void AggregatingSceneManager::addModel(const G3D::ReferenceCountedPointer<Chunk>& chunk)
		{
			addToScene(chunk);
		}

		void AggregatingSceneManager::removeModel(const G3D::ReferenceCountedPointer<Chunk>& chunk)
		{
			invalidSleepingModels.erase(chunk);
			invalidMovingModels.erase(chunk);

			G3D::ReferenceCountedPointer<AggregateChunk> aggregate = chunk->aggregate.createStrongPtr();
			if (aggregate.notNull())
				deconstructAggregate(aggregate);

			removeFromScene(chunk);
			dequeueSleepingChunk(chunk);
		}

		void AggregatingSceneManager::setSleeping(const G3D::ReferenceCountedPointer<Chunk>& chunk, bool sleeping)
		{
			if (sleeping)
			{
				bool erased = invalidMovingModels.erase(chunk) == 1;

				if (erased)
					invalidSleepingModels.insert(chunk);

				queueSleepingChunk(chunk);
			}
			else
			{
				bool erased = invalidSleepingModels.erase(chunk) == 1;

				if (erased)
					invalidMovingModels.insert(chunk);

				G3D::ReferenceCountedPointer<AggregateChunk> aggregate = chunk->aggregate.createStrongPtr();
				if (aggregate.notNull())
					deconstructAggregate(aggregate);

				dequeueSleepingChunk(chunk);
			}
		}

		void AggregatingSceneManager::invalidateModel(const G3D::ReferenceCountedPointer<Chunk>& chunk, bool isSleeping)
		{
			if (isSleeping)
				invalidSleepingModels.insert(chunk);
			else
				invalidMovingModels.insert(chunk);
		}

		bool AggregatingSceneManager::BucketKey::operator<(const AggregatingSceneManager::BucketKey& that) const
		{
			if (this->material < that.material)
				return true;
			if (this->material > that.material)
				return false;

			if (this->castsShadows < that.castsShadows)
				return true;
			if (this->castsShadows > that.castsShadows)
				return false;

			if (this->polygonOffset < that.polygonOffset)
				return true;
			if (this->polygonOffset > that.polygonOffset)
				return false;

			if (this->cullable < that.cullable)
				return true;
			else
				return false;
		}
	}
}
