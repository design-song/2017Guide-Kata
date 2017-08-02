#include "StrategyManager.h"

using namespace MyBot;

StrategyManager & StrategyManager::Instance()
{
	static StrategyManager instance;
	return instance;
}

StrategyManager::StrategyManager()
{
}

void StrategyManager::onStart()
{
	// BasicBot 1.1 Patch Start ////////////////////////////////////////////////
	// 경기 결과 파일 Save / Load 및 로그파일 Save 예제 추가

	// 과거 게임 기록을 로딩합니다
	loadGameRecordList();

	// BasicBot 1.1 Patch End //////////////////////////////////////////////////
	
	/// 변수 초기값을 설정합니다
	setVariables();

	/// 게임 초기에 사용할 빌드오더를 세팅합니다
	setInitialBuildOrder();
}


/// 변수 초기값을 설정합니다
void StrategyManager::setVariables(){

	// 참가자께서 자유롭게 초기값을 수정하셔도 됩니다 

	myPlayer = BWAPI::Broodwar->self();
	myRace = BWAPI::Broodwar->self()->getRace();
	enemyPlayer = InformationManager::Instance().enemyPlayer;
	
	myKilledCombatUnitCount1 = 0;
	myKilledCombatUnitCount2 = 0;
	myKilledCombatUnitCount3 = 0;

	numberOfCompletedEnemyCombatUnit = 0;
	numberOfCompletedEnemyWorkerUnit = 0;
	enemyKilledCombatUnitCount = 0;
	enemyKilledWorkerUnitCount = 0;

	isInitialBuildOrderFinished = false;
	combatState = CombatState::defenseMode;

	if (myRace == BWAPI::Races::Protoss) {

		// 공격 유닛 종류 설정 
		myCombatUnitType1 = BWAPI::UnitTypes::Protoss_Zealot;
		myCombatUnitType2 = BWAPI::UnitTypes::Protoss_Dragoon;
		myCombatUnitType3 = BWAPI::UnitTypes::Protoss_Dark_Templar;

		// 공격 모드로 전환하기 위해 필요한 최소한의 유닛 숫자 설정
		necessaryNumberOfCombatUnitType1 = 6;							// 공격을 시작하기위해 필요한 최소한의 질럿 유닛 숫자 
		necessaryNumberOfCombatUnitType2 = 6;							// 공격을 시작하기위해 필요한 최소한의 드라군 유닛 숫자 
		necessaryNumberOfCombatUnitType3 = 2;							// 공격을 시작하기위해 필요한 최소한의 다크템플러 유닛 숫자 

		// 공격 유닛 생산 순서 설정
		buildOrderArrayOfMyCombatUnitType = { 1, 2, 2, 3 };				// 생산 순서 : 질럿 드라군 드라군 다크템플러 ...
		nextTargetIndexOfBuildOrderArray = 0; 							// 다음 생산 순서 index

		// 특수 유닛 종류 설정 
		mySpecialUnitType1 = BWAPI::UnitTypes::Protoss_Observer;
		mySpecialUnitType2 = BWAPI::UnitTypes::Protoss_High_Templar;

		// 공격 모드로 전환하기 위해 필요한 최소한의 유닛 숫자 설정
		necessaryNumberOfSpecialUnitType1 = 1;
		necessaryNumberOfSpecialUnitType2 = 1;

		// 특수 유닛을 최대 몇개까지 생산 / 전투참가 시킬것인가
		maxNumberOfSpecialUnitType1 = 4;
		maxNumberOfSpecialUnitType2 = 4;

		// 방어 건물 종류 및 건설 갯수 설정
		myDefenseBuildingType1 = BWAPI::UnitTypes::Protoss_Pylon;
		necessaryNumberOfDefenseBuilding1 = 1;
		myDefenseBuildingType2 = BWAPI::UnitTypes::Protoss_Photon_Cannon;
		necessaryNumberOfDefenseBuilding2 = 3;

		// 방어 건물 건설 위치 설정
		seedPositionStrategyOfMyDefenseBuildingType
			= BuildOrderItem::SeedPositionStrategy::SecondChokePoint;	// 두번째 길목
		seedPositionStrategyOfMyCombatUnitTrainingBuildingType
			= BuildOrderItem::SeedPositionStrategy::SecondChokePoint;	// 두번째 길목

		// 업그레이드 및 리서치 대상 설정
		necessaryUpgradeType1 = BWAPI::UpgradeTypes::Singularity_Charge;
		necessaryUpgradeType2 = BWAPI::UpgradeTypes::Leg_Enhancements;
		necessaryUpgradeType3 = BWAPI::UpgradeTypes::Khaydarin_Amulet;
		necessaryTechType1 = BWAPI::TechTypes::Psionic_Storm;
		necessaryTechType2 = BWAPI::TechTypes::Hallucination;
		necessaryTechType3 = BWAPI::TechTypes::None;
	}
	else if (myRace == BWAPI::Races::Terran) {

		// 공격 유닛 종류 설정  
		myCombatUnitType1 = BWAPI::UnitTypes::Terran_Marine;
		myCombatUnitType2 = BWAPI::UnitTypes::Terran_Medic;
		myCombatUnitType3 = BWAPI::UnitTypes::Terran_Siege_Tank_Tank_Mode;

		// 공격 모드로 전환하기 위해 필요한 최소한의 유닛 숫자 설정
		necessaryNumberOfCombatUnitType1 = 12;                          // 공격을 시작하기위해 필요한 최소한의 마린 유닛 숫자 
		necessaryNumberOfCombatUnitType2 = 4;                           // 공격을 시작하기위해 필요한 최소한의 메딕 유닛 숫자 
		necessaryNumberOfCombatUnitType3 = 2;							// 공격을 시작하기위해 필요한 최소한의 시즈탱크 유닛 숫자 

		// 공격 유닛 생산 순서 설정
		buildOrderArrayOfMyCombatUnitType = {1, 1, 1, 2, 3};			// 마린 마린 마린 메딕 시즈탱크 ...
		nextTargetIndexOfBuildOrderArray = 0; 							// 다음 생산 순서 index
		
		// 특수 유닛 종류 설정 
		mySpecialUnitType1 = BWAPI::UnitTypes::Terran_Science_Vessel;
		mySpecialUnitType2 = BWAPI::UnitTypes::Terran_Battlecruiser;

		// 공격 모드로 전환하기 위해 필요한 최소한의 유닛 숫자 설정
		necessaryNumberOfSpecialUnitType1 = 1;
		necessaryNumberOfSpecialUnitType2 = 1;

		// 특수 유닛을 최대 몇개까지 생산 / 전투참가 시킬것인가
		maxNumberOfSpecialUnitType1 = 4;
		maxNumberOfSpecialUnitType2 = 4;

		// 방어 건물 종류 및 건설 갯수 설정
		myDefenseBuildingType1 = BWAPI::UnitTypes::Terran_Bunker;
		necessaryNumberOfDefenseBuilding1 = 2;
		myDefenseBuildingType2 = BWAPI::UnitTypes::Terran_Missile_Turret;
		necessaryNumberOfDefenseBuilding2 = 1;

		// 방어 건물 건설 위치 설정
		seedPositionStrategyOfMyDefenseBuildingType
			= BuildOrderItem::SeedPositionStrategy::SecondChokePoint;	// 두번째 길목
		seedPositionStrategyOfMyCombatUnitTrainingBuildingType
			= BuildOrderItem::SeedPositionStrategy::SecondChokePoint;	// 두번째 길목

		// 업그레이드 및 리서치 대상 설정
		necessaryUpgradeType1 = BWAPI::UpgradeTypes::U_238_Shells;
		necessaryUpgradeType2 = BWAPI::UpgradeTypes::Terran_Infantry_Weapons;
		necessaryUpgradeType3 = BWAPI::UpgradeTypes::Titan_Reactor;
		necessaryTechType1 = BWAPI::TechTypes::Tank_Siege_Mode;
		necessaryTechType2 = BWAPI::TechTypes::Irradiate;
		necessaryTechType3 = BWAPI::TechTypes::Yamato_Gun;
	}
	else if (myRace == BWAPI::Races::Zerg) {

		// 공격 유닛 종류 설정 
		myCombatUnitType1 = BWAPI::UnitTypes::Zerg_Zergling;
		myCombatUnitType2 = BWAPI::UnitTypes::Zerg_Hydralisk;
		myCombatUnitType3 = BWAPI::UnitTypes::Zerg_Lurker;

		// 공격 모드로 전환하기 위해 필요한 최소한의 유닛 숫자 설정
		necessaryNumberOfCombatUnitType1 = 8;							// 공격을 시작하기위해 필요한 최소한의 저글링 유닛 숫자 
		necessaryNumberOfCombatUnitType2 = 8;							// 공격을 시작하기위해 필요한 최소한의 히드라 유닛 숫자 
		necessaryNumberOfCombatUnitType3 = 2;                     		// 공격을 시작하기위해 필요한 최소한의 러커 유닛 숫자 

		// 공격 유닛 생산 순서 설정
		buildOrderArrayOfMyCombatUnitType = {1,1,2,2,2,3};				// 저글링 저글링 히드라 히드라 히드라 러커 ...
		nextTargetIndexOfBuildOrderArray = 0; 							// 다음 생산 순서 index

		// 특수 유닛 종류 설정 
		mySpecialUnitType1 = BWAPI::UnitTypes::Zerg_Overlord;
		mySpecialUnitType2 = BWAPI::UnitTypes::Zerg_Defiler;

		// 공격 모드로 전환하기 위해 필요한 최소한의 유닛 숫자 설정
		necessaryNumberOfSpecialUnitType1 = 1;
		necessaryNumberOfSpecialUnitType2 = 1;

		// 특수 유닛을 최대 몇개까지 생산 / 전투참가 시킬것인가
		maxNumberOfSpecialUnitType1 = 4;
		maxNumberOfSpecialUnitType2 = 2;

		// 방어 건물 종류 및 건설 갯수 설정
		myDefenseBuildingType1 = BWAPI::UnitTypes::Zerg_Creep_Colony;
		necessaryNumberOfDefenseBuilding1 = 3;
		myDefenseBuildingType2 = BWAPI::UnitTypes::Zerg_Sunken_Colony;
		necessaryNumberOfDefenseBuilding2 = 3;

		// 방어 건물 건설 위치 설정 
		seedPositionStrategyOfMyDefenseBuildingType
			= BuildOrderItem::SeedPositionStrategy::FirstExpansionLocation;	// 앞마당
		seedPositionStrategyOfMyCombatUnitTrainingBuildingType
			= BuildOrderItem::SeedPositionStrategy::FirstExpansionLocation;	// 앞마당
		
		// 업그레이드 및 리서치 대상 설정
		necessaryUpgradeType1 = BWAPI::UpgradeTypes::Grooved_Spines;
		necessaryUpgradeType2 = BWAPI::UpgradeTypes::Muscular_Augments;
		necessaryUpgradeType3 = BWAPI::UpgradeTypes::Pneumatized_Carapace;
		necessaryTechType1 = BWAPI::TechTypes::Lurker_Aspect;
		necessaryTechType2 = BWAPI::TechTypes::Consume;
		necessaryTechType3 = BWAPI::TechTypes::Plague;
	}
}

