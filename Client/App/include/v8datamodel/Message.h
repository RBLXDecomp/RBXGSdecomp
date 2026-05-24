#pragma once
#include "v8tree/Instance.h"
#include "util/IRenderable.h"

namespace RBX
{
	extern const char* sMessage;

	class Message : public DescribedCreatable<Message, Instance, &sMessage>, public IRenderable
	{
	protected:
		std::string text;

	protected:
		void renderFullScreen(Adorn* adorn);
		void renderPersonalMsg(Adorn* adorn);
	public:
		Message();

		virtual bool shouldRender2d() const
		{
			return true;

		}

		virtual void render2d(Adorn* adorn);

		virtual bool askSetParent(const Instance* instance) const
		{
			return true;
		}

		const std::string& getText() const
		{
			return text;
		}

		void setText(const std::string& value);
	};

	extern const char* sHint;

	class Hint : public DescribedCreatable<Hint, Message, &sHint>
	{
	private:
		virtual void render2d(Adorn* adorn);
	};
}
