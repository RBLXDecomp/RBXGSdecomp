#pragma once
#include "v8datamodel/JointInstance.h"
#include "v8tree/Instance.h"
#include "v8tree/Service.h"
#include "v8world/World.h"
#include "util/IRenderable.h"

namespace RBX
{
	class JointsService : public NonFactoryProduct<Instance, NULL>, 
						  public Service,
						  public IRenderableBucket,
						  public Listener<World, AutoJoin>,
						  public Listener<World, AutoDestroy>
	{
	public:
		World* world;

	private:
		virtual void onDescendentAdded(Instance* instance);
		virtual void onDescendentRemoving(const boost::shared_ptr<Instance>&);

		virtual bool askAddChild(const Instance* instance) const
		{
			return fastDynamicCast<const JointInstance>(instance) != NULL;
		}

		virtual XmlElement* write()
		{
			return NULL;
		}

		virtual void onServiceProvider(const ServiceProvider*, const ServiceProvider*);
		virtual void onEvent(const World* source, AutoDestroy event);
		virtual void onEvent(const World* source, AutoJoin event);
	public:
		JointsService()
			: world(NULL)
		{
			setName("JointsService");
		}
	};
}
