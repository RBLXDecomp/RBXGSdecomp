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
        void setShowAnchoredParts(bool);
        bool getShowAggregation() const;
        void setShowAggregation(bool);
        bool getShowUnalignedParts() const;
        void setShowUnalignedParts(bool);
        bool getHighlightSleepParts() const;
        void setHighlightSleepParts(bool);
        bool getHighlightAwakeParts() const;
        void setHighlightAwakeParts(bool);
        bool getShowPartCoordinateFrames() const;
        void setShowPartCoordinateFrames(bool);
        bool getShowModelCoordinateFrames() const;
        void setShowModelCoordinateFrames(bool);
        bool getShowWorldCoordinateFrames() const;
        void setShowWorldCoordinateFrames(bool);
        bool getDisableSleep() const;
        void setDisableSleep(bool);
        bool getDisableEnvironmentalThrottle() const;
        void setDisableEnvironmentalThrottle(bool);
        bool getUseNewGraphics() const;
        void setUseNewGraphics(bool);
        bool getShowSpanningTree() const;
        void setShowSpanningTree(bool);
        bool getValidatingDebug() const;
        void setValidatingDebug(bool);
        Debugable::AssertAction getAssertAction() const;
        void setAssertAction(Debugable::AssertAction);
        ErrorReporting getErrorReporting() const;
        void setErrorReporting(ErrorReporting);
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
