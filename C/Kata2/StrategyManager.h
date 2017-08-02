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
		/// 제목 : Kata2
		///
		/// 목표 : 사이오닉스톰 / 이라디에이트 / 다크스웜을 프로게이머처럼 써보자
		///
		/// 아이디어 : 방어를 튼튼히 갖추고 공격 유닛을 모으다가, 특수 유닛과 함께 진격하여 승리한다 
		///
		/// 제공 :
		///         1. 종족별 방어형 빌드오더
		///
		///         2. 일꾼 훈련, 방어 건물 건설, 업그레이드, 리서치, 공격 유닛/특수유닛 생산, 방어형 배치, 적 Eliminate 시키기 메소드 
		/// 
		/// 참가자 구현 과제 :
		///
		///         TODO 1. 아군 특수 유닛을 공격 유닛들과 함께 이동시키는 로직   (예상 개발시간 10분)
		/// 
		///         TODO 2. 아군 특수 유닛이 기술을 적절히 사용하게 하는 로직     (예상 개발시간 20분)
		///
		///                 ※ 프로토스 : 사이오닉스톰, 할루시네이션
		///                   테란     : 디펜시브매트릭스, 이라디에이트, 야마토건
		///                   저그     : 다크스웜, 플레이그
		/// 
		/// 성공 조건 : 컴퓨터와 1대1로 싸워 승리한다
		/// 
		///////////////////////////////////////////////////////////////////

		// 아군
		BWAPI::Player myPlayer;
		BWAPI::Race myRace;

		// 적군
		BWAPI::Player enemyPlayer;
		BWAPI::Race enemyRace;


		// 아군 공격 유닛 첫번째, 두번째, 세번째 타입             프로토스     테란            저그
		BWAPI::UnitType myCombatUnitType1;					/// 질럿         마린           저글링
		BWAPI::UnitType myCombatUnitType2;			  		/// 드라군       메딕           히드라리스크
		BWAPI::UnitType myCombatUnitType3;			  		/// 다크템플러   시즈탱크       러커

		// 아군 특수 유닛 첫번째, 두번째 타입
		BWAPI::UnitType mySpecialUnitType1;			  		/// 옵저버       사이언스베쓸   오버로드
		BWAPI::UnitType mySpecialUnitType2;					/// 하이템플러   배틀크루저     디파일러

		// 업그레이드 / 리서치 할 것                              프로토스           테란                    저그
		BWAPI::UpgradeType 	necessaryUpgradeType1;			/// 드라군사정거리업    마린공격력업            히드라사정거리업
		BWAPI::UpgradeType 	necessaryUpgradeType2;			/// 질럿발업            마린사정거리업          히드라발업
		BWAPI::UpgradeType 	necessaryUpgradeType3;			/// 하이템플러에너지업  사이언스베슬에너지업    오버로드속도업

		BWAPI::TechType 		necessaryTechType1;			/// 사이오닉스톰        시즈모드                러커
		BWAPI::TechType 		necessaryTechType2;			/// 할루시네이션        이라디에이트            컨슘           
		BWAPI::TechType 		necessaryTechType3;			///                    야마토건                플레이그       

		// 아군 공격 유닛 생산 순서 
		std::vector<int> buildOrderArrayOfMyCombatUnitType;	/// 아군 공격 유닛 첫번째 타입, 두번째 타입 생산 순서
		int nextTargetIndexOfBuildOrderArray;					/// buildOrderArrayOfMyCombatUnitType 에서 다음 생산대상 아군 공격 유닛

		// 아군의 공격유닛 숫자
		int necessaryNumberOfCombatUnitType1;		/// 공격을 시작하기위해 필요한 최소한의 유닛 숫자 
		int necessaryNumberOfCombatUnitType2;		/// 공격을 시작하기위해 필요한 최소한의 유닛 숫자 
		int necessaryNumberOfCombatUnitType3;		/// 공격을 시작하기위해 필요한 최소한의 유닛 숫자
		int myKilledCombatUnitCount1;				/// 첫번째 유닛 타입의 사망자 숫자 누적값
		int myKilledCombatUnitCount2;				/// 두번째 유닛 타입의 사망자 숫자 누적값
		int myKilledCombatUnitCount3;				/// 세번째 유닛 타입의 사망자 숫자 누적값

		// 아군의 특수유닛 숫자
		int necessaryNumberOfSpecialUnitType1;		/// 공격을 시작하기위해 필요한 최소한의 특수 유닛 숫자 
		int necessaryNumberOfSpecialUnitType2;		/// 공격을 시작하기위해 필요한 최소한의 특수 유닛 숫자 
		int maxNumberOfSpecialUnitType1;			/// 최대 몇개까지 생산 / 전투참가 시킬것인가  
		int maxNumberOfSpecialUnitType2;			/// 최대 몇개까지 생산 / 전투참가 시킬것인가 
		int myKilledSpecialUnitCount1;				/// 첫번째 특수 유닛 타입의 사망자 숫자 누적값
		int myKilledSpecialUnitCount2;				/// 두번째 특수 유닛 타입의 사망자 숫자 누적값

		// 아군 공격 유닛 목록
		std::vector<BWAPI::Unit> myAllCombatUnitList;
		std::vector<BWAPI::Unit> myCombatUnitType1List;
		std::vector<BWAPI::Unit> myCombatUnitType2List;
		std::vector<BWAPI::Unit> myCombatUnitType3List;
		std::vector<BWAPI::Unit> mySpecialUnitType1List;
		std::vector<BWAPI::Unit> mySpecialUnitType2List;

		// 아군 방어 건물 첫번째, 두번째 타입
		BWAPI::UnitType myDefenseBuildingType1;					/// 파일런 벙커 크립
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

		// 적군 공격 유닛 숫자
		int numberOfCompletedEnemyCombatUnit;
		int numberOfCompletedEnemyWorkerUnit;
		// 적군 공격 유닛 사망자 수 
		int enemyKilledCombatUnitCount;							/// 적군 공격 유닛 사망자 숫자 누적값
		int enemyKilledWorkerUnitCount;							/// 적군 일꾼 유닛 사망자 숫자 누적값


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

		/// 방어 모드로 전환할 때인지 여부를 리턴합니다
		bool isTimeToStartDefense();

		/// 아군 공격 유닛들에게 공격을 지시합니다 
		void commandMyCombatUnitToAttack();

		/// 적군을 Eliminate 시키는 모드로 전환할지 여부를 리턴합니다 
		bool isTimeToStartElimination();

		/// 적군을 Eliminate 시키도록 아군 공격 유닛들에게 지시합니다
		void commandMyCombatUnitToEliminate();

		/// 시즈탱크 유닛에 대해 컨트롤 명령을 내립니다
		bool controlSiegeTankUnitType(BWAPI::Unit unit);

		/// 러커 유닛에 대해 컨트롤 명령을 내립니다
		bool controlLurkerUnitType(BWAPI::Unit unit);
		
		/// 첫번째 특수 유닛 타입의 유닛에 대해 컨트롤 명령을 내립니다
		bool controlSpecialUnitType1(BWAPI::Unit unit);
		
		/// 두번째 특수 유닛 타입의 유닛에 대해 컨트롤 명령을 내립니다
		bool controlSpecialUnitType2(BWAPI::Unit unit);

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

		/// 특수 유닛을 생산할 수 있도록 테크트리에 따라 건설을 실시합니다
		void executeTechTreeUpConstruction();

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
