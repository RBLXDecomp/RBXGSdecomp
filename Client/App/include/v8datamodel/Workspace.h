#pragma once
#include "v8datamodel/RootInstance.h"
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
		boost::shared_ptr<Camera> currentCamera;
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
		virtual bool askAddChild(const Instance*) const;
		virtual void onChildChanged(Instance*, const PropertyChanged&);
		virtual void onDescendentAdded(Instance*);
		virtual void onDescendentRemoving(const boost::shared_ptr<Instance>&);
		virtual void onServiceProvider(const ServiceProvider*, const ServiceProvider*);
	public:
		void setCursor(Adorn*);
		virtual void render2d(Adorn*);
		virtual void render3dAdorn(Adorn*);
		virtual void render3dSelect(Adorn*, SelectState);
	private:
		virtual IScriptOwner* scriptShouldRun(Script*);
		virtual void runScript(Script*, ScriptContext*);
		virtual void releaseScript(Script*);
		virtual Extents computeCameraOwnerExtents();
		virtual void cameraMoved();
		virtual void onExtentsChanged() const;
		virtual void onEvent(const RunService*, Heartbeat);
	public:
		//Workspace(Workspace&);
		Workspace(IDataState*);
		virtual ~Workspace();
	public:
		World* getWorld() const
		{
			return world.get();
		}
		IDataState& getDataState() const;
		MouseCommand* getCurrentMouseCommand();
		void cancelMouseCommand();
		void setMouseCommand(MouseCommand*);
		void setDefaultMouseCommand();
		void setNullMouseCommand();
		bool getInMouselookMode();
		void setInMouselookMode(bool);
		virtual const G3D::GCamera& getGCamera() const;
		virtual Camera* getCamera() const;
		void setCamera(Camera*);
		void setImageServerView(const G3D::Rect2D&);
		void zoomToExtents();
		void onWrapMouse(const G3D::Vector2&);
		virtual GuiResponse process(const GuiEvent&);
		void selectAll();
	private:
		void handleFallenParts(const G3D::Array<boost::shared_ptr<PartInstance>>&);
	public:
		void start();
		void stop();
		void reset();
		float step(float);
		void setThrottleEnabled(bool);
		void joinAllHack();
		void insertItems(XmlElement*, std::vector<boost::shared_ptr<Instance>>&, InsertMode, PromptMode);
		boost::shared_ptr<const std::vector<boost::shared_ptr<Instance>>> insertContent(ContentId);
		void insertContent(ContentId, std::vector<boost::shared_ptr<Instance>>&, InsertMode, PromptMode);
		void makeJoints(boost::shared_ptr<const std::vector<boost::shared_ptr<Instance>>>);
		void breakJoints(boost::shared_ptr<const std::vector<boost::shared_ptr<Instance>>>);
		void raiseDrawChanged() const;
	public:
		//Workspace& operator=(Workspace&);

	public:
		static Instance* findTopInstance(Instance*);
		static Workspace* findWorkspace(const Instance*);
		static Workspace* getMyWorkspaceFast(const Instance*);
		static World* getWorldIfInWorkspace(const Instance*);
		static Workspace* getWorkspaceIfInWorkspace(const Instance*);
		static bool contextInWorkspace(const Instance*);
		static World* getMyWorldFast(const Instance*);
	};
}
