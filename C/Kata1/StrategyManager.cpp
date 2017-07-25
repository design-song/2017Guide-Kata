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

	numberOfCompletedCombatUnitType1 = 0;
	numberOfCompletedCombatUnitType2 = 0;
	myKilledUnitCount1 = 0;
	myKilledUnitCount2 = 0;
	necessaryNumberOfDefenseBuilding1 = 0;
	necessaryNumberOfDefenseBuilding2 = 0;

	numberOfCompletedEnemyCombatUnit = 0;
	enemyKilledUnitCount = 0;

	isInitialBuildOrderFinished = false;
	combatState = CombatState::defenseMode;

	if (myRace == BWAPI::Races::Protoss) {

		// 공격 유닛 종류 설정 
		myCombatUnitType1 = BWAPI::UnitTypes::Protoss_Zealot;
		myCombatUnitType2 = BWAPI::UnitTypes::Protoss_Dragoon;
		// 공격 모드로 전환하기 위해 필요한 최소한의 유닛 숫자 설정
		necessaryNumberOfCombatUnitType1 = 6;							// 공격을 시작하기위해 필요한 최소한의 질럿 유닛 숫자 
		necessaryNumberOfCombatUnitType2 = 6;							// 공격을 시작하기위해 필요한 최소한의 드라군 유닛 숫자 

		// 공격 유닛 생산 순서 설정
		buildOrderArrayOfMyCombatUnitType = {1, 2, 2};					// 생산 순서 : 질럿 드라군 드라군 질럿 드라군 드라군 ...
		nextTargetIndexOfBuildOrderArray = 0; 							// 다음 생산 순서 index

		// 방어 건물 종류 및 건설 갯수 설정
		myDefenseBuildingType1 = BWAPI::UnitTypes::Protoss_Pylon;
		necessaryNumberOfDefenseBuilding1 = 1;
		myDefenseBuildingType2 = BWAPI::UnitTypes::Protoss_Photon_Cannon;
		necessaryNumberOfDefenseBuilding2 = 3;

		// 방어 건물 건설 위치 설정
		seedPositionStrategyOfMyDefenseBuildingType
			= BuildOrderItem::SeedPositionStrategy::FirstChokePoint;	// 첫번째 길목
		seedPositionStrategyOfMyCombatUnitTrainingBuildingType
			= BuildOrderItem::SeedPositionStrategy::FirstChokePoint;	// 첫번째 길목

		// 업그레이드 및 리서치 대상 설정
		nessaryUpgradeType1 = BWAPI::UpgradeTypes::Singularity_Charge;
		nessaryUpgradeType2 = BWAPI::UpgradeTypes::Leg_Enhancements;
		nessaryTechType = BWAPI::TechTypes::None;
	}
	else if (myRace == BWAPI::Races::Terran) {

		// 공격 유닛 종류 설정  
		myCombatUnitType1 = BWAPI::UnitTypes::Terran_Marine;
		myCombatUnitType2 = BWAPI::UnitTypes::Terran_Medic;

		// 공격 모드로 전환하기 위해 필요한 최소한의 유닛 숫자 설정
		necessaryNumberOfCombatUnitType1 = 12;                         // 공격을 시작하기위해 필요한 최소한의 마린 유닛 숫자 
		necessaryNumberOfCombatUnitType2 = 2;                          // 공격을 시작하기위해 필요한 최소한의 메딕 유닛 숫자 

		// 공격 유닛 생산 순서 설정
		buildOrderArrayOfMyCombatUnitType = {1, 1, 1, 1, 2};				// 마린 마린 마린 마린 메딕 ...
		nextTargetIndexOfBuildOrderArray = 0; 							// 다음 생산 순서 index

		// 방어 건물 종류 및 건설 갯수 설정
		myDefenseBuildingType1 = BWAPI::UnitTypes::Terran_Bunker;
		necessaryNumberOfDefenseBuilding1 = 2;
		myDefenseBuildingType2 = BWAPI::UnitTypes::Terran_Missile_Turret;
		necessaryNumberOfDefenseBuilding2 = 1;

		// 방어 건물 건설 위치 설정
		seedPositionStrategyOfMyDefenseBuildingType
			= BuildOrderItem::SeedPositionStrategy::FirstChokePoint;	// 첫번째 길목
		seedPositionStrategyOfMyCombatUnitTrainingBuildingType
			= BuildOrderItem::SeedPositionStrategy::FirstChokePoint;	// 첫번째 길목

		// 업그레이드 및 리서치 대상 설정
		nessaryUpgradeType1 = BWAPI::UpgradeTypes::U_238_Shells;
		nessaryUpgradeType2 = BWAPI::UpgradeTypes::Terran_Infantry_Weapons;
		nessaryTechType = BWAPI::TechTypes::Stim_Packs;
	}
	else if (myRace == BWAPI::Races::Zerg) {

		// 공격 유닛 종류 설정 
		myCombatUnitType1 = BWAPI::UnitTypes::Zerg_Zergling;
		myCombatUnitType2 = BWAPI::UnitTypes::Zerg_Hydralisk;

		// 공격 모드로 전환하기 위해 필요한 최소한의 유닛 숫자 설정
		necessaryNumberOfCombatUnitType1 = 8;							// 공격을 시작하기위해 필요한 최소한의 저글링 유닛 숫자 
		necessaryNumberOfCombatUnitType2 = 8;							// 공격을 시작하기위해 필요한 최소한의 히드라 유닛 숫자 

		// 공격 유닛 생산 순서 설정
		buildOrderArrayOfMyCombatUnitType = {1, 2, 2};					// 저글링 히드라 히드라 저글리 히드라 히드라 ...
		nextTargetIndexOfBuildOrderArray = 0; 							// 다음 생산 순서 index

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
		nessaryUpgradeType1 = BWAPI::UpgradeTypes::Grooved_Spines;
		nessaryUpgradeType2 = BWAPI::UpgradeTypes::Muscular_Augments;
		nessaryTechType = BWAPI::TechTypes::None;
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
		BuildManager::Instance().buildQueue.queueAsLowestPriority(BWAPI::UnitTypes::Protoss_Probe); // 18

		BuildManager::Instance().buildQueue.queueAsLowestPriority(BWAPI::UnitTypes::Protoss_Cybernetics_Core);
		BuildManager::Instance().buildQueue.queueAsLowestPriority(BWAPI::UnitTypes::Protoss_Probe); // 19
		BuildManager::Instance().buildQueue.queueAsLowestPriority(BWAPI::UnitTypes::Protoss_Zealot); // 21

		BuildManager::Instance().buildQueue.queueAsLowestPriority(BWAPI::UnitTypes::Protoss_Gateway); // 두번째 게이트웨이
		BuildManager::Instance().buildQueue.queueAsLowestPriority(BWAPI::UnitTypes::Protoss_Dragoon); // 23
		BuildManager::Instance().buildQueue.queueAsLowestPriority(BWAPI::UnitTypes::Protoss_Citadel_of_Adun, false);
	}
	else if (BWAPI::Broodwar->self()->getRace() == BWAPI::Races::Terran) {
		BuildManager::Instance().buildQueue.queueAsLowestPriority(BWAPI::UnitTypes::Terran_SCV); // 5
		BuildManager::Instance().buildQueue.queueAsLowestPriority(BWAPI::UnitTypes::Terran_SCV); // 6
		BuildManager::Instance().buildQueue.queueAsLowestPriority(BWAPI::UnitTypes::Terran_SCV); // 7
		BuildManager::Instance().buildQueue.queueAsLowestPriority(BWAPI::UnitTypes::Terran_SCV); // 8
		BuildManager::Instance().buildQueue.queueAsLowestPriority(BWAPI::UnitTypes::Terran_Barracks,
			seedPositionStrategyOfMyCombatUnitTrainingBuildingType);
		BuildManager::Instance().buildQueue.queueAsLowestPriority(BWAPI::UnitTypes::Terran_SCV); // 9
		BuildManager::Instance().buildQueue.queueAsLowestPriority(BWAPI::UnitTypes::Terran_Supply_Depot);
		BuildManager::Instance().buildQueue.queueAsLowestPriority(BWAPI::UnitTypes::Terran_SCV); // 10

		BuildManager::Instance().buildQueue.queueAsLowestPriority(BWAPI::UnitTypes::Terran_Marine); // 11
		BuildManager::Instance().buildQueue.queueAsLowestPriority(BWAPI::UnitTypes::Terran_Bunker,
			seedPositionStrategyOfMyDefenseBuildingType);
		BuildManager::Instance().buildQueue.queueAsLowestPriority(BWAPI::UnitTypes::Terran_Marine); // 12
		BuildManager::Instance().buildQueue.queueAsLowestPriority(BWAPI::UnitTypes::Terran_SCV); // 13
		BuildManager::Instance().buildQueue.queueAsLowestPriority(BWAPI::UnitTypes::Terran_Marine); // 14
		BuildManager::Instance().buildQueue.queueAsLowestPriority(BWAPI::UnitTypes::Terran_SCV); // 15
		BuildManager::Instance().buildQueue.queueAsLowestPriority(BWAPI::UnitTypes::Terran_Supply_Depot);
		BuildManager::Instance().buildQueue.queueAsLowestPriority(BWAPI::UnitTypes::Terran_Marine); // 16
		BuildManager::Instance().buildQueue.queueAsLowestPriority(BWAPI::UnitTypes::Terran_SCV); // 17
		BuildManager::Instance().buildQueue.queueAsLowestPriority(BWAPI::UnitTypes::Terran_Refinery);

		BuildManager::Instance().buildQueue.queueAsLowestPriority(BWAPI::UnitTypes::Terran_Marine); // 18
		BuildManager::Instance().buildQueue.queueAsLowestPriority(BWAPI::UnitTypes::Terran_SCV); // 19

		BuildManager::Instance().buildQueue.queueAsLowestPriority(BWAPI::UnitTypes::Terran_Marine); // 20
		BuildManager::Instance().buildQueue.queueAsLowestPriority(BWAPI::UnitTypes::Terran_SCV); // 21

		BuildManager::Instance().buildQueue.queueAsLowestPriority(BWAPI::UnitTypes::Terran_Academy, false);

		BuildManager::Instance().buildQueue.queueAsLowestPriority(BWAPI::UnitTypes::Terran_Marine); // 22
		BuildManager::Instance().buildQueue.queueAsLowestPriority(BWAPI::UnitTypes::Terran_SCV); // 23

		BuildManager::Instance().buildQueue.queueAsLowestPriority(BWAPI::UnitTypes::Terran_Barracks,
			seedPositionStrategyOfMyCombatUnitTrainingBuildingType);

		BuildManager::Instance().buildQueue.queueAsLowestPriority(BWAPI::UnitTypes::Terran_Engineering_Bay, false);

		BuildManager::Instance().buildQueue.queueAsLowestPriority(BWAPI::UnitTypes::Terran_Comsat_Station, false);

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

		BuildManager::Instance().buildQueue.queueAsLowestPriority(BWAPI::UnitTypes::Zerg_Zergling, false);	//15

		BuildManager::Instance().buildQueue.queueAsLowestPriority(BWAPI::UnitTypes::Zerg_Creep_Colony,
			seedPositionStrategyOfMyDefenseBuildingType);	//14

		BuildManager::Instance().buildQueue.queueAsLowestPriority(BWAPI::UnitTypes::Zerg_Zergling);	//15

		BuildManager::Instance().buildQueue.queueAsLowestPriority(BWAPI::UnitTypes::Zerg_Sunken_Colony);

		BuildManager::Instance().buildQueue.queueAsLowestPriority(BWAPI::UnitTypes::Zerg_Creep_Colony,
			seedPositionStrategyOfMyDefenseBuildingType);	//14

		BuildManager::Instance().buildQueue.queueAsLowestPriority(BWAPI::UnitTypes::Zerg_Overlord);	// 세번째 오버로드

		BuildManager::Instance().buildQueue.queueAsLowestPriority(BWAPI::UnitTypes::Zerg_Zergling, false);	//15
		BuildManager::Instance().buildQueue.queueAsLowestPriority(BWAPI::UnitTypes::Zerg_Sunken_Colony);

		BuildManager::Instance().buildQueue.queueAsLowestPriority(BWAPI::UnitTypes::Zerg_Drone);	//16
		BuildManager::Instance().buildQueue.queueAsLowestPriority(BWAPI::UnitTypes::Zerg_Zergling, false);	//17

		BuildManager::Instance().buildQueue.queueAsLowestPriority(BWAPI::UnitTypes::Zerg_Creep_Colony,
			seedPositionStrategyOfMyDefenseBuildingType);	//16

		BuildManager::Instance().buildQueue.queueAsLowestPriority(BWAPI::UnitTypes::Zerg_Drone);	//17
		BuildManager::Instance().buildQueue.queueAsLowestPriority(BWAPI::UnitTypes::Zerg_Zergling, false);	//18

		BuildManager::Instance().buildQueue.queueAsLowestPriority(BWAPI::UnitTypes::Zerg_Drone);	//19
		BuildManager::Instance().buildQueue.queueAsLowestPriority(BWAPI::UnitTypes::Zerg_Zergling);	//20
		BuildManager::Instance().buildQueue.queueAsLowestPriority(BWAPI::UnitTypes::Zerg_Zergling);	//21
		BuildManager::Instance().buildQueue.queueAsLowestPriority(BWAPI::UnitTypes::Zerg_Sunken_Colony);

		BuildManager::Instance().buildQueue.queueAsLowestPriority(BWAPI::UnitTypes::Zerg_Extractor); //20

		BuildManager::Instance().buildQueue.queueAsLowestPriority(BWAPI::UnitTypes::Zerg_Zergling);	//21
		BuildManager::Instance().buildQueue.queueAsLowestPriority(BWAPI::UnitTypes::Zerg_Zergling);	//22
		BuildManager::Instance().buildQueue.queueAsLowestPriority(BWAPI::UnitTypes::Zerg_Zergling);	//23

		BuildManager::Instance().buildQueue.queueAsLowestPriority(BWAPI::UnitTypes::Zerg_Overlord);	// 네번째 오버로드

		BuildManager::Instance().buildQueue.queueAsLowestPriority(BWAPI::UnitTypes::Zerg_Drone);	//24
		BuildManager::Instance().buildQueue.queueAsLowestPriority(BWAPI::UnitTypes::Zerg_Drone);	//25
		BuildManager::Instance().buildQueue.queueAsLowestPriority(BWAPI::UnitTypes::Zerg_Drone);	//26

		BuildManager::Instance().buildQueue.queueAsLowestPriority(BWAPI::UnitTypes::Zerg_Hydralisk_Den);	//25
		BuildManager::Instance().buildQueue.queueAsLowestPriority(BWAPI::UnitTypes::Zerg_Evolution_Chamber); //24
	}
}

