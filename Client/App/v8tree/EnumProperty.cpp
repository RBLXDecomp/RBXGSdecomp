#include "reflection/type.h"
#include <g3d/color3.h>
#include <g3d/vector3.h>
#include <boost/shared_ptr.hpp>
#include "v8tree/Instance.h"
#include "v8datamodel/BrickColor.h"

namespace RBX
{
	namespace Reflection
	{
		// TODO: check if type singletons are matching
		template<>
		const Type& Type::singleton<boost::shared_ptr<RBX::Reflection::DescribedBase>>()
		{
			static Type type("Object", typeid(boost::shared_ptr<RBX::Reflection::DescribedBase>));
			return type;
		}

		template<>
		const Type& Type::singleton<boost::shared_ptr<Instance>>()
		{
			static Type type("Instance", typeid(boost::shared_ptr<Instance>));
			return type;
		}

		template<>
		const Type& Type::singleton<boost::shared_ptr<const Instances>>()
		{
			static Type type("Objects", typeid(boost::shared_ptr<const Instances>));
			return type;
		}

		template<>
		const Type& Type::singleton<int>()
		{
			static Type type("int", typeid(int));
			return type;
		}

		template<>
		const Type& Type::singleton<bool>()
		{
			static Type type("bool", typeid(bool));
			return type;
		}

		template<>
		const Type& Type::singleton<float>()
		{
			static Type type("float", typeid(float));
			return type;
		}

		template<>
		const Type& Type::singleton<double>()
		{
			static Type type("double", typeid(double));
			return type;
		}

		template<>
		const Type& Type::singleton<ContentId>()
		{
			static Type type("ContentId", typeid(ContentId));
			return type;
		}

		template<>
		const Type& Type::singleton<std::string>()
		{
			static Type type("string", typeid(std::string));
			return type;
		}

		template<>
		const Type& Type::singleton<G3D::Vector3>()
		{
			static Type type("Vector3", typeid(G3D::Vector3));
			return type;
		}

		template<>
		const Type& Type::singleton<G3D::Color3>()
		{
			static Type type("Color3", typeid(G3D::Color3));
			return type;
		}

		template<>
		const Type& Type::singleton<std::vector<Value>>()
		{
			static Type type("Table", typeid(std::vector<Value>));
			return type;
		}

		template<>
		const Type& Type::singleton<BrickColor>()
		{
			static Type type("BrickColor", typeid(BrickColor));
			return type;
		}
	}
}
