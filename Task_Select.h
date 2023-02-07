#pragma warning(disable:4996)
#pragma once
//-------------------------------------------------------------------
//セレクトシーン
//-------------------------------------------------------------------
#include "GameEngine_Ver3_83.h"

namespace  Select
{
	//タスクに割り当てるグループ名と固有名
	const  string  defGroupName("Select");	//グループ名
	const  string  defName("NoName");	//タスク名
	//-------------------------------------------------------------------
	class  Resource : public BResource
	{
		bool  Initialize()	override;
		bool  Finalize()	override;
		Resource();
	public:
		~Resource();
		typedef  shared_ptr<Resource>	SP;
		typedef  weak_ptr<Resource>		WP;
		static   WP  instance;
		static  Resource::SP  Create();
		//共有する変数はここに追加する
		DG::Image::SP selectBack;
		DG::Image::SP selectUI;
		DG::Image::SP stageNumberUI;
		DG::Image::SP stageNumber;
		DG::Image::SP sword;
		DG::Image::SP axe;
		DG::Image::SP gun;
		DG::Image::SP jump;
		DG::Image::SP dash;
		DG::Image::SP heart;
		DG::Image::SP powerUP;
		DG::Image::SP swordSP;
		DG::Image::SP axeSP;
		DG::Image::SP gunSP;
		DG::Image::SP map[15];
	};
	//-------------------------------------------------------------------
	class  Object : public  BTask
	{
		//変更不可◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆
	public:
		virtual  ~Object();
		typedef  shared_ptr<Object>		SP;
		typedef  weak_ptr<Object>		WP;
		//生成窓口 引数はtrueでタスクシステムへ自動登録
		static  Object::SP  Create(bool flagGameEnginePushBack_);
		Resource::SP	res;
	private:
		Object();
		bool  B_Initialize();
		bool  B_Finalize();
		bool  Initialize();	//「初期化」タスク生成時に１回だけ行う処理
		void  UpDate()			override;//「実行」１フレーム毎に行う処理
		void  Render2D_AF()		override;//「2D描画」１フレーム毎に行う処理
		bool  Finalize();		//「終了」タスク消滅時に１回だけ行う処理
	//変更可◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇
	public:
		//追加したい変数・メソッドはここに追加する

		//選ばれたスキル、二つの選んだスキルの番号を格納する。

		//選択用
		MyPG::MyGameEngine::SelectedSkill s;

		//選択状態
		enum class SelectionState
		{
			Before,		//選択前
			Weapon,		//武器選択
			Skill,		//スキル選択
			Stage,		//ステージ選択
			After		//選択後
		};
		SelectionState state;

		//State更新メソッド
		bool UpDateSelectionState(SelectionState state_);

		//選択メソッド スキル、ステージを選択、初回は武器も選択
		void Select();
		bool SelectWeapon();
		bool SelectSkill();
		bool SelectStage();

		//SE再生メソッド
		void PlaySE();

		//選択された番号
		int selectedNumber;

		//ステージ選択、表示用変数
		int stage[3];

		//選択UI
		//表示最小値
		ML::Vec2 posMin;
		//表示位置
		ML::Vec2 pos;
		//表示最大値
		ML::Vec2 posMax;
		//移動距離
		ML::Vec2 moveVec;
		//描画サイズ
		ML::Box2D drawBase;

		//全15mapのsrc
		ML::Box2D mapSize[15] =
		{
			ML::Box2D(0,0,887,441),
			ML::Box2D(0,0,887,443),
			ML::Box2D(0,0,887,444),
			ML::Box2D(0,0,887,440),
			ML::Box2D(0,0,887,441),
			ML::Box2D(0,0,887,440),
			ML::Box2D(0,0,887,445),
			ML::Box2D(0,0,887,448),
			ML::Box2D(0,0,887,440),
			ML::Box2D(0,0,887,447),
			ML::Box2D(0,0,887,267),
			ML::Box2D(0,0,887,265),
			ML::Box2D(0,0,887,268),
			ML::Box2D(0,0,887,266),
			ML::Box2D(0,0,887,266)
		};

		//再生停止カウンター
		int stopCnt;
	};
}