void StrategyManager::update()
{
	if (BuildManager::Instance().buildQueue.isEmpty()) {
		isInitialBuildOrderFinished = true;
	}
	
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
	else {
		if (combatState != CombatState::eliminateEnemy) {
			/// 아군 공격 유닛들에게 공격을 지시합니다 
			commandMyCombatUnitToAttack();

			/// 적군을 Eliminate 시키는 모드로 전환할지 여부를 판단합니다 
			if (isTimeToStartElimination()) {
				combatState = CombatState::eliminateEnemy;
			}
		}
		else {
			/// 적군을 Eliminate 시키도록 아군 공격 유닛들에게 지시합니다
			commandMyCombatUnitToEliminate();
		}
	}
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
	for (auto & unit : BWAPI::Broodwar->self()->getUnits()) {
		if (unit->isIdle()) {
			if (unit->getType() == myCombatUnitType1 || unit->getType() == myCombatUnitType2) {

				bool hasCommanded = false;

				// 테란 종족의 경우 마린을 벙커안에 집어넣기
				if (unit->getType() == BWAPI::UnitTypes::Terran_Marine) {
					for (auto & bunker : myDefenseBuildingType1List) {
						if (bunker->getLoadedUnits().size() < 4 && bunker->canLoad(unit)) {
							CommandUtil::rightClick(unit, bunker);
							hasCommanded = true;
						}
					}
				}
				
				// 명령 내린 적이 없으면, 방어 건물 위치로 이동
				if (hasCommanded == false) {
					CommandUtil::attackMove(unit, myDefenseBuildingPosition);
				}
			}
		}
	}
}

