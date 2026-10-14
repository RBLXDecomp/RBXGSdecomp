#include "util/Sound.h"
#include "util/Math.h"
#include "util/Utilities.h"
#include "util/standardout.h"
#include "util/ContentProvider.h"
#include "util/RunStateOwner.h"
#include "v8datamodel/PartInstance.h"
#include "v8datamodel/Workspace.h"
#include "v8datamodel/Camera.h"
#include "v8datamodel/Stats.h"
#include "v8datamodel/GameSettings.h"
#include "v8datamodel/DebugSettings.h"
#include "../../fmod/api/inc/fmod.hpp"
#include "../../fmod/api/inc/fmod_errors.h"

void checkResult(FMOD_RESULT result)
{
	if (result == FMOD_OK)
		return;

	std::string message = G3D::format("FMOD %d: %s", result, FMOD_ErrorString(result));
	RBX::StandardOut::singleton()->print(RBX::MESSAGE_WARNING, message.c_str());
	throw std::runtime_error(message);
}

void checkResultNoThrow(FMOD_RESULT result)
{
	if (result == FMOD_OK)
		return;

	if (RBX::DebugSettings::singleton().soundWarnings)
	{
		char buffer[512];
		strncpy(buffer, FMOD_ErrorString(result), sizeof(buffer));
		RBX::StandardOut::singleton()->print(RBX::MESSAGE_WARNING, "FMOD %d: %s", result, buffer);
		RBXASSERT(false);
	}
}

FMOD_RESULT F_CALLBACK callbackChannelEnd(FMOD_CHANNEL* channel, FMOD_CHANNEL_CALLBACKTYPE type, int command, unsigned int commanddata1, unsigned int commanddata2)
{
	void* userdata;
	FMOD_Channel_GetUserData(channel, &userdata);
	if (userdata)
	{
		RBX::Soundscape::SoundChannel* soundChannel = reinterpret_cast<RBX::Soundscape::SoundChannel*>(userdata);
		soundChannel->onChannelEnd();
	}

	return FMOD_OK;
}

bool convert(const G3D::Vector3& src, FMOD_VECTOR& dst)
{
	G3D::Vector3 temp = src;

	if (RBX::Math::isDenormal(temp.x))
		temp.x = 0.0f;
	if (RBX::Math::isDenormal(temp.y))
		temp.y = 0.0f;
	if (RBX::Math::isDenormal(temp.z))
		temp.z = 0.0f;

	if (RBX::Math::isNanInfDenormVector3(temp))
		return false;

	temp = temp.clamp(-5000.0f, 5000.0f);

	dst.x = temp.x;
	dst.y = temp.y;
	dst.z = temp.z;

	return true;
}

static void releaseSound(const std::pair<RBX::Soundscape::SoundId, boost::shared_ptr<RBX::Soundscape::Sound>>& p)
{
	p.second->release();
}

namespace RBX
{
	template<>
	class StringConverter<Soundscape::SoundId>
	{
	public:
		static std::string convertToString(const Soundscape::SoundId& value)
		{
			return value.toString();
		}

		// TODO: does not match
		static bool convertToValue(const std::string& text, Soundscape::SoundId& value)
		{
			value = Soundscape::SoundId(text);
			return true;
		}
	};

	namespace Soundscape
	{
		Reflection::BoundProp<float, 1> SoundService::prop_dopplerscale("DopplerScale", "Data", &SoundService::dopplerscale, &SoundService::on3DSettingChanged, Reflection::PropertyDescriptor::LEGACY);
		Reflection::BoundProp<float, 1> SoundService::prop_distancefactor("DistanceFactor", "Data", &SoundService::distancefactor, &SoundService::on3DSettingChanged, Reflection::PropertyDescriptor::STANDARD);
		Reflection::BoundProp<float, 1> SoundService::prop_rolloffscale("RolloffScale", "Data", &SoundService::rolloffscale, &SoundService::on3DSettingChanged, Reflection::PropertyDescriptor::STANDARD);
		Reflection::EnumPropDescriptor<SoundService, ReverbType> SoundService::prop_AmbientReverb("AmbientReverb", "Data", &SoundService::getAmbientReverb, &SoundService::setAmbientReverb, Reflection::PropertyDescriptor::STANDARD);

