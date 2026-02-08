#include "Clusterer.h"

namespace RBX
{
	namespace Render
	{
		Clusterer::Clusterer(size_t clusterCount)
			: sampleSize(0),
			  clusterCount(clusterCount),
			  clusters(clusterCount)
		{
		}

		void Clusterer::Cluster::computeCentroid()
		{
			G3D::Vector3 c(0.0f, 0.0f, 0.0f);

			std::vector<Chunk*>::iterator iter = samples.begin();
			std::vector<Chunk*>::iterator end = samples.end();

			for (; iter != end; iter++)
			{
				c += (*iter)->cframe().translation;
			}

			centroid = c / samples.size();
		}

		float Clusterer::Cluster::getDistanceFromCentroid(Chunk* sample)
		{
			return (sample->cframe().translation - centroid).magnitude();
		}

		Clusterer::Cluster& Clusterer::findClosestCluster(Chunk* sample)
		{
			Clusterer::Cluster* bestCluster = NULL;
			float bestDistance = G3D::inf();

			std::vector<Clusterer::Cluster>::iterator end = clusters.end();

			for (std::vector<Clusterer::Cluster>::iterator iter = clusters.begin(); iter != end; iter++)
			{
				float distance = iter->getDistanceFromCentroid(sample);
				if (distance < bestDistance)
				{
					bestCluster = &(*iter);
					bestDistance = distance;
				}
			}

			return *bestCluster;
		}

		size_t Clusterer::moveSamples()
		{
			size_t moveCount = 0;


			for (std::vector<Clusterer::Cluster>::iterator it = clusters.begin(); it != clusters.end(); it++)
			{
				it->visitIndex = 0;
			}

			for (std::vector<Clusterer::Cluster>::iterator it = clusters.begin(); it != clusters.end(); it++)
			{
				Clusterer::Cluster& cluster = *it;

				while (cluster.visitIndex < cluster.samples.size())
				{
					Chunk* sample = cluster.samples[cluster.visitIndex];
					Clusterer::Cluster& bestCluster = findClosestCluster(sample);

					if (&bestCluster != &cluster)
					{
						cluster.samples[cluster.visitIndex] = *(cluster.samples.end() - 1);
						cluster.samples.pop_back();

						moveCount += moveSample(sample, bestCluster);
					}
					else
					{
						cluster.visitIndex++;
					}
				}
			}

			return moveCount;
		}

		template<class Iterator>
		void Clusterer::addSamples(Iterator _Iter, Iterator _End)
		{
			size_t currentCluster = 0;

			for (; _Iter != _End; _Iter++)
			{
				clusters[currentCluster].samples.push_back(_Iter->pointer()); // NOTE: this function may have been specialized, although only one template parameter for this function exists in RBXGS

				currentCluster = (currentCluster + 1) % clusterCount;
				sampleSize++;
			}
		}
	}
}
