#include "RenderLib/Chunk.h"
#include "RenderLib/Profiler.h"
#include "v8datamodel/DebugSettings.h"
#include "v8datamodel/PartInstance.h"
#include "v8datamodel/ModelInstance.h"
#include "v8datamodel/Workspace.h"
#include "v8world/Primitive.h"
#include "v8world/World.h"

RBX::Reflection::PropDescriptor<RBX::DebugSettings, bool> prop_AnchoredParts("ShowAnchors", "Display", &RBX::DebugSettings::getShowAnchoredParts, &RBX::DebugSettings::setShowAnchoredParts, RBX::Reflection::PropertyDescriptor::STANDARD);
RBX::Reflection::PropDescriptor<RBX::DebugSettings, bool> prop_HighlightSleepParts("HighlightSleepParts", "Display", &RBX::DebugSettings::getHighlightSleepParts, &RBX::DebugSettings::setHighlightSleepParts, RBX::Reflection::PropertyDescriptor::STANDARD);
RBX::Reflection::PropDescriptor<RBX::DebugSettings, bool> prop_HighlightAwakeParts("HighlightAwakeParts", "Display", &RBX::DebugSettings::getHighlightAwakeParts, &RBX::DebugSettings::setHighlightAwakeParts, RBX::Reflection::PropertyDescriptor::STANDARD);
RBX::Reflection::PropDescriptor<RBX::DebugSettings, bool> prop_PartCoordinateFrames("ShowPartCoords", "Display", &RBX::DebugSettings::getShowPartCoordinateFrames, &RBX::DebugSettings::setShowPartCoordinateFrames, RBX::Reflection::PropertyDescriptor::STANDARD);
RBX::Reflection::PropDescriptor<RBX::DebugSettings, bool> prop_UnalignedParts("ShowUnalignedParts", "Display", &RBX::DebugSettings::getShowUnalignedParts, &RBX::DebugSettings::setShowUnalignedParts, RBX::Reflection::PropertyDescriptor::STANDARD);
RBX::Reflection::PropDescriptor<RBX::DebugSettings, bool> prop_ShowAggregation("ShowAggregation", "Display", &RBX::DebugSettings::getShowAggregation, &RBX::DebugSettings::setShowAggregation, RBX::Reflection::PropertyDescriptor::STANDARD);
RBX::Reflection::PropDescriptor<RBX::DebugSettings, bool> prop_ModelCoordinateFrames("ShowModelCoords", "Display", &RBX::DebugSettings::getShowModelCoordinateFrames, &RBX::DebugSettings::setShowModelCoordinateFrames, RBX::Reflection::PropertyDescriptor::STANDARD);
RBX::Reflection::PropDescriptor<RBX::DebugSettings, bool> prop_WorldCoordinateFrames("ShowWorldCoords", "Display", &RBX::DebugSettings::getShowWorldCoordinateFrames, &RBX::DebugSettings::setShowWorldCoordinateFrames, RBX::Reflection::PropertyDescriptor::STANDARD);
RBX::Reflection::PropDescriptor<RBX::DebugSettings, bool> prop_SpanningTree("ShowTree", "Display", &RBX::DebugSettings::getShowSpanningTree, &RBX::DebugSettings::setShowSpanningTree, RBX::Reflection::PropertyDescriptor::STANDARD);
RBX::Reflection::PropDescriptor<RBX::DebugSettings, bool> prop_DisableSleep("DisableSleep", "Display", &RBX::DebugSettings::getDisableSleep, &RBX::DebugSettings::setDisableSleep, RBX::Reflection::PropertyDescriptor::STANDARD);
RBX::Reflection::PropDescriptor<RBX::DebugSettings, bool> prop_DisableEnvironmentalThrottle("DisableEnvironmentalThrottle", "Display", &RBX::DebugSettings::getDisableEnvironmentalThrottle, &RBX::DebugSettings::setDisableEnvironmentalThrottle, RBX::Reflection::PropertyDescriptor::STANDARD);

RBX::Reflection::PropDescriptor<RBX::DebugSettings, bool> prop_ValidatingDebug("ValidatingDebug", "Errors", &RBX::DebugSettings::getValidatingDebug, &RBX::DebugSettings::setValidatingDebug, RBX::Reflection::PropertyDescriptor::STANDARD);
RBX::Reflection::PropDescriptor<RBX::DebugSettings, RBX::Debugable::AssertAction> prop_assertAction("AssertAction", "Errors", &RBX::DebugSettings::getAssertAction, &RBX::DebugSettings::setAssertAction, RBX::Reflection::PropertyDescriptor::STANDARD);
RBX::Reflection::PropDescriptor<RBX::DebugSettings, RBX::DebugSettings::ErrorReporting> prop_errorReporting("errorReporting", "Errors", &RBX::DebugSettings::getErrorReporting, &RBX::DebugSettings::setErrorReporting, RBX::Reflection::PropertyDescriptor::STANDARD);