/// 공격 모드로 전환할 때인지 여부를 리턴합니다
bool StrategyManager::isTimeToStartAttack(){

	//////////////////////////////////////////////////////////////////////////
	///////////////////////// 아래의 코드를 수정해보세요 ///////////////////////
	//
	// TODO 1. 적군 기지로 역습 (공격)을 갈 타이밍인지 판단하는 로직            (예상 개발시간  5분)
	//
	// 목표 : 현재는 단순히 아군 공격 유닛 숫자가 최소 숫자를 넘으면 공격하도록 되어있습니다.
	//      그러나, 아군 공격 유닛 숫자가 최소 숫자를 넘었더라도 
	//      먼저 적군이 공격해올때까지 기다렸다가 적군의 공격을 방어한 후 역습을 가는 것이 훨씬 나은 전략입니다.
	//
	//      적군 공격 유닛 사망자 수 numEnemyCombatUnitKilled 를 이용하여 
	//		방어모드에서 공격모드로 전환할지 여부를 리턴하도록 수정해보세요
	//      return false = 방어모드를 유지
	//      return true = 공격모드로 전환
	// 
	// Hint : 적군 공격 유닛 사망자 수가 대략 8 이상이면 충분히 역습갈만한 타이밍이라고 볼 수 있지 않을까요?
	//
	//////////////////////////////////////////////////////////////////////////

	if (myPlayer->completedUnitCount(myCombatUnitType1) >= necessaryNumberOfCombatUnitType1
		&& myPlayer->completedUnitCount(myCombatUnitType2) >= necessaryNumberOfCombatUnitType2)
	{
		return true;
	}

	if (myPlayer->completedUnitCount(myCombatUnitType1)
		+ myPlayer->completedUnitCount(myCombatUnitType2) >= 40)
	{
		return true;
	}

	return false;
}

