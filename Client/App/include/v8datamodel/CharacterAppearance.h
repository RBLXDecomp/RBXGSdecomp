#include "humanoid/Humanoid.h"
#include "util/TextureId.h"
#include "v8datamodel/BrickColor.h"
#include "Network/Players.h"

namespace RBX
{
	class CharacterAppearance : public Instance
	{
	private:
		virtual void apply(Humanoid* humanoid) = 0;
	public:
		void apply();
	protected:
		virtual void onAncestorChanged(const AncestorChanged& event);
		virtual bool askSetParent(const Instance* instance) const;
	};

	extern const char* sBodyColors;
	class BodyColors : public DescribedCreatable<BodyColors, CharacterAppearance, &sBodyColors>
	{
	private:
		BrickColor headColor;
		BrickColor leftArmColor;
		BrickColor rightArmColor;
		BrickColor torsoColor;
		BrickColor leftLegColor;
		BrickColor rightLegColor;

	public:
		static Reflection::BoundProp<BrickColor, 1> prop_HeadColor;
		static Reflection::BoundProp<BrickColor, 1> prop_LeftArmColor;
		static Reflection::BoundProp<BrickColor, 1> prop_RightArmColor;
		static Reflection::BoundProp<BrickColor, 1> prop_TorsoColor;
		static Reflection::BoundProp<BrickColor, 1> prop_LeftLegColor;
		static Reflection::BoundProp<BrickColor, 1> prop_RightLegColor;

	public:
		BodyColors();
	private:
		virtual void apply(Humanoid* humanoid);

		void dataChanged(const Reflection::PropertyDescriptor&)
		{
			if (Network::Players::backendProcessing(this, false))
			{
				Humanoid* humanoid = Humanoid::modelIsCharacter(getParent());
				if (humanoid)
					apply(humanoid);
			}
		}
	};

	extern const char* sShirtGraphic;
	class ShirtGraphic : public DescribedCreatable<ShirtGraphic, CharacterAppearance, &sShirtGraphic>
	{
	private:
		TextureId graphic;
	public:
		static Reflection::BoundProp<TextureId, 1> prop_Graphic;

	public:
		ShirtGraphic();
	protected:
		virtual void apply(Humanoid* humanoid);
	private:
		void dataChanged(const Reflection::PropertyDescriptor&)
		{
			if (Network::Players::backendProcessing(this, false))
			{
				Humanoid* humanoid = Humanoid::modelIsCharacter(getParent());
				if (humanoid)
					apply(humanoid);
			}
		}
	};

	extern const char* sShirt;
	class Shirt : public DescribedCreatable<Shirt, ShirtGraphic, &sShirt>
	{
	private:
		BrickColor leftSleeveColor;
		BrickColor rightSleeveColor;
		BrickColor torsoColor;
	public:
		static Reflection::BoundProp<BrickColor, 1> prop_leftSleeveColor;
		static Reflection::BoundProp<BrickColor, 1> prop_rightSleeveColor;
		static Reflection::BoundProp<BrickColor, 1> prop_torsoColor;

	public:
		Shirt();
	private:
		virtual void apply(Humanoid* humanoid);

		void dataChanged(const Reflection::PropertyDescriptor&)
		{
			if (Network::Players::backendProcessing(this, false))
			{
				Humanoid* humanoid = Humanoid::modelIsCharacter(getParent());
				if (humanoid)
					apply(humanoid);
			}
		}
	};

	extern const char* sSkin;
	class Skin : public DescribedCreatable<Skin, CharacterAppearance, &sSkin>
	{
	private:
		BrickColor skinColor;
	public:
		static Reflection::BoundProp<BrickColor, 1> prop_skinColor;

	public:
		Skin();
	private:
		virtual void apply(Humanoid* humanoid);

		void dataChanged(const Reflection::PropertyDescriptor&)
		{
			if (Network::Players::backendProcessing(this, false))
			{
				Humanoid* humanoid = Humanoid::modelIsCharacter(getParent());
				if (humanoid)
					apply(humanoid);
			}
		}
	};
}