		static Reflection::PropDescriptor<SoundChannel, SoundId> sound_desc_SoundId("SoundId", "Data", &SoundChannel::getSoundId, &SoundChannel::setSoundId, Reflection::PropertyDescriptor::STANDARD);
		static Reflection::PropDescriptor<SoundChannel, float> sound_desc_Volume("Volume", "Data", &SoundChannel::getVolume, &SoundChannel::setVolume, Reflection::PropertyDescriptor::STANDARD);
		static Reflection::PropDescriptor<SoundChannel, int> sound_desc_PlayCount("PlayCount", "Data", &SoundChannel::getPlayCount, &SoundChannel::setPlayCount, Reflection::PropertyDescriptor::STREAMING);
		static Reflection::PropDescriptor<SoundChannel, bool> sound_desc_Looped("Looped", "Behavior", &SoundChannel::getLooped, &SoundChannel::setLooped, Reflection::PropertyDescriptor::STANDARD);
		static Reflection::PropDescriptor<SoundChannel, bool> sound_desc_IsPlaying("IsPlaying", "Data", &SoundChannel::isPlaying, NULL, Reflection::PropertyDescriptor::UI);
		static Reflection::PropDescriptor<SoundChannel, bool> sound_desc_isPaused("IsPaused", "Data", &SoundChannel::isPaused, NULL, Reflection::PropertyDescriptor::UI);
		Reflection::BoundProp<bool, 1> SoundChannel::sound_desc_playOnRemove("PlayOnRemove", "Behavior", &SoundChannel::playOnRemove, Reflection::PropertyDescriptor::UI);
		static Reflection::BoundFuncDesc<SoundChannel, void(void), 0> sound_playFunction(&SoundChannel::play, "Play", Reflection::FunctionDescriptor::AnyCaller);
		static Reflection::BoundFuncDesc<SoundChannel, void(void), 0> sound_pauseFunction(&SoundChannel::pause, "Pause", Reflection::FunctionDescriptor::AnyCaller);
		static Reflection::BoundFuncDesc<SoundChannel, void(void), 0> sound_stopFunction(&SoundChannel::stop, "Stop", Reflection::FunctionDescriptor::AnyCaller);

		Sound::Sound(FMOD::System* system, SoundId id, bool is3D)
			: system(system),
			  id(id),
			  is3D(is3D),
			  fmod_sound(NULL),
			  refCount(0)
		{
		}

		Sound::~Sound()
		{
			release();
		}

		FMOD::Sound* Sound::get()
		{
			if (!fmod_sound)
			{
				boost::shared_ptr<const std::string> data = ContentProvider::singleton().requestContentString(id);
				if (!data)
					return NULL;

				FMOD_MODE mode = is3D ? FMOD_3D : 0;
				mode |= GameSettings::singleton().softwareSound ? FMOD_SOFTWARE : FMOD_HARDWARE;
				mode |= FMOD_OPENMEMORY;

				FMOD_CREATESOUNDEXINFO exinfo = {};
				exinfo.cbsize = sizeof(FMOD_CREATESOUNDEXINFO);
				exinfo.length = static_cast<unsigned int>(data->size());

				checkResult(system->createSound(data->data(), mode, &exinfo, &fmod_sound));
			}

			return fmod_sound;
		}

		void Sound::release()
		{
			if (fmod_sound)
			{
				checkResultNoThrow(fmod_sound->release());
				fmod_sound = NULL;
			}
		}

		char* sSoundChannel = "Sound";

		SoundChannel::SoundChannel()
			: fmod_channel(NULL),
			  is3D(false),
			  looped(false),
			  soundDisabled(false),
			  playOnRemove(false),
			  volume(0.5f),
			  playCount(-1),
			  part(NULL)
		{
			setName("Sound");
		}

		SoundChannel::~SoundChannel()
		{
			RBXASSERT(!fmod_channel);
			RBXASSERT(!sound);
		}

