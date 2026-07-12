#pragma once
#include "v8datamodel/RootInstance.h"
#include "v8datamodel/Stats.h"
#include "v8tree/Verb.h"
#include "script/Script.h"
#include "v8world/IMoving.h"
#include "gui/GuiEvent.h"
#include "util/RunStateOwner.h"
#include "util/ContentProvider.h"
#include "util/Profiling.h"
#include "util/UIEvent.h"

namespace RBX
{
	class MouseCommand;
	class Camera;

	class DrawChanged
	{
	};

	class ToolChanged
	{
	};

	extern const char* sWorkspace;
	class Workspace : public DescribedNonCreatable<Workspace, RootInstance, &sWorkspace>,
					  public Service,
					  public GuiTarget,
					  public VerbContainer,
					  public IRenderableBucket,
					  public IMovingManager,
					  public IScriptOwner,
					  public Notifier<Workspace, DrawChanged>,
					  public Notifier<Workspace, ToolChanged>,
					  public Listener<RunService, Heartbeat>
	{
	private:
		boost::shared_ptr<Instance> statsItem;
		IDataState* dataState;
		bool arrowCameraControls;
		std::auto_ptr<MouseCommand> currentCommand;
		std::auto_ptr<MouseCommand> stickyCommand;
		UIEvent idleMouseEvent;
		mutable boost::shared_ptr<Camera> currentCamera;
		bool isSimulating;
		std::set<Script*> pendingScripts;
		ScriptContext* scriptContext;
		bool inMouselookMode;
	public:
		boost::scoped_ptr<Profiling::CodeProfiler> profileWorkspaceStep;
		boost::scoped_ptr<Profiling::CodeProfiler> profileRunServiceStepped;
		boost::scoped_ptr<Profiling::CodeProfiler> profileRunServiceHeartbeat;
		boost::scoped_ptr<Profiling::ThreadProfiler> profileRun;
		int imageServerViewHack;

	public:
		static bool showWorldCoord;
		static bool showHashGrid;
	  
	private:
		virtual bool askAddChild(const Instance* instance) const;
		virtual void onChildChanged(Instance* instance, const PropertyChanged& event);
		virtual void onDescendentAdded(Instance* instance);
		virtual void onDescendentRemoving(const boost::shared_ptr<Instance>& instance);
		virtual void onServiceProvider(const ServiceProvider*, const ServiceProvider*);
	public:
		void setCursor(Adorn*);
		virtual void render2d(Adorn* adorn);
		virtual void render3dAdorn(Adorn*);
		virtual void render3dSelect(Adorn* adorn, SelectState selectState)
		{
			return;
		}
	private:
		virtual IScriptOwner* scriptShouldRun(Script*);
		virtual void runScript(Script* script, ScriptContext* context);
		virtual void releaseScript(Script* script);
		virtual Extents computeCameraOwnerExtents()
		{
			return getExtentsWorld();
		}
		virtual void cameraMoved()
		{
			raiseDrawChanged();
		}
		virtual void onExtentsChanged() const
		{
			ModelInstance::onExtentsChanged();
			raiseDrawChanged();
		}
		virtual void onEvent(const RunService*, Heartbeat);
	public:
		Workspace(IDataState* dataState);
		virtual ~Workspace();
	public:
		World* getWorld() const
		{
			return world.get();
		}
		IDataState& getDataState() const
		{
			return *dataState;
		}
		MouseCommand* getCurrentMouseCommand()
		{
			RBXASSERT(currentCommand.get());
			return currentCommand.get();
		}
		void cancelMouseCommand();
		void setMouseCommand(MouseCommand* newMouseCommand);
		void setDefaultMouseCommand();
		void setNullMouseCommand();
		bool getInMouselookMode();
		void setInMouselookMode(bool);
		virtual const G3D::GCamera& getGCamera() const;
		virtual Camera* getCamera() const;
		void setCamera(Camera* value);
		void setImageServerView(const G3D::Rect2D& viewPortRect);
		void zoomToExtents();
		void onWrapMouse(const G3D::Vector2&);
		virtual GuiResponse process(const GuiEvent&);
		void selectAll();
	private:
		void handleFallenParts(const G3D::Array<boost::shared_ptr<PartInstance>>& fallenParts);
	public:
		void start();
		void stop();
		void reset();
		float step(float timeInterval);
		void setThrottleEnabled(bool value);
		void joinAllHack();
		void insertItems(XmlElement* root, std::vector<boost::shared_ptr<Instance>>& instances, InsertMode insertMode, PromptMode promptMode);
		boost::shared_ptr<const std::vector<boost::shared_ptr<Instance>>> insertContent(ContentId);
		void insertContent(ContentId, std::vector<boost::shared_ptr<Instance>>&, InsertMode, PromptMode);
		void makeJoints(boost::shared_ptr<const std::vector<boost::shared_ptr<Instance>>> instances);
		void breakJoints(boost::shared_ptr<const std::vector<boost::shared_ptr<Instance>>> instances);
		void raiseDrawChanged() const
		{
			Notifier<Workspace, DrawChanged>::raise(DrawChanged());
		}

	public:
		static Instance* findTopInstance(Instance*);
		static Workspace* findWorkspace(const Instance* context);
		static Workspace* getMyWorkspaceFast(const Instance* context);
		static World* getWorldIfInWorkspace(const Instance* context);
		static Workspace* getWorkspaceIfInWorkspace(const Instance* context);
		static bool contextInWorkspace(const Instance* context);
		static World* getMyWorldFast(const Instance* context);
	};
	
	class WorkspaceStatsItem : public Stats::Item
	{
	public:
		WorkspaceStatsItem(const Workspace*, const World*);
	};
}
