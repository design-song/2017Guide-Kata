#pragma once

#include "Common.h"

#include "UnitData.h"
#include "BuildOrderQueue.h"
#include "InformationManager.h"
#include "WorkerManager.h"
#include "BuildManager.h"
#include "ConstructionManager.h"
#include "ScoutManager.h"
#include "StrategyManager.h"

namespace MyBot
{
	/// 상황을 판단하여, 정찰, 빌드, 공격, 방어 등을 수행하도록 총괄 지휘를 하는 class<br>
	/// InformationManager 에 있는 정보들로부터 상황을 판단하고, <br>
	/// BuildManager 의 buildQueue에 빌드 (건물 건설 / 유닛 훈련 / 테크 리서치 / 업그레이드) 명령을 입력합니다.<br>
	/// 정찰, 빌드, 공격, 방어 등을 수행하는 코드가 들어가는 class
	class StrategyManager
	{
		///////////////////////////////////////////////////////////////////
		/// 
		/// 제목 : Kata1
		///
		/// 목표 : 컴퓨터 정도는 이겨보자
		///
		/// 아이디어 : 방어를 튼튼히 갖추고 기다리고 있다가, 적군 유닛들이 쳐들어온 것을 방어 성공해낸 후 역습을 가서 승리한다
		///
		/// 제공 :
		///         1. 방어형 빌드오더
		///
		///         2. 일꾼 훈련, 방어 건물 건설, 업그레이드, 리서치, 공격 유닛 훈련, 아군 공격유닛 방어형 배치, 적 Eliminate 시키기 메소드 
		///
		/// 참가자 구현 과제 :
		///
		///         TODO 1. 적 기지로 공격을 갈 타이밍인지 판단하는 로직             (예상 개발시간  5분)
		/// 
		///         TODO 2. 적 기지로 아군 공격 유닛들이 공격가게 하는 로직          (예상 개발시간 20분)
		///
		///         TODO 3. 적 Eliminate 시키기 모드로 전환할 때인지 판단하는 로직   (예상 개발시간  5분)
		///
		/// 성공 조건 : 컴퓨터와 1대1로 싸워 승리한다
		///
		/// 도전 과제 :
		///           "더 빠른 시간 내에" 승리를 달성하도록 노력해보세요.
		///
		///           방어 시 아군 공격유닛들의 피해를 최소화하고 
		///
		///           역습 타이밍을 가능한한 이른 시간으로 결정하고
		///
		///           아군 공격유닛들을 이동 시키되 가능한한 떼지어서 빠르게 이동시키고  
		///
		///           적군의 남은 건물들을 가능한한 빠른 시간내에 찾아 Eliminate시키면,
		///
		///           "더 빠른 시간 내에" 승리를 달성할 수 있을 것입니다.
		/// 
		///////////////////////////////////////////////////////////////////

		// 아군
		BWAPI::Player myPlayer;
		BWAPI::Race myRace;

		// 적군
		BWAPI::Player enemyPlayer;
		BWAPI::Race enemyRace;

		// 아군 공격 유닛 첫번째 타입
		BWAPI::UnitType myCombatUnitType1;						/// 질럿 마린 저글링

		// 아군 공격 유닛 두번째 타입
		BWAPI::UnitType myCombatUnitType2;			  			/// 드라군 메딕 히드라

		std::vector<int> buildOrderArrayOfMyCombatUnitType;		/// 아군 공격 유닛 첫번째 타입, 두번째 타입 생산 순서
		int nextTargetIndexOfBuildOrderArray;					/// buildOrderArrayOfMyCombatUnitType 에서 다음 생산대상 아군 공격 유닛

		// 아군의 공격유닛 숫자
		int necessaryNumberOfCombatUnitType1;					/// 공격을 시작하기위해 필요한 최소한의 유닛 숫자 
		int necessaryNumberOfCombatUnitType2;					/// 공격을 시작하기위해 필요한 최소한의 유닛 숫자 
		int numberOfCompletedCombatUnitType1;					/// 첫번째 유닛 타입의 현재 유닛 숫자
		int numberOfCompletedCombatUnitType2;					/// 두번째 유닛 타입의 현재 유닛 숫자
		int myKilledUnitCount1;	 								/// 첫번째 유닛 타입의 사망자 숫자 누적값
		int myKilledUnitCount2;	 								/// 두번째 유닛 타입의 사망자 숫자 누적값

		// 아군 공격 유닛 목록
		std::vector<BWAPI::Unit> myCombatUnitType1List;			// 질럿   마린 저글링
		std::vector<BWAPI::Unit> myCombatUnitType2List;			// 드라군 메딕 히드라

		// 아군 방어 건물 첫번째 타입
		BWAPI::UnitType myDefenseBuildingType1;					/// 파일런 벙커 크립

		// 아군 방어 건물 두번째 타입
		BWAPI::UnitType myDefenseBuildingType2;					/// 포톤  터렛  성큰

		// 아군 방어 건물 건설 숫자
		int necessaryNumberOfDefenseBuilding1;					/// 방어 건물 건설 갯수
		int necessaryNumberOfDefenseBuilding2;					/// 방어 건물 건설 갯수

		// 아군 방어 건물 건설 위치
		BuildOrderItem::SeedPositionStrategy seedPositionStrategyOfMyDefenseBuildingType;
		BuildOrderItem::SeedPositionStrategy seedPositionStrategyOfMyCombatUnitTrainingBuildingType;

		// 아군 방어 건물 목록
		std::vector<BWAPI::Unit> myDefenseBuildingType1List;	// 파일런 벙커 크립
		std::vector<BWAPI::Unit> myDefenseBuildingType2List;	// 캐논   터렛 성큰