		SoundId SoundChannel::getSoundId() const
		{
			return soundId;
		}

		void SoundChannel::setSoundId(SoundId value)
		{
			if (soundId != value)
			{
				soundId = value;
				if (!soundDisabled && GameSettings::singleton().soundEnabled)
					ContentProvider::singleton().requestContentString(soundId);
				releaseChannel();
				raisePropertyChanged(sound_desc_SoundId);
			}
		}

		float SoundChannel::getVolume() const
		{
			return volume;
		}

		void SoundChannel::setVolume(float value)
		{
			value = G3D::clamp(value, 0.0f, 1.0f);
			if (volume != value)
			{
				volume = value;
				if (fmod_channel)
					checkResultNoThrow(fmod_channel->setVolume(volume));
				raisePropertyChanged(sound_desc_Volume);
			}
		}

		bool SoundChannel::getLooped() const
		{
			return looped;
		}

		void SoundChannel::setLooped(bool value)
		{
			if (looped != value)
			{
				looped = value;
				updateLooped();
				raisePropertyChanged(sound_desc_Volume);
			}
		}

		int SoundChannel::getPlayCount() const
		{
			return playCount;
		}

		void SoundChannel::setPlayCount(int value)
		{
			if (value == -1)
			{
				stop();
			}
			else if (value == 0)
			{
				pause();
			}
			else if (value > playCount)
			{
				playSound();
				playCount = value;
				raisePropertyChanged(sound_desc_PlayCount);
			}
		}

		bool SoundChannel::isPlaying() const
		{
			if (!fmod_channel)
				return false;

			bool playing;
			if (fmod_channel->isPlaying(&playing) == FMOD_ERR_INVALID_HANDLE)
				return false;

			return playing;
		}

		bool SoundChannel::isPaused() const
		{
			if (!fmod_channel)
				return true;

			bool paused;
			if (fmod_channel->getPaused(&paused) == FMOD_ERR_INVALID_HANDLE)
				return true;

			return paused;
		}

		void SoundChannel::play()
		{
			playSound();

			playCount = G3D::max(playCount + 1, 1);
			raisePropertyChanged(sound_desc_PlayCount);
		}

		void SoundChannel::pause()
		{
			if (fmod_channel)
				checkResultNoThrow(fmod_channel->setPaused(true));

			updateListenState();

			if (playCount != 0)
			{
				playCount = 0;
				raisePropertyChanged(sound_desc_PlayCount);
			}
		}

		void SoundChannel::stop()
		{
			if (fmod_channel)
			{
				checkResultNoThrow(fmod_channel->setUserData(NULL));
				checkResultNoThrow(fmod_channel->stop());
				fmod_channel = NULL;
			}

			updateListenState();

			if (playCount != -1)
			{
				playCount = -1;
				raisePropertyChanged(sound_desc_PlayCount);
			}
		}

		void SoundChannel::updateListenState()
		{
			if (part && is3D && fmod_channel)
			{
				Notifier<RunService, Heartbeat>::connect(ServiceProvider::find<RunService>(this), this);
			}
			else
			{
				Notifier<RunService, Heartbeat>::disconnect(ServiceProvider::find<RunService>(this), this);
			}
		}

		void SoundChannel::updateLooped()
		{
			if (fmod_channel)
			{
				FMOD_MODE mode;
				checkResultNoThrow(fmod_channel->getMode(&mode));

				mode = (mode | (looped ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF)) & (looped ? ~FMOD_LOOP_OFF : ~FMOD_LOOP_NORMAL);

				checkResultNoThrow(fmod_channel->setMode(mode));
				checkResultNoThrow(fmod_channel->setLoopCount(looped ? -1 : 0));
			}
		}

		void SoundChannel::update3D(FMOD::Channel* channel)
		{
			if (is3D && part)
			{
				FMOD_VECTOR pos;
				if (!convert(part->getCoordinateFrame().translation, pos))
					return;

				FMOD_VECTOR vel;
				if (!convert(part->getVelocity().linear, vel))
					return;

				checkResultNoThrow(channel->set3DAttributes(&pos, &vel));
			}
		}

