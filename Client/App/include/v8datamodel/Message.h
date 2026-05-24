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
		void renderFullScreen(Adorn*);
		void renderPersonalMsg(Adorn*);
	public:
		Message();
		virtual bool shouldRender2d() const;
		virtual void render2d(Adorn*);
		virtual bool askSetParent(const Instance*) const;
		const std::string& getText() const;
		void setText(const std::string&);
	};

	extern const char* sHint;

	class Hint : public DescribedCreatable<Hint, Message, &sHint>
	{
	private:
		virtual void render2d(Adorn*);
	};
}
