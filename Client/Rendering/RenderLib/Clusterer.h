#include "RenderLib/Chunk.h"

namespace RBX
{
	namespace Render
	{
		class Clusterer
		{
			class Cluster
			{
			public:
				G3D::Vector3 centroid;
				std::vector<Chunk*> samples;
				size_t visitIndex;
			public:
				void computeCentroid();
				float getDistanceFromCentroid(Chunk* sample);
			};

		private:
			size_t sampleSize;
			const size_t clusterCount;
			std::vector<Cluster> clusters;
		private:
			size_t moveSamples();
			size_t moveSample(Chunk* sample, Cluster& cluster);
			Cluster& findClosestCluster(Chunk* sample);
		public:
			Clusterer(size_t clusterCount);
			std::vector<Cluster>& go(size_t);

			template<class Iterator>
			void addSamples(Iterator _Iter, Iterator _End);
		};
	}
}
