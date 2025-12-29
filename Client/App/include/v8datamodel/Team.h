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
		//Team(const Team&);
		Team();
		virtual ~Team();
	public:
		virtual bool askSetParent(const Instance*) const;
		virtual bool askAddChild(const Instance*) const;
		int getScore() const;
		void setScore(int);
		BrickColor getTeamColor() const;
		void setTeamColor(BrickColor);
		bool getAutoAssignable() const;
		void setAutoAssignable(bool);
	public:
		//Team& operator=(const Team&);
	};
}
