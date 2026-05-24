#pragma once
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
		virtual void onDescendentAdded(Instance*);
		virtual void onDescendentRemoving(const boost::shared_ptr<Instance>&);
		virtual bool askAddChild(const Instance*) const;
		virtual XmlElement* write();
		virtual void onServiceProvider(const ServiceProvider*, const ServiceProvider*);
		virtual void onEvent(const World*, AutoDestroy);
		virtual void onEvent(const World*, AutoJoin);
	public:
		JointsService();
	};
}