/// 게임 초기에 사용할 빌드오더를 세팅합니다
void StrategyManager::setInitialBuildOrder() {

	// 프로토스 : 초기에 포톤 캐논으로 방어하며 질럿 드라군 을 생산합니다
	// 테란     : 초기에 벙커와 마린으로 방어하며 마린 메딕 을 생산합니다
	// 저그     : 초기에 성큰과 저글링으로 방어하며 저글링 히드라 를 생산합니다

	// 참가자께서 자유롭게 빌드오더를 수정하셔도 됩니다 

	if (BWAPI::Broodwar->self()->getRace() == BWAPI::Races::Protoss) {

		BuildManager::Instance().buildQueue.queueAsLowestPriority(BWAPI::UnitTypes::Protoss_Probe); // 5
		BuildManager::Instance().buildQueue.queueAsLowestPriority(BWAPI::UnitTypes::Protoss_Probe); // 6
		BuildManager::Instance().buildQueue.queueAsLowestPriority(BWAPI::UnitTypes::Protoss_Probe); // 7

		BuildManager::Instance().buildQueue.queueAsLowestPriority(BWAPI::UnitTypes::Protoss_Pylon,
			seedPositionStrategyOfMyDefenseBuildingType); // 첫번째 파일런

		BuildManager::Instance().buildQueue.queueAsLowestPriority(BWAPI::UnitTypes::Protoss_Probe); // 8			
		BuildManager::Instance().buildQueue.queueAsLowestPriority(BWAPI::UnitTypes::Protoss_Probe); // 9
		BuildManager::Instance().buildQueue.queueAsLowestPriority(BWAPI::UnitTypes::Protoss_Probe); // 10

		BuildManager::Instance().buildQueue.queueAsLowestPriority(BWAPI::UnitTypes::Protoss_Forge,
			seedPositionStrategyOfMyDefenseBuildingType); // 포지

		BuildManager::Instance().buildQueue.queueAsLowestPriority(BWAPI::UnitTypes::Protoss_Probe); // 11
		BuildManager::Instance().buildQueue.queueAsLowestPriority(BWAPI::UnitTypes::Protoss_Probe); // 12
		BuildManager::Instance().buildQueue.queueAsLowestPriority(BWAPI::UnitTypes::Protoss_Probe); // 13

		BuildManager::Instance().buildQueue.queueAsLowestPriority(BWAPI::UnitTypes::Protoss_Photon_Cannon,
			seedPositionStrategyOfMyDefenseBuildingType); // 첫번째 포톤캐논
		BuildManager::Instance().buildQueue.queueAsLowestPriority(BWAPI::UnitTypes::Protoss_Photon_Cannon,
			seedPositionStrategyOfMyDefenseBuildingType); // 두번째 포톤캐논
		BuildManager::Instance().buildQueue.queueAsLowestPriority(BWAPI::UnitTypes::Protoss_Gateway,
			seedPositionStrategyOfMyCombatUnitTrainingBuildingType); // 첫번째 게이트웨이
		BuildManager::Instance().buildQueue.queueAsLowestPriority(BWAPI::UnitTypes::Protoss_Probe); // 14
		BuildManager::Instance().buildQueue.queueAsLowestPriority(BWAPI::UnitTypes::Protoss_Photon_Cannon,
			seedPositionStrategyOfMyDefenseBuildingType); // 세번째 포톤캐논

		BuildManager::Instance().buildQueue.queueAsLowestPriority(BWAPI::UnitTypes::Protoss_Probe); // 15
		BuildManager::Instance().buildQueue.queueAsLowestPriority(BWAPI::UnitTypes::Protoss_Assimilator);
		BuildManager::Instance().buildQueue.queueAsLowestPriority(BWAPI::UnitTypes::Protoss_Pylon); // 두번째 파일런
		BuildManager::Instance().buildQueue.queueAsLowestPriority(BWAPI::UnitTypes::Protoss_Zealot); // 17

		BuildManager::Instance().buildQueue.queueAsLowestPriority(BWAPI::UnitTypes::Protoss_Cybernetics_Core);
		BuildManager::Instance().buildQueue.queueAsLowestPriority(BWAPI::UnitTypes::Protoss_Probe); // 18
		BuildManager::Instance().buildQueue.queueAsLowestPriority(BWAPI::UnitTypes::Protoss_Zealot); // 20
		BuildManager::Instance().buildQueue.queueAsLowestPriority(BWAPI::UnitTypes::Protoss_Pylon); // 세번째 파일런
		BuildManager::Instance().buildQueue.queueAsLowestPriority(BWAPI::UnitTypes::Protoss_Probe); // 21
		BuildManager::Instance().buildQueue.queueAsLowestPriority(BWAPI::UnitTypes::Protoss_Zealot); // 23

		BuildManager::Instance().buildQueue.queueAsLowestPriority(BWAPI::UnitTypes::Protoss_Gateway); // 두번째 게이트웨이
		BuildManager::Instance().buildQueue.queueAsLowestPriority(BWAPI::UnitTypes::Protoss_Probe); // 24
		BuildManager::Instance().buildQueue.queueAsLowestPriority(BWAPI::UnitTypes::Protoss_Zealot); // 26
		BuildManager::Instance().buildQueue.queueAsLowestPriority(BWAPI::UnitTypes::Protoss_Dragoon); // 28

		BuildManager::Instance().buildQueue.queueAsLowestPriority(BWAPI::UnitTypes::Protoss_Probe); // 29
		BuildManager::Instance().buildQueue.queueAsLowestPriority(BWAPI::UnitTypes::Protoss_Zealot); // 31
		BuildManager::Instance().buildQueue.queueAsLowestPriority(BWAPI::UnitTypes::Protoss_Dragoon); // 33
		BuildManager::Instance().buildQueue.queueAsLowestPriority(BWAPI::UnitTypes::Protoss_Pylon); // 네번째 파일런

		BuildManager::Instance().buildQueue.queueAsLowestPriority(BWAPI::UnitTypes::Protoss_Nexus,
			BuildOrderItem::SeedPositionStrategy::FirstExpansionLocation);
		BuildManager::Instance().buildQueue.queueAsLowestPriority(BWAPI::UnitTypes::Protoss_Assimilator,
			BuildOrderItem::SeedPositionStrategy::FirstExpansionLocation);
		BuildManager::Instance().buildQueue.queueAsLowestPriority(BWAPI::UnitTypes::Protoss_Probe); // 34
		BuildManager::Instance().buildQueue.queueAsLowestPriority(BWAPI::UnitTypes::Protoss_Probe); // 35
	}
	else if (BWAPI::Broodwar->self()->getRace() == BWAPI::Races::Terran) {
		BuildManager::Instance().buildQueue.queueAsLowestPriority(BWAPI::UnitTypes::Terran_SCV); // 5
		BuildManager::Instance().buildQueue.queueAsLowestPriority(BWAPI::UnitTypes::Terran_SCV); // 6
		BuildManager::Instance().buildQueue.queueAsLowestPriority(BWAPI::UnitTypes::Terran_SCV); // 7
		BuildManager::Instance().buildQueue.queueAsLowestPriority(BWAPI::UnitTypes::Terran_SCV); // 8
		BuildManager::Instance().buildQueue.queueAsLowestPriority(BWAPI::UnitTypes::Terran_Supply_Depot);
		BuildManager::Instance().buildQueue.queueAsLowestPriority(BWAPI::UnitTypes::Terran_SCV); // 9
		BuildManager::Instance().buildQueue.queueAsLowestPriority(BWAPI::UnitTypes::Terran_SCV); // 10

		BuildManager::Instance().buildQueue.queueAsLowestPriority(BWAPI::UnitTypes::Terran_Barracks,
			seedPositionStrategyOfMyCombatUnitTrainingBuildingType);

		BuildManager::Instance().buildQueue.queueAsLowestPriority(BWAPI::UnitTypes::Terran_SCV); // 11
		BuildManager::Instance().buildQueue.queueAsLowestPriority(BWAPI::UnitTypes::Terran_SCV); // 12

		BuildManager::Instance().buildQueue.queueAsLowestPriority(BWAPI::UnitTypes::Terran_Bunker,
			seedPositionStrategyOfMyDefenseBuildingType);

		BuildManager::Instance().buildQueue.queueAsLowestPriority(BWAPI::UnitTypes::Terran_SCV); // 13
		BuildManager::Instance().buildQueue.queueAsLowestPriority(BWAPI::UnitTypes::Terran_Marine); // 14

		BuildManager::Instance().buildQueue.queueAsLowestPriority(BWAPI::UnitTypes::Terran_Engineering_Bay);

		BuildManager::Instance().buildQueue.queueAsLowestPriority(BWAPI::UnitTypes::Terran_Marine); // 15
		BuildManager::Instance().buildQueue.queueAsLowestPriority(BWAPI::UnitTypes::Terran_Supply_Depot);
		BuildManager::Instance().buildQueue.queueAsLowestPriority(BWAPI::UnitTypes::Terran_SCV); // 16
		BuildManager::Instance().buildQueue.queueAsLowestPriority(BWAPI::UnitTypes::Terran_Marine); // 17

		BuildManager::Instance().buildQueue.queueAsLowestPriority(BWAPI::UnitTypes::Terran_Missile_Turret,
			seedPositionStrategyOfMyCombatUnitTrainingBuildingType);

		BuildManager::Instance().buildQueue.queueAsLowestPriority(BWAPI::UnitTypes::Terran_Barracks);

		BuildManager::Instance().buildQueue.queueAsLowestPriority(BWAPI::UnitTypes::Terran_Marine); // 18

		BuildManager::Instance().buildQueue.queueAsLowestPriority(BWAPI::UnitTypes::Terran_Bunker,
			seedPositionStrategyOfMyDefenseBuildingType);

		BuildManager::Instance().buildQueue.queueAsLowestPriority(BWAPI::UnitTypes::Terran_SCV); // 19
		BuildManager::Instance().buildQueue.queueAsLowestPriority(BWAPI::UnitTypes::Terran_Marine); // 20
		BuildManager::Instance().buildQueue.queueAsLowestPriority(BWAPI::UnitTypes::Terran_Marine); // 21

		BuildManager::Instance().buildQueue.queueAsLowestPriority(BWAPI::UnitTypes::Terran_Academy);
		BuildManager::Instance().buildQueue.queueAsLowestPriority(BWAPI::UnitTypes::Terran_Refinery);

		BuildManager::Instance().buildQueue.queueAsLowestPriority(BWAPI::UnitTypes::Terran_Supply_Depot);

		BuildManager::Instance().buildQueue.queueAsLowestPriority(BWAPI::UnitTypes::Terran_SCV); // 22
		BuildManager::Instance().buildQueue.queueAsLowestPriority(BWAPI::UnitTypes::Terran_Marine); // 23
		BuildManager::Instance().buildQueue.queueAsLowestPriority(BWAPI::UnitTypes::Terran_Marine); // 24

		BuildManager::Instance().buildQueue.queueAsLowestPriority(BWAPI::UnitTypes::Terran_Bunker,
			seedPositionStrategyOfMyDefenseBuildingType);

		BuildManager::Instance().buildQueue.queueAsLowestPriority(BWAPI::UnitTypes::Terran_SCV); // 25
		BuildManager::Instance().buildQueue.queueAsLowestPriority(BWAPI::UnitTypes::Terran_Marine); // 26
		BuildManager::Instance().buildQueue.queueAsLowestPriority(BWAPI::UnitTypes::Terran_Marine); // 27
		BuildManager::Instance().buildQueue.queueAsLowestPriority(BWAPI::UnitTypes::Terran_SCV); // 28
		BuildManager::Instance().buildQueue.queueAsLowestPriority(BWAPI::UnitTypes::Terran_Marine); // 29
		BuildManager::Instance().buildQueue.queueAsLowestPriority(BWAPI::UnitTypes::Terran_Marine); // 30

		BuildManager::Instance().buildQueue.queueAsLowestPriority(BWAPI::UnitTypes::Terran_Supply_Depot);
		BuildManager::Instance().buildQueue.queueAsLowestPriority(BWAPI::UnitTypes::Terran_Factory);

		BuildManager::Instance().buildQueue.queueAsLowestPriority(BWAPI::UnitTypes::Terran_Command_Center,
			BuildOrderItem::SeedPositionStrategy::FirstExpansionLocation);
		BuildManager::Instance().buildQueue.queueAsLowestPriority(BWAPI::UnitTypes::Terran_Refinery,
			BuildOrderItem::SeedPositionStrategy::FirstExpansionLocation);
	}
	else if (BWAPI::Broodwar->self()->getRace() == BWAPI::Races::Zerg) {
		BuildManager::Instance().buildQueue.queueAsLowestPriority(BWAPI::UnitTypes::Zerg_Drone);	//5
		BuildManager::Instance().buildQueue.queueAsLowestPriority(BWAPI::UnitTypes::Zerg_Drone);	//6
		BuildManager::Instance().buildQueue.queueAsLowestPriority(BWAPI::UnitTypes::Zerg_Drone);	//7
		BuildManager::Instance().buildQueue.queueAsLowestPriority(BWAPI::UnitTypes::Zerg_Drone);	//8
		BuildManager::Instance().buildQueue.queueAsLowestPriority(BWAPI::UnitTypes::Zerg_Drone);	//9
		BuildManager::Instance().buildQueue.queueAsLowestPriority(BWAPI::UnitTypes::Zerg_Overlord); // 두번째 오버로드

		BuildManager::Instance().buildQueue.queueAsLowestPriority(BWAPI::UnitTypes::Zerg_Drone);	//10
		BuildManager::Instance().buildQueue.queueAsLowestPriority(BWAPI::UnitTypes::Zerg_Drone);	//11
		BuildManager::Instance().buildQueue.queueAsLowestPriority(BWAPI::UnitTypes::Zerg_Drone);	//12

		BuildManager::Instance().buildQueue.queueAsLowestPriority(BWAPI::UnitTypes::Zerg_Spawning_Pool); //11 스포닝풀

		BuildManager::Instance().buildQueue.queueAsLowestPriority(BWAPI::UnitTypes::Zerg_Hatchery,
			seedPositionStrategyOfMyDefenseBuildingType); //10 해처리

		BuildManager::Instance().buildQueue.queueAsLowestPriority(BWAPI::UnitTypes::Zerg_Drone);	//11
		BuildManager::Instance().buildQueue.queueAsLowestPriority(BWAPI::UnitTypes::Zerg_Drone);	//12

		BuildManager::Instance().buildQueue.queueAsLowestPriority(BWAPI::UnitTypes::Zerg_Zergling, false);	//13
		BuildManager::Instance().buildQueue.queueAsLowestPriority(BWAPI::UnitTypes::Zerg_Zergling, false);	//14
		BuildManager::Instance().buildQueue.queueAsLowestPriority(BWAPI::UnitTypes::Zerg_Zergling, false);	//15

		BuildManager::Instance().buildQueue.queueAsLowestPriority(BWAPI::UnitTypes::Zerg_Hatchery,
			BuildOrderItem::SeedPositionStrategy::FirstExpansionLocation); //14 해처리

		BuildManager::Instance().buildQueue.queueAsLowestPriority(BWAPI::UnitTypes::Zerg_Overlord);	// 세번째 오버로드

		BuildManager::Instance().buildQueue.queueAsLowestPriority(BWAPI::UnitTypes::Zerg_Drone);	//15
		BuildManager::Instance().buildQueue.queueAsLowestPriority(BWAPI::UnitTypes::Zerg_Drone);	//16
		BuildManager::Instance().buildQueue.queueAsLowestPriority(BWAPI::UnitTypes::Zerg_Drone);	//17

		BuildManager::Instance().buildQueue.queueAsLowestPriority(BWAPI::UnitTypes::Zerg_Creep_Colony,
			seedPositionStrategyOfMyDefenseBuildingType);	//16

		BuildManager::Instance().buildQueue.queueAsLowestPriority(BWAPI::UnitTypes::Zerg_Zergling);	//17
		BuildManager::Instance().buildQueue.queueAsLowestPriority(BWAPI::UnitTypes::Zerg_Zergling);	//18
		BuildManager::Instance().buildQueue.queueAsLowestPriority(BWAPI::UnitTypes::Zerg_Zergling);	//19
		BuildManager::Instance().buildQueue.queueAsLowestPriority(BWAPI::UnitTypes::Zerg_Drone);	//20
		BuildManager::Instance().buildQueue.queueAsLowestPriority(BWAPI::UnitTypes::Zerg_Drone);	//21

		BuildManager::Instance().buildQueue.queueAsLowestPriority(BWAPI::UnitTypes::Zerg_Sunken_Colony);

		BuildManager::Instance().buildQueue.queueAsLowestPriority(BWAPI::UnitTypes::Zerg_Creep_Colony,
			seedPositionStrategyOfMyDefenseBuildingType);	//20
		BuildManager::Instance().buildQueue.queueAsLowestPriority(BWAPI::UnitTypes::Zerg_Extractor); //19
		BuildManager::Instance().buildQueue.queueAsLowestPriority(BWAPI::UnitTypes::Zerg_Drone);	//20

		BuildManager::Instance().buildQueue.queueAsLowestPriority(BWAPI::UnitTypes::Zerg_Creep_Colony,
			seedPositionStrategyOfMyDefenseBuildingType);	//19
		BuildManager::Instance().buildQueue.queueAsLowestPriority(BWAPI::UnitTypes::Zerg_Drone);	//20
		BuildManager::Instance().buildQueue.queueAsLowestPriority(BWAPI::UnitTypes::Zerg_Drone);	//21
		BuildManager::Instance().buildQueue.queueAsLowestPriority(BWAPI::UnitTypes::Zerg_Drone);	//22

		BuildManager::Instance().buildQueue.queueAsLowestPriority(BWAPI::UnitTypes::Zerg_Overlord);	// 네번째 오버로드

		BuildManager::Instance().buildQueue.queueAsLowestPriority(BWAPI::UnitTypes::Zerg_Sunken_Colony);

		BuildManager::Instance().buildQueue.queueAsLowestPriority(BWAPI::UnitTypes::Zerg_Hydralisk_Den);	//21

		BuildManager::Instance().buildQueue.queueAsLowestPriority(BWAPI::UnitTypes::Zerg_Zergling);	//22
		BuildManager::Instance().buildQueue.queueAsLowestPriority(BWAPI::UnitTypes::Zerg_Zergling);	//23
		BuildManager::Instance().buildQueue.queueAsLowestPriority(BWAPI::UnitTypes::Zerg_Zergling);	//24

		BuildManager::Instance().buildQueue.queueAsLowestPriority(BWAPI::UnitTypes::Zerg_Hydralisk);	//25
		BuildManager::Instance().buildQueue.queueAsLowestPriority(BWAPI::UnitTypes::Zerg_Hydralisk);	//26
		BuildManager::Instance().buildQueue.queueAsLowestPriority(BWAPI::UnitTypes::Zerg_Hydralisk);	//27

		BuildManager::Instance().buildQueue.queueAsLowestPriority(BWAPI::UnitTypes::Zerg_Sunken_Colony);

		BuildManager::Instance().buildQueue.queueAsLowestPriority(BWAPI::UnitTypes::Zerg_Lair);
		BuildManager::Instance().buildQueue.queueAsLowestPriority(BWAPI::UnitTypes::Zerg_Evolution_Chamber, false); //26
		BuildManager::Instance().buildQueue.queueAsLowestPriority(BWAPI::UnitTypes::Zerg_Overlord);	// 다섯번째 오버로드

		BuildManager::Instance().buildQueue.queueAsLowestPriority(BWAPI::UnitTypes::Zerg_Drone);	//27
		BuildManager::Instance().buildQueue.queueAsLowestPriority(BWAPI::UnitTypes::Zerg_Drone);	//28
		BuildManager::Instance().buildQueue.queueAsLowestPriority(BWAPI::UnitTypes::Zerg_Drone);	//29
		BuildManager::Instance().buildQueue.queueAsLowestPriority(BWAPI::UnitTypes::Zerg_Drone);	//30
		BuildManager::Instance().buildQueue.queueAsLowestPriority(BWAPI::UnitTypes::Zerg_Drone);	//31
		BuildManager::Instance().buildQueue.queueAsLowestPriority(BWAPI::UnitTypes::Zerg_Drone);	//32

		BuildManager::Instance().buildQueue.queueAsLowestPriority(BWAPI::UnitTypes::Zerg_Extractor,
			BuildOrderItem::SeedPositionStrategy::FirstExpansionLocation); //31

		BuildManager::Instance().buildQueue.queueAsLowestPriority(BWAPI::UnitTypes::Zerg_Drone);	//32
		BuildManager::Instance().buildQueue.queueAsLowestPriority(BWAPI::UnitTypes::Zerg_Drone);	//33
		BuildManager::Instance().buildQueue.queueAsLowestPriority(BWAPI::UnitTypes::Zerg_Drone);	//34
		BuildManager::Instance().buildQueue.queueAsLowestPriority(BWAPI::UnitTypes::Zerg_Drone);	//35
		BuildManager::Instance().buildQueue.queueAsLowestPriority(BWAPI::UnitTypes::Zerg_Drone);	//36
		BuildManager::Instance().buildQueue.queueAsLowestPriority(BWAPI::UnitTypes::Zerg_Drone);	//37
	}
}