		void SoundChannel::playSound(Instance* context)
		{
			if (soundDisabled)
				return;

			try
			{
				if (!looped || !isPlaying())
				{
					if (SoundService* soundService = ServiceProvider::create<SoundService>(context))
					{
						if (!soundService->enabled())
						{
							soundDisabled = true;
						}
						else
						{
							releaseChannel();
							is3D = part != NULL;
							sound = soundService->loadSound(soundId, is3D);
							sound->refCount++;
							if (FMOD::Sound* fmodSound = sound->get())
							{
								checkResult(soundService->system->playSound(FMOD_CHANNEL_FREE, fmodSound, true, &fmod_channel));
								updateLooped();
								update3D(fmod_channel);
								checkResult(fmod_channel->setVolume(volume));
								checkResult(fmod_channel->setUserData(reinterpret_cast<void*>(this)));
								checkResult(fmod_channel->setCallback(FMOD_CHANNEL_CALLBACKTYPE_END, &callbackChannelEnd, 0));
								checkResult(fmod_channel->setPaused(false));
								updateListenState();
							}
						}
					}
				}
				else
				{
					checkResult(fmod_channel->setPaused(false));
				}
			}
			catch (std::exception& e)
			{
				StandardOut::singleton()->print(RBX::MESSAGE_ERROR, e.what());
			}
		}

		void SoundChannel::playSound()
		{
			playSound(this);
		}

		void SoundChannel::releaseChannel()
		{
			if (fmod_channel)
			{
				checkResultNoThrow(fmod_channel->setUserData(NULL));
				fmod_channel = NULL;
			}

			if (sound)
			{
				sound->refCount--;
				sound.reset();
			}
		}

		void SoundChannel::onChannelEnd()
		{
			releaseChannel();
			updateListenState();
		}

		void SoundChannel::onAncestorChanged(const AncestorChanged& event)
		{
			const ServiceProvider* oldProvider = ServiceProvider::findServiceProvider(event.oldParent);
			const ServiceProvider* newProvider = ServiceProvider::findServiceProvider(event.newParent);

			Notifier<RunService, Heartbeat>::disconnect(ServiceProvider::find<RunService>(oldProvider), this);
			if (oldProvider)
			{
				if (looped)
					stop();
				releaseChannel();
			}

			Instance::onAncestorChanged(event);

			if (event.child == this)
			{
				part = dynamic_cast<PartInstance*>(event.newParent);
				updateListenState();
			}

			if (newProvider)
			{
				if (SoundService* soundService = newProvider->create<SoundService>())
				{
					if (soundService->enabled())
					{
						soundDisabled = false;
						ContentProvider::singleton().requestContentString(soundId);
						updateListenState();
						if (playCount > 0)
							playSound();
					}
				}
			}

			Workspace* oldWorkspace = ServiceProvider::find<Workspace>(oldProvider);
			if (oldWorkspace && (event.oldParent == oldWorkspace || event.oldParent->isDescendentOf(oldWorkspace)))
			{
				Workspace* newWorkspace = ServiceProvider::find<Workspace>(newProvider);
				if (!(newWorkspace && (event.newParent == newWorkspace || event.newParent->isDescendentOf(newWorkspace))))
				{
					if (playOnRemove)
						playSound(event.oldParent);
					releaseChannel();
				}
			}
		}

		void SoundChannel::onServiceProvider(const ServiceProvider* oldProvider, const ServiceProvider* newProvider)
		{
			releaseChannel();
			Instance::onServiceProvider(oldProvider, newProvider);
		}

		void SoundChannel::onEvent(const RunService* source, Heartbeat event)
		{
			if (!isPaused())
				update3D(fmod_channel);
		}

		char* sSoundService = "SoundService";
		bool SoundService::soundDisabled = false;

		SoundService::SoundService()
			: system(NULL),
			  fmod_version(0),
			  dopplerscale(1.0f),
			  distancefactor(10.0f),
			  rolloffscale(1.0f),
			  ambientReverb(NoReverb),
			  nextGarbageCollectTime(G3D::System::getTick()),
			  mySoundDisabled(false)
		{
			setName("Soundscape");
		}

