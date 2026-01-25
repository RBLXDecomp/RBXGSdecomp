#include "script/Script.h"
#include "v8tree/Service.h"
#include "v8datamodel/Workspace.h"
#include "boost/thread/once.hpp"

const RBX::Reflection::PropDescriptor<RBX::Script, std::string> RBX::Script::prop_EmbeddedSourceCode("Source", "Data", &Script::getEmbeddedCode, &Script::setEmbeddedCode, Reflection::PropertyDescriptor::LEGACY);
const RBX::Reflection::PropDescriptor<RBX::Script, RBX::ContentId> RBX::Script::prop_SourceCodeId("LinkedSource", "Data", &Script::getScriptId, &Script::setScriptId, Reflection::PropertyDescriptor::LEGACY);
RBX::Reflection::BoundProp<bool, true> RBX::Script::prop_Disabled("Disabled", "Behavior", &Script::disabled, Reflection::PropertyDescriptor::STANDARD);

static boost::shared_ptr<const std::string> helloWorld;

boost::once_flag flagInitScriptCpp = BOOST_ONCE_INIT;

void initScriptCpp()
{
    helloWorld.reset(new std::string("print(\"Hello world!\")\r\n"));
}

namespace RBX
{
    const char* sScript = "Script";

    Script::Script()
        : DescribedCreatable("Script"),
          disabled(false),
          owner(NULL)
    {
        boost::call_once(&initScriptCpp, flagInitScriptCpp);
        embeddedSource = helloWorld;
    }

    const char* sLocalScript = "LocalScript";

    LocalScript::LocalScript()
    {
        setName("LocalScript");
    }

    Script::~Script()
    {
        RBXASSERT(owner == NULL);
    }

    void Script::setEmbeddedCode(const std::string& value)
    {
        if (getEmbeddedCode() != value)
        {
            embeddedSource.reset(new std::string(value));
            raisePropertyChanged(prop_EmbeddedSourceCode);
        }
    }

	void Script::setScriptId(const ContentId& value)
    {
        if (getScriptId() != value)
        {
            scriptId = value;
            requestCode();
            raisePropertyChanged(prop_SourceCodeId);
        }
    }

	void Script::onAncestorChanged(const AncestorChanged& event)
    {
        Instance::onAncestorChanged(event);

        IScriptOwner* newOwner = NULL;
        Instance* parent = this->getParent();

        while (parent)
        {
            IScriptOwner* parentOwner = dynamic_cast<IScriptOwner*>(parent);

            if (parentOwner)
            {
                newOwner = parentOwner->scriptShouldRun(this);

                if (newOwner)
                    break;
            }

            parent = parent->getParent();
        }

        if (newOwner != owner)
        {
            if (owner)
                owner->releaseScript(this);

            owner = newOwner;

            ScriptContext* sc = ServiceProvider::find<ScriptContext>(this);

            if (sc)
            {
                sc->removeScript(this);

                if (owner)
                    owner->runScript(this, sc);
            }
        }
    }

	void Script::onServiceProvider(const ServiceProvider* oldProvider, const ServiceProvider* newProvider)
    {
        if (oldProvider)
        {
            ScriptContext* sc = oldProvider->find<ScriptContext>();

            if (sc)
                sc->removeScript(this);
        }

        Instance::onServiceProvider(oldProvider, newProvider);
    }

	boost::shared_ptr<const std::string> Script::requestCode()
    {
        if (scriptId.isNull())
        {
            return embeddedSource;
        }
        else 
        {
            return ContentProvider::singleton().requestContentString(scriptId);
        }
    }
}
