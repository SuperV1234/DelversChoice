#ifndef GGJ2015_DCSTATRICHTEXT
#define GGJ2015_DCSTATRICHTEXT

#include "../GGJ2015/DCCommon.hpp"

namespace ggj
{
	struct StatRichText
	{
		Exp::BitmapTextRich txt{*getAssets().fontObStroked};

		Exp::BS::Chunk* pssExtra;
		Exp::BS::Chunk* psTotal;
		Exp::BS::Chunk* psBase;
		Exp::BS::Chunk* psBonus;
		Exp::BS::ColorFG* clfgBonus;

		inline StatRichText()
		{
			sfc gray{100, 100, 100, 255};

			txt.eff<Exp::BS::Tracking>(-3).eff(sfc::White).in(psTotal, "")
				.in(pssExtra, "")
					.eff(gray).in(" (").eff(sfc::White).in(psBase, "").eff(gray).in("+").eff<Exp::BS::ColorFG>(clfgBonus, sfc::Green)
						.in(psBonus, "").eff(gray).in(")");

			clfgBonus->setAnimPulse(0.05f, 100);
		}

		inline void update(FT mFT) { txt.update(mFT); }

		inline void set(StatType mX)
		{
			pssExtra->setEnabled(false);

			auto s(ssvu::toStr(mX));
			psTotal->setStr(s);
		}

		inline void set(StatType mBase, StatType mBonus)
		{
			if(mBonus == 0)
			{
				set(mBase);
				return;
			}

			pssExtra->setEnabled(true);

			auto sBase(ssvu::toStr(mBase));
			auto sBonus(ssvu::toStr(mBonus));
			auto sTotal(ssvu::toStr(mBase + mBonus));

			psTotal->setStr(sTotal);
			psBase->setStr(sBase);
			psBonus->setStr(sBonus);
		}
	};
}

#endif
