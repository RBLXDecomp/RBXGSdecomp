#pragma once
#include "util/ContentProvider.h"
#include "util/RunStateOwner.h"
#include "v8datamodel/PartInstance.h"
#include "v8datamodel/Stats.h"

namespace FMOD
{
	class Channel;
	class Sound;
	class System;
}

class SoundServiceStatsItem;

namespace RBX
{
	enum SoundType
	{
		NO_SOUND,
		BOING_SOUND,
		BOMB_SOUND,
		BREAK_SOUND,
		CLICK_SOUND,
		CLOCK_SOUND,
		RUBBERBAND_SOUND,
		PAGE_SOUND,
		PING_SOUND,
		SNAP_SOUND,
		SPLAT_SOUND,
		STEP_SOUND,
		STEP_ON_SOUND,
		SWOOSH_SOUND,
		VICTORY_SOUND
	};

	namespace Soundscape
	{
		enum ReverbType
		{
			NoReverb,
			GenericReverb,
			PaddedCell,
			Room,
			Bathroom,
			LivingRoom,
			StoneRoom,
			Auditorium,
			ConcertHall,
			Cave,
			Arena,
			Hangar,
			CarpettedHallway,
			Hallway,
			StoneCorridor,
			Alley,
			Forest,
			City,
			Mountains,
			Quarry,
			Plain,
			ParkingLot,
			SewerPipe,
			UnderWater
		};

		class SoundId : public ContentId
		{
		public:
			SoundId()
				: ContentId()
			{
			}
			SoundId(const std::string& id)
				: ContentId(id)
			{
			}
			SoundId(const char* id)
				: ContentId(id)
			{
			}
			SoundId(const ContentId& id)
				: ContentId(id)
			{
			}
		};

		class Sound : private boost::noncopyable
		{
		private:
			FMOD::Sound* fmod_sound;
			FMOD::System* system;
		public:
			int refCount;
			const SoundId id;
			const bool is3D;

		public:
			Sound(FMOD::System* system, SoundId id, bool is3D);
			~Sound();
			FMOD::Sound* get();
			void detach();
			void release();
		};

		extern char* sSoundChannel;

		class SoundChannel : public DescribedCreatable<SoundChannel, Instance, &sSoundChannel>,
							 public Listener<RunService, Heartbeat>
		{
		private:
			boost::shared_ptr<Sound> sound;
			FMOD::Channel* fmod_channel;
			SoundId soundId;
			float volume;
			bool playOnRemove;
			bool is3D : 1;
			bool looped : 1;
			bool soundDisabled : 1;
			int playCount;
			PartInstance* part;
		public:
			static Reflection::BoundProp<bool, 1> sound_desc_playOnRemove;

		public:
			SoundChannel();
			virtual ~SoundChannel();
			void setSoundId(SoundId value);
			SoundId getSoundId() const;
			float getVolume() const;
			void setVolume(float value);
			bool getLooped() const;
			void setLooped(bool value);
			int getPlayCount() const;
			void setPlayCount(int value);
			void play();
			void pause();
			void stop();
			bool isPlaying() const;
			bool isPaused() const;
		protected:
			virtual bool askSetParent(const Instance* instance) const
			{
				return true;
			}
			virtual void onAncestorChanged(const AncestorChanged& event);
			virtual void onServiceProvider(const ServiceProvider* oldProvider, const ServiceProvider* newProvider);
			virtual void onEvent(const RunService* source, Heartbeat event);
		private:
			void updateListenState();
			void updateLooped();
			void update3D(FMOD::Channel* channel);
			void playSound(Instance* context);
			void playSound();
			void releaseChannel();
		public:
			void onChannelEnd();
		};

		extern char* sSoundService;

		class SoundService : public DescribedCreatable<SoundService, Instance, &sSoundService>,
							 public Service,
							 public Listener<RunService, Heartbeat>
		{
			friend class SoundChannel;
			friend class SoundServiceStatsItem;

		private:
			typedef std::map<SoundType, boost::shared_ptr<SoundChannel>> StockSounds;
			typedef std::map<SoundId, boost::shared_ptr<Sound>> LoadedSounds;

		private:
			FMOD::System* system;
			unsigned int fmod_version;
			StockSounds stockSounds;
			float dopplerscale;
			float distancefactor;
			float rolloffscale;
			boost::shared_ptr<Instance> statsItem;
			ReverbType ambientReverb;
			double nextGarbageCollectTime;
			LoadedSounds loadedSounds;
			LoadedSounds loaded3DSounds;
			bool mySoundDisabled;
		public:
			static bool soundDisabled;
			static Reflection::BoundProp<float, 1> prop_dopplerscale;
			static Reflection::BoundProp<float, 1> prop_distancefactor;
			static Reflection::BoundProp<float, 1> prop_rolloffscale;
			static Reflection::EnumPropDescriptor<SoundService, ReverbType> prop_AmbientReverb;

		public:
			boost::shared_ptr<Sound> loadSound(SoundId id, bool is3D);
			bool enabled() const;
			void disable();
			SoundService();
			virtual ~SoundService();
			void playSound(SoundType soundType);
			unsigned int getfmod_version() const;
			ReverbType getAmbientReverb() const;
			void setAmbientReverb(const ReverbType& value);
		private:
			void openFmod();
			void closeFmod();
			void garbageCollectSounds();
		protected:
			virtual void onEvent(const RunService* source, Heartbeat event);
			virtual void onServiceProvider(const ServiceProvider* oldProvider, const ServiceProvider* newProvider);
		private:
			void loadStockSounds();
			void loadStockSound(SoundType soundType, std::string file);
			void update3DSettings();
			void on3DSettingChanged(const Reflection::PropertyDescriptor& property);
			void updateAmbientReverb();

		private:
			static void getSoundStats(const LoadedSounds& sounds, size_t& numSounds, size_t& numUnusedSounds);
			static void gcSounds(LoadedSounds& sounds);
		};
	}

	void registerSound();
}

extern char* sStockSound;

class StockSound : public RBX::DescribedCreatable<StockSound, RBX::Soundscape::SoundChannel, &sStockSound>
{
public:
	StockSound();
};

class SoundServiceStatsItem : public RBX::Stats::Item
{
private:
	const RBX::Soundscape::SoundService* service;
	size_t currentalloced;
	size_t maxalloced;
	size_t numSounds;
	size_t numUnusedSounds;

public:
	SoundServiceStatsItem(const RBX::Soundscape::SoundService*);
	virtual void update();
};
