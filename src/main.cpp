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

		// secretCoin_01_001
		// secretCoin_2_01_001
		if (std::string(p0).starts_with("secretCoin_")) {
			as<MyCoinObject*>(object)->m_fields->m_isCoin = true;
			as<MyCoinObject*>(object)->m_fields->m_index = coIndex;
			coIndex++;
		}

		return object;
	}

	#ifndef GEODE_IS_ANDROID
	bool triggerObject(GJBaseGameLayer* p0, int p1, gd::vector<int> const* p2) {
		if (!EffectGameObject::triggerObject(p0, p1, p2)) return false;
		if (m_fields->m_isCoin) {
			collected[m_fields->m_index] = true;
			if (!p0->m_level->isPlatformer()) {
				savedCollected[m_fields->m_index] = true;
			}
		}
		return true;
	}
	#endif
};

#ifndef GEODE_IS_ANDROID
class $modify(CheckpointGameObject) {
	bool triggerObject(GJBaseGameLayer* p0, int p1, gd::vector<int> const* p2) {
		if (!CheckpointGameObject::triggerObject(p0, p1, p2)) return false;

		if (p0->m_level->isPlatformer()) {
			for (int i = 0; i < 3; i++) savedCollected[i] = collected[i];
		}

		return true;
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

	void resetLevelFromStart() {
		PlayLayer::resetLevelFromStart();
		
		for (int i = 0; i < 3; i++) {
			savedCollected[i] = false;
			collected[i] = false;
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
			menu->setVisible(true);
			menu->setAnchorPoint({0.5f, 0});
			// idk either i just bruteforced a solution
			// cuz setting a layout with allow cross axis overflow fucks up the content size
			auto layout = ColumnLayout::create()
					->setGap(10.f)
					->setAutoScale(false)
					->setAxisAlignment(AxisAlignment::Start)
					->setCrossAxisAlignment(AxisAlignment::Center)
					->setCrossAxisLineAlignment(AxisAlignment::Center)
					->setCrossAxisOverflow(false);
			menu->setLayout(layout);
			menu->setContentSize({menu->getContentSize().height, menu->getContentSize().width});
			menu->setLayout(layout->setCrossAxisOverflow(true));
		} else if (pl->getChildByID("classic-pause-node")) {
			menu = CCMenu::create();
			menu->setAnchorPoint({0.5f, 0});
			auto layout = ColumnLayout::create()
					->setGap(5.f)
					->setAutoScale(false)
					->setAxisAlignment(AxisAlignment::Start)
					->setCrossAxisAlignment(AxisAlignment::Center)
					->setCrossAxisLineAlignment(AxisAlignment::Center)
					->setCrossAxisOverflow(true);
			menu->setLayout(layout);
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
	
		auto fadeAction1 = CCArray::create();
		fadeAction1->addObject(CCFadeOut::create(1.f));
		fadeAction1->addObject(CCFadeIn::create(1.f));
		auto fadeAction2 = CCArray::create();
		fadeAction2->addObject(CCFadeOut::create(1.f));
		fadeAction2->addObject(CCFadeIn::create(1.f));
		auto fadeAction3 = CCArray::create();
		fadeAction3->addObject(CCFadeOut::create(1.f));
		fadeAction3->addObject(CCFadeIn::create(1.f));

		// check if level is an official level
		if (level->m_levelType == GJLevelType::Local || (std::find(mainLevels, mainLevels + sizeof(mainLevels)/sizeof(mainLevels[0]), levelIDInt) != mainLevels + sizeof(mainLevels)/sizeof(mainLevels[0]))) {

			auto secretCoin1Slot = CCSprite::createWithSpriteFrameName("secretCoin_b_01_001.png");
			secretCoin1Slot->setTag(1);
			secretCoin1Slot->setScale(0.6);

			auto secretCoin2Slot = CCSprite::createWithSpriteFrameName("secretCoin_b_01_001.png");
			secretCoin2Slot->setTag(2);
			secretCoin2Slot->setScale(0.6);

			auto secretCoin3Slot = CCSprite::createWithSpriteFrameName("secretCoin_b_01_001.png");
			secretCoin3Slot->setTag(3);
			secretCoin3Slot->setScale(0.6);

			auto secretCoin1 = CCSprite::createWithSpriteFrameName("secretCoinUI_001.png");
			secretCoin1->setTag(1001);
			secretCoin1->setScale(0.6);

			auto secretCoin2 = CCSprite::createWithSpriteFrameName("secretCoinUI_001.png");
			secretCoin2->setTag(1002);
			secretCoin2->setScale(0.6);

			auto secretCoin3 = CCSprite::createWithSpriteFrameName("secretCoinUI_001.png");
			secretCoin3->setTag(1003);
			secretCoin3->setScale(0.6);

			if (level->m_coins >= 1) {
				if (GameStatsManager::sharedState()->hasSecretCoin((levelID + "_1").c_str())) menu->addChild(secretCoin1);
				else {
					menu->addChild(secretCoin1Slot);
					if (collected[0]) {
						secretCoin1Slot->addChild(secretCoin1);
						secretCoin1->setScale(1);
						secretCoin1->setPosition({ secretCoin1Slot->getContentWidth()/2, secretCoin1Slot->getContentHeight()/2 });
						secretCoin1->runAction(CCRepeatForever::create(CCSequence::create(fadeAction1)));
					}
				}
			}
			if (level->m_coins >= 2) {
				if (GameStatsManager::sharedState()->hasSecretCoin((levelID + "_2").c_str())) menu->addChild(secretCoin2);
				else {
					menu->addChild(secretCoin2Slot);
					if (collected[1]) {
						secretCoin2Slot->addChild(secretCoin2);
						secretCoin2->setScale(1);
						secretCoin2->setPosition({ secretCoin2Slot->getContentWidth()/2, secretCoin2Slot->getContentHeight()/2 });
						secretCoin2->runAction(CCRepeatForever::create(CCSequence::create(fadeAction2)));
					}
				}
			}
			if (level->m_coins >= 3) {
				if (GameStatsManager::sharedState()->hasSecretCoin((levelID + "_3").c_str())) menu->addChild(secretCoin3);
				else {
					menu->addChild(secretCoin3Slot);
					if (collected[2]) {
						secretCoin3Slot->addChild(secretCoin3);
						secretCoin3->setScale(1);
						secretCoin3->setPosition({ secretCoin3Slot->getContentWidth()/2, secretCoin3Slot->getContentHeight()/2 });
						secretCoin3->runAction(CCRepeatForever::create(CCSequence::create(fadeAction3)));
					}
				}
			}
			

		} else {
			// silver coin: verified
			// bronze coin: pending
			auto verifiedCoins = GameStatsManager::sharedState()->m_verifiedUserCoins;
			auto pendingCoins = GameStatsManager::sharedState()->m_pendingUserCoins;

			// coin sprites
			auto coin1Slot = CCSprite::createWithSpriteFrameName("secretCoin_2_b_01_001.png");
			coin1Slot->setScale(0.6);

			auto coin2Slot = CCSprite::createWithSpriteFrameName("secretCoin_2_b_01_001.png");
			coin2Slot->setScale(0.6);

			auto coin3Slot = CCSprite::createWithSpriteFrameName("secretCoin_2_b_01_001.png");
			coin3Slot->setScale(0.6);

			auto coin1 = CCSprite::createWithSpriteFrameName("secretCoinUI2_001.png");
			coin1->setScale(0.6);

			auto coin2 = CCSprite::createWithSpriteFrameName("secretCoinUI2_001.png");
			coin2->setScale(0.6);

			auto coin3 = CCSprite::createWithSpriteFrameName("secretCoinUI2_001.png");
			coin3->setScale(0.6);

			// silver coin in levels on the server
			if (areCoinsVerified) {
				if (level->m_coins >= 1) {
					if (verifiedCoins->objectForKey(levelID + "_1" + weeklySuffix)) menu->addChild(coin1);
					else {
						menu->addChild(coin1Slot);
						if (collected[0]) {
							coin1Slot->addChild(coin1);
							coin1->setScale(1);
							coin1->setPosition({ coin1Slot->getContentWidth()/2, coin1Slot->getContentHeight()/2 });
							coin1->runAction(CCRepeatForever::create(CCSequence::create(fadeAction1)));
						}
					}
				}
				if (level->m_coins >= 2) {
					if (verifiedCoins->objectForKey(levelID + "_2" + weeklySuffix)) menu->addChild(coin2);
					else {
						menu->addChild(coin2Slot);
						if (collected[1]) {
							coin2Slot->addChild(coin2);
							coin2->setScale(1);
							coin2->setPosition({ coin2Slot->getContentWidth()/2, coin2Slot->getContentHeight()/2 });
							coin2->runAction(CCRepeatForever::create(CCSequence::create(fadeAction2)));
						}
					}
				}
				if (level->m_coins >= 3) {
					if (verifiedCoins->objectForKey(levelID + "_3" + weeklySuffix)) menu->addChild(coin3);
					else {
						menu->addChild(coin3Slot);
						if (collected[2]) {
							coin3Slot->addChild(coin3);
							coin3->setScale(1);
							coin3->setPosition({ coin3Slot->getContentWidth()/2, coin3Slot->getContentHeight()/2 });
							coin3->runAction(CCRepeatForever::create(CCSequence::create(fadeAction3)));
						}
					}
				}
			// silver coin in user created levels that havent been uploaded
			// hence the level id 0
			} else if (levelIDInt == 0) {
				if (level->m_coins >= 1) {
					if (level->m_firstCoinVerified.value()) menu->addChild(coin1);
					else {
						menu->addChild(coin1Slot);
						if (collected[0]) {
							coin1Slot->addChild(coin1);
							coin1->setScale(1);
							coin1->setPosition({ coin1Slot->getContentWidth()/2, coin1Slot->getContentHeight()/2 });
							coin1->runAction(CCRepeatForever::create(CCSequence::create(fadeAction1)));
						}
					}
				}
				if (level->m_coins >= 2) {
					if (level->m_secondCoinVerified.value()) menu->addChild(coin2);
					else {
						menu->addChild(coin2Slot);
						if (collected[1]) {
							coin2Slot->addChild(coin2);
							coin2->setScale(1);
							coin2->setPosition({ coin2Slot->getContentWidth()/2, coin2Slot->getContentHeight()/2 });
							coin2->runAction(CCRepeatForever::create(CCSequence::create(fadeAction2)));
						}
					}
				}
				if (level->m_coins >= 3) {
					if (level->m_thirdCoinVerified.value()) menu->addChild(coin3);
					else {
						menu->addChild(coin3Slot);
						if (collected[2]) {
							coin3Slot->addChild(coin3);
							coin3->setScale(1);
							coin3->setPosition({ coin3Slot->getContentWidth()/2, coin3Slot->getContentHeight()/2 });
							coin3->runAction(CCRepeatForever::create(CCSequence::create(fadeAction3)));
						}
					}
				}
			// bronze coin
			} else {
				if (level->m_coins >= 1) {
					if (pendingCoins->objectForKey(levelID + "_1" + weeklySuffix)) menu->addChild(coin1);
					else {
						menu->addChild(coin1Slot);
						if (collected[0]) {
							coin1Slot->addChild(coin1);
							coin1->setScale(1);
							coin1->setPosition({ coin1Slot->getContentWidth()/2, coin1Slot->getContentHeight()/2 });
							coin1->runAction(CCRepeatForever::create(CCSequence::create(fadeAction1)));
						}
					}
				}
				if (level->m_coins >= 2) {
					if (pendingCoins->objectForKey(levelID + "_2" + weeklySuffix)) menu->addChild(coin2);
					else {
						menu->addChild(coin2Slot);
						if (collected[1]) {
							coin2Slot->addChild(coin2);
							coin2->setScale(1);
							coin2->setPosition({ coin2Slot->getContentWidth()/2, coin2Slot->getContentHeight()/2 });
							coin2->runAction(CCRepeatForever::create(CCSequence::create(fadeAction2)));
						}
					}
				}
				if (level->m_coins >= 3) {
					if (pendingCoins->objectForKey(levelID + "_3" + weeklySuffix)) menu->addChild(coin3);
					else {
						menu->addChild(coin3Slot);
						if (collected[2]) {
							coin3Slot->addChild(coin3);
							coin3->setScale(1);
							coin3->setPosition({ coin3Slot->getContentWidth()/2, coin3Slot->getContentHeight()/2 });
							coin3->runAction(CCRepeatForever::create(CCSequence::create(fadeAction3)));
						}
					}
				}

				coin1Slot->setColor({255, 175, 75});
				coin2Slot->setColor({255, 175, 75});
				coin3Slot->setColor({255, 175, 75});
				coin1->setColor({255, 175, 75});
				coin2->setColor({255, 175, 75});
				coin3->setColor({255, 175, 75});
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
bool EffectGameObject_triggerObject(MyCoinObject* self, GJBaseGameLayer* p0, int p1, gd::vector<int> const* p2) {
		if (!self->triggerObject(p0, p1, p2)) return false;

		if (self->m_fields->m_isCoin) {
			collected[self->m_fields->m_index] = true;
			if (!p0->m_level->isPlatformer()) {
				savedCollected[self->m_fields->m_index] = true;
			}
		}
		
		return true;
	}

bool CheckpointGameObject_triggerObject(CheckpointGameObject* self, GJBaseGameLayer* p0, int p1, gd::vector<int> const* p2) {
	if (!self->triggerObject(p0, p1, p2)) return false;

	if (p0->m_level->isPlatformer()) {
		for (int i = 0; i < 3; i++) savedCollected[i] = collected[i];
	}

	return true;
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