/// 아군 공격 유닛들에게 공격을 지시합니다 
void StrategyManager::commandMyCombatUnitToAttack(){

	//////////////////////////////////////////////////////////////////////////
	///////////////////////// 아래의 코드를 수정해보세요 ///////////////////////
	//
	// TODO 2. 적군 기지로 아군 공격 유닛들이 공격가게 하는 로직                 (예상 개발시간 20분)
	//
	// 목표 : 현재는 단순히 아군 공격 유닛들을 적군의 Main BaseLocation 을 향해 바로 공격하러 가도록 되어있습니다.
	//      이렇게하면, 한줄로 줄서서 이동하기 때문에 효과적인 공격이 되기 어려운데요
	//      아군 공격 유닛들이 떼지어서 군집이동을 하도록 수정해보세요
	// 
	// Hint : 군집이동을 하게 하는 방법은 수많은 방법이 있는데요, 
	//
	//        가장 간단한 방법은 중간 집결지를 두는 것입니다.
	//
	//        1단계로, 적군의 두번째 길목 (enemySecondChokePoint->center()) 을 향해 공격가게 한 후,
	// 
	//        적군의 두번째 길목 길목 주위에 병력의 상당수가 도착했을 때 (unit->distance() 메소드로 판단)
	//
	//        이제 2단계로 전환하여, 적군 기지 (enemyMainBaseLocation->getPosition()) 로 공격하게 하는 것이지요.
	//
	//////////////////////////////////////////////////////////////////////////

	// 최종 타겟은 적군의 Main BaseLocation 
	BWTA::BaseLocation* targetEnemyBaseLocation = enemyMainBaseLocation;
	BWAPI::Position targetPosition = BWAPI::Positions::None;

	if (targetEnemyBaseLocation != nullptr)
	{
		// 먼저, 테란 종족의 경우, 벙커 안에 있는 유닛은 밖으로 빼낸다
		if (myRace == BWAPI::Races::Terran) {
			for (auto & bunker : myDefenseBuildingType1List) {
				if (bunker->getLoadedUnits().size() > 0) {

					boolean isThereSomeEnemyUnit = false;
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

		// 모든 아군 공격유닛들로 하여금 targetPosition 을 향해 attackMove 로 공격하게 한다
		for (auto & unit : BWAPI::Broodwar->self()->getUnits()) {
			if (unit->getType() == myCombatUnitType1 || unit->getType() == myCombatUnitType2) {
				if (unit->isIdle()) {
					CommandUtil::attackMove(unit, targetPosition);
				}
			}
		}



	}
}

/// 적군을 Eliminate 시키는 모드로 전환할지 여부를 리턴합니다 
bool StrategyManager::isTimeToStartElimination(){

	//////////////////////////////////////////////////////////////////////////
	///////////////////////// 아래의 코드를 수정해보세요 ///////////////////////
	//
	// TODO 3. 적군을 Eliminate 시키는 모드로 전환할 때인지 판단하는 로직   (예상 개발시간  5분)
	//
	// 목표 : 공격이 성공하여 적군의 BaseLocation 의 많은 건물들과 유닛들을 파괴했지만, 
	//      지도 곳곳에 적군의 건물들이 몇개씩 남아서 게임이 끝나지 않는 상태가 될 수 있습니다.
	// 
	//      적군을 Eliminate 시키는 모드로 전환을 하면, 모든 공격유닛들을 지도 곳곳으로 보내게 되는데요
	// 
	//      현재는 적군의 BaseLocation 에 도착했는지만 갖고 판단하여 모드를 전환하므로,
	//      아직 적군 공격유닛이 다수 남아있는 상태에서도 모드를 전환하게 될 수 있습니다.
	//
	//      이를 더욱 잘 판단하는 코드로 수정해보세요
	//
	//      return false : 적군을 Eliminate 시키는 모드로 전환할 때가 아님
	//      return true  : 적군을 Eliminate 시키는 모드로 전환할 때임
	// 
	// Hint : 적군 공격 유닛 숫자  numberOfCompletedEnemyCombatUnit,
	//        적군 공격 유닛 사망자 수 numEnemyCombatUnitKilled,
	//        아군 공격 유닛 숫자 numberOfCompletedCombatUnitType1, numberOfCompletedCombatUnitType2 
	//        등을 조건문에 추가하면 더 적절한 시점을 판단할 수 있지 않을까요?
	// 
	//////////////////////////////////////////////////////////////////////////

	// 적군의 Main BaseLocation
	BWTA::BaseLocation* enemyMainBaseLocation = InformationManager::Instance().getMainBaseLocation(enemyPlayer);

	if (enemyMainBaseLocation != nullptr)
	{
		// 적군의 Main BaseLocation 에 아군 공격 유닛이 도착하였는가
		bool isMyCombatUnitArrivedAtEnemyBaseLocation = false;
		for (BWAPI::Unit unit : BWAPI::Broodwar->self()->getUnits()) {
			if (unit->getType() == myCombatUnitType1 || unit->getType() == myCombatUnitType2) {

				if (unit->getDistance(enemyMainBaseLocation->getPosition()) < 4 * TILE_SIZE) {
					isMyCombatUnitArrivedAtEnemyBaseLocation = true;
					break;
				}
			}
		}

		// 적군의 Main BaseLocation 에 도착했으면 이제 Eliminate 모드로 전환할 때이다
		if (isMyCombatUnitArrivedAtEnemyBaseLocation == true) {
			return true;
		}
	}


	return false;
}

/// 적군을 Eliminate 시키도록 아군 공격 유닛들에게 지시합니다
void StrategyManager::commandMyCombatUnitToEliminate(){

	if (enemyPlayer == nullptr || enemyRace == BWAPI::Races::Unknown)
	{
		return;
	}

	int mapHeight = BWAPI::Broodwar->mapHeight();	// 128
	int mapWidth = BWAPI::Broodwar->mapWidth();		// 128

	// 맵 전체의 BaseLocation들 목록
	std::set<BWTA::BaseLocation*> baselocationList = BWTA::getBaseLocations();

	// 적군의 남은 건물 정보
	BWAPI::Position targetPosition = BWAPI::Positions::None;

	bool isThereRemainingBuildingUnitInfo = false;
	double minDistance = 1000000000;
	double tempDistance = 0;
	for (auto & unitInfoEntry : InformationManager::Instance().getUnitAndUnitInfoMap(enemyPlayer)) {
		if (unitInfoEntry.second.type.isBuilding()
			&& unitInfoEntry.second.lastHealth > 0)
		{
			tempDistance = myMainBaseLocation->getPosition().getDistance(unitInfoEntry.second.unit->getPosition());
			if (minDistance > tempDistance) {
				minDistance = tempDistance;
				targetPosition = unitInfoEntry.second.lastPosition;
				isThereRemainingBuildingUnitInfo = true;
			}
		}
	}

	// 아군 공격 유닛들로 하여금 적군의 남은 건물을 알고 있으면 그것을 공격하게 하고, 그렇지 않으면 맵 전체를 랜덤하게 돌아다니도록 합니다 
	for (auto & unit : myPlayer->getUnits()) {
		if (unit->getType() == myCombatUnitType1 || unit->getType() == myCombatUnitType2) {

			if (unit->isIdle()) {

				if (isThereRemainingBuildingUnitInfo == false) {
					targetPosition = BWAPI::Position(rand() % (mapWidth * TILE_SIZE), rand() % (mapHeight * TILE_SIZE));
				}

				if (unit->canAttack()) {
					CommandUtil::attackMove(unit, targetPosition);
				}
				else {
					CommandUtil::move(unit, targetPosition);
				}
			}
		}
	}
}

/// StrategyManager 의 수행상황을 표시합니다
void StrategyManager::drawStrategyManagerStatus() {

	// 아군 공격유닛 숫자 및 적군 공격유닛 숫자
	BWAPI::Broodwar->drawTextScreen(200, 250, "My %s", myCombatUnitType1.getName().c_str());
	BWAPI::Broodwar->drawTextScreen(300, 250, "alive %d", numberOfCompletedCombatUnitType1);
	BWAPI::Broodwar->drawTextScreen(350, 250, "killed %d", myKilledUnitCount1);
	BWAPI::Broodwar->drawTextScreen(200, 260, "My %s", myCombatUnitType2.getName().c_str());
	BWAPI::Broodwar->drawTextScreen(300, 260, "alive %d", numberOfCompletedCombatUnitType2);
	BWAPI::Broodwar->drawTextScreen(350, 260, "killed %d", myKilledUnitCount2);
	BWAPI::Broodwar->drawTextScreen(200, 270, "Enemy CombatUnit");
	BWAPI::Broodwar->drawTextScreen(300, 270, "alive %d", numberOfCompletedEnemyCombatUnit);
	BWAPI::Broodwar->drawTextScreen(350, 270, "killed %d", enemyKilledUnitCount);

	// setInitialBuildOrder 에서 입력한 빌드오더가 다 끝나서 빌드오더큐가 empty 되었는지 여부
	BWAPI::Broodwar->drawTextScreen(300, 280, "isInitialBuildOrderFinished %d", isInitialBuildOrderFinished);
	// 전투 상황
	BWAPI::Broodwar->drawTextScreen(300, 290, "combatState %d", combatState);
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

	// 아군의 공격유닛 숫자
	numberOfCompletedCombatUnitType1 = myPlayer->completedUnitCount(myCombatUnitType1);
	numberOfCompletedCombatUnitType2 = myPlayer->completedUnitCount(myCombatUnitType2);

	// 적군의 공격유닛 숫자
	numberOfCompletedEnemyCombatUnit = 0;
	for (auto & unitInfoEntry : InformationManager::Instance().getUnitAndUnitInfoMap(enemyPlayer)) {
		UnitInfo & enemyUnitInfo = unitInfoEntry.second;
		if (enemyUnitInfo.type.isWorker() == false && enemyUnitInfo.type.isBuilding() == false) {
			numberOfCompletedEnemyCombatUnit++;
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
	myCombatUnitType1List.clear();
	myCombatUnitType2List.clear();
	for (BWAPI::Unit unit : myPlayer->getUnits()) {
		if (unit->getType() == myCombatUnitType1) { myCombatUnitType1List.push_back(unit); }
		else if (unit->getType() == myCombatUnitType2) { myCombatUnitType2List.push_back(unit); }
		else if (unit->getType() == myDefenseBuildingType1) { myDefenseBuildingType1List.push_back(unit); }
		else if (unit->getType() == myDefenseBuildingType2) { myDefenseBuildingType2List.push_back(unit); }
	}
}

/// 아군 / 적군 공격 유닛 사망 유닛 숫자 누적값을 업데이트 합니다
void StrategyManager::onUnitDestroy(BWAPI::Unit unit) {
	if (unit->getType().isNeutral()) {
		return;
	}

	if (unit->getPlayer() == myPlayer) {
		// 아군 공격 유닛 첫번째 타입의 사망 유닛 숫자 누적값
		if (unit->getType() == myCombatUnitType1) {
			myKilledUnitCount1++;
		}
		// 아군 공격 유닛 두번째 타입의 사망 유닛 숫자 누적값
		else if (unit->getType() == myCombatUnitType2) {
			myKilledUnitCount2++;
		}
	}
	else if (unit->getPlayer() == enemyPlayer) {
		/// 적군 공격 유닛타입의 사망 유닛 숫자 누적값
		if (unit->getType().isWorker() == false && unit->getType().isBuilding() == false) {
			enemyKilledUnitCount++;
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
				if (unit->getType().isResourceDepot())
				{
					if (unit->isTraining() == false || unit->getLarva().size() > 0) {

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
	if (isInitialBuildOrderFinished == false && BWAPI::Broodwar->self()->supplyUsed() <= BWAPI::Broodwar->self()->supplyTotal()) {
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
	bool			isTimeToStartResearchTech = false;	/// 리서치할 타이밍인가

	// 업그레이드 / 리서치할 타이밍인지 판단
	if (myRace == BWAPI::Races::Protoss) {
		if (myPlayer->completedUnitCount(BWAPI::UnitTypes::Protoss_Cybernetics_Core) > 0) {
			isTimeToStartUpgradeType1 = true;
		}
		if (myPlayer->completedUnitCount(BWAPI::UnitTypes::Protoss_Zealot) > 2) {
			isTimeToStartUpgradeType2 = true;
		}
	}
	else if (myRace == BWAPI::Races::Terran) {
		if (myPlayer->completedUnitCount(BWAPI::UnitTypes::Terran_Academy) > 0) {
			isTimeToStartUpgradeType1 = true;
		}
		if (myPlayer->completedUnitCount(BWAPI::UnitTypes::Terran_Engineering_Bay) > 0) {
			isTimeToStartUpgradeType2 = true;
		}
		if (myPlayer->getUpgradeLevel(BWAPI::UpgradeTypes::U_238_Shells) > 0) {
			isTimeToStartResearchTech = true;
		}
	}
	else if (myRace == BWAPI::Races::Zerg) {
		if (myPlayer->completedUnitCount(BWAPI::UnitTypes::Zerg_Hydralisk_Den) > 0) {
			isTimeToStartUpgradeType1 = true;
		}
		if (myPlayer->getUpgradeLevel(BWAPI::UpgradeTypes::Grooved_Spines) > 0) {
			isTimeToStartUpgradeType2 = true;
		}
	}

	if (isTimeToStartUpgradeType1)
	{
		if (myPlayer->getUpgradeLevel(nessaryUpgradeType1) == 0
			&& myPlayer->isUpgrading(nessaryUpgradeType1) == false
			&& BuildManager::Instance().buildQueue.getItemCount(nessaryUpgradeType1) == 0)
		{
			BuildManager::Instance().buildQueue.queueAsLowestPriority(nessaryUpgradeType1, false);
		}
	}

	if (isTimeToStartUpgradeType2)
	{
		if (myPlayer->getUpgradeLevel(nessaryUpgradeType2) == 0
			&& myPlayer->isUpgrading(nessaryUpgradeType2) == false
			&& BuildManager::Instance().buildQueue.getItemCount(nessaryUpgradeType2) == 0)
		{
			BuildManager::Instance().buildQueue.queueAsLowestPriority(nessaryUpgradeType2, false);
		}
	}

	if (isTimeToStartResearchTech)
	{
		if (myPlayer->isResearching(nessaryTechType) == false
			&& BuildManager::Instance().buildQueue.getItemCount(nessaryTechType) == 0)
		{
			BuildManager::Instance().buildQueue.queueAsLowestPriority(nessaryTechType, false);
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
		for (BWAPI::Unit unit : myPlayer->getUnits()) {
			if (unit->getType() == InformationManager::Instance().getBasicCombatBuildingType()) {
				if (unit->isTraining() == false || unit->getLarva().size() > 0) {

					BWAPI::UnitType nextUnitTypeToTrain = getNextCombatUnitTypeToTrain();

					if (BuildManager::Instance().buildQueue.getItemCount(nextUnitTypeToTrain) == 0) {
						BuildManager::Instance().buildQueue.queueAsLowestPriority(nextUnitTypeToTrain, false);

						nextTargetIndexOfBuildOrderArray++;
						if (nextTargetIndexOfBuildOrderArray >= (int)(buildOrderArrayOfMyCombatUnitType.size())) {
							nextTargetIndexOfBuildOrderArray = 0;
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
	else {
		nextUnitTypeToTrain = myCombatUnitType2;
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
