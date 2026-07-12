#include "v8datamodel/Workspace.h"
#include "v8datamodel/PartInstance.h"
#include "v8datamodel/MouseCommand.h"
#include "v8datamodel/Flag.h"
#include "v8datamodel/Camera.h"
#include "v8datamodel/Hopper.h"
#include "humanoid/Humanoid.h"
#include "v8world/World.h"
#include "tool/NullTool.h"
#include "tool/ToolsArrow.h"
#include "Network/Players.h"

namespace RBX
{
	static Reflection::RefPropDescriptor<Workspace, Camera> currentCameraProxyProp("CurrentCamera", "Data", &Workspace::getCamera, &Workspace::setCamera, Reflection::PropertyDescriptor::STANDARD);

	static Reflection::BoundFuncDesc<Workspace, boost::shared_ptr<const std::vector<boost::shared_ptr<Instance>>>(ContentId), 1> workspace_insertContent(&Workspace::insertContent, "InsertContent", "url", Reflection::FunctionDescriptor::NeedTrustedCaller);
	static Reflection::BoundFuncDesc<Workspace, void(boost::shared_ptr<const std::vector<boost::shared_ptr<Instance>>>), 1> workspace_makeJoints(&Workspace::makeJoints, "MakeJoints", "objects", Reflection::FunctionDescriptor::NeedTrustedCaller);
	static Reflection::BoundFuncDesc<Workspace, void(boost::shared_ptr<const std::vector<boost::shared_ptr<Instance>>>), 1> workspace_breakJoints(&Workspace::breakJoints, "BreakJoints", "objects", Reflection::FunctionDescriptor::NeedTrustedCaller);
	static Reflection::BoundFuncDesc<Workspace, void(bool), 1> workspace_SetThrottleEnabled(&Workspace::setThrottleEnabled, "SetPhysicsThrottleEnabled", "value", Reflection::FunctionDescriptor::NeedTrustedCaller);
	static Reflection::BoundFuncDesc<Workspace, void(void), 0> workspace_zoomToExtents(&Workspace::zoomToExtents, "ZoomToExtents", Reflection::FunctionDescriptor::NeedTrustedCaller);

	Workspace::Workspace(IDataState* dataState)
		: VerbContainer(NULL),
		  dataState(dataState),
		  profileWorkspaceStep(new Profiling::CodeProfiler("DataModel Step")),
		  profileRunServiceStepped(new Profiling::CodeProfiler("RunService Stepped")),
		  profileRunServiceHeartbeat(new Profiling::CodeProfiler("RunService Heartbeat")),
		  profileRun(new Profiling::ThreadProfiler("Run Thread")),
		  arrowCameraControls(false),
		  inMouselookMode(false),
		  isSimulating(false),
		  scriptContext(NULL),
		  imageServerViewHack(0)
	{
		RBXASSERT(dataState);
		setName("Workspace");

		world->getProfileWorldStep().parent = profileWorkspaceStep.get();
		profileRunServiceStepped->parent = profileWorkspaceStep.get();
		profileRunServiceHeartbeat->parent = profileWorkspaceStep.get();
	}

	Workspace::~Workspace() {}

	bool Workspace::askAddChild(const Instance* instance) const
	{
		return fastDynamicCast<const IRenderable>(instance) != NULL;
	}

	const G3D::GCamera& Workspace::getGCamera() const
	{
		RBXASSERT(getCamera());
		return getCamera()->getGCamera();
	}

	void Workspace::zoomToExtents()
	{
		getCamera()->zoomExtents(viewPort);
	}

	void Workspace::insertItems(XmlElement* root, std::vector<boost::shared_ptr<Instance>>& instances, InsertMode insertMode, PromptMode promptMode)
	{
		RBXASSERT(instances.size() == 0);

		SerializerV2 serializer;
		serializer.loadInstances(root, instances);

		if (instances.size() != 0)
			insertInstances(instances, this, insertMode, promptMode);
	}

	void Workspace::stop()
	{
		RBXASSERT(isSimulating);
		isSimulating = false;

		getCamera()->autoMode();
		RBXASSERT(!getCurrentMouseCommand()->captured());
	}

	MouseCommand* newNullTool(Workspace* workspace)
	{
		return new NewNullTool(workspace);
	}

	void Workspace::setNullMouseCommand()
	{
		stickyCommand.reset();
		currentCommand.reset(newNullTool(this));
	}

	void Workspace::render2d(Adorn* adorn)
	{
		getCurrentMouseCommand()->render2d(adorn);
		render2dItems(adorn);
	}

	void Workspace::setThrottleEnabled(bool value)
	{
		world->setCanThrottle(value);
	}

	void Workspace::joinAllHack()
	{
		world->joinAll();
	}

	void Workspace::reset()
	{
		stop();
		world->reset();
	}

