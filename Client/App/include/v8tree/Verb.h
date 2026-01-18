#pragma once
#include "util/StateStack.h"
#include "util/Name.h"
#include "util/Debug.h"
#include "v8xml/XmlElement.h"
#include "v8tree/Instance.h"
#include <string>
#include <map>

namespace RBX
{
	class VerbContainer;

	class IDataState : public StateStack<XmlState> // NOTE: may not be intended for this file
	{
	public:
		virtual void setDirty(bool) = 0;
		virtual bool isDirty() const = 0;
		//IDataState(const IDataState&);
		IDataState();
		~IDataState();
		//IDataState& operator=(const IDataState&);
	};

	class Verb : public Debugable
	{
	private:
		const Name& name;
		VerbContainer* container;
	public:
		Verb(const Verb&);
	protected:
		Verb(VerbContainer*, const Name&);
		Verb(VerbContainer*, std::string);
	public:
		virtual ~Verb();
		virtual bool isEnabled() const;
		virtual bool isChecked() const;
		const Name& getName() const;
		virtual void doIt(IDataState*) = 0;
		VerbContainer* getContainer() const;
	};

	class VerbContainer
	{
	private:
		std::map<const Name*, Verb*> verbs;
		VerbContainer* parent;
	public:
		VerbContainer(const VerbContainer&);
		VerbContainer(VerbContainer* parent);
		virtual ~VerbContainer();

		Verb* getVerb(std::string name);
		Verb* getVerb(const Name& name);
		void setVerbParent(VerbContainer* parent);
		VerbContainer* getVerbParent() const;
	private:
		void addVerb(Verb*);
		void removeVerb(Verb*);
	public:
		//VerbContainer& operator=(const VerbContainer&);
	};
}
