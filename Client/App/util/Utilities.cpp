#define _CRT_SECURE_NO_DEPRECATE
#include "util/Utilities.h"
#include "util/ContentProvider.h"
#include "util/TextureId.h"
#include <stdio.h>
#include <boost/lexical_cast.hpp>

namespace RBX
{
	template<>
	static std::string StringConverter<bool>::convertToString(const bool& value)
	{
		return value ? "true" : "false";
	}

	template<>
	static std::string StringConverter<int>::convertToString(const int& value)
	{
		char buffer[16];
		_snprintf(buffer, sizeof(buffer), "%d", value);
		return buffer;
	}

	template<>
	static std::string StringConverter<unsigned>::convertToString(const unsigned& value)
	{
		char buffer[16];
		_snprintf(buffer, sizeof(buffer), "%u", value);
		return buffer;
	}

	template<>
	static std::string StringConverter<double>::convertToString(const double& value)
	{
		if (value == std::numeric_limits<double>::infinity())
		{
			return "INF";
		}
		else if (value == -std::numeric_limits<double>::infinity())
		{
			return "-INF";
		}
		else if (!(value < 0.0 || value >= 0.0))
		{
			return "NAN";
		}
		else
		{
			char buffer[32];
			_snprintf(buffer, 32, "%.9g", value);
			return buffer;
		}
	}

	template<>
	static std::string StringConverter<float>::convertToString(const float& value)
	{
		if (value == std::numeric_limits<float>::infinity())
		{
			return "INF";
		}
		else if (value == -std::numeric_limits<float>::infinity())
		{
			return "-INF";
		}
		else if (!(value < 0.0f || value >= 0.0f))
		{
			return "NAN";
		}
		else
		{
			char buffer[32];
			_snprintf(buffer, 32, "%.9g", value);
			return buffer;
		}
	}

	template<>
	static std::string StringConverter<TextureId>::convertToString(const TextureId& value)
	{
		return value.toString();
	}

	template<>
	static bool StringConverter<bool>::convertToValue(const std::string& text, bool& value)
	{
		if (text == "true" || text == "True" || text == "TRUE")
		{
			value = true;
			return true;
		}
		else if (text == "false" || text == "False" || text == "FALSE")
		{
			value = false;
			return true;
		}

		return false;
	}

	template<>
	static bool StringConverter<int>::convertToValue(const std::string& text, int& value)
	{
		if (text.size() == 0)
			return false;

		try
		{
			value = boost::lexical_cast<int>(text);
			return true;
		}
		catch (boost::bad_lexical_cast&)
		{
			return false;
		}
	}

	template<>
	static bool StringConverter<unsigned>::convertToValue(const std::string& text, unsigned& value)
	{
		if (text.size() == 0)
			return false;

		try
		{
			value = boost::lexical_cast<unsigned>(text);
			return true;
		}
		catch (boost::bad_lexical_cast&)
		{
			return false;
		}
	}

	template<>
	static bool StringConverter<double>::convertToValue(const std::string& text, double& value)
	{
		if (text.size() == 0)
			return false;

		if (text == "INF")
		{
			value = std::numeric_limits<double>::infinity();
			return true;
		}
		else if (text == "-INF")
		{
			value = -std::numeric_limits<double>::infinity();
			return true;
		}
		else if (text == "NAN")
		{
			value = std::numeric_limits<double>::quiet_NaN();
			return true;
		}
		else
		{
			try
			{
				value = boost::lexical_cast<double>(text);
				return true;
			}
			catch (boost::bad_lexical_cast&)
			{
				return false;
			}
		}
	}

	template<>
	static bool StringConverter<float>::convertToValue(const std::string& text, float& value)
	{
		if (text.size() == 0)
			return false;

		if (text == "INF")
		{
			value = std::numeric_limits<float>::infinity();
			return true;
		}
		else if (text == "-INF")
		{
			value = -std::numeric_limits<float>::infinity();
			return true;
		}
		else if (text == "NAN")
		{
			value = std::numeric_limits<float>::quiet_NaN();
			return true;
		}
		else
		{
			try
			{
				value = boost::lexical_cast<float>(text);
				return true;
			}
			catch (boost::bad_lexical_cast&)
			{
				return false;
			}
		}
	}

	// TODO: does not match
	template<>
	static bool StringConverter<TextureId>::convertToValue(const std::string& text, TextureId& value)
	{
		value = TextureId(text);
		return true;
	}
}
