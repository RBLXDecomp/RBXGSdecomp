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
		ChatOption(std::string text);
		ChatOption();
		~ChatOption();
	};

	class SafeChat
	{
	private:
		boost::scoped_ptr<ChatOption> chatRoot;

	public:
		SafeChat();
		ChatOption* getChatRoot()
		{
			return chatRoot.get();
		}
		std::string getMessage(std::vector<std::string> code);
	private:
		void loadChatTree();
		void saveChatTree();
		void loadChildren(ChatOption* parentOption, const XmlElement* parentElement);

	public:
		static SafeChat& singleton();
	};
}