void StrategyManager::update()
{	
	/// 변수 값을 업데이트 합니다
	updateVariables();

	/// 일꾼을 계속 추가 생산합니다
	executeWorkerTraining();

	/// Supply DeadLock 예방 및 SupplyProvider 가 부족해질 상황 에 대한 선제적 대응으로서 SupplyProvider를 추가 건설/생산합니다
	executeSupplyManagement();

	/// 방어건물 및 공격유닛 생산 건물을 건설합니다
	executeBuildingConstruction();

	/// 업그레이드 및 테크 리서치를 실행합니다
	executeUpgradeAndTechResearch();

	/// 특수 유닛을 생산할 수 있도록 테크트리에 따라 건설을 실시합니다
	executeTechTreeUpConstruction();

	/// 공격유닛을 계속 추가 생산합니다
	executeCombatUnitTraining();

	/// 전반적인 전투 로직 을 갖고 전투를 수행합니다
	executeCombat();

	/// StrategyManager 의 수행상황을 표시합니다
	drawStrategyManagerStatus();

	// BasicBot 1.1 Patch Start ////////////////////////////////////////////////

	// 이번 게임의 로그를 남깁니다
	saveGameLog();

	// BasicBot 1.1 Patch End //////////////////////////////////////////////////
}


void StrategyManager::executeCombat()
{
	// 공격을 시작할만한 상황이 되기 전까지는 방어를 합니다
	if (combatState == CombatState::defenseMode) {
		
		/// 아군 공격유닛 들에게 방어를 지시합니다
		commandMyCombatUnitToDefense();

		/// 공격 모드로 전환할 때인지 여부를 판단합니다
		if (isTimeToStartAttack()) {
			combatState = CombatState::attackStarted;
		}
	}
	// 공격을 시작한 후에는 공격을 계속 실행하다가, 거의 적군 기지를 파괴하면 Eliminate 시키기를 합니다 
	else if (combatState == CombatState::attackStarted) {
		
		/// 아군 공격유닛 들에게 공격을 지시합니다
		commandMyCombatUnitToAttack();

		/// 방어 모드로 전환할 때인지 여부를 판단합니다			
		if (isTimeToStartDefense()) {
			combatState = CombatState::defenseMode;
		}

		/// 적군을 Eliminate 시키는 모드로 전환할지 여부를 판단합니다 
		if (isTimeToStartElimination()) {
			combatState = CombatState::eliminateEnemy;
		}
	}
	else if (combatState == CombatState::eliminateEnemy) {
		/// 적군을 Eliminate 시키도록 아군 공격 유닛들에게 지시합니다
		commandMyCombatUnitToEliminate();
	}
}

/// 공격 모드로 전환할 때인지 여부를 리턴합니다
bool StrategyManager::isTimeToStartAttack(){

	// 유닛 종류별로 최소 숫자 이상 있으면
	if (myCombatUnitType1List.size() >= necessaryNumberOfCombatUnitType1
		&& myCombatUnitType2List.size() >= necessaryNumberOfCombatUnitType2
		&& myCombatUnitType3List.size() >= necessaryNumberOfCombatUnitType3
		&& mySpecialUnitType1List.size() >= necessaryNumberOfSpecialUnitType1
		&& mySpecialUnitType2List.size() >= necessaryNumberOfSpecialUnitType2)
	{
		// 공격 유닛이 40 이상 있으면
		if (myCombatUnitType1List.size() + myCombatUnitType2List.size() + myCombatUnitType3List.size() > 40) {

			// 에너지 100 이상 갖고있는 특수 유닛이 존재하면 
			bool isSpecialUnitHasEnoughEnergy = false;
			for (BWAPI::Unit unit : mySpecialUnitType1List) {
				if (unit->getEnergy() > 100) {
					isSpecialUnitHasEnoughEnergy = true;
					break;
				}
			}
			for (BWAPI::Unit unit : mySpecialUnitType2List) {
				if (unit->getEnergy() > 100) {
					isSpecialUnitHasEnoughEnergy = true;
					break;
				}
			}
			if (isSpecialUnitHasEnoughEnergy) {
				return true;
			}
		}
	}

	return false;
}

/// 방어 모드로 전환할 때인지 여부를 리턴합니다
bool StrategyManager::isTimeToStartDefense() {

	// 공격 유닛 숫자가 10 미만으로 떨어지면 후퇴
	if (myCombatUnitType1List.size() + myCombatUnitType2List.size() + myCombatUnitType3List.size() < 10)
	{
		return true;
	}
	return false;
}

/// 적군을 Eliminate 시키는 모드로 전환할지 여부를 리턴합니다 
bool StrategyManager::isTimeToStartElimination(){

	// 적군 유닛을 많이 죽였고, 아군 서플라이가 100 을 넘었으면
	if (enemyKilledCombatUnitCount >= 20 && enemyKilledWorkerUnitCount >= 10 && myPlayer->supplyUsed() > 100 * 2) {

		// 적군 본진에 아군 유닛이 30 이상 도착했으면 거의 게임 끝난 것
		int myUnitCountAroundEnemyMainBaseLocation = 0;
		for (BWAPI::Unit unit : BWAPI::Broodwar->getUnitsInRadius(enemyMainBaseLocation->getPosition(), 8 * TILE_SIZE)) {
			if (unit->getPlayer() == myPlayer) {
				myUnitCountAroundEnemyMainBaseLocation++;
			}
		}
		if (myUnitCountAroundEnemyMainBaseLocation > 30) {
			return true;
		}
	}

	return false;
}

/// 아군 공격유닛 들에게 방어를 지시합니다
void StrategyManager::commandMyCombatUnitToDefense(){

	// 아군 방어 건물이 세워져있는 위치
	BWAPI::Position myDefenseBuildingPosition = BWAPI::Positions::None;
	
	switch (seedPositionStrategyOfMyDefenseBuildingType) {
		case BuildOrderItem::SeedPositionStrategy::MainBaseLocation: myDefenseBuildingPosition = myMainBaseLocation->getPosition(); break;
		case BuildOrderItem::SeedPositionStrategy::FirstChokePoint: myDefenseBuildingPosition = myFirstChokePoint->getCenter(); break;
		case BuildOrderItem::SeedPositionStrategy::FirstExpansionLocation: myDefenseBuildingPosition = myFirstExpansionLocation->getPosition(); break;
		case BuildOrderItem::SeedPositionStrategy::SecondChokePoint: myDefenseBuildingPosition = mySecondChokePoint->getCenter(); break;
		default: myDefenseBuildingPosition = myMainBaseLocation->getPosition(); break;
	}

	// 아군 공격유닛을 방어 건물이 세워져있는 위치로 배치시킵니다
	// 아군 공격유닛을 아군 방어 건물 뒤쪽에 배치시켰다가 적들이 방어 건물을 공격하기 시작했을 때 다함께 싸우게하면 더 좋을 것입니다
	for (BWAPI::Unit unit : myAllCombatUnitList) {

		if (unit == nullptr || unit->exists() == false) continue;

		bool hasCommanded = false;

		// 테란 종족 마린의 경우 마린을 벙커안에 집어넣기
		if (unit->getType() == BWAPI::UnitTypes::Terran_Marine) {
			for (BWAPI::Unit bunker : myDefenseBuildingType1List) {
				if (bunker->getLoadedUnits().size() < 4 && bunker->canLoad(unit)) {
					CommandUtil::rightClick(unit, bunker);
					hasCommanded = true;
				}
			}
		}

		if (unit->getType() == BWAPI::UnitTypes::Terran_Siege_Tank_Tank_Mode || unit->getType() == BWAPI::UnitTypes::Terran_Siege_Tank_Siege_Mode) {
			hasCommanded = controlSiegeTankUnitType(unit);
		}
		if (unit->getType() == BWAPI::UnitTypes::Zerg_Lurker) {
			hasCommanded = controlLurkerUnitType(unit);
		}
		if (unit->getType() == mySpecialUnitType1) {
			hasCommanded = controlSpecialUnitType1(unit);
		}
		if (unit->getType() == mySpecialUnitType2) {
			hasCommanded = controlSpecialUnitType2(unit);
		}

		// 따로 명령 내린 적이 없으면, 방어 건물 주위로 이동시킨다
		if (hasCommanded == false) {

			if (unit->isIdle()) {
				if (unit->canAttack()) {
					CommandUtil::attackMove(unit, myDefenseBuildingPosition);
				}
				else {
					CommandUtil::move(unit, myDefenseBuildingPosition);
				}
			}
		}
	}
}

/// 아군 공격 유닛들에게 공격을 지시합니다 
void StrategyManager::commandMyCombatUnitToAttack(){

	// 최종 타겟은 적군의 Main BaseLocation 
	BWTA::BaseLocation* targetEnemyBaseLocation = enemyMainBaseLocation;
	BWAPI::Position targetPosition = BWAPI::Positions::None;

	if (targetEnemyBaseLocation != nullptr)
	{
		// 테란 종족의 경우, 벙커 안에 있는 유닛은 밖으로 빼낸다
		if (myRace == BWAPI::Races::Terran) {
			for (auto & bunker : myDefenseBuildingType1List) {
				if (bunker->getLoadedUnits().size() > 0) {

					bool isThereSomeEnemyUnit = false;
					for (BWAPI::Unit someUnit : BWAPI::Broodwar->getUnitsInRadius(bunker->getPosition(), 6 * TILE_SIZE)) {
						if (someUnit->getPlayer() == enemyPlayer) {
							isThereSomeEnemyUnit = true;
							break;
						}
					}
					if (isThereSomeEnemyUnit == false) {
						bunker->unloadAll();
					}
				}
			}
		}

		// targetPosition 을 설정한다
		targetPosition = targetEnemyBaseLocation->getPosition();

		// 모든 아군 공격유닛들로 하여금 targetPosition 을 향해 공격하게 한다
		for (auto & unit : myAllCombatUnitList) {
			bool hasCommanded = false;

			if (unit->getType() == BWAPI::UnitTypes::Terran_Siege_Tank_Tank_Mode || unit->getType() == BWAPI::UnitTypes::Terran_Siege_Tank_Siege_Mode) {
				hasCommanded = controlSiegeTankUnitType(unit);
			}
			if (unit->getType() == BWAPI::UnitTypes::Zerg_Lurker) {
				hasCommanded = controlLurkerUnitType(unit);
			}
			if (unit->getType() == mySpecialUnitType1) {
				hasCommanded = controlSpecialUnitType1(unit);
			}
			if (unit->getType() == mySpecialUnitType2) {
				hasCommanded = controlSpecialUnitType2(unit);
			}

			// 따로 명령 내린 적이 없으면, targetPosition 을 향해 공격 이동시킵니다
			if (hasCommanded == false) {

				if (unit->isIdle()) {

					if (unit->canAttack()) {
						CommandUtil::attackMove(unit, targetPosition);
						hasCommanded = true;
					}
					else {
						// canAttack 기능이 없는 유닛타입 중 메딕은 마린 유닛에 대해 Heal 하러 가게 하고, 마린 유닛이 없으면 아군 지역으로 돌아오게 합니다
						if (unit->getType() == BWAPI::UnitTypes::Terran_Medic) {
							BWAPI::Position targetMyUnitPosition = BWAPI::Positions::None;
							for (BWAPI::Unit myUnit : myCombatUnitType1List) {
								if (myUnit == nullptr || myUnit->exists() == false || myUnit->getHitPoints() < 0) { continue; }

								if (myUnit->getHitPoints() < myUnit->getInitialHitPoints()
									|| rand() % 2 == 0)
								{
									targetMyUnitPosition = myUnit->getPosition();
									break;
								}
							}
							if (targetMyUnitPosition != BWAPI::Positions::None) {
								unit->useTech(BWAPI::TechTypes::Healing, targetMyUnitPosition);
								hasCommanded = true;
							}
							else {
								unit->useTech(BWAPI::TechTypes::Healing, mySecondChokePoint->getCenter());
								hasCommanded = true;
							}
						}
						// canAttack 기능이 없는 유닛타입 중 러커는 일반 공격유닛처럼 targetPosition 을 향해 이동시킵니다
						else if (unit->getType() == BWAPI::UnitTypes::Zerg_Lurker){
							CommandUtil::move(unit, targetPosition);
							hasCommanded = true;
						}
						// canAttack 기능이 없는 다른 유닛타입 (하이템플러, 옵저버, 사이언스베슬, 오버로드) 는
						// 따로 명령을 내린 적이 없으면 다른 공격유닛들과 동일하게 이동하도록 되어있습니다.
						else {
							CommandUtil::move(unit, targetPosition);
							hasCommanded = true;
						}
					}
				}
			}
		}
	}
}

/// 적군을 Eliminate 시키도록 아군 공격 유닛들에게 지시합니다
void StrategyManager::commandMyCombatUnitToEliminate(){

	if (enemyPlayer == nullptr || enemyRace == BWAPI::Races::Unknown)
	{
		return;
	}

	int mapHeight = BWAPI::Broodwar->mapHeight();	// 128
	int mapWidth = BWAPI::Broodwar->mapWidth();		// 128

	// 아군 공격 유닛들로 하여금 적군의 남은 건물을 알고 있으면 그것을 공격하게 하고, 그렇지 않으면 맵 전체를 랜덤하게 돌아다니도록 합니다
	BWAPI::Unit targetEnemyBuilding = nullptr;

	for (BWAPI::Unit enemyUnit : enemyPlayer->getUnits()) {
		if (enemyUnit == nullptr || enemyUnit->exists() == false || enemyUnit->getHitPoints() < 0) continue;
		if (enemyUnit->getType().isBuilding()) {
			targetEnemyBuilding = enemyUnit;
			break;
		}
	}

	for (BWAPI::Unit unit : myAllCombatUnitList) {

		bool hasCommanded = false;

		if (unit->getType() == BWAPI::UnitTypes::Terran_Siege_Tank_Tank_Mode || unit->getType() == BWAPI::UnitTypes::Terran_Siege_Tank_Siege_Mode) {
			hasCommanded = controlSiegeTankUnitType(unit);
		}
		if (unit->getType() == BWAPI::UnitTypes::Zerg_Lurker) {
			hasCommanded = controlLurkerUnitType(unit);
		}
		if (unit->getType() == mySpecialUnitType1) {
			hasCommanded = controlSpecialUnitType1(unit);
		}
		if (unit->getType() == mySpecialUnitType2) {
			hasCommanded = controlSpecialUnitType2(unit);
		}

		// 따로 명령 내린 적이 없으면, 적군의 남은 건물 혹은 랜덤 위치로 이동시킨다
		if (hasCommanded == false) {

			if (unit->isIdle()) {

				BWAPI::Position targetPosition = BWAPI::Positions::None;
				if (targetEnemyBuilding != nullptr) {
					targetPosition = targetEnemyBuilding->getPosition();
				}
				else {
					targetPosition = BWAPI::Position(rand() % (mapWidth * TILE_SIZE), rand() % (mapHeight * TILE_SIZE));
				}

				if (unit->canAttack()) {
					CommandUtil::attackMove(unit, targetPosition);
					hasCommanded = true;
				}
				else {
					// canAttack 기능이 없는 유닛타입 중 메딕은 마린 유닛에 대해 Heal 하러 가게 하고, 마린 유닛이 없으면 아군 지역으로 돌아오게 합니다
					if (unit->getType() == BWAPI::UnitTypes::Terran_Medic) {
						BWAPI::Position targetMyUnitPosition = BWAPI::Positions::None;
						for (BWAPI::Unit myUnit : myCombatUnitType1List) {
							if (myUnit == nullptr || myUnit->exists() == false || myUnit->getHitPoints() < 0) { continue; }

							if (myUnit->getHitPoints() < myUnit->getInitialHitPoints()
								|| rand() % 2 == 0)
							{
								targetMyUnitPosition = myUnit->getPosition();
								break;
							}
						}
						if (targetMyUnitPosition != BWAPI::Positions::None) {
							unit->useTech(BWAPI::TechTypes::Healing, targetMyUnitPosition);
							hasCommanded = true;
						}
						else {
							unit->useTech(BWAPI::TechTypes::Healing, mySecondChokePoint->getCenter());
							hasCommanded = true;
						}
					}
					// canAttack 기능이 없는 유닛타입 중 러커는 일반 공격유닛처럼 targetPosition 을 향해 이동시킵니다
					else if (unit->getType() == BWAPI::UnitTypes::Zerg_Lurker){
						CommandUtil::move(unit, targetPosition);
						hasCommanded = true;
					}
					// canAttack 기능이 없는 다른 유닛타입 (하이템플러, 옵저버, 사이언스베슬, 오버로드) 는
					// 따로 명령을 내린 적이 없으면 다른 공격유닛들과 동일하게 이동하도록 되어있습니다.
					else {
						CommandUtil::move(unit, targetPosition);
						hasCommanded = true;
					}
				}
			}
		}
	}
}


