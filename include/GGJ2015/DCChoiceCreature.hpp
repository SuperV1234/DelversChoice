#ifndef GGJ2015_DCCHOICECREATURE
#define GGJ2015_DCCHOICECREATURE

#include "../GGJ2015/DCCommon.hpp"

namespace ggj
{
	struct ChoiceCreature : public Choice
	{
		Creature creature;
		CreatureStatsDraw csd;
		sf::Sprite enemySprite;
		float hoverRads;

		inline ChoiceCreature(GameSession& mGameState, SizeT mIdx)
			: Choice{mGameState, mIdx}
		{
			enemySprite.setTexture(*getAssets().enemy);
			ssvs::setOrigin(enemySprite, ssvs::getLocalCenter);
			hoverRads = ssvu::getRndR(0.f, ssvu::tau);
		}

		inline void execute() override;
		inline void update(FT mFT) override { csd.update(mFT); }
		inline void draw(ssvs::GameWindow&, const Vec2f&, const Vec2f&) override;

		inline std::string getChoiceStr() override { return "Fight"; }
	};
}

#endif