		SoundService::~SoundService()
		{
			RBXASSERT(stockSounds.empty());
			RBXASSERT(loadedSounds.empty());
			RBXASSERT(loaded3DSounds.empty());
			RBXASSERT(!system);
		}

		bool SoundService::enabled() const
		{
			return system != NULL;
		}

		unsigned int SoundService::getfmod_version() const
		{
			return fmod_version;
		}

		ReverbType SoundService::getAmbientReverb() const
		{
			return ambientReverb;
		}

		void SoundService::setAmbientReverb(const ReverbType& value)
		{
			if (value != ambientReverb)
			{
				ambientReverb = value;
				raisePropertyChanged(prop_AmbientReverb);
				updateAmbientReverb();
			}
		}

		void SoundService::openFmod()
		{
			if (soundDisabled || mySoundDisabled || !GameSettings::singleton().soundEnabled)
				return;

			try
			{
				checkResult(FMOD::System_Create(&system));

				checkResult(system->getVersion(&fmod_version));
				if (fmod_version < FMOD_VERSION)
					throw std::runtime_error("Incorrect FMODEX DLL version");

				int driver = -1;
				checkResultNoThrow(system->getDriver(&driver));
				if (driver >= 0)
				{
					char name[256] = "?";
					checkResultNoThrow(system->getDriverName(driver, name, sizeof(name)));
					StandardOut::singleton()->print(MESSAGE_INFO, "Active sound driver \"%s\"", name);
				}
				else
				{
					StandardOut::singleton()->print(MESSAGE_WARNING, "No active sound driver detected");
				}

				FMOD_CAPS caps;
				FMOD_SPEAKERMODE speakerMode;
				checkResult(system->getDriverCaps(0, &caps, NULL, NULL, &speakerMode));
				checkResult(system->setSpeakerMode(speakerMode));
				if ((caps & FMOD_CAPS_HARDWARE_EMULATED) != 0)
				{
					StandardOut::singleton()->print(MESSAGE_WARNING, "The acceleration slider in the Sound Control Panel set to 'off'");
					checkResult(system->setDSPBufferSize(1024, 10));
				}

				if (system->init(100, 0, NULL) == FMOD_ERR_OUTPUT_CREATEBUFFER)
				{
					StandardOut::singleton()->print(MESSAGE_WARNING, "The speaker mode selected isn't supported by this soundcard.  Using Stereo");
					checkResult(system->setSpeakerMode(FMOD_SPEAKERMODE_STEREO));
					checkResult(system->init(100, 0, NULL));
				}

				update3DSettings();
				if (ambientReverb != NoReverb)
					updateAmbientReverb();
			}
			catch (std::exception&)
			{
				if (system)
				{
					checkResultNoThrow(system->release());
					system = NULL;
				}
			}
		}

		void SoundService::closeFmod()
		{
			stockSounds.clear();
			removeAllChildren();

			std::for_each(loadedSounds.begin(), loadedSounds.end(), releaseSound);
			std::for_each(loaded3DSounds.begin(), loaded3DSounds.end(), releaseSound);
			loadedSounds.clear();
			loaded3DSounds.clear();

			if (system)
			{
				StandardOut::singleton()->print(MESSAGE_INFO, "Closing FMOD System");

				checkResultNoThrow(system->release());
				StandardOut::singleton()->print(MESSAGE_INFO, "Closed FMOD System");
				system = NULL;
			}
		}

		boost::shared_ptr<Sound> SoundService::loadSound(SoundId id, bool is3D)
		{
			LoadedSounds& sounds = is3D ? loaded3DSounds : loadedSounds;

			LoadedSounds::iterator it = sounds.find(id);
			if (it != sounds.end())
			{
				return it->second;
			}
			else
			{
				boost::shared_ptr<Sound> sound(new Sound(system, id, is3D));
				sounds[id] = sound;
				return sound;
			}
		}

