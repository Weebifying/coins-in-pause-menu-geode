#include <Geode/Geode.hpp>
#include <Geode/modify/PauseLayer.hpp>

using namespace geode::prelude;

int mainLevels[] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,1001,1002,1003,2001,2002,2003,2004,2005,2006,2007,2008,2009,2010,3001,4001,4002,4003,5001,5002,5003,5004};

class $modify(PauseLayer) {
	static void onModify(auto& self) {
        self.setHookPriority("PauseLayer::create", -100);
    }

	static PauseLayer* create(bool p0) {
		auto pl = PauseLayer::create(p0);
		auto menu = pl->getChildByID("coins-menu");

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
		} else {
			menu->setLayout(
				RowLayout::create()
					->setGap(10.f)
					->setAutoScale(false)
					->setAxisAlignment(AxisAlignment::Center)
					->setCrossAxisAlignment(AxisAlignment::Center)
					->setCrossAxisLineAlignment(AxisAlignment::Center)
			);
		}

		return pl;
	}

	TodoReturn customSetup() {
		PauseLayer::customSetup();

		auto winSize = CCDirector::get()->getWinSize();
		auto level = GameManager::sharedState()->getPlayLayer()->m_level;

		auto levelIDInt =  level->m_levelID.value();
		auto levelID = std::to_string(levelIDInt);
		auto levelIDKey = std::string(level->getCoinKey(levelIDInt));
		auto areCoinsVerified = level->m_coinsVerified.value();
		std::string weeklySuffix = "";
		if (levelIDKey.size() != 2*levelID.size() + 1) weeklySuffix = levelIDKey.substr(2*levelID.size() + 1, 7);
	
		auto bottom_menu = this->getChildByID("bottom-button-menu");
		auto menu = CCMenu::create();
		this->addChild(menu);
		menu->setPosition(bottom_menu->getPosition());
		menu->setContentSize(bottom_menu->getContentSize());
		menu->setZOrder(bottom_menu->getZOrder());
		menu->setID("coins-menu");

		
		// check if level is a main level
		// NOT accurate since level id 2004 still exists on the server but idgaf :D
		if (std::find(mainLevels, mainLevels + sizeof(mainLevels)/sizeof(mainLevels[0]), levelIDInt) != mainLevels + sizeof(mainLevels)/sizeof(mainLevels[0])) {
			auto amountSecretCoinsCollected = GameStatsManager::sharedState()->getCollectedCoinsForLevel(level);

			// i will make secret coins work properly later
			auto secretCoin1Slot = CCSprite::createWithSpriteFrameName("secretCoin_b_01_001.png");
			secretCoin1Slot->setScale(0.6);
			auto secretCoin2Slot = CCSprite::createWithSpriteFrameName("secretCoin_b_01_001.png");
			secretCoin2Slot->setScale(0.6);
			auto secretCoin3Slot = CCSprite::createWithSpriteFrameName("secretCoin_b_01_001.png");
			secretCoin3Slot->setScale(0.6);


			auto secretCoin1 = CCSprite::createWithSpriteFrameName("secretCoinUI_001.png");
			secretCoin1->setScale(0.6);

			auto secretCoin2 = CCSprite::createWithSpriteFrameName("secretCoinUI_001.png");
			secretCoin2->setScale(0.6);

			auto secretCoin3 = CCSprite::createWithSpriteFrameName("secretCoinUI_001.png");
			secretCoin3->setScale(0.6);

			if (amountSecretCoinsCollected >= 1) menu->addChild(secretCoin1);
			if (amountSecretCoinsCollected >= 2) menu->addChild(secretCoin2);
			if (amountSecretCoinsCollected >= 3) menu->addChild(secretCoin3);


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
				if (level->m_coins >= 1)
					if (verifiedCoins->objectForKey(levelID + "_1" + weeklySuffix)) menu->addChild(coin1);
					else menu->addChild(coin1Slot);
				if (level->m_coins >= 2)
					if (verifiedCoins->objectForKey(levelID + "_2" + weeklySuffix)) menu->addChild(coin2);
					else menu->addChild(coin2Slot);
				if (level->m_coins >= 3)
					if (verifiedCoins->objectForKey(levelID + "_3" + weeklySuffix)) menu->addChild(coin3);
					else menu->addChild(coin3Slot);
			// silver coin in user created levels that havent been uploaded
			// hence the level id 0
			} else if (levelIDInt == 0 || level->m_levelType == GJLevelType::Editor) {
				if (level->m_coins >= 1)
					if (level->m_firstCoinVerified.value()) menu->addChild(coin1);
					else menu->addChild(coin1Slot);
				if (level->m_coins >= 2)
					if (level->m_secondCoinVerified.value()) menu->addChild(coin2);
					else menu->addChild(coin2Slot);
				if (level->m_coins >= 3)
					if (level->m_thirdCoinVerified.value()) menu->addChild(coin3);
					else menu->addChild(coin3Slot);
			// bronze coin
			} else {
				if (level->m_coins >= 1)
					if (pendingCoins->objectForKey(levelID + "_1" + weeklySuffix)) menu->addChild(coin1);
					else menu->addChild(coin1Slot);
				if (level->m_coins >= 2)
					if (pendingCoins->objectForKey(levelID + "_2" + weeklySuffix)) menu->addChild(coin2);
					else menu->addChild(coin2Slot);
				if (level->m_coins >= 3)
					if (pendingCoins->objectForKey(levelID + "_3" + weeklySuffix)) menu->addChild(coin3);
					else menu->addChild(coin3Slot);

				coin1Slot->setColor({255, 175, 75});
				coin2Slot->setColor({255, 175, 75});
				coin3Slot->setColor({255, 175, 75});
				coin1->setColor({255, 175, 75});
				coin2->setColor({255, 175, 75});
				coin3->setColor({255, 175, 75});
			}
		}

		menu->updateLayout();

	}
};