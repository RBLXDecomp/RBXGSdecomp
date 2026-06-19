#include "v8datamodel/CharacterAppearance.h"
#include "v8datamodel/PartInstance.h"
#include "v8datamodel/Decal.h"

namespace RBX
{
	void CharacterAppearance::onAncestorChanged(const AncestorChanged& event)
	{
		if (event.child == this && Network::Players::backendProcessing(this, false))
		{
			Humanoid* humanoid = Humanoid::modelIsCharacter(getParent());
			if (humanoid)
				apply(humanoid);
		}
	}

	bool CharacterAppearance::askSetParent(const Instance* instance) const
	{
		return fastDynamicCast<const ModelInstance>(instance) != NULL;
	}

	ShirtGraphic::ShirtGraphic()
	{
		setName("Shirt Graphic");
	}
	
	void ShirtGraphic::apply(Humanoid* humanoid)
	{
		PartInstance* torso = humanoid->getTorso();
		if (torso)
		{
			Decal* decal = torso->findFirstChildOfType<Decal>();
			if (decal)
				decal->setTexture(graphic);
		}
	}

	Shirt::Shirt()
		: leftSleeveColor(BrickColor::lego_23),
		  rightSleeveColor(BrickColor::lego_23),
		  torsoColor(BrickColor::lego_1)
	{
		setName("Shirt");
	}

	void Shirt::apply(Humanoid* humanoid)
	{
		ShirtGraphic::apply(humanoid);

		PartInstance* torso = humanoid->getTorso();
		if (torso)
			torso->setColor(torsoColor);

		PartInstance* leftArm = humanoid->getLeftArm();
		if (leftArm)
			leftArm->setColor(leftSleeveColor);

		PartInstance* rightArm = humanoid->getRightArm();
		if (rightArm)
			rightArm->setColor(rightSleeveColor);
	}

	Skin::Skin()
		: skinColor(BrickColor::lego_226)
	{
		setName("Skin");
	}

	void Skin::apply(Humanoid* humanoid)
	{
		bool hasShirt = getParent()->findFirstChildOfType<Shirt>() != NULL;

		PartInstance* head = humanoid->getHead();
		if (head)
			head->setColor(skinColor);

		PartInstance* leftLeg = humanoid->getLeftLeg();
		if (leftLeg)
			leftLeg->setColor(skinColor);

		PartInstance* rightLeg = humanoid->getRightLeg();
		if (rightLeg)
			rightLeg->setColor(skinColor);

		if (!hasShirt)
		{
			PartInstance* torso = humanoid->getTorso();
			if (torso)
				torso->setColor(skinColor);

			PartInstance* leftArm = humanoid->getLeftArm();
			if (leftArm)
				leftArm->setColor(skinColor);

			PartInstance* rightArm = humanoid->getRightArm();
			if (rightArm)
				rightArm->setColor(skinColor);
		}
	}

	BodyColors::BodyColors()
		: headColor(BrickColor::lego_226),
		  leftArmColor(BrickColor::lego_226),
		  rightArmColor(BrickColor::lego_226),
		  torsoColor(BrickColor::lego_28),
		  leftLegColor(BrickColor::lego_23),
		  rightLegColor(BrickColor::lego_23)
	{
		setName("Body Colors");
	}

	void BodyColors::apply(Humanoid* humanoid)
	{
		Skin* skin = getParent()->findFirstChildOfType<Skin>();

		if (!skin)
		{
			bool hasShirt = getParent()->findFirstChildOfType<Shirt>() != NULL;

			PartInstance* head = humanoid->getHead();
			if (head)
				head->setColor(headColor);

			PartInstance* leftLeg = humanoid->getLeftLeg();
			if (leftLeg)
				leftLeg->setColor(leftLegColor);

			PartInstance* rightLeg = humanoid->getRightLeg();
			if (rightLeg)
				rightLeg->setColor(rightLegColor);

			if (!hasShirt)
			{
				PartInstance* torso = humanoid->getTorso();
				if (torso)
					torso->setColor(torsoColor);

				PartInstance* leftArm = humanoid->getLeftArm();
				if (leftArm)
					leftArm->setColor(leftArmColor);

				PartInstance* rightArm = humanoid->getRightArm();
				if (rightArm)
					rightArm->setColor(rightArmColor);
			}
		}
	}
}
