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
		DG::Image::SP UI;
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

		//選択回数
		const int selectNum = 2;

		//選択された武器
		enum class Weapon
		{
			Sword,
			Axe,
			Gun
		};
		Weapon weapon;

		//選ばれたスキル、二つの選んだスキルの番号を格納する。
		vector<int> selectedActionSkill;
		vector<int> selectedStatusSkill;
		vector<int> selectedWeaponSkill;

		//選ばれたステージ
		int selectedStage;

		//ステージ通過数、ゲーム進行度
		int stageNum;
		//プレイヤーステータス
		int hp;				//体力
		int hpMax;			//最大体力
		int atk;			//攻撃力
		int jumpCnt;		//ジャンプ回数
		int jumpMax;		//ジャンプ上限回数
		int dashCnt;		//ダッシュ回数
		int dashMax;		//ダッシュ上限回数

		//選択状態
		enum class SelectionState
		{
			Before,		//選択前
			Weapon,		//武器選択
			Skill,		//スキル選択
			Stage,		//ステージ選択
			After		//選択後
		};
		SelectionState ss;

		//選択メソッド スキル、ステージを選択、初回は武器も選択
		void Select();
		void SelectedWeapon();
		void SelectedSkill();
		void SelectedStage();

		//選択回数 number of selections
		int nos;
		//選択された番号
		int selectedNumber;

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
	};
}