		void SoundService::update3DSettings()
		{
			if (system)
				checkResultNoThrow(system->set3DSettings(dopplerscale, distancefactor, rolloffscale));
		}

		void SoundService::on3DSettingChanged(const Reflection::PropertyDescriptor& property)
		{
			update3DSettings();
		}

		void SoundService::updateAmbientReverb()
		{
			if (system)
			{
				switch (ambientReverb)
				{
				case NoReverb:
				{
					FMOD_REVERB_PROPERTIES p = FMOD_PRESET_OFF;
					checkResultNoThrow(system->setReverbProperties(&p));
					break;
				}
				case GenericReverb:
				{
					FMOD_REVERB_PROPERTIES p = FMOD_PRESET_GENERIC;
					checkResultNoThrow(system->setReverbProperties(&p));
					break;
				}
				case PaddedCell:
				{
					FMOD_REVERB_PROPERTIES p = FMOD_PRESET_PADDEDCELL;
					checkResultNoThrow(system->setReverbProperties(&p));
					break;
				}
				case Room:
				{
					FMOD_REVERB_PROPERTIES p = FMOD_PRESET_PADDEDCELL;
					checkResultNoThrow(system->setReverbProperties(&p));
					break;
				}
				case Bathroom:
				{
					FMOD_REVERB_PROPERTIES p = FMOD_PRESET_BATHROOM;
					checkResultNoThrow(system->setReverbProperties(&p));
					break;
				}
				case LivingRoom:
				{
					FMOD_REVERB_PROPERTIES p = FMOD_PRESET_LIVINGROOM;
					checkResultNoThrow(system->setReverbProperties(&p));
					break;
				}
				case StoneRoom:
				{
					FMOD_REVERB_PROPERTIES p = FMOD_PRESET_STONEROOM;
					checkResultNoThrow(system->setReverbProperties(&p));
					break;
				}
				case Auditorium:
				{
					FMOD_REVERB_PROPERTIES p = FMOD_PRESET_AUDITORIUM;
					checkResultNoThrow(system->setReverbProperties(&p));
					break;
				}
				case ConcertHall:
				{
					FMOD_REVERB_PROPERTIES p = FMOD_PRESET_CONCERTHALL;
					checkResultNoThrow(system->setReverbProperties(&p));
					break;
				}
				case Cave:
				{
					FMOD_REVERB_PROPERTIES p = FMOD_PRESET_CAVE;
					checkResultNoThrow(system->setReverbProperties(&p));
					break;
				}
				case Arena:
				{
					FMOD_REVERB_PROPERTIES p = FMOD_PRESET_ARENA;
					checkResultNoThrow(system->setReverbProperties(&p));
					break;
				}
				case Hangar:
				{
					FMOD_REVERB_PROPERTIES p = FMOD_PRESET_HANGAR;
					checkResultNoThrow(system->setReverbProperties(&p));
					break;
				}
				case CarpettedHallway:
				{
					FMOD_REVERB_PROPERTIES p = FMOD_PRESET_CARPETTEDHALLWAY;
					checkResultNoThrow(system->setReverbProperties(&p));
					break;
				}
				case Hallway:
				{
					FMOD_REVERB_PROPERTIES p = FMOD_PRESET_HALLWAY;
					checkResultNoThrow(system->setReverbProperties(&p));
					break;
				}
				case StoneCorridor:
				{
					FMOD_REVERB_PROPERTIES p = FMOD_PRESET_STONECORRIDOR;
					checkResultNoThrow(system->setReverbProperties(&p));
					break;
				}
				case Alley:
				{
					FMOD_REVERB_PROPERTIES p = FMOD_PRESET_ALLEY;
					checkResultNoThrow(system->setReverbProperties(&p));
					break;
				}
				case Forest:
				{
					FMOD_REVERB_PROPERTIES p = FMOD_PRESET_FOREST;
					checkResultNoThrow(system->setReverbProperties(&p));
					break;
				}
				case City:
				{
					FMOD_REVERB_PROPERTIES p = FMOD_PRESET_CITY;
					checkResultNoThrow(system->setReverbProperties(&p));
					break;
				}
				case Mountains:
				{
					FMOD_REVERB_PROPERTIES p = FMOD_PRESET_MOUNTAINS;
					checkResultNoThrow(system->setReverbProperties(&p));
					break;
				}
				case Quarry:
				{
					FMOD_REVERB_PROPERTIES p = FMOD_PRESET_QUARRY;
					checkResultNoThrow(system->setReverbProperties(&p));
					break;
				}
				case Plain:
				{
					FMOD_REVERB_PROPERTIES p = FMOD_PRESET_PLAIN;
					checkResultNoThrow(system->setReverbProperties(&p));
					break;
				}
				case ParkingLot:
				{
					FMOD_REVERB_PROPERTIES p = FMOD_PRESET_PARKINGLOT;
					checkResultNoThrow(system->setReverbProperties(&p));
					break;
				}
				case SewerPipe:
				{
					FMOD_REVERB_PROPERTIES p = FMOD_PRESET_SEWERPIPE;
					checkResultNoThrow(system->setReverbProperties(&p));
					break;
				}
				case UnderWater:
				{
					FMOD_REVERB_PROPERTIES p = FMOD_PRESET_UNDERWATER;
					checkResultNoThrow(system->setReverbProperties(&p));
					break;
				}
				}
			}
		}

