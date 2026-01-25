#include "v8datamodel/Lighting.h"
#include "util/RunStateOwner.h"

namespace RBX
{
	const char* sLighting = "Lighting";

	static const Reflection::PropDescriptor<Lighting, G3D::Color3> desc_AmbientTop("TopAmbientV9", "Appearance", &Lighting::getAmbientTop, &Lighting::setAmbientTop, Reflection::PropertyDescriptor::STANDARD);
	static const Reflection::PropDescriptor<Lighting, G3D::Color3> desc_AmbientBottom("BottomAmbientV9", "Appearance", &Lighting::getAmbientBottom, &Lighting::setAmbientBottom, Reflection::PropertyDescriptor::STANDARD);
	static const Reflection::PropDescriptor<Lighting, G3D::Color3> desc_LightColor("SpotLightV9", "Appearance", &Lighting::getLightColor, &Lighting::setLightColor, Reflection::PropertyDescriptor::STANDARD);
	static const Reflection::PropDescriptor<Lighting, G3D::Color3> desc_ClearColor("ClearColor", "Appearance", &Lighting::getClearColor3, &Lighting::setClearColor3, Reflection::PropertyDescriptor::STANDARD);
	static const Reflection::PropDescriptor<Lighting, std::string> prop_Time("TimeOfDay", "Data", &Lighting::getTimeStr, &Lighting::setTimeStr, Reflection::PropertyDescriptor::STANDARD);
	static const Reflection::PropDescriptor<Lighting, float> prop_GeographicLatitude("GeographicLatitude", "Data", &Lighting::getGeographicLatitude, &Lighting::setGeographicLatitude, Reflection::PropertyDescriptor::STANDARD);
	
	Lighting::Lighting()
		: Base(),
		  ambientTop(209/255.0f, 208/255.0f, 217/255.0f),
		  ambientBottom(122/255.0f, 134/255.0f, 120/255.0f),
		  hasSky(true),
		  clearColor(G3D::Color3::white()),
		  timeOfDay(boost::posix_time::duration_from_string("14:00:00"))
	{
		skyParameters.lightColor = G3D::Color3(152/255.0f, 137/255.0f, 102/255.0f);
		skyParameters.setTime(timeOfDay.total_seconds());
		setName("Lighting");
	}

	Lighting::~Lighting()
	{
	}

	G3D::Vector3 Lighting::getMoonPosition()
	{
		return (skyParameters.physicallyCorrect) ? skyParameters.trueMoonPosition : skyParameters.moonPosition;
	}

	G3D::Vector3 Lighting::getSunPosition()
	{
		return (skyParameters.physicallyCorrect) ? skyParameters.trueSunPosition : skyParameters.sunPosition;
	}

	void Lighting::replaceSky(Sky* sky)
	{
		Sky* currentSky = findFirstChildOfType<Sky>();
		while (currentSky)
		{
			currentSky->setParent(NULL);
			currentSky = findFirstChildOfType<Sky>();
		}
		sky->setParent(this);
	}

	std::string Lighting::getTimeStr() const
	{
		return boost::posix_time::to_simple_string(timeOfDay);
	}

	void Lighting::setTimeStr(const std::string &time)
	{
		setTime(boost::posix_time::duration_from_string(time));
	}

	void Lighting::setClearColor(G3D::Color4 newClearColor)
	{
		if (newClearColor != clearColor)
		{
			clearColor = newClearColor;
			raisePropertyChanged(desc_ClearColor);
			fireLightingChanged(false);
			RunService* runService = ServiceProvider::findServiceProvider(this)->find<RunService>();
			if (runService)
				runService->invalidateRunViews();
		}
	}

	void Lighting::setLightColor(G3D::Color3 newLightColor)
	{
		if (newLightColor != skyParameters.lightColor)
		{
			skyParameters.lightColor = newLightColor;
			raisePropertyChanged(desc_LightColor);
			fireLightingChanged(false);
			RunService* runService = ServiceProvider::findServiceProvider(this)->find<RunService>();
			if (runService)
				runService->invalidateRunViews();
		}
	}

	void Lighting::setAmbientBottom(G3D::Color3 newAmbientBottom)
	{
		if (newAmbientBottom != ambientBottom)
		{
			ambientBottom = newAmbientBottom;
			raisePropertyChanged(desc_AmbientBottom);
			fireLightingChanged(false);
			RunService* runService = ServiceProvider::findServiceProvider(this)->find<RunService>();
			if (runService)
				runService->invalidateRunViews();
		}
	}

	void Lighting::setAmbientTop(G3D::Color3 newAmbientTop)
	{
		if (newAmbientTop != ambientTop)
		{
			ambientTop = newAmbientTop;
			raisePropertyChanged(desc_AmbientTop);
			fireLightingChanged(false);
			RunService* runService = ServiceProvider::findServiceProvider(this)->find<RunService>();
			if (runService)
				runService->invalidateRunViews();
		}
	}

	void Lighting::setGeographicLatitude(float newGeographicLatitude)
	{
		if (newGeographicLatitude != skyParameters.geoLatitude)
		{
			skyParameters.geoLatitude = newGeographicLatitude;
			raisePropertyChanged(prop_GeographicLatitude);
			fireLightingChanged(false);
			RunService* runService = ServiceProvider::findServiceProvider(this)->find<RunService>();
			if (runService)
				runService->invalidateRunViews();
		}
	}

	//43.30% matching.
	void Lighting::setTime(const boost::posix_time::time_duration& time)
	{
		if (timeOfDay != time)
		{
			timeOfDay = time;
			skyParameters.setTime(timeOfDay.total_seconds());
			raisePropertyChanged(prop_Time);
			fireLightingChanged(false);
			RunService* runService = ServiceProvider::findServiceProvider(this)->find<RunService>();
			if (runService)
				runService->invalidateRunViews();
		}
	}

	double Lighting::getMinutesAfterMidnight()
	{
		return timeOfDay.total_milliseconds() / 60000.0;
	}

	void Lighting::setMinutesAfterMidnight(double seconds)
	{
		setTime(boost::posix_time::time_duration(0, 0, seconds * 60));
	}

	void Lighting::onChildAdded(Instance* instance)
	{
		Sky* newSky = fastDynamicCast<Sky>(instance);

		if (newSky)
		{
			sky = shared_from(newSky);
			fireLightingChanged(true);
			RunService* runService = ServiceProvider::findServiceProvider(this)->find<RunService>();
			if (runService)
				runService->invalidateRunViews();
		}
	}

	void Lighting::onChildRemoving(Instance* instance)
	{
		if (sky.get() == instance)
		{
			sky.reset();
			fireLightingChanged(true);
			RunService* runService = ServiceProvider::findServiceProvider(this)->find<RunService>();
			if (runService)
				runService->invalidateRunViews();
		}
	}

	void Lighting::onChildChanged(Instance* instance, const PropertyChanged& propEvent)
	{
		Instance::onChildChanged(instance, propEvent);
		if (sky.get() == instance)
		{
			fireLightingChanged(true);
			RunService* runService = ServiceProvider::findServiceProvider(this)->find<RunService>();
			if (runService)
				runService->invalidateRunViews();
		}
	}

	bool Lighting::askAddChild(const Instance* instance) const
	{
		return fastDynamicCast<const Sky>(instance) != NULL;
	}
}