RBX::Reflection::PropDescriptor<RBX::DebugSettings, float> prop_shaderModel("ShaderModel", "Profile", &RBX::DebugSettings::shaderModel, NULL, RBX::Reflection::PropertyDescriptor::STANDARD);
RBX::Reflection::PropDescriptor<RBX::DebugSettings, int> prop_videoMemory("VideoMemory", "Profile", &RBX::DebugSettings::videoMemory, NULL, RBX::Reflection::PropertyDescriptor::STANDARD);
RBX::Reflection::PropDescriptor<RBX::DebugSettings, int> prop_cpuSpeed("CpuSpeed", "Profile", &RBX::DebugSettings::cpuSpeed, NULL, RBX::Reflection::PropertyDescriptor::STANDARD);
RBX::Reflection::PropDescriptor<RBX::DebugSettings, int> prop_osPlatformId("OsPlatformId", "Profile", &RBX::DebugSettings::osPlatformId, NULL, RBX::Reflection::PropertyDescriptor::STANDARD);
RBX::Reflection::PropDescriptor<RBX::DebugSettings, std::string> prop_osVer("OsVer", "Profile", &RBX::DebugSettings::osVer, NULL, RBX::Reflection::PropertyDescriptor::STANDARD);
RBX::Reflection::PropDescriptor<RBX::DebugSettings, std::string> prop_glVendor("GlVendor", "Profile", &RBX::DebugSettings::glVendor, NULL, RBX::Reflection::PropertyDescriptor::STANDARD);
RBX::Reflection::PropDescriptor<RBX::DebugSettings, std::string> prop_gfxcard("GfxCard", "Profile", &RBX::DebugSettings::gfxcard, NULL, RBX::Reflection::PropertyDescriptor::STANDARD);
RBX::Reflection::PropDescriptor<RBX::DebugSettings, std::string> prop_cpu("CPU", "Profile", &RBX::DebugSettings::cpu, NULL, RBX::Reflection::PropertyDescriptor::STANDARD);
RBX::Reflection::PropDescriptor<RBX::DebugSettings, int> prop_ram("RAM", "Profile", &RBX::DebugSettings::ram, NULL, RBX::Reflection::PropertyDescriptor::STANDARD);
RBX::Reflection::PropDescriptor<RBX::DebugSettings, std::string> prop_resolution("Resolution", "Profile", &RBX::DebugSettings::resolution, NULL, RBX::Reflection::PropertyDescriptor::STANDARD);

RBX::Reflection::BoundProp<bool, 1> prop_SoundWarnings("SoundWarnings", "Errors", &RBX::DebugSettings::soundWarnings, RBX::Reflection::PropertyDescriptor::STANDARD);

namespace RBX
{
	DebugSettings::DebugSettings()
		: stackTracingEnabled(true),
		  ioEnabled(false),
		  soundWarnings(false),
		  errorReporting(Prompt)
	{
		setName("Diagnostics");
	}

	bool DebugSettings::getShowAnchoredParts() const
	{
		return PartInstance::showAnchoredParts;
	}

	bool DebugSettings::getShowAggregation() const
	{
		return Render::AggregateChunk::randomColors;
	}

	bool DebugSettings::getShowUnalignedParts() const
	{
		return PartInstance::showUnalignedParts;
	}

	bool DebugSettings::getHighlightSleepParts() const
	{
		return PartInstance::highlightSleepParts;
	}

	bool DebugSettings::getHighlightAwakeParts() const
	{
		return PartInstance::highlightAwakeParts;
	}

	bool DebugSettings::getShowPartCoordinateFrames() const
	{
		return PartInstance::showPartCoord;
	}

	bool DebugSettings::getShowModelCoordinateFrames() const
	{
		return ModelInstance::showModelCoord;
	}

	bool DebugSettings::getShowWorldCoordinateFrames() const
	{
		return Workspace::showWorldCoord;
	}

	bool DebugSettings::getDisableSleep() const
	{
		return Primitive::disableSleep;
	}

	bool DebugSettings::getDisableEnvironmentalThrottle() const
	{
		return World::disableEnvironmentalThrottle;
	}

	bool DebugSettings::getShowSpanningTree() const
	{
		return PartInstance::showSpanningTree;
	}

	void DebugSettings::setShowAnchoredParts(bool value)
	{
		if (value != PartInstance::showAnchoredParts)
		{
			PartInstance::showAnchoredParts = value;
			raisePropertyChanged(prop_AnchoredParts);
		}
	}

