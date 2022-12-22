//-----------------------------------------------------------------------------
//キャラクタ汎用スーパークラス
//-----------------------------------------------------------------------------
#include "BChara.h"
#include "MyPG.h"
#include "Task_Map2D.h"



//-----------------------------------------------------------------------------
//モーションを更新（変更なしの場合	false)
bool  BChara::UpdateMotion(Motion  nm_)
{
	if (nm_ == this->motion) {
		return  false;
	}
	else {
		this->motion = nm_;		//モーション変更
		this->moveCnt = 0;		//行動カウンタクリア
		this->animCnt = 0;		//アニメーションカウンタのクリア
		return  true;
	}
}

//-----------------------------------------------------------------------------
//頭上接触判定
bool  BChara::CheckHead()
{
	//あたり判定を基にして頭上矩形を生成
	ML::Box2D  head(this->hitBase.x,
		this->hitBase.y - 1,
		this->hitBase.w,
		1);
	head.Offset((int)this->pos.x, (int)this->pos.y);


	auto   map = ge->GetTask<Map2D::Object>(Map2D::defGroupName, Map2D::defName);
	if (nullptr == map) { return false; }//マップが無ければ判定しない(出来ない）
	return map->CheckHit(head);
}
//-----------------------------------------------------------------------------
//めり込まない移動処理
void BChara::CheckMove(ML::Vec2&  e_)
{
	//マップが存在するか調べてからアクセス
	auto   map = ge->GetTask<Map2D::Object>(Map2D::defGroupName, Map2D::defName);
	if (nullptr == map) { return; }//マップが無ければ判定しない(出来ない）

   //横軸に対する移動
	while (e_.x != 0) {
		float  preX = this->pos.x;
		if (e_.x >= 1) { this->pos.x += 1;		e_.x -= 1; }
		else if (e_.x <= -1) { this->pos.x -= 1;		e_.x += 1; }
		else { this->pos.x += e_.x;		e_.x = 0; }
		ML::Box2D  hit = this->hitBase.OffsetCopy(this->pos);
		if (true == map->CheckHit(hit)) {
			this->pos.x = preX;		//移動をキャンセル
			break;
		}
	}
	//縦軸に対する移動
	while (e_.y != 0) {
		float  preY = this->pos.y;
		if (e_.y >= 1) { this->pos.y += 1;		e_.y -= 1; }
		else if (e_.y <= -1) { this->pos.y -= 1;		e_.y += 1; }
		else { this->pos.y += e_.y;		e_.y = 0; }
		ML::Box2D  hit = this->hitBase.OffsetCopy(this->pos);
		if (true == map->CheckHit(hit)) {
			this->pos.y = preY;		//移動をキャンセル
			break;
		}
	}
}
//-----------------------------------------------------------------------------
//足元接触判定
bool  BChara::CheckFoot()
{
	//あたり判定を基にして足元矩形を生成
	ML::Box2D  foot(this->hitBase.x,
		this->hitBase.y + this->hitBase.h,
		this->hitBase.w,
		1);
	foot.Offset(this->pos);

	auto   map = ge->GetTask<Map2D::Object>(Map2D::defGroupName, Map2D::defName);
	if (nullptr == map) { return  false; }//マップが無ければ判定しない(出来ない）
	//マップと接触判定
	return map->CheckHit(foot);
}
//-----------------------------------------------------------------------------
//正面接触判定
bool BChara::CheckFront_LR()
{
	//当たり判定を基にして矩形を生成(とりあえず、横幅だけ1になった矩形を用意する)
	ML::Box2D front(this->hitBase.x, this->hitBase.y, 1, this->hitBase.h);
	//キャラクタの向きにより矩形の位置を調整
	if (this->angle_LR == Angle_LR::Left)
	{
		front.Offset(-1, 0);//左側に移動
	}
	else
	{
		front.Offset(this->hitBase.w, 0);//右側に移動
	}
	//現在位置に合わせる
	front.Offset((int)this->pos.x, (int)this->pos.y);
	auto map = ge->GetTask<Map2D::Object>(Map2D::defGroupName, Map2D::defName);
	if (nullptr == map)
	{
		return false;
		//マップが無ければ判定しない(出来ない)
	}
	return map->CheckHit(front);
}
//-----------------------------------------------------------------------------
// 
bool BChara::CheckFrontFoot_LR()
{
	ML::Box2D front(this->hitBase.x, this->hitBase.y + this->hitBase.h, 1, 1);
	//キャラクタの向きにより矩形の位置を調整
	if (this->angle_LR == Angle_LR::Left)
	{
		front.Offset(-1, 0);//左側に移動
	}
	else
	{
		front.Offset(this->hitBase.w, 0);//右側に移動
	}
	//現在位置に合わせる
	front.Offset((int)this->pos.x, (int)this->pos.y);
	auto map = ge->GetTask<Map2D::Object>(Map2D::defGroupName, Map2D::defName);
	if (nullptr == map)
	{
		return false;
		//マップが無ければ判定しない(出来ない)
	}
	return map->CheckHit(front);

}
//-----------------------------------------------------------------------------
//接触時の応答処理(これ自体はダミーのようなもの)
void BChara::Received(BChara* from_, AttackInfo at_)
{
	//ML::MsgBox("Received 実装されていません");
}
//-----------------------------------------------------------------------------
//接触判定
bool BChara::CheckHit(const ML::Box2D& hit_)
{
	ML::Box2D me = this->hitBase.OffsetCopy(this->pos);
	return me.Hit(hit_);
}
//-----------------------------------------------------------------------
//索敵範囲判定
bool BChara::CheckNear(const ML::Vec2& tg_)
{
	ML::Vec2 newVec = this->pos - tg_;
	return (newVec.Length() <= this->searchRadius);
}