/// 시즈탱크 유닛에 대해 컨트롤 명령을 내립니다
bool StrategyManager::controlSiegeTankUnitType(BWAPI::Unit unit){

	bool hasCommanded = false;

	// defenseMode 일 경우
	if (combatState == CombatState::defenseMode) {

		// 아군 방어 건물이 세워져있는 위치 주위에 시즈모드 시켜놓는다
		BWAPI::Position myDefenseBuildingPosition = BWAPI::Positions::None;
		switch (seedPositionStrategyOfMyDefenseBuildingType) {
			case BuildOrderItem::SeedPositionStrategy::MainBaseLocation: myDefenseBuildingPosition = myMainBaseLocation->getPosition(); break;
			case BuildOrderItem::SeedPositionStrategy::FirstChokePoint: myDefenseBuildingPosition = myFirstChokePoint->getCenter(); break;
			case BuildOrderItem::SeedPositionStrategy::FirstExpansionLocation: myDefenseBuildingPosition = myFirstExpansionLocation->getPosition(); break;
			case BuildOrderItem::SeedPositionStrategy::SecondChokePoint: myDefenseBuildingPosition = mySecondChokePoint->getCenter(); break;
			default: myDefenseBuildingPosition = myMainBaseLocation->getPosition(); break;
		}

		if (myDefenseBuildingPosition != BWAPI::Positions::None) {
			if (unit->isSieged() == false) {
				if (unit->getDistance(myDefenseBuildingPosition) < 5 * TILE_SIZE) {
					unit->siege();
					hasCommanded = true;
				}
			}
		}
	}
	else {
		// 적이 근처에 있으면 시즈모드 시키고, 없으면 시즈모드를 해제한다
		BWAPI::Position nearEnemyUnitPosition = BWAPI::Positions::None;
		double tempDistance = 0;
		for (BWAPI::Unit enemyUnit : BWAPI::Broodwar->enemy()->getUnits()) {

			if (enemyUnit->isFlying() || enemyUnit->exists() == false) continue;

			tempDistance = unit->getDistance(enemyUnit->getPosition());
			if (tempDistance < 12 * TILE_SIZE) {
				nearEnemyUnitPosition = enemyUnit->getPosition();
			}
		}

		if (unit->isSieged() == false) {
			if (nearEnemyUnitPosition != BWAPI::Positions::None) {
				unit->siege();
				hasCommanded = true;
			}
		}
		else {
			if (nearEnemyUnitPosition == BWAPI::Positions::None) {
				unit->unsiege();
				hasCommanded = true;
			}
		}
	}

	return hasCommanded;
}

/// 러커 유닛에 대해 컨트롤 명령을 내립니다
bool StrategyManager::controlLurkerUnitType(BWAPI::Unit unit){

	bool hasCommanded = false;

	// defenseMode 일 경우
	if (combatState == CombatState::defenseMode) {

		// 아군 방어 건물이 세워져있는 위치 주위에 버로우시켜놓는다
		BWAPI::Position myDefenseBuildingPosition = BWAPI::Positions::None;
		switch (seedPositionStrategyOfMyDefenseBuildingType) {
			case BuildOrderItem::SeedPositionStrategy::MainBaseLocation: myDefenseBuildingPosition = myMainBaseLocation->getPosition(); break;
			case BuildOrderItem::SeedPositionStrategy::FirstChokePoint: myDefenseBuildingPosition = myFirstChokePoint->getCenter(); break;
			case BuildOrderItem::SeedPositionStrategy::FirstExpansionLocation: myDefenseBuildingPosition = myFirstExpansionLocation->getPosition(); break;
			case BuildOrderItem::SeedPositionStrategy::SecondChokePoint: myDefenseBuildingPosition = mySecondChokePoint->getCenter(); break;
			default: myDefenseBuildingPosition = myMainBaseLocation->getPosition(); break;
		}


		if (myDefenseBuildingPosition != BWAPI::Positions::None) {
			if (unit->isBurrowed() == false) {
				if (unit->getDistance(myDefenseBuildingPosition) < 5 * TILE_SIZE) {
					unit->burrow();
					hasCommanded = true;
				}
			}
		}
	}
	// defenseMode 가 아닐 경우
	else {

		// 근처에 적 유닛이 있으면 버로우 시키고, 없으면 언버로우 시킨다
		BWAPI::Position nearEnemyUnitPosition = BWAPI::Positions::None;
		double tempDistance = 0;
		for (BWAPI::Unit enemyUnit : BWAPI::Broodwar->enemy()->getUnits()) {

			if (enemyUnit->isFlying()) continue;

			tempDistance = unit->getDistance(enemyUnit->getPosition());
			if (tempDistance < 6 * TILE_SIZE) {
				nearEnemyUnitPosition = enemyUnit->getPosition();
			}
		}

		if (unit->isBurrowed() == false) {

			if (nearEnemyUnitPosition != BWAPI::Positions::None) {
				unit->burrow();
				hasCommanded = true;
			}
		}
		else {
			if (nearEnemyUnitPosition == BWAPI::Positions::None) {
				unit->unburrow();
				hasCommanded = true;
			}
		}
	}

	return hasCommanded;
}

/// 첫번째 특수 유닛 타입의 유닛에 대해 컨트롤 명령을 입력합니다
bool StrategyManager::controlSpecialUnitType1(BWAPI::Unit unit) {

	///////////////////////////////////////////////////////////////////
	///////////////////////// 아래의 코드를 수정해보세요 ///////////////////////
	//
	// TODO 1. 아군 옵저버/사이언스베슬/오버로드를 공격 유닛들과 함께 이동하게 하는 로직  (예상 개발시간 10분)
	//
	// 목표 : 첫번째 특수유닛 타입은 적군 투명 유닛 탐지 능력이 있고 시야가 넓은 옵저버/사이언스베슬/오버로드 입니다. 
	// 
	//      현재는 아군 옵저버/사이언스베슬/오버로드에게 따로 컨트롤 명령을 입력하지 않으면 
	//      다른 공격유닛들과 동일하게 적군 본진을 향해 이동하도록 되어있습니다.  
	//
	//      그러나 이렇게하면 적군 유닛들이 있는데도 무시하고 계속 이동하다가 사망하게 됩니다
	//
	//      아군 공격 유닛들의 목록 myCombatUnitType1List, myCombatUnitType2List, myCombatUnitType3List  
	//		을 사용해서, 다른 아군 공격 유닛들과 함께 다니도록 해보세요
	// 
	//      return false = 유닛에게 따로 컨트롤 명령을 입력하지 않음  -> 다른 공격유닛과 동일하게 이동하도록 합니다 
	//      return true = 유닛에게 따로 컨트롤 명령을 입력했음
	// 
	// Hint : myCombatUnitType1List 에서 랜덤하게 한 유닛을 선택해서 그 유닛을 따라다니게 하면 어떨까요?  
	//
	///////////////////////////////////////////////////////////////////

	bool hasCommanded = false;
	if (unit->getType() == BWAPI::UnitTypes::Protoss_Observer) {

		BWAPI::Position targetPosition = BWAPI::Positions::None;

		// targetPosition 을 적절히 정해서 이동시켜보세요

	}
	else if (unit->getType() == BWAPI::UnitTypes::Terran_Science_Vessel) {

		BWAPI::Position targetPosition = BWAPI::Positions::None;

		// targetPosition 을 적절히 정해서 이동시켜보세요

		if (unit->getEnergy() >= BWAPI::TechTypes::Defensive_Matrix.energyCost()) {

			BWAPI::Unit targetMyUnit = nullptr;

			// targetMyUnit 을 적절히 정해보세요

			if (targetMyUnit != nullptr) {
				unit->useTech(BWAPI::TechTypes::Defensive_Matrix, targetMyUnit);
				hasCommanded = true;
			}
		}

		if (unit->getEnergy() >= BWAPI::TechTypes::Irradiate.energyCost() && myPlayer->hasResearched(BWAPI::TechTypes::Irradiate)) {

			BWAPI::Unit targetEnemyUnit = nullptr;

			// targetEnemyUnit 을 적절히 정해보세요

			if (targetEnemyUnit != nullptr) {
				unit->useTech(BWAPI::TechTypes::Irradiate, targetEnemyUnit);
				hasCommanded = true;
			}
		}

	}
	else if (unit->getType() == BWAPI::UnitTypes::Zerg_Overlord) {

		BWAPI::Position targetPosition = BWAPI::Positions::None;

		// targetPosition 을 적절히 정해서 이동시켜보세요
	}

	return hasCommanded;
}

/// 두번째 특수 유닛 타입의 유닛에 대해 컨트롤 명령을 내립니다
bool StrategyManager::controlSpecialUnitType2(BWAPI::Unit unit) {

	///////////////////////////////////////////////////////////////////
	///////////////////////// 아래의 코드를 수정해보세요 ///////////////////////
	//
	// TODO 2. 아군 하이템플러/배틀크루저/디파일러가 특수 기술을 사용하게 하는 로직       (예상 개발시간 20분)
	//
	// 목표 : 두번째 특수유닛 타입은 특수 기술을 갖고있는 하이템플러/배틀크루저/디파일러 입니다. 
	//
	//      현재는 특수기술 사용 대상을 정하는 로직이 구현 안되어있습니다.
	//
	//      적군 유닛들의 목록 BWAPI::Broodwar->enemy()->getUnits() 을 사용하여
	//      특수 기술 사용 대상을 적절히 정하도록 해보세요
	// 
	//      return false = 유닛에게 따로 컨트롤 명령을 입력하지 않음  -> 다른 공격유닛과 동일하게 이동하도록 합니다
	//      return true = 유닛에게 따로 컨트롤 명령을 입력했음
	// 
	// 추가 : 테란 종족 첫번째 특수유닛 타입 사이언스베슬에 대해서도 특수 기술을 사용하게 하려면
	//		controlSpecialUnitType1 함수를 수정하시면 됩니다
	// 
	///////////////////////////////////////////////////////////////////

	bool hasCommanded = false;

	// 프로토스 종족 하이템플러의 경우 
	if (unit->getType() == BWAPI::UnitTypes::Protoss_High_Templar) {

		if (unit->getEnergy() >= BWAPI::TechTypes::Psionic_Storm.energyCost() && myPlayer->hasResearched(BWAPI::TechTypes::Psionic_Storm)) {

			BWAPI::Position targetPosition = BWAPI::Positions::None;

			// targetPosition 을 적절히 정해보세요

			if (targetPosition != BWAPI::Positions::None) {
				unit->useTech(BWAPI::TechTypes::Psionic_Storm, targetPosition);
				hasCommanded = true;
			}
		}
	}
	else if (unit->getType() == BWAPI::UnitTypes::Terran_Battlecruiser) {

		if (unit->getEnergy() >= BWAPI::TechTypes::Yamato_Gun.energyCost() && myPlayer->hasResearched(BWAPI::TechTypes::Yamato_Gun)) {

			BWAPI::Unit targetEnemyUnit = nullptr;

			// targetEnemyUnit 을 적절히 정해보세요

			if (targetEnemyUnit != nullptr) {
				unit->useTech(BWAPI::TechTypes::Yamato_Gun, targetEnemyUnit);
				hasCommanded = true;
			}
		}
	}
	else if (unit->getType() == BWAPI::UnitTypes::Zerg_Defiler) {

		if (unit->getEnergy() < 200 && myPlayer->hasResearched(BWAPI::TechTypes::Consume)) {

			BWAPI::Unit targetMyUnit = nullptr;

			// 가장 가까운 저글링을 컨슘 한다
			double minDistance = 1000000000;
			double tempDistance = 0;
			for (BWAPI::Unit zerglingUnit : myCombatUnitType1List) {
				tempDistance = unit->getDistance(zerglingUnit->getPosition());
				if (minDistance > tempDistance) {
					minDistance = tempDistance;
					targetMyUnit = zerglingUnit;
				}
			}

			if (targetMyUnit != nullptr) {
				unit->useTech(BWAPI::TechTypes::Consume, targetMyUnit);
				hasCommanded = true;
			}
		}

		if (unit->getEnergy() >= BWAPI::TechTypes::Plague.energyCost() && myPlayer->hasResearched(BWAPI::TechTypes::Plague)) {

			BWAPI::Unit targetEnemyUnit = nullptr;

			// targetEnemyUnit 을 적절히 정해보세요

			if (targetEnemyUnit != nullptr) {
				unit->useTech(BWAPI::TechTypes::Plague, targetEnemyUnit);
				hasCommanded = true;
			}
		}
		else if (unit->getEnergy() >= BWAPI::TechTypes::Dark_Swarm.energyCost()) {

			BWAPI::Position targetPosition = BWAPI::Positions::None;

			// targetPosition 을 적절히 정해보세요

			if (targetPosition != BWAPI::Positions::None) {
				unit->useTech(BWAPI::TechTypes::Dark_Swarm, targetPosition);
				hasCommanded = true;
			}
		}
	}

	return hasCommanded;
}