	Camera* Workspace::getCamera() const
	{
		if (!currentCamera || !currentCamera->isDescendentOf(this))
		{
			boost::shared_ptr<Camera> newCamera = Creatable::create<Camera>();
			newCamera->setParent(const_cast<Workspace*>(this));
			currentCamera = newCamera;
		}

		RBXASSERT(currentCamera->isDescendentOf(this));
		return currentCamera.get();
	}

	void Workspace::onChildChanged(Instance* instance, const PropertyChanged& event)
	{
		Instance::onChildChanged(instance, event);
		raiseDrawChanged();
	}

	void Workspace::onDescendentAdded(Instance* instance)
	{
		RootInstance::onDescendentAdded(instance);
		raiseDrawChanged();

		IRenderable* renderable = fastDynamicCast<IRenderable>(instance);
		if (renderable)
			onAdded(renderable);

		PVInstance* pvInstance = fastDynamicCast<PVInstance>(instance);
		if (pvInstance && pvInstance->getLegacyOffset())
		{
			RBXASSERT(instance->getParent() == this);

			pvInstance->legacyTraverseState(G3D::CoordinateFrame());
			pvInstance->clearLegacyOffset();
			if (legacyOffset)
				legacyOffset.reset();
		}
	}

	void Workspace::onDescendentRemoving(const boost::shared_ptr<Instance>& instance)
	{
		IRenderable* renderable = fastDynamicCast<IRenderable>(instance.get());
		if (renderable)
			onRemoving(renderable);

		RootInstance::onDescendentRemoving(instance);
		raiseDrawChanged();
	}

	template<bool isJoin>
	static void wrapper(Instance* instance)
	{
		PartInstance* part = Instance::fastDynamicCast<PartInstance>(instance);
		if (part)
		{
			if (isJoin)
				part->join();
			else
				part->destroyJoints();
		}
		else
		{
			instance->for_eachChild(&wrapper<isJoin>);
		}
	}

	template<bool isJoin>
	static void wrapper2(boost::shared_ptr<Instance> instance)
	{
		wrapper<isJoin>(instance.get());
	}

	void Workspace::makeJoints(boost::shared_ptr<const std::vector<boost::shared_ptr<Instance>>> instances)
	{
		std::for_each(instances->begin(), instances->end(), &wrapper2<true>);
	}

	void Workspace::breakJoints(boost::shared_ptr<const std::vector<boost::shared_ptr<Instance>>> instances)
	{
		std::for_each(instances->begin(), instances->end(), &wrapper2<false>);
	}

	void clearEmptiedModels(boost::shared_ptr<Instance> test, Instance* parent)
	{
		if (test.get() != parent)
		{
			ModelInstance* model = Instance::fastDynamicCast<ModelInstance>(test.get());
			if (model && !Humanoid::modelIsCharacter(model) && !model->findFirstChildOfType<PartInstance>() && !model->findFirstChildOfType<ModelInstance>())
			{
				boost::shared_ptr<Instance> oldParent = shared_from(model->getParent());
				model->setParent(NULL);
				clearEmptiedModels(oldParent, parent);
			}

			Flag* flag = Instance::fastDynamicCast<Flag>(test.get());
			if (flag && !flag->findFirstChildOfType<PartInstance>())
			{
				boost::shared_ptr<Instance> oldParent = shared_from(flag->getParent());
				flag->setParent(NULL);
				clearEmptiedModels(oldParent, parent);
			}
		}
	}

	void Workspace::handleFallenParts(const G3D::Array<boost::shared_ptr<PartInstance>>& fallenParts)
	{
		for (int i = 0; i < fallenParts.size(); i++)
		{
			boost::shared_ptr<PartInstance> part = fallenParts[i];

			boost::shared_ptr<Instance> oldParent = shared_from(part->getParent());
			part->setParent(NULL);
			clearEmptiedModels(oldParent, this);
		}
	}

	void Workspace::setMouseCommand(MouseCommand* newMouseCommand)
	{
		if (!newMouseCommand)
		{
			if (!stickyCommand.get() || (newMouseCommand = stickyCommand->isSticky(), !newMouseCommand))
			{
				if (!Network::Players::findLocalPlayer(this))
					newMouseCommand = new ArrowTool(this);
				else
					newMouseCommand = newNullTool(this);
			}
		}

		RBXASSERT(newMouseCommand);

		if (newMouseCommand != currentCommand.get())
		{
			RBXASSERT(!currentCommand.get() || !currentCommand->captured());

			currentCommand.reset(newMouseCommand);

			MouseCommand* sticky = newMouseCommand->isSticky();
			if (sticky)
				stickyCommand.reset(sticky);

			Notifier<Workspace, ToolChanged>::raise(ToolChanged());
		}
	}