		void SoundService::playSound(SoundType soundType)
		{
			StockSounds::iterator it = stockSounds.find(soundType);
			if (it != stockSounds.end())
				it->second->play();
		}

		void SoundService::loadStockSound(SoundType soundType, std::string file)
		{
			boost::shared_ptr<StockSound> sound = Creatable<Instance>::create<StockSound>();
			sound->setName(Reflection::EnumDesc<SoundType>::singleton().convertToString(soundType));
			sound->setSoundId(SoundId::fromAssets(file));
			sound->setParent(this);
			stockSounds[soundType] = sound;
		}

		void SoundService::loadStockSounds()
		{
			stockSounds.clear();
			removeAllChildren();

			loadStockSound(BOING_SOUND, "Sounds\\bass.wav");
			loadStockSound(BOMB_SOUND, "Sounds\\collide.wav");
			loadStockSound(BREAK_SOUND, "Sounds\\glassbreak.wav");
			loadStockSound(CLICK_SOUND, "Sounds\\switch.wav");
			loadStockSound(CLOCK_SOUND, "Sounds\\clickfast.wav");
			loadStockSound(RUBBERBAND_SOUND, "Sounds\\Rubber band sling shot.wav");
			loadStockSound(PAGE_SOUND, "Sounds\\pageturn.wav");
			loadStockSound(PING_SOUND, "Sounds\\electronicpingshort.wav");
			loadStockSound(SNAP_SOUND, "Sounds\\snap.wav");
			loadStockSound(SPLAT_SOUND, "Sounds\\splat.wav");
			loadStockSound(STEP_SOUND, "Sounds\\SWITCH3.wav");
			loadStockSound(STEP_ON_SOUND, "Sounds\\flashbulb.wav");
			loadStockSound(SWOOSH_SOUND, "Sounds\\swoosh.wav");
			loadStockSound(VICTORY_SOUND, "Sounds\\victory.wav");
		}

		void SoundService::onServiceProvider(const ServiceProvider* oldProvider, const ServiceProvider* newProvider)
		{
			Notifier<RunService, Heartbeat>::disconnect(ServiceProvider::find<RunService>(oldProvider), this);
			if (statsItem)
			{
				statsItem->setParent(NULL);
				statsItem.reset();
			}
			if (oldProvider)
				closeFmod();

			Instance::onServiceProvider(oldProvider, newProvider);

			Notifier<RunService, Heartbeat>::connect(ServiceProvider::find<RunService>(newProvider), this);
			if (newProvider)
			{
				openFmod();
				loadStockSounds();
				if (Stats::StatsService* statsService = newProvider->create<Stats::StatsService>())
				{
					statsItem = Creatable<Instance>::create<SoundServiceStatsItem>(this);
					statsItem->setParent(statsService);
				}
			}
		}

