#pragma once
#include "script/ScriptContext.h"
#include "v8tree/Instance.h"
#include "util/Debug.h"
#include "boost/shared_ptr.hpp"

namespace RBX
{
	class ScriptContext;

	extern const char* sScript;
	class Script : public DescribedCreatable<Script, Instance, &sScript>
	{
	private:
	  	boost::shared_ptr<const std::string> embeddedSource;
	  	ContentId scriptId;
	  	bool disabled;
	  	class IScriptOwner* owner;

	public:
	  	static Reflection::BoundProp<bool, true> prop_Disabled;
	  	static const Reflection::PropDescriptor<Script, std::string> prop_EmbeddedSourceCode;
	  	static const Reflection::PropDescriptor<Script, ContentId> prop_SourceCodeId;
	
	public:
	  	Script(const Script&);
	  	Script();
	  	virtual ~Script();

		bool isDisabled() const
		{
			return disabled;
		}

	  	virtual bool askSetParent(const Instance*) const;
	  	bool isCodeEmbedded() const;
	  	boost::shared_ptr<const std::string> requestCode();

		void setEmbeddedCode(const std::string& value);
		const std::string& getEmbeddedCode() const
		{
			return *embeddedSource.get();
		}

		const ContentId& getScriptId() const
		{
			return scriptId;
		}
		void setScriptId(const ContentId& value);
	
	protected:
		virtual void onServiceProvider(const ServiceProvider* oldProvider, const ServiceProvider* newProvider);
		virtual void onAncestorChanged(const AncestorChanged& event);
	};

	extern const char* sLocalScript;
	class LocalScript : public DescribedCreatable<LocalScript, Script, &sLocalScript>
	{
	public:
		LocalScript();
		virtual ~LocalScript();
	};

	class IScriptOwner
	{
		friend class Script;

	protected:
		virtual IScriptOwner* scriptShouldRun(Script* script);
		virtual void runScript(Script* script, ScriptContext* context);
		virtual void releaseScript(Script* script);

	public:
		IScriptOwner();
	};
}