	float Workspace::step(float timeInterval)
	{
		RBXASSERT(isSimulating);

		timeInterval = world->step(timeInterval);
		
		G3D::Array<boost::shared_ptr<PartInstance>> touchParts;
		G3D::Array<boost::shared_ptr<PartInstance>> touchOtherParts;
		G3D::Array<Primitive*> fallen;
		G3D::Array<boost::shared_ptr<PartInstance>> fallenParts;

		PartInstance::primitivesToParts(world->getTouch(), touchParts);
		PartInstance::primitivesToParts(world->getTouchOther(), touchOtherParts);
		world->computeFallen(fallen);
		PartInstance::primitivesToParts(fallen, fallenParts);
		
		handleFallenParts(fallenParts);

		for (int i = 0; i < touchParts.size(); i++)
		{
			touchParts[i]->onTouchThisStep(touchOtherParts[i]);
		}

		raiseDrawChanged();
		return timeInterval;
	}

	void Workspace::setDefaultMouseCommand()
	{
		stickyCommand.reset();
		setMouseCommand(NULL);
	}

	void Workspace::runScript(Script* script, ScriptContext* context)
	{
		RBXASSERT(context == scriptContext);

		if (Network::Players::backendProcessing(this, true) && !isSimulating)
			pendingScripts.insert(script);
		else
			context->addScript(script);
	}

	void Workspace::releaseScript(Script* script)
	{
		pendingScripts.erase(script);
	}

	Workspace* Workspace::findWorkspace(const Instance* context)
	{
		return ServiceProvider::find<Workspace>(context);
	}

	Workspace* Workspace::getWorkspaceIfInWorkspace(const Instance* context)
	{
		Workspace* workspace = ServiceProvider::find<Workspace>(context);

		if (workspace && (context == workspace || context->isDescendentOf(workspace)))
			return workspace;
		else
			return NULL;
	}

	World* Workspace::getWorldIfInWorkspace(const Instance* context)
	{
		Workspace* workspace = ServiceProvider::find<Workspace>(context);

		if (workspace && (context == workspace || context->isDescendentOf(workspace)))
			return workspace->getWorld();
		else
			return NULL;
	}

	bool Workspace::contextInWorkspace(const Instance* context)
	{
		return getWorkspaceIfInWorkspace(context) != NULL;
	}

	void Workspace::setCamera(Camera* value)
	{
		if (value != currentCamera.get())
		{
			if (currentCamera)
				currentCamera->setParent(NULL);

			currentCamera = shared_from(value);
			raisePropertyChanged(currentCameraProxyProp);
		}
	}

	Workspace* Workspace::getMyWorkspaceFast(const Instance* context)
	{
		RBXASSERT(getWorldIfInWorkspace(context));

		Instance* parent = context->getParent();
		RBXASSERT(parent);

		Instance* parent2 = parent->getParent();
		RBXASSERT(parent2);

		while (parent2->getParent())
		{
			parent = parent2;
			parent2 = parent2->getParent();
		}

		return rbx_static_cast<Workspace*>(parent);
	}

	World* Workspace::getMyWorldFast(const Instance* context)
	{
		return getMyWorkspaceFast(context)->getWorld();
	}

	//94% match
	//std::set<>::clear is being inlined more aggressively
	void Workspace::start()
	{
		RBXASSERT(!isSimulating);
		isSimulating = true;

		getCamera()->alwaysMode();

		RBXASSERT(!getCurrentMouseCommand()->captured());

		if (scriptContext)
		{
			std::vector<boost::shared_ptr<Script>> startScripts;

			std::set<Script*>::const_iterator iter = pendingScripts.begin();
			std::set<Script*>::const_iterator end = pendingScripts.end();

			for (; iter != end; iter++)
			{
				startScripts.push_back(shared_from(*iter));
			}

			pendingScripts.clear();
			std::for_each(startScripts.begin(), startScripts.end(), boost::bind(&ScriptContext::addScript, scriptContext, boost::bind(&boost::shared_ptr<Script>::get, _1)));
		}
	}

	void Workspace::setImageServerView(const G3D::Rect2D& viewPortRect)
	{
		HopperBin* superHack = NULL;

		for (size_t i = 0; i < numChildren(); i++)
		{
			HopperBin* hopperBin = fastDynamicCast<HopperBin>(getChild(i));
			ILocation* location = fastDynamicCast<ILocation>(getChild(i));
			IRenderable* renderable = fastDynamicCast<IRenderable>(getChild(i));

			if (location && renderable)
			{
				G3D::CoordinateFrame modelCoord = location->getLocation();
				getCamera()->setImageServerViewNoLerp(modelCoord, viewPortRect); 
			}

			if (hopperBin)
				superHack = hopperBin;
		}

		if (superHack)
		{
			StarterPackService* starterPack = ServiceProvider::create<StarterPackService>(this);
			superHack->setParent(starterPack);
		}

		imageServerViewHack = (imageServerViewHack <= 0);
	}
}