		void SoundService::onEvent(const RunService* source, Heartbeat event)
		{
			if (!system)
				return;

			if (Workspace* workspace = ServiceProvider::find<Workspace>(this))
			{
				if (Camera* camera = workspace->getCamera())
				{
					G3D::CoordinateFrame cf = camera->getCameraCoordinateFrame();
					FMOD_VECTOR listenerPos;
					if (convert(cf.translation, listenerPos))
					{
						G3D::Vector3 forward = -cf.lookVector();
						FMOD_VECTOR listenerForward;
						if (convert(forward, listenerForward))
						{
							G3D::Vector3 up = cf.rotation.getColumn(1);
							FMOD_VECTOR listenerUp;
							if (convert(up, listenerUp))
							{
								FMOD_VECTOR listenerVel = {0, 0, 0};
								if (PVInstance* pvInstance = dynamic_cast<PVInstance*>(camera->getCameraSubjectInstance()))
								{
									if (PartInstance* primaryPart = pvInstance->getPrimaryPart())
									{
										convert(primaryPart->getVelocity().linear, listenerVel);
									}
								}

								checkResultNoThrow(system->set3DListenerAttributes(0, &listenerPos, &listenerVel, &listenerForward, &listenerUp));
							}
						}
					}
				}
			}

			garbageCollectSounds();

			checkResultNoThrow(system->update());
		}

		void SoundService::garbageCollectSounds()
		{
			if (nextGarbageCollectTime <= G3D::System::getTick())
			{
				gcSounds(loadedSounds);
				gcSounds(loaded3DSounds);
				nextGarbageCollectTime = G3D::System::getTick() + 5.0;
			}
		}

		void SoundService::gcSounds(LoadedSounds& sounds)
		{
			for (LoadedSounds::iterator it = sounds.begin(); it != sounds.end();)
			{
				if (it->second->refCount == 0 && G3D::uniformRandom() >= 0.7f)
				{
					it->second->release();
					it = sounds.erase(it);
				}
				else
				{
					++it;
				}
			}
		}

		void SoundService::getSoundStats(const LoadedSounds& sounds, size_t& numSounds, size_t& numUnusedSounds)
		{
			for (LoadedSounds::const_iterator it = sounds.begin(); it != sounds.end(); ++it)
			{
				++numSounds;
				if (it->second->refCount == 0)
					++numUnusedSounds;
			}
		}
	}

	void registerSound()
	{
		Soundscape::SoundChannel::classDescriptor();
		StockSound::classDescriptor();
		Soundscape::SoundService::classDescriptor();
	}
}

char* sStockSound = "StockSound";

StockSound::StockSound()
{
	propArchivable.setValue(this, false);
}

SoundServiceStatsItem::SoundServiceStatsItem(const RBX::Soundscape::SoundService* service)
	: service(service),
	  currentalloced(0),
	  maxalloced(0)
{
	setName("Sound");

	createBoundMemChildItem("Current", currentalloced);
	createBoundMemChildItem("Max", maxalloced);
	createBoundChildItem("# Sounds", numSounds);
	createBoundChildItem("# Unused", numUnusedSounds);
}

void SoundServiceStatsItem::update()
{
	if (service->enabled())
	{
		unsigned int fmod_version = service->getfmod_version();
		formatValue(fmod_version, "fmod %08x", fmod_version);

		int fmodcurrentalloced;
		int fmodmaxalloced;
		if (FMOD_Memory_GetStats(&fmodcurrentalloced, &fmodmaxalloced) == FMOD_OK)
		{
			currentalloced = fmodcurrentalloced;
			maxalloced = fmodmaxalloced;
		}

		numUnusedSounds = 0;
		numSounds = 0;
		RBX::Soundscape::SoundService::getSoundStats(service->loadedSounds, numSounds, numUnusedSounds);
		RBX::Soundscape::SoundService::getSoundStats(service->loaded3DSounds, numSounds, numUnusedSounds);
	}
	else
	{
		setValue(0.0, "-disabled-");
	}
}
