#pragma once
#include <g3d/Vector3.h>

namespace RBX
{
	// These classes are only used in ICharacterSubject (found in v8datamodel).
	// Since the constructors of these classes are completely inlined in all builds of RBXGS, their implementation is guessed.

	class floatERA
	{
		float weight;
		float avg;

	public:
		floatERA(float weight)
			: weight(weight)
		{
			reset();
		}

		floatERA()
			: weight(1.0f)
		{
			reset();
		}

		void reset();
		float pushAndGetAverage(float value);
		float getAverage();
	};

	class Vector3ERA
	{
		float weight;
		float x, y, z;

	public:
		Vector3ERA(float weight)
			: weight(weight)
		{
			reset();
		}

		Vector3ERA()
			: weight(1.0f)
		{
			reset();
		}

		void reset();
		G3D::Vector3 pushAndGetAverage(G3D::Vector3 value);
		G3D::Vector3 getAverage();
	};
}