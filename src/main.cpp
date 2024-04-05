#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/PauseLayer.hpp>
#include <Geode/modify/EffectGameObject.hpp>
#include <Geode/modify/CheckpointGameObject.hpp>

using namespace geode::prelude;

int mainLevels[] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,1001,1002,1003,2001,2002,2003,2004,2005,2006,2007,2008,2009,2010,3001,4001,4002,4003,5001,5002,5003,5004};
int coIndex = 0;
bool collected[3] = {false};
bool savedCollected[3]	= {false};

class $modify(MyCoinObject, EffectGameObject) {
	bool m_isCoin = false;
	int m_index = 0;

	static EffectGameObject* create(char const* p0) {
		auto object = EffectGameObject::create(p0);

		// secretCoin_01_001.png
		// secretCoin_2_01_001.png
		if (std::string(p0).starts_with("secretCoin_")) {
			as<MyCoinObject*>(object)->m_fields->m_isCoin = true;
			as<MyCoinObject*>(object)->m_fields->m_index = coIndex;
			coIndex++;
		}

		return object;
	}

	#ifndef GEODE_IS_ANDROID
	void triggerObject(GJBaseGameLayer* p0, int p1, gd::vector<int> const* p2) {
		EffectGameObject::triggerObject(p0, p1, p2);
		if (m_fields->m_isCoin) {
			collected[m_fields->m_index] = true;
			if (!p0->m_level->isPlatformer()) {
				savedCollected[m_fields->m_index] = true;
			}
		}
	}
	#endif
};

#ifndef GEODE_IS_ANDROID
class $modify(CheckpointGameObject) {
	void triggerObject(GJBaseGameLayer* p0, int p1, gd::vector<int> const* p2) {
		CheckpointGameObject::triggerObject(p0, p1, p2);

		if (p0->m_level->isPlatformer()) {
			for (int i = 0; i < 3; i++) savedCollected[i] = collected[i];
		}
	}
};
#endif


class $modify(PlayLayer) {
	bool init(GJGameLevel* level, bool useReplay, bool dontCreateObjects) {
		coIndex = 0;
		for (int i = 0; i < 3; i++) {
			savedCollected[i] = false;
			collected[i] = false;
		}
		return PlayLayer::init(level, useReplay, dontCreateObjects);
	}

	void resetLevel() {
		PlayLayer::resetLevel();

		if (!this->m_level->isPlatformer())
			for (int i = 0; i < 3; i++) savedCollected[i] = false;

		for (int i = 0; i < 3; i++) collected[i] = savedCollected[i];

	}

	void fullReset() {
		PlayLayer::fullReset();
		
		for (int i = 0; i < 3; i++) {
			savedCollected[i] = false;
			collected[i] = false;
		}
	}

	void resumeAndRestart(bool fullReset) {
		PlayLayer::resumeAndRestart(fullReset);

		if (fullReset) {
			for (int i = 0; i < 3; i++) {
				savedCollected[i] = false;
				collected[i] = false;
			}
		}
	}

	void togglePracticeMode(bool isPractice) {
		PlayLayer::togglePracticeMode(isPractice);

		if (!isPractice) {
			for (int i = 0; i < 3; i++) {
				savedCollected[i] = false;
				collected[i] = false;
			}
		}
	}
};

class $modify(PauseLayer) {
	static void onModify(auto& self) {
        self.setHookPriority("PauseLayer::create", -101); // cuz another fucking mod uses -100
    }

