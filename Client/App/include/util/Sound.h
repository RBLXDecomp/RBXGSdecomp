#pragma once
#include "util/ContentProvider.h"

namespace RBX
{
	namespace Soundscape
	{
		class SoundId : public ContentId
		{
		public:
			SoundId()
				: ContentId()
			{
			}
			SoundId(const std::string& id)
				: ContentId(id)
			{
			}
			SoundId(const char* id)
				: ContentId(id)
			{
			}
			SoundId(const ContentId& id)
				: ContentId(id)
			{
			}
		};
	}
}