	void DebugSettings::setShowAggregation(bool value)
	{
		if (value != Render::AggregateChunk::randomColors)
		{
			Render::AggregateChunk::randomColors = value;
			raisePropertyChanged(prop_PartCoordinateFrames);
		}
	}

	void DebugSettings::setShowUnalignedParts(bool value)
	{
		if (value != PartInstance::showUnalignedParts)
		{
			PartInstance::showUnalignedParts = value;
			raisePropertyChanged(prop_PartCoordinateFrames);
		}
	}

	void DebugSettings::setHighlightSleepParts(bool value)
	{
		if (value != PartInstance::highlightSleepParts)
		{
			PartInstance::highlightSleepParts = value;
			raisePropertyChanged(prop_PartCoordinateFrames);
		}
	}

	void DebugSettings::setHighlightAwakeParts(bool value)
	{
		if (value != PartInstance::highlightAwakeParts)
		{
			PartInstance::highlightAwakeParts = value;
			raisePropertyChanged(prop_PartCoordinateFrames);
		}
	}

	void DebugSettings::setShowPartCoordinateFrames(bool value)
	{
		if (value != PartInstance::showPartCoord)
		{
			PartInstance::showPartCoord = value;
			raisePropertyChanged(prop_PartCoordinateFrames);
		}
	}

	void DebugSettings::setShowModelCoordinateFrames(bool value)
	{
		if (value != ModelInstance::showModelCoord)
		{
			ModelInstance::showModelCoord = value;
			raisePropertyChanged(prop_ModelCoordinateFrames);
		}
	}

	void DebugSettings::setShowWorldCoordinateFrames(bool value)
	{
		if (value != Workspace::showWorldCoord)
		{
			Workspace::showWorldCoord = value;
			raisePropertyChanged(prop_WorldCoordinateFrames);
		}
	}

	void DebugSettings::setDisableSleep(bool value)
	{
		if (value != Primitive::disableSleep)
		{
			Primitive::disableSleep = value;
			raisePropertyChanged(prop_DisableSleep);
		}
	}

	void DebugSettings::setDisableEnvironmentalThrottle(bool value)
	{
		if (value != World::disableEnvironmentalThrottle)
		{
			World::disableEnvironmentalThrottle = value;
			raisePropertyChanged(prop_DisableEnvironmentalThrottle);
		}
	}

	void DebugSettings::setShowSpanningTree(bool value)
	{
		if (value != PartInstance::showSpanningTree)
		{
			PartInstance::showSpanningTree = value;
			raisePropertyChanged(prop_SpanningTree);
		}
	}

	void DebugSettings::setValidatingDebug(bool value)
	{
		if (value != Debugable::validatingDebug)
		{
			Debugable::validatingDebug = value;
			raisePropertyChanged(prop_ValidatingDebug);
		}
	}

	void DebugSettings::setAssertAction(Debugable::AssertAction value)
	{
		if (value != Debugable::assertAction)
		{
			Debugable::assertAction = value;
			raisePropertyChanged(prop_assertAction);
		}
	}

	void DebugSettings::setErrorReporting(ErrorReporting value)
	{
		if (value != errorReporting)
		{
			errorReporting = value;
			raisePropertyChanged(prop_errorReporting);
		}
	}

	float DebugSettings::shaderModel() const
	{
		return Render::SpecData().shaderModel;
	}

	int DebugSettings::videoMemory() const
	{
		return Render::SpecData().videoMemory;
	}

	int DebugSettings::cpuSpeed() const
	{
		return Render::SpecData().CPUSpeed;
	}

	std::string DebugSettings::glVendor() const
	{
		return Render::SpecData().glVendor;
	}

	std::string DebugSettings::cpu() const
	{
		return Render::SpecData().cpu;
	}

	int DebugSettings::ram() const
	{
		return Render::SpecData().ram;
	}

	std::string DebugSettings::resolution() const
	{
		return Render::SpecData().resolution;
	}

	std::string DebugSettings::osVer() const
	{
		OSVERSIONINFOA osvi;
		ZeroMemory((char*)&osvi + sizeof(DWORD), sizeof(OSVERSIONINFOA) - sizeof(DWORD)); // dwOSVersionInfoSize is the first element
		osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOA);

		GetVersionExA(&osvi);

		return G3D::format("%d.%d.%d.%d", osvi.dwOSVersionInfoSize, osvi.dwMajorVersion, osvi.dwMinorVersion, osvi.dwBuildNumber);
	}

	int DebugSettings::osPlatformId() const
	{
		OSVERSIONINFOA osvi;
		ZeroMemory((char*)&osvi + sizeof(DWORD), sizeof(OSVERSIONINFOA) - sizeof(DWORD));
		osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOA);

		GetVersionExA(&osvi);

		return osvi.dwPlatformId;
	}
}