/// StrategyManager 의 수행상황을 표시합니다
void StrategyManager::drawStrategyManagerStatus() {

	int y = 250;

	// 아군 공격유닛 숫자 및 적군 공격유닛 숫자
	BWAPI::Broodwar->drawTextScreen(200, y, "My %s", myCombatUnitType1.getName().c_str());
	BWAPI::Broodwar->drawTextScreen(350, y, "alive %d", myCombatUnitType1List.size());
	BWAPI::Broodwar->drawTextScreen(400, y, "killed %d", myKilledCombatUnitCount1);
	y += 10;
	BWAPI::Broodwar->drawTextScreen(200, y, "My %s", myCombatUnitType2.getName().c_str());
	BWAPI::Broodwar->drawTextScreen(350, y, "alive %d", myCombatUnitType2List.size());
	BWAPI::Broodwar->drawTextScreen(400, y, "killed %d", myKilledCombatUnitCount2);
	y += 10;
	BWAPI::Broodwar->drawTextScreen(200, y, "My %s", myCombatUnitType3.getName().c_str());
	BWAPI::Broodwar->drawTextScreen(350, y, "alive %d", myCombatUnitType3List.size());
	BWAPI::Broodwar->drawTextScreen(400, y, "killed %d", myKilledCombatUnitCount3);
	y += 10;
	BWAPI::Broodwar->drawTextScreen(200, y, "My %s", mySpecialUnitType1.getName().c_str());
	BWAPI::Broodwar->drawTextScreen(350, y, "alive %d", mySpecialUnitType1List.size());
	BWAPI::Broodwar->drawTextScreen(400, y, "killed %d", myKilledSpecialUnitCount1);
	y += 10;
	BWAPI::Broodwar->drawTextScreen(200, y, "My %s", mySpecialUnitType2.getName().c_str());
	BWAPI::Broodwar->drawTextScreen(350, y, "alive %d", mySpecialUnitType2List.size());
	BWAPI::Broodwar->drawTextScreen(400, y, "killed %d", myKilledSpecialUnitCount2);
	y += 20;

	BWAPI::Broodwar->drawTextScreen(200, y, "Enemy CombatUnit");
	BWAPI::Broodwar->drawTextScreen(350, y, "alive %d", numberOfCompletedEnemyCombatUnit);
	BWAPI::Broodwar->drawTextScreen(400, y, "killed %d", enemyKilledCombatUnitCount);
	y += 10;
	BWAPI::Broodwar->drawTextScreen(200, y, "Enemy WorkerUnit");
	BWAPI::Broodwar->drawTextScreen(350, y, "alive %d", numberOfCompletedEnemyWorkerUnit);
	BWAPI::Broodwar->drawTextScreen(400, y, "killed %d", enemyKilledWorkerUnitCount);
	y += 20;

	// setInitialBuildOrder 에서 입력한 빌드오더가 다 끝나서 빌드오더큐가 empty 되었는지 여부
	BWAPI::Broodwar->drawTextScreen(200, y, "isInitialBuildOrderFinished %d", isInitialBuildOrderFinished);
	y += 10;
	// 전투 상황
	BWAPI::Broodwar->drawTextScreen(200, y, "combatState %d", combatState);
}






























void StrategyManager::onEnd(bool isWinner)
{
	// BasicBot 1.1 Patch Start ////////////////////////////////////////////////
	// 경기 결과 파일 Save / Load 및 로그파일 Save 예제 추가

	// 과거 게임 기록 + 이번 게임 기록을 저장합니다
	saveGameRecordList(isWinner);

	// BasicBot 1.1 Patch End //////////////////////////////////////////////////
}



/// 변수 값을 업데이트 합니다 
void StrategyManager::updateVariables(){

	enemyRace = InformationManager::Instance().enemyRace;

	if (BuildManager::Instance().buildQueue.isEmpty()) {
		isInitialBuildOrderFinished = true;
	}

	// 적군의 공격유닛 숫자
	numberOfCompletedEnemyCombatUnit = 0;
	numberOfCompletedEnemyWorkerUnit = 0;
	for (auto & unitInfoEntry : InformationManager::Instance().getUnitAndUnitInfoMap(enemyPlayer)) {
		UnitInfo & enemyUnitInfo = unitInfoEntry.second;
		if (enemyUnitInfo.type.isWorker() == false && enemyUnitInfo.type.isBuilding() == false) {
			numberOfCompletedEnemyCombatUnit++;
		}
		if (enemyUnitInfo.type.isWorker() == true) {
			numberOfCompletedEnemyWorkerUnit++;
		}
	}


	// 아군 / 적군의 본진, 첫번째 길목, 두번째 길목
	myMainBaseLocation = InformationManager::Instance().getMainBaseLocation(myPlayer);
	myFirstExpansionLocation = InformationManager::Instance().getFirstExpansionLocation(myPlayer);
	myFirstChokePoint = InformationManager::Instance().getFirstChokePoint(myPlayer);
	mySecondChokePoint = InformationManager::Instance().getSecondChokePoint(myPlayer);
	enemyMainBaseLocation = InformationManager::Instance().getMainBaseLocation(enemyPlayer);
	enemyFirstExpansionLocation = InformationManager::Instance().getFirstExpansionLocation(enemyPlayer);
	enemyFirstChokePoint = InformationManager::Instance().getFirstChokePoint(enemyPlayer);
	enemySecondChokePoint = InformationManager::Instance().getSecondChokePoint(enemyPlayer);

	// 아군 방어 건물 목록, 공격 유닛 목록
	myDefenseBuildingType1List.clear();
	myDefenseBuildingType2List.clear();
	myAllCombatUnitList.clear();
	myCombatUnitType1List.clear();
	myCombatUnitType2List.clear();
	myCombatUnitType3List.clear();
	mySpecialUnitType1List.clear();
	mySpecialUnitType2List.clear();

	for (BWAPI::Unit unit : myPlayer->getUnits()) {

		if (unit == nullptr || unit->exists() == false || unit->getHitPoints() <= 0) continue;

		if (unit->getType() == myCombatUnitType1) {
			myCombatUnitType1List.push_back(unit);
			myAllCombatUnitList.push_back(unit);
		}
		else if (unit->getType() == myCombatUnitType2) {
			myCombatUnitType2List.push_back(unit);
			myAllCombatUnitList.push_back(unit);
		}
		else if (unit->getType() == myCombatUnitType3 || unit->getType() == BWAPI::UnitTypes::Terran_Siege_Tank_Siege_Mode) {
			myCombatUnitType3List.push_back(unit);
			myAllCombatUnitList.push_back(unit);
		}
		else if (unit->getType() == mySpecialUnitType1) {
			// maxNumberOfSpecialUnitType1 숫자까지만 특수유닛 부대에 포함시킨다 (저그 종족의 경우 오버로드가 전부 전투참여했다가 위험해질 수 있으므로)
			if (mySpecialUnitType1List.size() < maxNumberOfSpecialUnitType1) {
				mySpecialUnitType1List.push_back(unit);
				myAllCombatUnitList.push_back(unit);
			}
		}
		else if (unit->getType() == mySpecialUnitType2) {
			// maxNumberOfSpecialUnitType2 숫자까지만 특수유닛 부대에 포함시킨다
			if (mySpecialUnitType2List.size() < maxNumberOfSpecialUnitType2) {
				mySpecialUnitType2List.push_back(unit);
				myAllCombatUnitList.push_back(unit);
			}
		}
		else if (unit->getType() == myDefenseBuildingType1) {
			myDefenseBuildingType1List.push_back(unit);
		}
		else if (unit->getType() == myDefenseBuildingType2) {
			myDefenseBuildingType2List.push_back(unit);
		}
	}
}

/// 아군 / 적군 공격 유닛 사망 유닛 숫자 누적값을 업데이트 합니다
void StrategyManager::onUnitDestroy(BWAPI::Unit unit) {
	if (unit->getType().isNeutral()) {
		return;
	}

	if (unit->getPlayer() == myPlayer) {
		if (unit->getType() == myCombatUnitType1) {
			myKilledCombatUnitCount1++;
		}
		else if (unit->getType() == myCombatUnitType2) {
			myKilledCombatUnitCount2++;
		}
		else if (unit->getType() == myCombatUnitType3) {
			myKilledCombatUnitCount3++;
		}
		else if (myCombatUnitType3 == BWAPI::UnitTypes::Terran_Siege_Tank_Tank_Mode && unit->getType() == BWAPI::UnitTypes::Terran_Siege_Tank_Siege_Mode) {
			myKilledCombatUnitCount3++;
		}
		else if (unit->getType() == mySpecialUnitType1) {
			myKilledSpecialUnitCount1++;
		}
		else if (unit->getType() == mySpecialUnitType2) {
			myKilledSpecialUnitCount2++;
		}
	}
	else if (unit->getPlayer() == enemyPlayer) {
		/// 적군 공격 유닛타입의 사망 유닛 숫자 누적값
		if (unit->getType().isWorker() == false && unit->getType().isBuilding() == false) {
			enemyKilledCombatUnitCount++;
		}
		/// 적군 일꾼 유닛타입의 사망 유닛 숫자 누적값
		if (unit->getType().isWorker() == true) {
			enemyKilledWorkerUnitCount++;
		}
	}
}

// 일꾼 계속 추가 생산
void StrategyManager::executeWorkerTraining()
{
	// InitialBuildOrder 진행중에는 아무것도 하지 않습니다
	if (isInitialBuildOrderFinished == false) {
		return;
	}

	if (BWAPI::Broodwar->self()->minerals() >= 50) {
		// workerCount = 현재 일꾼 수 + 생산중인 일꾼 수
		int workerCount = BWAPI::Broodwar->self()->allUnitCount(InformationManager::Instance().getWorkerType());

		if (BWAPI::Broodwar->self()->getRace() == BWAPI::Races::Zerg) {

			for (auto & unit : BWAPI::Broodwar->self()->getUnits())
			{
				if (unit->getType() == BWAPI::UnitTypes::Zerg_Egg) {
					// Zerg_Egg 에게 morph 명령을 내리면 isMorphing = true, isBeingConstructed = true, isConstructing = true 가 된다
					// Zerg_Egg 가 다른 유닛으로 바뀌면서 새로 만들어진 유닛은 잠시 isBeingConstructed = true, isConstructing = true 가 되었다가, 
					if (unit->isMorphing() && unit->getBuildType() == BWAPI::UnitTypes::Zerg_Drone) {
						workerCount++;
					}
				}
			}
		}
		else {
			for (auto & unit : BWAPI::Broodwar->self()->getUnits())
			{
				if (unit->getType().isResourceDepot())
				{
					if (unit->isTraining()) {
						workerCount += unit->getTrainingQueue().size();
					}
				}
			}
		}

		// 최적의 일꾼 수 = 미네랄 * (1~1.5) + 가스 * 3
		int optimalWorkerCount = 0;
		for (BWTA::BaseLocation* baseLocation : InformationManager::Instance().getOccupiedBaseLocations(myPlayer)) {
			optimalWorkerCount += (int)(baseLocation->getMinerals().size() * 1.5);
			optimalWorkerCount += baseLocation->getGeysers().size() * 3;
		}
		if (workerCount < optimalWorkerCount) {
			for (auto & unit : BWAPI::Broodwar->self()->getUnits())
			{
				if (unit->getType() == BWAPI::UnitTypes::Protoss_Nexus || unit->getType() == BWAPI::UnitTypes::Terran_Command_Center || unit->getType() == BWAPI::UnitTypes::Zerg_Larva) {
					if (unit->isTraining() == false && unit->isMorphing() == false) {
						// 빌드큐에 일꾼 생산이 1개는 있도록 한다
						if (BuildManager::Instance().buildQueue.getItemCount(InformationManager::Instance().getWorkerType()) == 0) {
							//std::cout << "worker enqueue" << std::endl;
							BuildManager::Instance().buildQueue.queueAsLowestPriority(MetaType(InformationManager::Instance().getWorkerType()), false);
						}
					}
				}
			}
		}
	}
}

// Supply DeadLock 예방 및 SupplyProvider 가 부족해질 상황 에 대한 선제적 대응으로서 SupplyProvider를 추가 건설/생산한다
void StrategyManager::executeSupplyManagement()
{
	// BasicBot 1.1 Patch Start ////////////////////////////////////////////////
	// 가이드 추가 및 콘솔 출력 명령 주석 처리

	// InitialBuildOrder 진행중 혹은 그후라도 서플라이 건물이 파괴되어 데드락이 발생할 수 있는데, 이 상황에 대한 해결은 참가자께서 해주셔야 합니다.
	// 오버로드가 학살당하거나, 서플라이 건물이 집중 파괴되는 상황에 대해  무조건적으로 서플라이 빌드 추가를 실행하기 보다 먼저 전략적 대책 판단이 필요할 것입니다

	// BWAPI::Broodwar->self()->supplyUsed() > BWAPI::Broodwar->self()->supplyTotal()  인 상황이거나
	// BWAPI::Broodwar->self()->supplyUsed() + 빌드매니저 최상단 훈련 대상 유닛의 unit->getType().supplyRequired() > BWAPI::Broodwar->self()->supplyTotal() 인 경우
	// 서플라이 추가를 하지 않으면 더이상 유닛 훈련이 안되기 때문에 deadlock 상황이라고 볼 수도 있습니다.
	// 저그 종족의 경우 일꾼을 건물로 Morph 시킬 수 있기 때문에 고의적으로 이런 상황을 만들기도 하고, 
	// 전투에 의해 유닛이 많이 죽을 것으로 예상되는 상황에서는 고의적으로 서플라이 추가를 하지 않을수도 있기 때문에
	// 참가자께서 잘 판단하셔서 개발하시기 바랍니다.

	// InitialBuildOrder 진행중에는 아무것도 하지 않습니다
	// InitialBuildOrder 진행중이라도 supplyUsed 가 supplyTotal 보다 커져버리면 실행하도록 합니다
	if (isInitialBuildOrderFinished == false && BWAPI::Broodwar->self()->supplyUsed() < BWAPI::Broodwar->self()->supplyTotal()) {
		return;
	}

	// 1초에 한번만 실행
	if (BWAPI::Broodwar->getFrameCount() % 24 != 0) {
		return;
	}

	// 게임에서는 서플라이 값이 200까지 있지만, BWAPI 에서는 서플라이 값이 400까지 있다
	// 저글링 1마리가 게임에서는 서플라이를 0.5 차지하지만, BWAPI 에서는 서플라이를 1 차지한다
	if (BWAPI::Broodwar->self()->supplyTotal() <= 400)
	{
		// 서플라이가 다 꽉찼을때 새 서플라이를 지으면 지연이 많이 일어나므로, supplyMargin (게임에서의 서플라이 마진 값의 2배)만큼 부족해지면 새 서플라이를 짓도록 한다
		// 이렇게 값을 정해놓으면, 게임 초반부에는 서플라이를 너무 일찍 짓고, 게임 후반부에는 서플라이를 너무 늦게 짓게 된다
		int supplyMargin = 12;

		// currentSupplyShortage 를 계산한다
		int currentSupplyShortage = BWAPI::Broodwar->self()->supplyUsed() + supplyMargin - BWAPI::Broodwar->self()->supplyTotal();

		if (currentSupplyShortage > 0) {

			// 생산/건설 중인 Supply를 센다
			int onBuildingSupplyCount = 0;

			// 저그 종족인 경우, 생산중인 Zerg_Overlord (Zerg_Egg) 를 센다. Hatchery 등 건물은 세지 않는다
			if (BWAPI::Broodwar->self()->getRace() == BWAPI::Races::Zerg) {
				for (auto & unit : BWAPI::Broodwar->self()->getUnits())
				{					
					if (unit->getType() == BWAPI::UnitTypes::Zerg_Egg && unit->getBuildType() == BWAPI::UnitTypes::Zerg_Overlord) {
						onBuildingSupplyCount += BWAPI::UnitTypes::Zerg_Overlord.supplyProvided();
					}
					// 갓태어난 Overlord 는 아직 SupplyTotal 에 반영안되어서, 추가 카운트를 해줘야함 
					if (unit->getType() == BWAPI::UnitTypes::Zerg_Overlord && unit->isConstructing()) {
						onBuildingSupplyCount += BWAPI::UnitTypes::Zerg_Overlord.supplyProvided();
					}
				}
			}
			// 저그 종족이 아닌 경우, 건설중인 Protoss_Pylon, Terran_Supply_Depot 를 센다. Nexus, Command Center 등 건물은 세지 않는다
			else {
				onBuildingSupplyCount += ConstructionManager::Instance().getConstructionQueueItemCount(InformationManager::Instance().getBasicSupplyProviderUnitType()) * InformationManager::Instance().getBasicSupplyProviderUnitType().supplyProvided();
			}

			// 주석처리
			//std::cout << "currentSupplyShortage : " << currentSupplyShortage << " onBuildingSupplyCount : " << onBuildingSupplyCount << std::endl;

			if (currentSupplyShortage > onBuildingSupplyCount) {

				// BuildQueue 최상단에 SupplyProvider 가 있지 않으면 enqueue 한다
				bool isToEnqueue = true;
				if (!BuildManager::Instance().buildQueue.isEmpty()) {
					BuildOrderItem currentItem = BuildManager::Instance().buildQueue.getHighestPriorityItem();
					if (currentItem.metaType.isUnit() && currentItem.metaType.getUnitType() == InformationManager::Instance().getBasicSupplyProviderUnitType()) {
						isToEnqueue = false;
					}
				}
				if (isToEnqueue) {
					// 주석처리
					//std::cout << "enqueue supply provider " << InformationManager::Instance().getBasicSupplyProviderUnitType().getName().c_str() << std::endl;
					BuildManager::Instance().buildQueue.queueAsHighestPriority(MetaType(InformationManager::Instance().getBasicSupplyProviderUnitType()), true);
				}
			}

		}
	}
	// BasicBot 1.1 Patch End ////////////////////////////////////////////////
}



