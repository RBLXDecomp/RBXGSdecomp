#pragma once
#include "reflection/reflection.h"
#include "v8datamodel/GlobalSettings.h"
#include "util/Debug.h"

namespace RBX
{
    extern const char* sDebugSettings;
    class DebugSettings : public GlobalSettingsItem<DebugSettings, &sDebugSettings>
    {
    public:
        enum ErrorReporting
        {
            DontReport,
            Prompt,
            Report,
        };

    private:
        bool stackTracingEnabled;
        bool ioEnabled;
        bool validatingDebug;

    public:
        bool soundWarnings;
        ErrorReporting errorReporting;
    
    public:
        static Reflection::BoundProp<bool, true> prop_stackTracingEnabled;
        static Reflection::BoundProp<bool, true> prop_ioEnabled;
    
    public:
        DebugSettings();

        bool getStackTracingEnabled() const
        {
            return stackTracingEnabled;
        }

        bool getioEnabled() const
        {
            return ioEnabled;
        }

        bool getShowAnchoredParts() const;
        void setShowAnchoredParts(bool value);
        bool getShowAggregation() const;
        void setShowAggregation(bool value);
        bool getShowUnalignedParts() const;
        void setShowUnalignedParts(bool value);
        bool getHighlightSleepParts() const;
        void setHighlightSleepParts(bool value);
        bool getHighlightAwakeParts() const;
        void setHighlightAwakeParts(bool value);
        bool getShowPartCoordinateFrames() const;
        void setShowPartCoordinateFrames(bool value);
        bool getShowModelCoordinateFrames() const;
        void setShowModelCoordinateFrames(bool value);
        bool getShowWorldCoordinateFrames() const;
        void setShowWorldCoordinateFrames(bool value);
        bool getDisableSleep() const;
        void setDisableSleep(bool value);
        bool getDisableEnvironmentalThrottle() const;
        void setDisableEnvironmentalThrottle(bool value);
        bool getUseNewGraphics() const;
        void setUseNewGraphics(bool value);
        bool getShowSpanningTree() const;
        void setShowSpanningTree(bool value);

        bool getValidatingDebug() const
		{
			return Debugable::validatingDebug;
		}

        void setValidatingDebug(bool value);

        Debugable::AssertAction getAssertAction() const
		{
			return Debugable::assertAction;
		}

        void setAssertAction(Debugable::AssertAction value);

        ErrorReporting getErrorReporting() const
		{
			return errorReporting;
		}

        void setErrorReporting(ErrorReporting value);
        float shaderModel() const;
        int videoMemory() const;
        int cpuSpeed() const;
        std::string osVer() const;
        int osPlatformId() const;
        std::string glVendor() const;
        std::string gfxcard() const;
        std::string cpu() const;
        int ram() const;
        std::string resolution() const;
    };
}
