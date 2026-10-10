#define _CRT_SECURE_NO_DEPRECATE
#include <G3D/Color3.h>
#include <G3D/Vector3.h>
#include "v8datamodel/BrickColor.h"
#include "util/ContentProvider.h"
#include "util/Utilities.h"

namespace RBX
{
	template<>
	static std::string StringConverter<G3D::Vector3>::convertToString(const G3D::Vector3& value)
	{
		std::string text = StringConverter<float>::convertToString(value.x);
		text += ", ";
		text = StringConverter<float>::convertToString(value.y);
		text += ", ";
		text = StringConverter<float>::convertToString(value.z);
		return text;
	}

	template<>
	static std::string StringConverter<BrickColor>::convertToString(const BrickColor& value)
	{
		return value.name();
	}

	template<>
	static std::string StringConverter<G3D::Color3>::convertToString(const G3D::Color3& value)
	{
		char buffer[64];
		_snprintf(buffer, sizeof(buffer), "%g, %g, %g", value.r, value.g, value.b);
		return buffer;
	}

	template<>
	static std::string StringConverter<ContentId>::convertToString(const ContentId& value)
	{
		return value.toString();
	}

	template<>
	static bool StringConverter<G3D::Vector3>::convertToValue(const std::string& text, G3D::Vector3& value)
	{
		int index1 = (int)text.find_first_of(",;", 0, 2);
		if (index1 < 0)
			return false;

		value.x = atof(text.substr(0, index1).c_str());

		int startIndex2 = index1 + 1;
		int index2 = (int)text.find_first_of(",;", startIndex2, 2);
		if (index2 < startIndex2)
			return false;

		value.y = atof(text.substr(startIndex2, index2 - startIndex2).c_str());

		int startIndex3 = index2 + 1;
		int index3 = (int)text.size();
		if (index3 < startIndex3)
			return false;

		value.z = atof(text.substr(startIndex3, index3 - startIndex3).c_str());

		return true;
	}

	template<>
	static bool StringConverter<G3D::Color3>::convertToValue(const std::string& text, G3D::Color3& value)
	{
		if (text[0] == '#')
		{
			if (text.size() != 7)
				return false;

			value.r = strtol(text.substr(1, 2).c_str(), 0, 16) * (1.0f/255.0f);
			value.g = strtol(text.substr(3, 2).c_str(), 0, 16) * (1.0f/255.0f);
			value.b = strtol(text.substr(5, 2).c_str(), 0, 16) * (1.0f/255.0f);
			return true;
		}
		else if (text.substr(0, 2) == "0x")
		{
			if (text.size() != 8)
				return false;

			value.r = strtol(text.substr(2, 2).c_str(), 0, 16) * (1.0f/255.0f);
			value.g = strtol(text.substr(4, 2).c_str(), 0, 16) * (1.0f/255.0f);
			value.b = strtol(text.substr(6, 2).c_str(), 0, 16) * (1.0f/255.0f);
			return true;
		}
		else
		{
			int index1 = (int)text.find_first_of(",;", 0, 2);
			if (index1 < 0)
				return false;

			value.r = atof(text.substr(0, index1).c_str());

			int startIndex2 = index1 + 1;
			int index2 = (int)text.find_first_of(",;", startIndex2, 2);
			if (index2 < startIndex2)
				return false;

			value.g = atof(text.substr(startIndex2, index2 - startIndex2).c_str());

			int startIndex3 = index2 + 1;
			int index3 = (int)text.size();
			if (index3 < startIndex3)
				return false;

			value.b = atof(text.substr(startIndex3, index3 - startIndex3).c_str());

			return true;
		}
	}

	template<>
	static bool StringConverter<ContentId>::convertToValue(const std::string& text, ContentId& value)
	{
		value = ContentId(text);
		return true;
	}
}