/// 방어건물 및 공격유닛 생산 건물을 건설합니다
void StrategyManager::executeBuildingConstruction() {

	// InitialBuildOrder 진행중에는 아무것도 하지 않습니다
	if (isInitialBuildOrderFinished == false) {
		return;
	}

	// 1초에 한번만 실행
	if (BWAPI::Broodwar->getFrameCount() % 24 != 0) {
		return;
	}

	bool			isPossibleToConstructDefenseBuildingType1 = false;
	bool			isPossibleToConstructDefenseBuildingType2 = false;
	bool			isPossibleToConstructCombatUnitTrainingBuildingType = false;

	// 방어 건물 증설을 우선적으로 실시한다

	// 현재 방어 건물 갯수
	int numberOfMyDefenseBuildingType1 = 0;
	int numberOfMyDefenseBuildingType2 = 0;

	if (myRace == BWAPI::Races::Protoss) {
		numberOfMyDefenseBuildingType1 += myPlayer->allUnitCount(myDefenseBuildingType1);
		numberOfMyDefenseBuildingType1 += BuildManager::Instance().buildQueue.getItemCount(myDefenseBuildingType1);
		numberOfMyDefenseBuildingType1 += ConstructionManager::Instance().getConstructionQueueItemCount(myDefenseBuildingType1);
		numberOfMyDefenseBuildingType2 += myPlayer->allUnitCount(myDefenseBuildingType2);
		numberOfMyDefenseBuildingType2 += BuildManager::Instance().buildQueue.getItemCount(myDefenseBuildingType2);
		numberOfMyDefenseBuildingType2 += ConstructionManager::Instance().getConstructionQueueItemCount(myDefenseBuildingType2);

		isPossibleToConstructDefenseBuildingType1 = true;
		if (myPlayer->completedUnitCount(BWAPI::UnitTypes::Protoss_Forge) > 0) {
			isPossibleToConstructDefenseBuildingType2 = true;
		}
		if (myPlayer->completedUnitCount(BWAPI::UnitTypes::Protoss_Pylon) > 0) {
			isPossibleToConstructCombatUnitTrainingBuildingType = true;
		}

	}
	else if (myRace == BWAPI::Races::Terran) {
		numberOfMyDefenseBuildingType1 += myPlayer->allUnitCount(myDefenseBuildingType1);
		numberOfMyDefenseBuildingType1 += BuildManager::Instance().buildQueue.getItemCount(myDefenseBuildingType1);
		numberOfMyDefenseBuildingType1 += ConstructionManager::Instance().getConstructionQueueItemCount(myDefenseBuildingType1);
		numberOfMyDefenseBuildingType2 += myPlayer->allUnitCount(myDefenseBuildingType2);
		numberOfMyDefenseBuildingType2 += BuildManager::Instance().buildQueue.getItemCount(myDefenseBuildingType2);
		numberOfMyDefenseBuildingType2 += ConstructionManager::Instance().getConstructionQueueItemCount(myDefenseBuildingType2);

		if (myPlayer->completedUnitCount(BWAPI::UnitTypes::Terran_Barracks) > 0) {
			isPossibleToConstructDefenseBuildingType1 = true;
		}
		if (myPlayer->completedUnitCount(BWAPI::UnitTypes::Terran_Engineering_Bay) > 0) {
			isPossibleToConstructDefenseBuildingType2 = true;
		}
		isPossibleToConstructCombatUnitTrainingBuildingType = true;

	}
	else if (myRace == BWAPI::Races::Zerg) {
		// 저그의 경우 크립 콜로니 갯수를 셀 때 성큰 콜로니 갯수까지 포함해서 세어야, 크립 콜로니를 지정한 숫자까지만 만든다
		numberOfMyDefenseBuildingType1 += myPlayer->allUnitCount(myDefenseBuildingType1);
		numberOfMyDefenseBuildingType1 += BuildManager::Instance().buildQueue.getItemCount(myDefenseBuildingType1);
		numberOfMyDefenseBuildingType1 += ConstructionManager::Instance().getConstructionQueueItemCount(myDefenseBuildingType1);
		numberOfMyDefenseBuildingType1 += myPlayer->allUnitCount(myDefenseBuildingType2);
		numberOfMyDefenseBuildingType1 += BuildManager::Instance().buildQueue.getItemCount(myDefenseBuildingType2);
		numberOfMyDefenseBuildingType2 += myPlayer->allUnitCount(myDefenseBuildingType2);
		numberOfMyDefenseBuildingType2 += BuildManager::Instance().buildQueue.getItemCount(myDefenseBuildingType2);

		if (myPlayer->completedUnitCount(BWAPI::UnitTypes::Zerg_Spawning_Pool) > 0) {
			isPossibleToConstructDefenseBuildingType1 = true;
		}
		if (myPlayer->completedUnitCount(BWAPI::UnitTypes::Zerg_Creep_Colony) > 0) {
			isPossibleToConstructDefenseBuildingType2 = true;
		}
		isPossibleToConstructCombatUnitTrainingBuildingType = true;
	}

	if (isPossibleToConstructDefenseBuildingType1 == true
		&& numberOfMyDefenseBuildingType1 < necessaryNumberOfDefenseBuilding1) {
		if (BuildManager::Instance().buildQueue.getItemCount(myDefenseBuildingType1) == 0) {
			if (BWAPI::Broodwar->self()->minerals() - ConstructionManager::Instance().getReservedMinerals() >= myDefenseBuildingType1.mineralPrice()) {
				BuildManager::Instance().buildQueue.queueAsHighestPriority(myDefenseBuildingType1,
					seedPositionStrategyOfMyDefenseBuildingType, false);
			}
		}
	}
	if (isPossibleToConstructDefenseBuildingType2 == true
		&& numberOfMyDefenseBuildingType2 < necessaryNumberOfDefenseBuilding2) {
		if (BuildManager::Instance().buildQueue.getItemCount(myDefenseBuildingType2) == 0) {
			if (BWAPI::Broodwar->self()->minerals() - ConstructionManager::Instance().getReservedMinerals() >= myDefenseBuildingType2.mineralPrice()) {
				BuildManager::Instance().buildQueue.queueAsHighestPriority(myDefenseBuildingType2,
					seedPositionStrategyOfMyDefenseBuildingType, false);
			}
		}
	}

	// 현재 공격 유닛 생산 건물 갯수
	int numberOfMyCombatUnitTrainingBuilding = myPlayer->allUnitCount(InformationManager::Instance().getBasicCombatBuildingType());
	numberOfMyCombatUnitTrainingBuilding += BuildManager::Instance().buildQueue.getItemCount(InformationManager::Instance().getBasicCombatBuildingType());
	numberOfMyCombatUnitTrainingBuilding += ConstructionManager::Instance().getConstructionQueueItemCount(InformationManager::Instance().getBasicCombatBuildingType());

	// 공격 유닛 생산 건물 증설 : 돈이 남아돌면 실시. 최대 6개 까지만
	if (isPossibleToConstructCombatUnitTrainingBuildingType == true
		&& BWAPI::Broodwar->self()->minerals() - ConstructionManager::Instance().getReservedMinerals() > 300
		&& numberOfMyCombatUnitTrainingBuilding < 6) {
		// 게이트웨이 / 배럭 / 해처리 증설
		if (BuildManager::Instance().buildQueue.getItemCount(InformationManager::Instance().getBasicCombatBuildingType()) == 0)
		{
			BuildManager::Instance().buildQueue.queueAsHighestPriority(InformationManager::Instance().getBasicCombatBuildingType(),
				seedPositionStrategyOfMyCombatUnitTrainingBuildingType, false);
		}
	}
}

/// 업그레이드 및 테크 리서치를 실행합니다
void StrategyManager::executeUpgradeAndTechResearch() {

	// InitialBuildOrder 진행중에는 아무것도 하지 않습니다
	if (isInitialBuildOrderFinished == false) {
		return;
	}

	// 1초에 한번만 실행
	if (BWAPI::Broodwar->getFrameCount() % 24 != 0) {
		return;
	}

	bool			isTimeToStartUpgradeType1 = false;	/// 업그레이드할 타이밍인가
	bool			isTimeToStartUpgradeType2 = false;	/// 업그레이드할 타이밍인가
	bool			isTimeToStartUpgradeType3 = false;	/// 업그레이드할 타이밍인가
	bool			isTimeToStartResearchTech1 = false;	/// 리서치할 타이밍인가
	bool			isTimeToStartResearchTech2 = false;	/// 리서치할 타이밍인가
	bool			isTimeToStartResearchTech3 = false;	/// 리서치할 타이밍인가

	// 업그레이드 / 리서치할 타이밍인지 판단
	if (myRace == BWAPI::Races::Protoss) {
		// 업그레이드 / 리서치를 너무 성급하게 하다가 위험에 빠질 수 있으므로, 최소 드라군 4기 생산 후 업그레이드한다
		if (myPlayer->completedUnitCount(BWAPI::UnitTypes::Protoss_Cybernetics_Core) > 0
			&& myPlayer->completedUnitCount(BWAPI::UnitTypes::Protoss_Dragoon) >= 4) {
			isTimeToStartUpgradeType1 = true;
		}
		// 업그레이드 / 리서치를 너무 성급하게 하다가 위험에 빠질 수 있으므로, 최소 질럿 6기 생산 후 업그레이드한다
		if (myPlayer->completedUnitCount(BWAPI::UnitTypes::Protoss_Citadel_of_Adun) > 0
			&& myPlayer->completedUnitCount(BWAPI::UnitTypes::Protoss_Zealot) >= 6) {
			isTimeToStartUpgradeType2 = true;
		}
		// 사이오닉스톰은 최우선으로 업그레이드한다
		if (myPlayer->completedUnitCount(BWAPI::UnitTypes::Protoss_Templar_Archives) > 0) {
			isTimeToStartResearchTech1 = true;
		}
		// 업그레이드 / 리서치를 너무 성급하게 하다가 위험에 빠질 수 있으므로, 최소 사이오닉스톰 리서치 후 업그레이드한다
		if (myPlayer->completedUnitCount(BWAPI::UnitTypes::Protoss_Templar_Archives) > 0
			&& myPlayer->hasResearched(necessaryTechType1) == true) {
			isTimeToStartUpgradeType3 = true;
			isTimeToStartResearchTech2 = true;
		}

	}
	else if (myRace == BWAPI::Races::Terran) {
		// 시즈모드는 최우선으로 업그레이드한다
		if (myPlayer->completedUnitCount(BWAPI::UnitTypes::Terran_Machine_Shop) > 0) {
			isTimeToStartResearchTech1 = true;
		}
		// 업그레이드 / 리서치를 너무 성급하게 하다가 위험에 빠질 수 있으므로, 최소 탱크 2기 생산 후 업그레이드한다
		if (myPlayer->completedUnitCount(BWAPI::UnitTypes::Terran_Academy) > 0
			&& myPlayer->completedUnitCount(BWAPI::UnitTypes::Terran_Siege_Tank_Tank_Mode) + myPlayer->completedUnitCount(BWAPI::UnitTypes::Terran_Siege_Tank_Siege_Mode) >= 2) {
			isTimeToStartUpgradeType1 = true;
		}
		// 업그레이드 / 리서치를 너무 성급하게 하다가 위험에 빠질 수 있으므로, 최소 탱크 2기 생산 후 업그레이드한다
		if (myPlayer->completedUnitCount(BWAPI::UnitTypes::Terran_Engineering_Bay) > 0
			&& myPlayer->completedUnitCount(BWAPI::UnitTypes::Terran_Siege_Tank_Tank_Mode) + myPlayer->completedUnitCount(BWAPI::UnitTypes::Terran_Siege_Tank_Siege_Mode) >= 2) {
			isTimeToStartUpgradeType2 = true;
		}
		// 업그레이드 / 리서치를 너무 성급하게 하다가 위험에 빠질 수 있으므로, 최소 이라디에이트 리서치 후 리서치한다
		if (myPlayer->completedUnitCount(BWAPI::UnitTypes::Terran_Science_Facility) > 0) {
			isTimeToStartResearchTech2 = true;
		}
		// 업그레이드 / 리서치를 너무 성급하게 하다가 위험에 빠질 수 있으므로, 최소 이라디에이트 리서치 후 업그레이드한다
		if (myPlayer->completedUnitCount(BWAPI::UnitTypes::Terran_Science_Facility) > 0
			&& myPlayer->hasResearched(necessaryTechType2) == true) {
			isTimeToStartUpgradeType3 = true;
		}
		// 업그레이드 / 리서치를 너무 성급하게 하다가 위험에 빠질 수 있으므로, 최소 사이언스베슬 2기 생산 후 업그레이드한다
		if (myPlayer->completedUnitCount(BWAPI::UnitTypes::Terran_Physics_Lab) > 0
			&& myPlayer->completedUnitCount(BWAPI::UnitTypes::Terran_Science_Vessel) >= 2) {
			isTimeToStartResearchTech3 = true;
		}
	}
	else if (myRace == BWAPI::Races::Zerg) {
		// 업그레이드 / 리서치를 너무 성급하게 하다가 위험에 빠질 수 있으므로, 최소 히드라 4기 생산 후 업그레이드한다
		if (myPlayer->completedUnitCount(BWAPI::UnitTypes::Zerg_Hydralisk_Den) > 0
			&& myPlayer->completedUnitCount(BWAPI::UnitTypes::Zerg_Hydralisk) >= 4) {
			isTimeToStartUpgradeType1 = true;
		}
		// 업그레이드 / 리서치를 너무 성급하게 하다가 위험에 빠질 수 있으므로, 최소 히드라 사정거리 업그레이드 후 업그레이드한다
		if (myPlayer->getUpgradeLevel(BWAPI::UpgradeTypes::Grooved_Spines) > 0) {
			isTimeToStartUpgradeType2 = true;
		}
		// 업그레이드 / 리서치를 너무 성급하게 하다가 위험에 빠질 수 있으므로, 최소 러커 리서치 후 업그레이드한다
		if (myPlayer->completedUnitCount(BWAPI::UnitTypes::Zerg_Lair) > 0 && myPlayer->hasResearched(BWAPI::TechTypes::Lurker_Aspect)) {
			isTimeToStartUpgradeType3 = true;
		}
		// 러커는 최우선으로 리서치한다
		if (myPlayer->completedUnitCount(BWAPI::UnitTypes::Zerg_Hydralisk_Den) > 0 && myPlayer->completedUnitCount(BWAPI::UnitTypes::Zerg_Lair) > 0) {
			isTimeToStartResearchTech1 = true;
		}
		// 컨슘은 최우선으로 리서치한다
		if (myPlayer->completedUnitCount(BWAPI::UnitTypes::Zerg_Defiler_Mound) > 0) {
			isTimeToStartResearchTech2 = true;
		}
		// 업그레이드 / 리서치를 너무 성급하게 하다가 위험에 빠질 수 있으므로, 최소 컨슘 리서치 후 리서치한다
		if (myPlayer->completedUnitCount(BWAPI::UnitTypes::Zerg_Defiler_Mound) > 0
			&& myPlayer->hasResearched(necessaryTechType2) == true) {
			isTimeToStartResearchTech3 = true;
		}
	}

	// 테크 리서치는 높은 우선순위로 우선적으로 실행
	if (isTimeToStartResearchTech1)
	{
		if (myPlayer->isResearching(necessaryTechType1) == false
			&& myPlayer->hasResearched(necessaryTechType1) == false
			&& BuildManager::Instance().buildQueue.getItemCount(necessaryTechType1) == 0)
		{
			BuildManager::Instance().buildQueue.queueAsHighestPriority(necessaryTechType1, true);
		}
	}

	if (isTimeToStartResearchTech2)
	{
		if (myPlayer->isResearching(necessaryTechType2) == false
			&& myPlayer->hasResearched(necessaryTechType2) == false
			&& BuildManager::Instance().buildQueue.getItemCount(necessaryTechType2) == 0)
		{
			BuildManager::Instance().buildQueue.queueAsHighestPriority(necessaryTechType2, true);
		}
	}

	if (isTimeToStartResearchTech3)
	{
		if (myPlayer->isResearching(necessaryTechType3) == false
			&& myPlayer->hasResearched(necessaryTechType3) == false
			&& BuildManager::Instance().buildQueue.getItemCount(necessaryTechType3) == 0)
		{
			BuildManager::Instance().buildQueue.queueAsHighestPriority(necessaryTechType3, true);
		}
	}

	// 업그레이드는 낮은 우선순위로 실행
	if (isTimeToStartUpgradeType1)
	{
		if (myPlayer->getUpgradeLevel(necessaryUpgradeType1) == 0
			&& myPlayer->isUpgrading(necessaryUpgradeType1) == false
			&& BuildManager::Instance().buildQueue.getItemCount(necessaryUpgradeType1) == 0)
		{
			BuildManager::Instance().buildQueue.queueAsLowestPriority(necessaryUpgradeType1, false);
		}
	}

	if (isTimeToStartUpgradeType2)
	{
		if (myPlayer->getUpgradeLevel(necessaryUpgradeType2) == 0
			&& myPlayer->isUpgrading(necessaryUpgradeType2) == false
			&& BuildManager::Instance().buildQueue.getItemCount(necessaryUpgradeType2) == 0)
		{
			BuildManager::Instance().buildQueue.queueAsLowestPriority(necessaryUpgradeType2, false);
		}
	}

	if (isTimeToStartUpgradeType3)
	{
		if (myPlayer->getUpgradeLevel(necessaryUpgradeType3) == 0
			&& myPlayer->isUpgrading(necessaryUpgradeType3) == false
			&& BuildManager::Instance().buildQueue.getItemCount(necessaryUpgradeType3) == 0)
		{
			BuildManager::Instance().buildQueue.queueAsLowestPriority(necessaryUpgradeType3, false);
		}
	}

	// BWAPI 4.1.2 의 버그때문에, 오버로드 업그레이드를 위해서는 반드시 Zerg_Lair 가 있어야함		
	if (myRace == BWAPI::Races::Zerg) {
		if (BuildManager::Instance().buildQueue.getItemCount(BWAPI::UpgradeTypes::Pneumatized_Carapace) > 0) {
			if (myPlayer->allUnitCount(BWAPI::UnitTypes::Zerg_Lair) == 0
				&& BuildManager::Instance().buildQueue.getItemCount(BWAPI::UnitTypes::Zerg_Lair) == 0)
			{
				BuildManager::Instance().buildQueue.queueAsHighestPriority(BWAPI::UnitTypes::Zerg_Lair, false);
			}
		}
	}
}