		// 업그레이드 / 리서치 할 것 
		BWAPI::UpgradeType 	nessaryUpgradeType1;				/// 드라군사정거리업 마린공격력업     히드라사정거리업
		BWAPI::UpgradeType 	nessaryUpgradeType2;				/// 질럿발업         마린사정거리업   히드라발업
		BWAPI::TechType 		nessaryTechType;				///                 마린스팀팩

		// 적군 공격 유닛 숫자
		int numberOfCompletedEnemyCombatUnit;
		// 적군 공격 유닛 사망자 수 
		int enemyKilledUnitCount;								/// 적군 공격 유닛타입의 사망 유닛 숫자 누적값


		// 아군 / 적군의 본진, 첫번째 길목, 두번째 길목
		BWTA::BaseLocation* myMainBaseLocation;
		BWTA::BaseLocation* myFirstExpansionLocation;
		BWTA::Chokepoint* myFirstChokePoint;
		BWTA::Chokepoint* mySecondChokePoint;
		BWTA::BaseLocation* enemyMainBaseLocation;
		BWTA::BaseLocation* enemyFirstExpansionLocation;
		BWTA::Chokepoint* enemyFirstChokePoint;
		BWTA::Chokepoint* enemySecondChokePoint;

		bool isInitialBuildOrderFinished;						/// setInitialBuildOrder 에서 입력한 빌드오더가 다 끝나서 빌드오더큐가 empty 되었는지 여부
		
		enum CombatState {
			defenseMode,										// 아군 진지 방어
			attackStarted,										// 적 공격 시작
			attackMySecondChokepoint,							// 아군 두번째 길목을 공격
			attackEnemySecondChokepoint,						// 적진 두번째 길목을 공격
			attackEnemyFirstExpansionLocation,					// 적진 앞마당을 공격
			attackEnemyMainBaseLocation,						// 적진 본진을 공격
			eliminateEnemy										// 적 Eliminate 
		};

		CombatState combatState;				/// 전투 상황

		/// 변수 초기값을 설정합니다
		void setVariables();
		
		/// 변수 값을 업데이트 합니다
		void updateVariables();
		
		/// 게임 초기에 사용할 빌드오더를 세팅합니다
		void setInitialBuildOrder();
		
		/// 전반적인 전투 로직 을 갖고 전투를 수행합니다
		void executeCombat();

		/// 아군 공격유닛 들에게 방어를 지시합니다
		void commandMyCombatUnitToDefense();

		/// 공격 모드로 전환할 때인지 여부를 리턴합니다
		bool isTimeToStartAttack();

		/// 아군 공격 유닛들에게 공격을 지시합니다 
		void commandMyCombatUnitToAttack();

		/// 적군을 Eliminate 시키는 모드로 전환할지 여부를 리턴합니다 
		bool isTimeToStartElimination();

		/// 적군을 Eliminate 시키도록 아군 공격 유닛들에게 지시합니다
		void commandMyCombatUnitToEliminate();
		
	private:
		StrategyManager();

		/// 일꾼을 계속 추가 생산합니다
		void executeWorkerTraining();

		/// Supply DeadLock 예방 및 SupplyProvider 가 부족해질 상황 에 대한 선제적 대응으로서 SupplyProvider를 추가 건설/생산합니다
		void executeSupplyManagement();

		/// 방어건물 및 공격유닛 생산 건물을 건설합니다
		void executeBuildingConstruction();

		/// 업그레이드 및 테크 리서치를 실행합니다
		void executeUpgradeAndTechResearch();

		/// 공격유닛을 계속 추가 생산합니다
		void executeCombatUnitTraining();

		/// 다음에 생산할 공격유닛 UnitType 을 리턴합니다
		BWAPI::UnitType getNextCombatUnitTypeToTrain();
		
		/// StrategyManager 의 수행상황을 표시합니다
		void drawStrategyManagerStatus();

	public:
		/// static singleton 객체를 리턴합니다
		static StrategyManager &	Instance();

		/// 경기가 시작될 때 일회적으로 전략 초기 세팅 관련 로직을 실행합니다
		void onStart();

		///  경기가 종료될 때 일회적으로 전략 결과 정리 관련 로직을 실행합니다
		void onEnd(bool isWinner);

		/// 경기 진행 중 매 프레임마다 경기 전략 관련 로직을 실행합니다
		void update();

		/// 아군 / 적군 공격 유닛 사망 유닛 숫자 누적값을 업데이트 합니다
		void onUnitDestroy(BWAPI::Unit unit);

	// BasicBot 1.1 Patch Start ////////////////////////////////////////////////
	// 경기 결과 파일 Save / Load 및 로그파일 Save 예제 추가를 위한 변수 및 메소드 선언

	private:
		/// 한 게임에 대한 기록을 저장하는 자료구조
		class GameRecord {
		public:
			std::string mapName;
			std::string enemyName;
			std::string enemyRace;
			std::string enemyRealRace;
			std::string myName;
			std::string myRace;
			int gameFrameCount = 0;
			int myWinCount = 0;
			int myLoseCount = 0;
		};
		/// 과거 전체 게임들의 기록을 저장하는 자료구조
		std::vector<GameRecord> gameRecordList;

		/// 과거 전체 게임 기록을 로딩합니다
		void loadGameRecordList();
		/// 과거 전체 게임 기록 + 이번 게임 기록을 저장합니다
		void saveGameRecordList(bool isWinner);
		/// 이번 게임 중간에 상시적으로 로그를 저장합니다
		void saveGameLog();

	// BasicBot 1.1 Patch End //////////////////////////////////////////////////

	};
}