	static PauseLayer* create(bool p0) {
		auto pl = PauseLayer::create(p0);
		auto menu = pl->getChildByID("bottom-button-menu");

		auto winSize = CCDirector::get()->getWinSize();
		auto level = GameManager::sharedState()->getPlayLayer()->m_level;

		if (pl->getChildByID("better-pause-node")) {
			menu->setVisible(false);
		} else if (pl->getChildByID("classic-pause-node")) {
			menu = CCMenu::create();
			menu->setAnchorPoint({0.5f, 0});
			menu->setLayout(
				ColumnLayout::create()
					->setGap(5.f)
					->setAutoScale(false)
					->setAxisAlignment(AxisAlignment::Start)
					->setCrossAxisAlignment(AxisAlignment::Center)
					->setCrossAxisLineAlignment(AxisAlignment::Center)
					->setCrossAxisOverflow(true));
			menu->setContentSize({30.f, 100.f});
			pl->getChildByID("left-button-menu")->addChild(menu);
			
			menu->updateLayout();
			pl->getChildByID("left-button-menu")->updateLayout();
		} else {
			menu->setVisible(true);
			menu->setLayout(
				RowLayout::create()
					->setGap(10.f)
					->setAutoScale(false)
					->setAxisAlignment(AxisAlignment::Center)
					->setCrossAxisAlignment(AxisAlignment::Center)
					->setCrossAxisLineAlignment(AxisAlignment::Center)
			);
		}

		auto levelIDInt =  level->m_levelID.value();
		auto levelID = std::to_string(levelIDInt);
		auto levelIDKey = std::string(level->getCoinKey(levelIDInt));
		auto areCoinsVerified = level->m_coinsVerified.value();
		std::string weeklySuffix = "";
		if (levelIDKey.size() != 2*levelID.size() + 1) weeklySuffix = levelIDKey.substr(2*levelID.size() + 1, 7);

		// check if level is an official level
		if (level->m_levelType == GJLevelType::Local || (std::find(mainLevels, mainLevels + sizeof(mainLevels)/sizeof(mainLevels[0]), levelIDInt) != mainLevels + sizeof(mainLevels)/sizeof(mainLevels[0]))) {

			CCArrayExt<CCSprite*> coinSlots = CCArray::create();
			CCArrayExt<CCSprite*> coins = CCArray::create();
			for (int i = 0; i < 3; i++) {
				auto coinSlot = CCSprite::createWithSpriteFrameName("secretCoin_b_01_001.png");
				coinSlot->setTag(i + 1001);
				coinSlot->setScale(0.6);
				coinSlots.push_back(coinSlot);

				auto coin = CCSprite::createWithSpriteFrameName("secretCoinUI_001.png");
				coin->setTag(i + 1);
				coin->setScale(0.6);
				coins.push_back(coin);
			}

			for (int i = 0; i < 3; i++) {
				if (level->m_coins > i) {
					if (GameStatsManager::sharedState()->hasSecretCoin((levelID + "_" + std::to_string(i + 1)).c_str())) {
						menu->addChild(coins[i]);
					} else {
						menu->addChild(coinSlots[i]);
						if (collected[i]) {
							coinSlots[i]->addChild(coins[i]);
							coins[i]->setScale(1);
							coins[i]->setPosition({ coinSlots[i]->getContentWidth()/2, coinSlots[i]->getContentHeight()/2 });
							auto actions = CCArray::create();
							actions->addObject(CCFadeOut::create(1.f));
							actions->addObject(CCFadeIn::create(1.f));
							coins[i]->runAction(CCRepeatForever::create(CCSequence::create(actions)));
						}
					}
				} else break;
			}

		} else {
			// silver coin: verified
			// bronze coin: pending
			auto verifiedCoins = GameStatsManager::sharedState()->m_verifiedUserCoins;
			auto pendingCoins = GameStatsManager::sharedState()->m_pendingUserCoins;

			CCArrayExt<CCSprite*> coinSlots = CCArray::create();
			CCArrayExt<CCSprite*> coins = CCArray::create();
			for (int i = 0; i < 3; i++) {
				auto coinSlot = CCSprite::createWithSpriteFrameName("secretCoin_2_b_01_001.png");
				coinSlot->setTag(i + 1001);
				coinSlot->setScale(0.6);
				coinSlots.push_back(coinSlot);

				auto coin = CCSprite::createWithSpriteFrameName("secretCoinUI2_001.png");
				coin->setTag(i + 1);
				coin->setScale(0.6);
				coins.push_back(coin);
			}

			bool balls[3] = {false};
			// this is a fat middle finger for you and specifically you, 230v
			for (int i = 0; i < 3; i++) {
				if (level->m_coins > i) {
					if (areCoinsVerified) {
						if (verifiedCoins->objectForKey(levelID + "_" + std::to_string(i + 1) + weeklySuffix)) balls[i] = true;
					} else if (levelIDInt == 0) {
						if (i == 0) {
							if (level->m_firstCoinVerified.value()) balls[i] = true;
						} else if (i == 1) {
							if (level->m_secondCoinVerified.value()) balls[i] = true;
						} else if (i == 2) {
							if (level->m_thirdCoinVerified.value()) balls[i] = true;
						}
					} else {
						if (pendingCoins->objectForKey(levelID + "_" + std::to_string(i + 1) + weeklySuffix)) balls[i] = true;
						coinSlots[i]->setColor({255, 175, 75});
						coins[i]->setColor({255, 175, 75});
					}
				} else break;
			}

			for (int i = 0; i < 3; i++) {
				if (level->m_coins > i) {
					if (balls[i]) {
						menu->addChild(coins[i]);
					} else {
						menu->addChild(coinSlots[i]);
						if (collected[i]) {
							coinSlots[i]->addChild(coins[i]);
							coins[i]->setScale(1);
							coins[i]->setPosition({ coinSlots[i]->getContentWidth()/2, coinSlots[i]->getContentHeight()/2 });
							auto actions = CCArray::create();
							actions->addObject(CCFadeOut::create(1.f));
							actions->addObject(CCFadeIn::create(1.f));
							coins[i]->runAction(CCRepeatForever::create(CCSequence::create(actions)));
						}
					}
				} else break;
			}
		}

		menu->updateLayout();

		// hidden feature :D
		// cleanup right-button-menu and left-button-menu cuz theres too many buttons there now man
		// and i dont really think its necessary to make a separate mod for just this
		// this mod is pretty popular anyway
		if (!pl->getChildByID("better-pause-node")) {
			auto rLayout = ColumnLayout::create()
						->setGap(5.f)
						->setAxisReverse(true)
						->setGrowCrossAxis(true)
						->setCrossAxisOverflow(true)
						->setAxisAlignment(AxisAlignment::End)
						->setCrossAxisAlignment(AxisAlignment::Center)
						->setCrossAxisLineAlignment(AxisAlignment::Center);
			auto lLayout = ColumnLayout::create()
						->setGap(4.f)
						->setCrossAxisReverse(true)
						->setGrowCrossAxis(true)
						->setCrossAxisOverflow(true)
						->setAxisAlignment(AxisAlignment::Start)
						->setCrossAxisAlignment(AxisAlignment::Center)
						->setCrossAxisLineAlignment(AxisAlignment::Center);


			auto rMenu = pl->getChildByID("right-button-menu");
			rMenu->setAnchorPoint({1, 1});
			rMenu->setPositionX(rMenu->getPositionX() + rMenu->getContentWidth()/2);
			rMenu->setPositionY(rMenu->getPositionY() + rMenu->getContentHeight()/2);
			if (pl->getChildByID("dankmeme.globed2/playerlist-menu")) {
				rMenu->setContentHeight(rMenu->getContentHeight() - 55.f);
			}
			rMenu->setLayout(rLayout);
			rMenu->updateLayout();

			auto lMenu = pl->getChildByID("left-button-menu");
			lMenu->setAnchorPoint({0, 0.5});
			lMenu->setPositionX(lMenu->getPositionX() - lMenu->getContentWidth()/2);
			lMenu->setLayout(lLayout);
			lMenu->updateLayout();
		}

		return pl;
	}
};