/// 특수 유닛을 생산할 수 있도록 테크트리에 따라 건설을 실시합니다
void StrategyManager::executeTechTreeUpConstruction() {

	// InitialBuildOrder 진행중에는 아무것도 하지 않습니다
	if (isInitialBuildOrderFinished == false) {
		return;
	}

	// 1초에 한번만 실행
	if (BWAPI::Broodwar->getFrameCount() % 24 != 0) {
		return;
	}

	if (myRace == BWAPI::Races::Protoss) {

		// 고급 건물 생산을 너무 성급하게 하다가 위험에 빠질 수 있으므로, 최소 드라군 2기 생산 후 건설한다
		if (myPlayer->completedUnitCount(BWAPI::UnitTypes::Protoss_Cybernetics_Core) > 0
			&& myPlayer->completedUnitCount(BWAPI::UnitTypes::Protoss_Dragoon) >= 4
			&& myPlayer->allUnitCount(BWAPI::UnitTypes::Protoss_Robotics_Facility) == 0
			&& BuildManager::Instance().buildQueue.getItemCount(BWAPI::UnitTypes::Protoss_Robotics_Facility) == 0
			&& ConstructionManager::Instance().getConstructionQueueItemCount(BWAPI::UnitTypes::Protoss_Robotics_Facility) == 0)
		{
			BuildManager::Instance().buildQueue.queueAsHighestPriority(BWAPI::UnitTypes::Protoss_Robotics_Facility, true);
		}

		if (myPlayer->completedUnitCount(BWAPI::UnitTypes::Protoss_Robotics_Facility) > 0
			&& myPlayer->allUnitCount(BWAPI::UnitTypes::Protoss_Observatory) == 0
			&& BuildManager::Instance().buildQueue.getItemCount(BWAPI::UnitTypes::Protoss_Observatory) == 0
			&& ConstructionManager::Instance().getConstructionQueueItemCount(BWAPI::UnitTypes::Protoss_Observatory) == 0)
		{
			BuildManager::Instance().buildQueue.queueAsHighestPriority(BWAPI::UnitTypes::Protoss_Observatory, true);
		}

		// 고급 건물 생산을 너무 성급하게 하다가 위험에 빠질 수 있으므로, 최소 질럿 2기 생산 후 건설한다
		if (myPlayer->completedUnitCount(BWAPI::UnitTypes::Protoss_Cybernetics_Core) > 0
			&& myPlayer->completedUnitCount(BWAPI::UnitTypes::Protoss_Zealot) >= 2
			&& myPlayer->allUnitCount(BWAPI::UnitTypes::Protoss_Citadel_of_Adun) == 0
			&& BuildManager::Instance().buildQueue.getItemCount(BWAPI::UnitTypes::Protoss_Citadel_of_Adun) == 0
			&& ConstructionManager::Instance().getConstructionQueueItemCount(BWAPI::UnitTypes::Protoss_Citadel_of_Adun) == 0)
		{
			BuildManager::Instance().buildQueue.queueAsHighestPriority(BWAPI::UnitTypes::Protoss_Citadel_of_Adun, true);
		}

		// 고급 건물 생산을 너무 성급하게 하다가 위험에 빠질 수 있으므로, 최소 드라군 4기 생산 후 건설한다
		if (myPlayer->completedUnitCount(BWAPI::UnitTypes::Protoss_Citadel_of_Adun) > 0
			&& myPlayer->completedUnitCount(BWAPI::UnitTypes::Protoss_Dragoon) >= 4
			&& myPlayer->allUnitCount(BWAPI::UnitTypes::Protoss_Templar_Archives) == 0
			&& BuildManager::Instance().buildQueue.getItemCount(BWAPI::UnitTypes::Protoss_Templar_Archives) == 0
			&& ConstructionManager::Instance().getConstructionQueueItemCount(BWAPI::UnitTypes::Protoss_Templar_Archives) == 0)
		{
			BuildManager::Instance().buildQueue.queueAsHighestPriority(BWAPI::UnitTypes::Protoss_Templar_Archives, true);
		}

	}
	else if (myRace == BWAPI::Races::Terran) {

		if (myPlayer->completedUnitCount(BWAPI::UnitTypes::Terran_Barracks) > 0
			&& myPlayer->allUnitCount(BWAPI::UnitTypes::Terran_Factory) == 0
			&& BuildManager::Instance().buildQueue.getItemCount(BWAPI::UnitTypes::Terran_Factory) == 0
			&& ConstructionManager::Instance().getConstructionQueueItemCount(BWAPI::UnitTypes::Terran_Factory) == 0)
		{
			BuildManager::Instance().buildQueue.queueAsHighestPriority(BWAPI::UnitTypes::Terran_Factory, true);
		}

		if (myPlayer->completedUnitCount(BWAPI::UnitTypes::Terran_Factory) > 0) {

			// myPlayer->allUnitCount() 으로 제대로 카운트 안되는 경우가 있어서, 별도 카운트
			int addonBuildingCount = 0;
			for (BWAPI::Unit unit : myPlayer->getUnits()) {
				if (unit->getType() == BWAPI::UnitTypes::Terran_Machine_Shop) {
					addonBuildingCount++;
					break;
				}

				if (unit->getType() == BWAPI::UnitTypes::Terran_Factory) {
					if (unit->isCompleted() && unit->isConstructing()) {
						addonBuildingCount++;
					}
				}
			}

			if (addonBuildingCount == 0
				&& BuildManager::Instance().buildQueue.getItemCount(BWAPI::UnitTypes::Terran_Machine_Shop) == 0
				&& ConstructionManager::Instance().getConstructionQueueItemCount(BWAPI::UnitTypes::Terran_Machine_Shop) == 0)
			{
				BuildManager::Instance().buildQueue.queueAsHighestPriority(BWAPI::UnitTypes::Terran_Machine_Shop, true);
			}
		}

		// 고급 건물 생산을 너무 성급하게 하다가 위험에 빠질 수 있으므로, 최소 탱크 2기 생산 후 건설한다
		if (myPlayer->completedUnitCount(BWAPI::UnitTypes::Terran_Factory) > 0
			&& myPlayer->completedUnitCount(BWAPI::UnitTypes::Terran_Siege_Tank_Tank_Mode) + myPlayer->completedUnitCount(BWAPI::UnitTypes::Terran_Siege_Tank_Siege_Mode) >= 2
			&& myPlayer->allUnitCount(BWAPI::UnitTypes::Terran_Starport) == 0
			&& BuildManager::Instance().buildQueue.getItemCount(BWAPI::UnitTypes::Terran_Starport) == 0
			&& ConstructionManager::Instance().getConstructionQueueItemCount(BWAPI::UnitTypes::Terran_Starport) == 0)
		{
			BuildManager::Instance().buildQueue.queueAsHighestPriority(BWAPI::UnitTypes::Terran_Starport, true);
		}

		if (myPlayer->completedUnitCount(BWAPI::UnitTypes::Terran_Starport) > 0) {

			// myPlayer->allUnitCount() 으로 제대로 카운트 안되는 경우가 있어서, 별도 카운트
			int addonBuildingCount = 0;
			for (BWAPI::Unit unit : myPlayer->getUnits()) {
				if (unit->getType() == BWAPI::UnitTypes::Terran_Control_Tower) {
					addonBuildingCount++;
					break;
				}

				if (unit->getType() == BWAPI::UnitTypes::Terran_Starport) {
					if (unit->isCompleted() && unit->isConstructing()) {
						addonBuildingCount++;
					}
				}
			}

			if (addonBuildingCount == 0
				&& BuildManager::Instance().buildQueue.getItemCount(BWAPI::UnitTypes::Terran_Control_Tower) == 0
				&& ConstructionManager::Instance().getConstructionQueueItemCount(BWAPI::UnitTypes::Terran_Control_Tower) == 0)
			{
				BuildManager::Instance().buildQueue.queueAsHighestPriority(BWAPI::UnitTypes::Terran_Control_Tower, true);
			}
		}

		// 고급 건물 생산을 너무 성급하게 하다가 위험에 빠질 수 있으므로, 최소 탱크 2기 생산 후 건설한다
		if (myPlayer->completedUnitCount(BWAPI::UnitTypes::Terran_Starport) > 0
			&& myPlayer->completedUnitCount(BWAPI::UnitTypes::Terran_Siege_Tank_Tank_Mode) + myPlayer->completedUnitCount(BWAPI::UnitTypes::Terran_Siege_Tank_Siege_Mode) >= 2
			&& myPlayer->allUnitCount(BWAPI::UnitTypes::Terran_Science_Facility) == 0
			&& BuildManager::Instance().buildQueue.getItemCount(BWAPI::UnitTypes::Terran_Science_Facility) == 0
			&& ConstructionManager::Instance().getConstructionQueueItemCount(BWAPI::UnitTypes::Terran_Science_Facility) == 0)
		{
			BuildManager::Instance().buildQueue.queueAsHighestPriority(BWAPI::UnitTypes::Terran_Science_Facility, true);
		}

		if (myPlayer->completedUnitCount(BWAPI::UnitTypes::Terran_Science_Facility) > 0) {

			// myPlayer->allUnitCount() 으로 제대로 카운트 안되는 경우가 있어서, 별도 카운트
			int addonBuildingCount = 0;
			for (BWAPI::Unit unit : myPlayer->getUnits()) {
				if (unit->getType() == BWAPI::UnitTypes::Terran_Physics_Lab) {
					addonBuildingCount++;
					break;
				}

				if (unit->getType() == BWAPI::UnitTypes::Terran_Science_Facility) {
					if (unit->isCompleted() && unit->isConstructing()) {
						addonBuildingCount++;
					}
				}
			}

			if (addonBuildingCount == 0
				&& BuildManager::Instance().buildQueue.getItemCount(BWAPI::UnitTypes::Terran_Physics_Lab) == 0
				&& ConstructionManager::Instance().getConstructionQueueItemCount(BWAPI::UnitTypes::Terran_Physics_Lab) == 0)
			{
				BuildManager::Instance().buildQueue.queueAsHighestPriority(BWAPI::UnitTypes::Terran_Physics_Lab, true);
			}
		}

	}
	else if (myRace == BWAPI::Races::Zerg) {
		// 고급 건물 생산을 너무 성급하게 하다가 위험에 빠질 수 있으므로, 최소 히드라리스크 4기 생산 후 건설한다
		if (myPlayer->completedUnitCount(BWAPI::UnitTypes::Zerg_Lair) > 0
			&& myPlayer->completedUnitCount(BWAPI::UnitTypes::Zerg_Hydralisk) >= 4
			&& myPlayer->allUnitCount(BWAPI::UnitTypes::Zerg_Queens_Nest) == 0
			&& BuildManager::Instance().buildQueue.getItemCount(BWAPI::UnitTypes::Zerg_Queens_Nest) == 0
			&& ConstructionManager::Instance().getConstructionQueueItemCount(BWAPI::UnitTypes::Zerg_Queens_Nest) == 0)
		{
			BuildManager::Instance().buildQueue.queueAsHighestPriority(BWAPI::UnitTypes::Zerg_Queens_Nest, true);
		}

		// 고급 건물 생산을 너무 성급하게 하다가 위험에 빠질 수 있으므로, 최소 히드라리스크 4기 생산 후 건설한다
		if (myPlayer->completedUnitCount(BWAPI::UnitTypes::Zerg_Lair) > 0
			&& myPlayer->completedUnitCount(BWAPI::UnitTypes::Zerg_Hydralisk) >= 4
			&& myPlayer->completedUnitCount(BWAPI::UnitTypes::Zerg_Queens_Nest) > 0
			&& myPlayer->allUnitCount(BWAPI::UnitTypes::Zerg_Hive) == 0
			&& BuildManager::Instance().buildQueue.getItemCount(BWAPI::UnitTypes::Zerg_Hive) == 0
			&& ConstructionManager::Instance().getConstructionQueueItemCount(BWAPI::UnitTypes::Zerg_Hive) == 0)
		{
			BuildManager::Instance().buildQueue.queueAsHighestPriority(BWAPI::UnitTypes::Zerg_Hive, true);
		}

		// 고급 건물 생산을 너무 성급하게 하다가 위험에 빠질 수 있으므로, 최소 히드라리스크 4기 생산 후 건설한다
		if (myPlayer->completedUnitCount(BWAPI::UnitTypes::Zerg_Hive) > 0
			&& myPlayer->completedUnitCount(BWAPI::UnitTypes::Zerg_Hydralisk) >= 4
			&& myPlayer->allUnitCount(BWAPI::UnitTypes::Zerg_Defiler_Mound) == 0
			&& BuildManager::Instance().buildQueue.getItemCount(BWAPI::UnitTypes::Zerg_Defiler_Mound) == 0
			&& ConstructionManager::Instance().getConstructionQueueItemCount(BWAPI::UnitTypes::Zerg_Defiler_Mound) == 0)
		{
			BuildManager::Instance().buildQueue.queueAsHighestPriority(BWAPI::UnitTypes::Zerg_Defiler_Mound, true);
		}

	}

}


