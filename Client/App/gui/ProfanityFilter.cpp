#include "gui/ProfanityFilter.h"
#include "util/ContentProvider.h"
#include <fstream>
#include <boost/algorithm/string.hpp>

namespace RBX
{
	WordList::WordList()
	{
		std::string diogenesFile = ContentProvider::singleton().getAssetFile("Fonts\\diogenes.fnt");

		std::ifstream fs(diogenesFile.c_str());
		std::string line;
		while (std::getline(fs, line))
		{
			boost::to_lower(line);
			boost::trim(line);
			blacklist.insert(line);
		}
		fs.close();
	}

	WordList::~WordList()
	{
	}

	bool WordList::ContainsProfanity(std::string text)
	{
		boost::to_lower(text);
		return blacklist.find(text) != blacklist.end();
	}

	ProfanityFilter::ProfanityFilter()
	{
	}

	ProfanityFilter::~ProfanityFilter()
	{
	}

	bool ProfanityFilter::ContainsProfanity(std::string text)
	{
		boost::to_lower(text);

		std::vector<std::string> words;
		boost::split(words, text, boost::is_any_of(" !.?,:;><[]{}|\\/@#$%^&*()_-+=\""));

		for (size_t i = 0; i < words.size(); ++i)
		{
			if (wordList.ContainsProfanity(words[i]))
				return true;
		}

		return false;
	}
}
