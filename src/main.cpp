#include <Geode/Geode.hpp>
#include <Geode/modify/PauseLayer.hpp>

#include <algorithm>

using namespace geode::prelude;

class $modify(PauseLayer) {
	TodoReturn customSetup() {
		PauseLayer::customSetup();

		auto winSize = CCDirector::get()->getWinSize();
		auto level = GameManager::sharedState()->getPlayLayer()->m_level;
		int mainLevels[] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,1001,1002,1003,2001,2002,2003,2004,2005,2006,2007,2008,2009,2010,3001,4001,4002,4003,5001,5002,5003,5004};

		auto levelIDInt =  level->m_levelID.value();
		auto levelID = std::to_string(levelIDInt);
		auto levelIDKey = std::string(level->getCoinKey(levelIDInt));
		auto areCoinsVerified = level->m_coinsVerified.value();
		std::string weeklySuffix = "";
		if (levelIDKey.size() != 2*levelID.size() + 1) weeklySuffix = levelIDKey.substr(2*levelID.size() + 1, 7);
		
		
		// log::info("coins: {}", level->m_coins);
		// log::info("id: {}", levelID);
		// log::info("id key: {}", levelIDKey);;
		// log::info("suffix: {}", weeklySuffix);
		// log::info("aaaaaa: {}", GameStatsManager::sharedState()->getCollectedCoinsForLevel(level)); 

		// check if level is a main level
		// NOT accurate since level id 2004 still exists on the server but idgaf :D
		// if (std::find(mainLevels, mainLevels + sizeof(mainLevels)/sizeof(mainLevels[0]), levelIDInt) != mainLevels + sizeof(mainLevels)/sizeof(mainLevels[0])) {
		if (true) {
			auto amountSecretCoinsCollected = GameStatsManager::sharedState()->getCollectedCoinsForLevel(level);

			auto secretCoin1Slot = CCSprite::createWithSpriteFrameName("secretCoin_b_01_001.png");
			auto secretCoin2Slot = CCSprite::createWithSpriteFrameName("secretCoin_b_01_001.png");
			auto secretCoin3Slot = CCSprite::createWithSpriteFrameName("secretCoin_b_01_001.png");

			auto secretCoin1 = CCSprite::createWithSpriteFrameName("secretCoinUI_001.png");
			secretCoin1->setScale(0.6);
			secretCoin1->setPosition({winSize.width/2 + secretCoin1Slot->getContentSize().width*(0.4f - amountSecretCoinsCollected/2.5f), winSize.height/11.5f});
			secretCoin1->setZOrder(8);

			auto secretCoin2 = CCSprite::createWithSpriteFrameName("secretCoinUI_001.png");
			secretCoin2->setScale(0.6);
			secretCoin2->setPosition({winSize.width/2 + secretCoin1Slot->getContentSize().width*(1.2f - amountSecretCoinsCollected/2.5f), winSize.height/11.5f});
			secretCoin2->setZOrder(8);

			auto secretCoin3 = CCSprite::createWithSpriteFrameName("secretCoinUI_001.png");
			secretCoin3->setScale(0.6);
			secretCoin3->setPosition({winSize.width/2 + secretCoin1Slot->getContentSize().width*(2.f - amountSecretCoinsCollected/2.5f), winSize.height/11.5f});
			secretCoin3->setZOrder(8);

			if (amountSecretCoinsCollected >= 1) this->addChild(secretCoin1);
			if (amountSecretCoinsCollected >= 2) this->addChild(secretCoin2);
			if (amountSecretCoinsCollected >= 3) this->addChild(secretCoin3);


		} else {
			// silver coin: verified
			// bronze coin: pending
			auto verifiedCoins = GameStatsManager::sharedState()->m_verifiedUserCoins;
			auto pendingCoins = GameStatsManager::sharedState()->m_pendingUserCoins;

			// IDK LOL 
			auto coin1Slot = CCSprite::createWithSpriteFrameName("secretCoin_2_b_01_001.png");
			coin1Slot->setScale(0.6);
			coin1Slot->setPosition({winSize.width/2 + coin1Slot->getContentSize().width*(0.4f - level->m_coins/2.5f), winSize.height/11.5f});
			coin1Slot->setZOrder(8);

			auto coin2Slot = CCSprite::createWithSpriteFrameName("secretCoin_2_b_01_001.png");
			coin2Slot->setScale(0.6);
			coin2Slot->setPosition({winSize.width/2 + coin1Slot->getContentSize().width*(1.2f - level->m_coins/2.5f), winSize.height/11.5f});
			coin2Slot->setZOrder(8);

			auto coin3Slot = CCSprite::createWithSpriteFrameName("secretCoin_2_b_01_001.png");
			coin3Slot->setScale(0.6);
			coin3Slot->setPosition({winSize.width/2 + coin1Slot->getContentSize().width*(2.f - level->m_coins/2.5f), winSize.height/11.5f});
			coin3Slot->setZOrder(8);

			auto coin1 = CCSprite::createWithSpriteFrameName("secretCoinUI2_001.png");
			coin1->setScale(0.6);
			coin1->setPosition({winSize.width/2 + coin1Slot->getContentSize().width*(0.4f - level->m_coins/2.5f), winSize.height/11.5f});
			coin1->setZOrder(9);

			auto coin2 = CCSprite::createWithSpriteFrameName("secretCoinUI2_001.png");
			coin2->setScale(0.6);
			coin2->setPosition({winSize.width/2 + coin1Slot->getContentSize().width*(1.2f - level->m_coins/2.5f), winSize.height/11.5f});
			coin2->setZOrder(9);

			auto coin3 = CCSprite::createWithSpriteFrameName("secretCoinUI2_001.png");
			coin3->setScale(0.6);
			coin3->setPosition({winSize.width/2 + coin1Slot->getContentSize().width*(2.f - level->m_coins/2.5f), winSize.height/11.5f});
			coin3->setZOrder(9);

			// self explanatory
			if (level->m_coins >= 1) this->addChild(coin1Slot);
			if (level->m_coins >= 2) this->addChild(coin2Slot);
			if (level->m_coins >= 3) this->addChild(coin3Slot);

			// silver coin in levels on the server
			if (areCoinsVerified) {
				if (verifiedCoins->objectForKey(levelID + "_1" + weeklySuffix)) this->addChild(coin1);
				if (verifiedCoins->objectForKey(levelID + "_2" + weeklySuffix)) this->addChild(coin2);
				if (verifiedCoins->objectForKey(levelID + "_3" + weeklySuffix)) this->addChild(coin3);
			// silver coin in user created levels that havent been uploaded
			// hence the level id 0
			} else if (levelIDInt == 0) {
				if (level->m_firstCoinVerified.value()) this->addChild(coin1);
				if (level->m_secondCoinVerified.value()) this->addChild(coin2);
				if (level->m_thirdCoinVerified.value()) this->addChild(coin3);
			// bronze coin
			} else {
				if (pendingCoins->objectForKey(levelID + "_1" + weeklySuffix)) this->addChild(coin1);
				if (pendingCoins->objectForKey(levelID + "_2" + weeklySuffix)) this->addChild(coin2);
				if (pendingCoins->objectForKey(levelID + "_3" + weeklySuffix)) this->addChild(coin3);

				coin1Slot->setColor({255, 175, 75});
				coin2Slot->setColor({255, 175, 75});
				coin3Slot->setColor({255, 175, 75});
				coin1->setColor({255, 175, 75});
				coin2->setColor({255, 175, 75});
				coin3->setColor({255, 175, 75});
			}
		}
		
		free(mainLevels);
	}
};