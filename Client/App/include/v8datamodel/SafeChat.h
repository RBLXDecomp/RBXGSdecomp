#pragma once
#include "v8xml/XmlElement.h"
#include <boost/scoped_ptr.hpp>
#include <vector>
#include <string>

namespace RBX
{
	class ChatOption
	{
	public:
		std::vector<ChatOption*> children;
		std::string text;

	public:
		ChatOption(std::string);
		ChatOption();
		~ChatOption();
	};

	class SafeChat
	{
	private:
		boost::scoped_ptr<ChatOption> chatRoot;

	public:
		SafeChat();
		ChatOption* getChatRoot();
		std::string getMessage(std::vector<std::string>);
	private:
		void loadChatTree();
		void saveChatTree();
		void loadChildren(ChatOption*, const XmlElement*);

	public:
		static SafeChat& singleton();
	};
}
