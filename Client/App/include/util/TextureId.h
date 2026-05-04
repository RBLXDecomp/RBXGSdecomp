#pragma once
#include "util/ContentProvider.h"

namespace RBX
{
	class TextureId : public ContentId
	{
	public:
		TextureId()
			: ContentId()
		{
		}
		TextureId(const std::string& id)
			: ContentId(id)
		{
		}
		TextureId(const char* id)
			: ContentId(id)
		{
		}
		TextureId(const ContentId& id)
			: ContentId(id)
		{
		}
	};
}
