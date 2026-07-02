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
			Sound(FMOD::System*, SoundId, bool);
			~Sound();
			FMOD::Sound* get();
			void detach();
			void release();
		};

		extern const char* sSoundChannel;

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
			void setSoundId(SoundId);
			SoundId getSoundId() const;
			float getVolume() const;
			void setVolume(float);
			bool getLooped() const;
			void setLooped(bool);
			int getPlayCount() const;
			void setPlayCount(int);
			void play();
			void pause();
			void stop();
			bool isPlaying() const;
			bool isPaused() const;
		protected:
			virtual bool askSetParent(const Instance*) const;
			virtual void onAncestorChanged(const AncestorChanged&);
			virtual void onServiceProvider(const ServiceProvider*, const ServiceProvider*);
			virtual void onEvent(const RunService*, Heartbeat);
		private:
			void updateListenState();
			void updateLooped();
			void update3D(FMOD::Channel*);
			void playSound(Instance*);
			void playSound();
			void releaseChannel();
		public:
			void onChannelEnd();
		};

		extern const char* sSoundService;

		class SoundService : public DescribedCreatable<SoundService, Instance, &sSoundService>,
							 public Service,
							 public Listener<RunService, Heartbeat>
		{
		private:
			FMOD::System* system;
			unsigned fmod_version;
			std::map<SoundType, boost::shared_ptr<SoundChannel>> stockSounds;
			float dopplerscale;
			float distancefactor;
			float rolloffscale;
			boost::shared_ptr<Instance> statsItem;
			ReverbType ambientReverb;
			double nextGarbageCollectTime;
			std::map<SoundId, boost::shared_ptr<Sound>> loadedSounds;
			std::map<SoundId, boost::shared_ptr<Sound>> loaded3DSounds;
			bool mySoundDisabled;
		public:
			bool soundDisabled;
			Reflection::BoundProp<float, 1> prop_dopplerscale;
			Reflection::BoundProp<float, 1> prop_distancefactor;
			Reflection::BoundProp<float, 1> prop_rolloffscale;
			Reflection::EnumPropDescriptor<SoundService, ReverbType> prop_AmbientReverb;

		public:
			boost::shared_ptr<Sound> loadSound(SoundId, bool);
			bool enabled() const;
			void disable();
			SoundService();
			virtual ~SoundService();
			void playSound(SoundType);
			unsigned getfmod_version() const;
			ReverbType getAmbientReverb() const;
			void setAmbientReverb(const ReverbType&);
		private:
			void openFmod();
			void closeFmod();
			void garbageCollectSounds();
		protected:
			virtual void onEvent(const RunService*, Heartbeat);
			virtual void onServiceProvider(const ServiceProvider*, const ServiceProvider*);
		private:
			void loadStockSounds();
			void loadStockSounds(SoundType, std::string);
			void update3DSettings();
			void on3DSettingChanged(const Reflection::PropertyDescriptor&);
			void updateAmbientReverb();

		private:
			static void getSoundStats(const std::map<SoundId, boost::shared_ptr<Sound>>&, size_t, size_t);
			static void gcSounds(std::map<SoundId, boost::shared_ptr<Sound>>&);
		};
	}

	void registerSound();
}

extern const char* sStockSound;

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
