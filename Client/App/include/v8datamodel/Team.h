#pragma once
#include "v8tree/Instance.h"
#include "v8datamodel/BrickColor.h"

namespace RBX
{
	extern const char* sTeam;
	class Team : public DescribedCreatable<Team, Instance, &sTeam>
	{
	protected:
		int score;
		BrickColor color;
		bool autoAssignable;
		bool autoColorCharacters;

	public:
		static Reflection::BoundProp<bool, 1> prop_AutoColorCharacters;
	  
	public:
		Team();
		virtual ~Team();
	public:
		virtual bool askSetParent(const Instance* instance) const
		{
			return true;
		}

		virtual bool askAddChild(const Instance* instance) const
		{
			return true;
		}

		int getScore() const;
		void setScore(int newScore);
		BrickColor getTeamColor() const;
		void setTeamColor(BrickColor newColor);
		bool getAutoAssignable() const;
		void setAutoAssignable(bool value);
	};
}