#ifdef GEODE_IS_ANDROID
void EffectGameObject_triggerObject(MyCoinObject* self, GJBaseGameLayer* p0, int p1, gd::vector<int> const* p2) {
		self->triggerObject(p0, p1, p2);

		if (self->m_fields->m_isCoin) {
			collected[self->m_fields->m_index] = true;
			if (!p0->m_level->isPlatformer()) {
				savedCollected[self->m_fields->m_index] = true;
			}
		}
	}

void CheckpointGameObject_triggerObject(CheckpointGameObject* self, GJBaseGameLayer* p0, int p1, gd::vector<int> const* p2) {
	self->triggerObject(p0, p1, p2);

	if (p0->m_level->isPlatformer()) {
		for (int i = 0; i < 3; i++) savedCollected[i] = collected[i];
	}
}


$execute {
    Mod::get()->hook(
        dlsym(dlopen("libcocos2dcpp.so", RTLD_NOW), "_ZN16EffectGameObject13triggerObjectEP15GJBaseGameLayeriPKSt6vectorIiSaIiEE"),
        &EffectGameObject_triggerObject,
        "EffectGameObject::triggerObject",
        tulip::hook::TulipConvention::Default
    );
	Mod::get()->hook(
        dlsym(dlopen("libcocos2dcpp.so", RTLD_NOW), "_ZN20CheckpointGameObject13triggerObjectEP15GJBaseGameLayeriPKSt6vectorIiSaIiEE"),
        &CheckpointGameObject_triggerObject,
        "CheckpointGameObject::triggerObject",
        tulip::hook::TulipConvention::Default
    );
}
#endif