/// 공격유닛을 계속 추가 생산합니다
void StrategyManager::executeCombatUnitTraining() {

	// InitialBuildOrder 진행중에는 아무것도 하지 않습니다
	if (isInitialBuildOrderFinished == false) {
		return;
	}

	if (myPlayer->supplyUsed() <= 390)
	{
		// 공격 유닛 생산
		BWAPI::UnitType nextUnitTypeToTrain = getNextCombatUnitTypeToTrain();

		BWAPI::UnitType producerType = (MetaType(nextUnitTypeToTrain)).whatBuilds();

		for (BWAPI::Unit unit : myPlayer->getUnits()) {

			if (unit->getType() == producerType) {
				if (unit->isTraining() == false && unit->isMorphing() == false) {

					if (BuildManager::Instance().buildQueue.getItemCount(nextUnitTypeToTrain) == 0) {

						boolean isPossibleToTrain = false;
						if (nextUnitTypeToTrain == BWAPI::UnitTypes::Protoss_Zealot) {
							if (myPlayer->completedUnitCount(BWAPI::UnitTypes::Protoss_Gateway) > 0) {
								isPossibleToTrain = true;
							}
						}
						else if (nextUnitTypeToTrain == BWAPI::UnitTypes::Protoss_Dragoon) {
							if (myPlayer->completedUnitCount(BWAPI::UnitTypes::Protoss_Gateway) > 0 && myPlayer->completedUnitCount(BWAPI::UnitTypes::Protoss_Cybernetics_Core) > 0) {
								isPossibleToTrain = true;
							}
						}
						else if (nextUnitTypeToTrain == BWAPI::UnitTypes::Protoss_Dark_Templar) {
							if (myPlayer->completedUnitCount(BWAPI::UnitTypes::Protoss_Gateway) > 0 && myPlayer->completedUnitCount(BWAPI::UnitTypes::Protoss_Templar_Archives) > 0) {
								isPossibleToTrain = true;
							}
						}
						else if (nextUnitTypeToTrain == BWAPI::UnitTypes::Terran_Marine) {
							if (myPlayer->completedUnitCount(BWAPI::UnitTypes::Terran_Barracks) > 0) {
								isPossibleToTrain = true;
							}
						}
						else if (nextUnitTypeToTrain == BWAPI::UnitTypes::Terran_Medic) {
							if (myPlayer->completedUnitCount(BWAPI::UnitTypes::Terran_Barracks) > 0 && myPlayer->completedUnitCount(BWAPI::UnitTypes::Terran_Academy) > 0) {
								isPossibleToTrain = true;
							}
						}
						else if (nextUnitTypeToTrain == BWAPI::UnitTypes::Terran_Siege_Tank_Tank_Mode) {
							if (myPlayer->completedUnitCount(BWAPI::UnitTypes::Factories) > 0 && myPlayer->completedUnitCount(BWAPI::UnitTypes::Terran_Machine_Shop) > 0) {
								isPossibleToTrain = true;
							}
						}
						else if (nextUnitTypeToTrain == BWAPI::UnitTypes::Zerg_Zergling) {
							if (myPlayer->completedUnitCount(BWAPI::UnitTypes::Zerg_Spawning_Pool) > 0) {
								isPossibleToTrain = true;
							}
						}
						else if (nextUnitTypeToTrain == BWAPI::UnitTypes::Zerg_Hydralisk) {
							if (myPlayer->completedUnitCount(BWAPI::UnitTypes::Zerg_Hydralisk_Den) > 0) {
								isPossibleToTrain = true;
							}
						}
						else if (nextUnitTypeToTrain == BWAPI::UnitTypes::Zerg_Lurker) {
							if (unit->getType() == BWAPI::UnitTypes::Zerg_Hydralisk
								&& myPlayer->completedUnitCount(BWAPI::UnitTypes::Zerg_Hydralisk_Den) > 0
								&& myPlayer->hasResearched(BWAPI::TechTypes::Lurker_Aspect) == true) {
								isPossibleToTrain = true;
							}
						}

						if (isPossibleToTrain) {
							BuildManager::Instance().buildQueue.queueAsLowestPriority(nextUnitTypeToTrain, false);
						}

						nextTargetIndexOfBuildOrderArray++;
						if (nextTargetIndexOfBuildOrderArray >= buildOrderArrayOfMyCombatUnitType.size()) {
							nextTargetIndexOfBuildOrderArray = 0;
						}

						break;
					}
				}
			}
		}

		// 특수 유닛 생산			
		if (BuildManager::Instance().buildQueue.getItemCount(mySpecialUnitType1) == 0) {

			boolean isPossibleToTrain = false;
			if (mySpecialUnitType1 == BWAPI::UnitTypes::Protoss_Observer) {
				if (myPlayer->completedUnitCount(BWAPI::UnitTypes::Protoss_Robotics_Facility) > 0
					&& myPlayer->completedUnitCount(BWAPI::UnitTypes::Protoss_Observatory) > 0) {
					isPossibleToTrain = true;
				}
			}
			else if (mySpecialUnitType1 == BWAPI::UnitTypes::Terran_Science_Vessel) {
				if (myPlayer->completedUnitCount(BWAPI::UnitTypes::Terran_Starport) > 0
					&& myPlayer->completedUnitCount(BWAPI::UnitTypes::Terran_Control_Tower) > 0
					&& myPlayer->completedUnitCount(BWAPI::UnitTypes::Terran_Science_Facility) > 0) {
					isPossibleToTrain = true;
				}
			}
			// 저그 오버로드는 executeSupplyManagement 에서 이미 생산하므로 추가 생산할 필요 없다

			boolean isNecessaryToTrainMore = false;
			if (myPlayer->allUnitCount(mySpecialUnitType1) + BuildManager::Instance().buildQueue.getItemCount(mySpecialUnitType1)
				< maxNumberOfSpecialUnitType1) {
				isNecessaryToTrainMore = true;
			}

			if (isPossibleToTrain && isNecessaryToTrainMore) {

				producerType = (MetaType(mySpecialUnitType1)).whatBuilds();

				for (BWAPI::Unit unit : myPlayer->getUnits()) {
					if (unit->getType() == producerType) {
						if (unit->isTraining() == false && unit->isMorphing() == false) {

							BuildManager::Instance().buildQueue.queueAsLowestPriority(mySpecialUnitType1, true);
							break;
						}
					}
				}
			}
		}

		if (BuildManager::Instance().buildQueue.getItemCount(mySpecialUnitType2) == 0) {

			boolean isPossibleToTrain = false;

			if (mySpecialUnitType2 == BWAPI::UnitTypes::Protoss_High_Templar) {
				if (myPlayer->completedUnitCount(BWAPI::UnitTypes::Protoss_Gateway) > 0
					&& myPlayer->completedUnitCount(BWAPI::UnitTypes::Protoss_Templar_Archives) > 0) {
					isPossibleToTrain = true;
				}
			}
			else if (mySpecialUnitType2 == BWAPI::UnitTypes::Terran_Battlecruiser) {
				if (myPlayer->completedUnitCount(BWAPI::UnitTypes::Terran_Starport) > 0
					&& myPlayer->completedUnitCount(BWAPI::UnitTypes::Terran_Physics_Lab) > 0) {
					isPossibleToTrain = true;
				}
			}
			else if (mySpecialUnitType2 == BWAPI::UnitTypes::Zerg_Defiler) {
				if (myPlayer->completedUnitCount(BWAPI::UnitTypes::Zerg_Defiler_Mound) > 0) {
					isPossibleToTrain = true;
				}
			}

			boolean isNecessaryToTrainMore = false;

			// 저그 종족의 경우, Egg 안에 있는 것까지 카운트 해야함 
			int allCountOfSpecialUnitType2 = myPlayer->allUnitCount(mySpecialUnitType2) + BuildManager::Instance().buildQueue.getItemCount(mySpecialUnitType2);
			if (mySpecialUnitType2.getRace() == BWAPI::Races::Zerg) {
				for (BWAPI::Unit unit : myPlayer->getUnits()) {

					if (unit->getType() == BWAPI::UnitTypes::Zerg_Egg && unit->getBuildType() == mySpecialUnitType2) {
						allCountOfSpecialUnitType2++;
					}
					// 갓태어난 유닛은 아직 반영안되어있을 수 있어서, 추가 카운트를 해줘야함
					//if (unit->getType() == mySpecialUnitType2 && unit->isConstructing()) {
					//	allCountOfSpecialUnitType2++;
					//}
				}

			}
			if (allCountOfSpecialUnitType2 < maxNumberOfSpecialUnitType2) {
				isNecessaryToTrainMore = true;
			}

			if (isPossibleToTrain && isNecessaryToTrainMore) {

				producerType = (MetaType(mySpecialUnitType2)).whatBuilds();

				for (BWAPI::Unit unit : myPlayer->getUnits()) {
					if (unit->getType() == producerType) {
						if (unit->isTraining() == false && unit->isMorphing() == false) {

							BuildManager::Instance().buildQueue.queueAsLowestPriority(mySpecialUnitType2, true);
							break;
						}

					}
				}
			}
		}

	}
}

/// 다음에 생산할 공격유닛 UnitType 을 리턴합니다
BWAPI::UnitType StrategyManager::getNextCombatUnitTypeToTrain() {

	BWAPI::UnitType nextUnitTypeToTrain;

	if (buildOrderArrayOfMyCombatUnitType[nextTargetIndexOfBuildOrderArray] == 1) {
		nextUnitTypeToTrain = myCombatUnitType1;
	}
	else if (buildOrderArrayOfMyCombatUnitType[nextTargetIndexOfBuildOrderArray] == 2) {
		nextUnitTypeToTrain = myCombatUnitType2;
	}
	else {
		nextUnitTypeToTrain = myCombatUnitType3;
	}

	return nextUnitTypeToTrain;
}


// BasicBot 1.1 Patch Start ////////////////////////////////////////////////
// 경기 결과 파일 Save / Load 및 로그파일 Save 예제 추가
void StrategyManager::loadGameRecordList()
{
	// 과거의 게임에서 bwapi-data\write 폴더에 기록했던 파일은 대회 서버가 bwapi-data\read 폴더로 옮겨놓습니다
	// 따라서, 파일 로딩은 bwapi-data\read 폴더로부터 하시면 됩니다

	// TODO : 파일명은 각자 봇 명에 맞게 수정하시기 바랍니다
	std::string gameRecordFileName = "bwapi-data\\read\\NoNameBot_GameRecord.dat";

	FILE *file;
	errno_t err;
	if ((err = fopen_s(&file, gameRecordFileName.c_str(), "r")) != 0)
	{
		std::cout << "loadGameRecord failed. Could not open file :" << gameRecordFileName.c_str() << std::endl;
	}
	else
	{
		std::cout << "loadGameRecord from file: " << gameRecordFileName.c_str() << std::endl;
		char line[4096];
		while (fgets(line, sizeof line, file) != nullptr)
		{
			std::stringstream ss(line);

			GameRecord tempGameRecord;
			ss >> tempGameRecord.mapName;
			ss >> tempGameRecord.myName;
			ss >> tempGameRecord.myRace;
			ss >> tempGameRecord.myWinCount;
			ss >> tempGameRecord.myLoseCount;
			ss >> tempGameRecord.enemyName;
			ss >> tempGameRecord.enemyRace;
			ss >> tempGameRecord.enemyRealRace;
			ss >> tempGameRecord.gameFrameCount;

			gameRecordList.push_back(tempGameRecord);
		}
		fclose(file);
	}
}

void StrategyManager::saveGameRecordList(bool isWinner)
{
	// 이번 게임의 파일 저장은 bwapi-data\write 폴더에 하시면 됩니다.
	// bwapi-data\write 폴더에 저장된 파일은 대회 서버가 다음 경기 때 bwapi-data\read 폴더로 옮겨놓습니다

	// TODO : 파일명은 각자 봇 명에 맞게 수정하시기 바랍니다
	std::string gameRecordFileName = "bwapi-data\\write\\NoNameBot_GameRecord.dat";

	std::cout << "saveGameRecord to file: " << gameRecordFileName.c_str() << std::endl;

	std::string mapName = BWAPI::Broodwar->mapFileName();
	std::replace(mapName.begin(), mapName.end(), ' ', '_');
	std::string enemyName = BWAPI::Broodwar->enemy()->getName();
	std::replace(enemyName.begin(), enemyName.end(), ' ', '_');
	std::string myName = BWAPI::Broodwar->self()->getName();
	std::replace(myName.begin(), myName.end(), ' ', '_');

	/// 이번 게임에 대한 기록
	GameRecord thisGameRecord;
	thisGameRecord.mapName = mapName;
	thisGameRecord.myName = myName;
	thisGameRecord.myRace = BWAPI::Broodwar->self()->getRace().c_str();
	thisGameRecord.enemyName = enemyName;
	thisGameRecord.enemyRace = BWAPI::Broodwar->enemy()->getRace().c_str();
	thisGameRecord.enemyRealRace = InformationManager::Instance().enemyRace.c_str();
	thisGameRecord.gameFrameCount = BWAPI::Broodwar->getFrameCount();
	if (isWinner) {
		thisGameRecord.myWinCount = 1;
		thisGameRecord.myLoseCount = 0;
	}
	else {
		thisGameRecord.myWinCount = 0;
		thisGameRecord.myLoseCount = 1;
	}
	// 이번 게임 기록을 전체 게임 기록에 추가
	gameRecordList.push_back(thisGameRecord);

	// 전체 게임 기록 write
	std::stringstream ss;
	for (GameRecord gameRecord : gameRecordList) {
		ss << gameRecord.mapName << " "
			<< gameRecord.myName << " "
			<< gameRecord.myRace << " "
			<< gameRecord.myWinCount << " "
			<< gameRecord.myLoseCount << " "
			<< gameRecord.enemyName << " "
			<< gameRecord.enemyRace << " "
			<< gameRecord.enemyRealRace << " "
			<< gameRecord.gameFrameCount << "\n";

	}
	Logger::overwriteToFile(gameRecordFileName, ss.str());
}

void StrategyManager::saveGameLog()
{
	// 100 프레임 (5초) 마다 1번씩 로그를 기록합니다
	// 참가팀 당 용량 제한이 있고, 타임아웃도 있기 때문에 자주 하지 않는 것이 좋습니다
	// 로그는 봇 개발 시 디버깅 용도로 사용하시는 것이 좋습니다
	if (BWAPI::Broodwar->getFrameCount() % 100 != 0) {
		return;
	}

	// TODO : 파일명은 각자 봇 명에 맞게 수정하시기 바랍니다
	std::string gameLogFileName = "bwapi-data\\write\\NoNameBot_LastGameLog.dat";

	std::string mapName = BWAPI::Broodwar->mapFileName();
	std::replace(mapName.begin(), mapName.end(), ' ', '_');
	std::string enemyName = BWAPI::Broodwar->enemy()->getName();
	std::replace(enemyName.begin(), enemyName.end(), ' ', '_');
	std::string myName = BWAPI::Broodwar->self()->getName();
	std::replace(myName.begin(), myName.end(), ' ', '_');

	std::stringstream ss;
	ss << mapName << " "
		<< myName << " "
		<< BWAPI::Broodwar->self()->getRace().c_str() << " "
		<< enemyName << " "
		<< InformationManager::Instance().enemyRace.c_str() << " "
		<< BWAPI::Broodwar->getFrameCount() << " "
		<< BWAPI::Broodwar->self()->supplyUsed() << " "
		<< BWAPI::Broodwar->self()->supplyTotal() << " "
		<< "\n";

	Logger::appendTextToFile(gameLogFileName, ss.str());
}

// BasicBot 1.1 Patch End //////////////////////////////////////////////////
