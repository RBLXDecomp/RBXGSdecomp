#pragma once
#include "v8tree/Service.h"
#include <GLG3D/LightingParameters.h>

namespace RBX
{
	class Sky;

	extern const char* sLighting;
	class Lighting : public DescribedCreatable<Lighting, Instance, &sLighting>,
					 public Service
	{
	private:
		G3D::LightingParameters skyParameters;
		G3D::Color3 ambientTop;
		G3D::Color3 ambientBottom;
		bool hasSky;
		G3D::Color4 clearColor;
		boost::posix_time::time_duration timeOfDay;
	public:
		boost::shared_ptr<Sky> sky;

	public:
		static Reflection::SignalDesc<Lighting, void(bool)> event_LightingChanged;

	public:
		//Lighting(const Lighting&);
		Lighting();
	public:
		void replaceSky(Sky*);
		bool isSkySuppressed() const;
		void suppressSky(const bool);
		const G3D::LightingParameters& getSkyParameters() const;
		G3D::Color4 getClearColor() const;
		void setClearColor(G3D::Color4);
		G3D::Color3 getLightColor() const;
		void setLightColor(G3D::Color3);
		G3D::Color3 getAmbientTop() const;
		void setAmbientTop(G3D::Color3);
		G3D::Color3 getAmbientBottom() const;
		void setAmbientBottom(G3D::Color3);
		std::string getTimeStr() const;
		void setTimeStr(const std::string&);
		void setTime(const boost::posix_time::time_duration&);
		double getGameTime() const;
		double getMinutesAfterMidnight();
		void setMinutesAfterMidnight(double);
		float getMoonPhase();
		G3D::Vector3 getMoonPosition();
		G3D::Vector3 getSunPosition();
		float getGeographicLatitude() const;
		void setGeographicLatitude(float);
		G3D::Color3 getClearColor3() const;
		void setClearColor3(G3D::Color3);
	protected:
		virtual void onChildAdded(Instance*);
		virtual void onChildRemoving(Instance*);
		virtual void onChildChanged(Instance*, const PropertyChanged&);
		virtual bool askAddChild(const Instance*) const;
	private:
		void fireLightingChanged(bool);
	public:
		virtual ~Lighting();
	public:
		//Lighting& operator=(const Lighting&);
	};
}
