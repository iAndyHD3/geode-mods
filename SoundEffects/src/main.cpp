/**
 * Include the Geode headers.
 */
#include <Geode/Geode.hpp>

#include <Geode/modify/PlayerObject.hpp>
#include <Geode/modify/GJBaseGameLayer.hpp>
#include <Geode/modify/EffectGameObject.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/GameObject.hpp>

USE_GEODE_NAMESPACE();

void playSound(const char* str) {
	GameSoundManager::sharedManager()->playEffect(str, 0, 0, 0);
	log::info("sound effect played: {}", str);
}

#define MEMBERBYOFFSET(type, class, offset) *reinterpret_cast<type*>(reinterpret_cast<uintptr_t>(class) + offset)
#define MBO MEMBERBYOFFSET


class $modify(PlayerObject) {
	void ringJump(GameObject* ring) {
		PlayerObject::ringJump(ring);
		if(ring->m_hasBeenActivated) playSound("orbJump01.ogg");
	}
};

class $modify(GJBaseGameLayer) {
	void bumpPlayer(PlayerObject* player, GameObject* pad) {
		GJBaseGameLayer::bumpPlayer(player, pad);
		if(pad->m_hasBeenActivated) playSound("padJump01.ogg");
	}
};

class $modify(EffectGameObject) {
	
	void triggerObject(GJBaseGameLayer* bgl) {
		
		EffectGameObject::triggerObject(bgl);
		int objectID = MBO(int, this, 0x360);
		//std::cout << fmt::format("trigger object, objid: {}", objectID) << std::endl;
		switch(objectID) {
		
			case 1275:
			case 1587:
			case 1588:
			case 1589:
			case 1614:
		//	log::info("oid: {}", objectID);
			playSound("collectItem01.ogg");
		}
	}
};

bool isPlayerRobot(PlayerObject* p) { return MBO(bool, p, 1596); }

bool isPlayerCube(PlayerObject* p)
{
	return ( !MBO(bool, p, 1592) && !MBO(bool, p, 1593) && !MBO(bool, p, 1594) && !MBO(bool, p, 1595) && !MBO(bool, p, 1596) && !MBO(bool, p, 1597) );
}

bool isPlayerCubeOrRobot(PlayerObject* p) { return isPlayerCube(p) || isPlayerRobot(p); }

int getPlayerGameMode(PlayerObject* p) {
	for(int i = 2; i < 8; i++) {
		if(MBO(bool, p, 1590 + i)) return i;
	}
	return 1;
}

class $modify(PlayLayer) {
	
	void update(float dt) {
	
		auto p1 = MBO(PlayerObject*, this, 548);
		if(!isPlayerCubeOrRobot(p1)) return PlayLayer::update(dt);
	
		//std::cout << fmt::format("isPlayerCubeOrRobot: {}", isPlayerCubeOrRobot(p1)) << std::endl;
	
		bool isFalling = p1->playerIsFalling();
		bool isPressingDown = MBO(int, p1, 0x611) != 0;
		//bool onGround = !p1->m_isOnGround && (int)p1->unk610 == 213;
		bool onGround = MBO(int, p1, 1616) && MBO(int, p1, 1568) == 213;
	
		PlayLayer::update(dt);

		if(!isFalling && !p1->playerIsFalling() && isPressingDown && onGround) {
			if(isPlayerRobot(p1)) playSound("robotJump01.ogg");
			else if(isPlayerCube(p1)) playSound("cubeJump01.ogg");
		}
	}
	
	void playerWillSwitchMode(PlayerObject* player, GameObject* portal) {
		int objectID = MBO(int, portal, 0x360);
	
		//cube
		int portalGameMode = 0;
		switch(objectID) {
			case 12: portalGameMode = 1; break;
			case 13: portalGameMode = 2; break;
			case 47: portalGameMode = 4; break; // (yes)
			case 111: portalGameMode = 3; break; // (yes)
			case 660: portalGameMode = 5; break;
			case 745: portalGameMode = 6; break;
			case 1331: portalGameMode = 7; break;
		}
		if(getPlayerGameMode(player) != portalGameMode) playSound("gameModeChanged01.ogg");
		//std::cout << fmt::format("playergm: {}, fnportalgm: {}, objid: {}", m, portalGameMode, objectID) << std::endl;
		PlayLayer::playerWillSwitchMode(player, portal);
	}
};

class $modify(GameObject) {
	
	void destroyObject() {
	
		int objectID = MBO(int, this, 0x360);
		if(objectID == 142 || objectID == 1329) playSound("collectCoin01.ogg");
		GameObject::destroyObject();
	}
};
/*
class $modify(MenuLayer) {
	
	void onMoreGames(CCObject*) {
		FLAlertLayer::create("Geode", "Hello from my SOUND EFFECTS mod!", "OK")->show(); 
	}
};
*/