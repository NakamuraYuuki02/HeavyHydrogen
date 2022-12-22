#pragma once
#pragma warning(disable:4996)
#pragma once
//-----------------------------------------------------------------------------
//�L�����N�^�ėp�X�[�p�[�N���X
//-----------------------------------------------------------------------------
#include "GameEngine_Ver3_83.h"

class BChara : public BTask
{
	//�ύX�s����������������������������������������������������
public:
	typedef shared_ptr<BChara>		SP;
	typedef weak_ptr<BChara>		WP;
public:
	//�ύX������������������������������������������������������
	//�L�����N�^���ʃ����o�ϐ�
	ML::Vec2    pos;		//�L�����N�^�ʒu
	ML::Box2D   hitBase;	//�����蔻��͈�
	ML::Vec2	moveVec;	//�ړ��x�N�g��
	int			moveCnt;	//�s���J�E���^
	int         hp;         //�L�����N�^�̗�
	int			atk;		//�U����
	//���E�̌����i2D�����_�Q�[����p�j
	enum class Angle_LR { Left, Right };
	Angle_LR	angle_LR;
	WP			target;

	//�L�����N�^�̍s����ԃt���O
	enum class Motion
	{
		Unnon = -1,	//	����(�g���܂���j
		Stand,		//	��~
		Walk,		//	���s
		Attack,		//	�U��
		Jump,		//	�W�����v
		Jump2,      //  �W�����v2
		Jump3,      //	�W�����v3
		Fall,		//	����
		Fall2,      //  ����2
		Fall3,		//	����3
		TakeOff,	//	��ї��u��
		Landing,	//	���n
		Turn,       //  �����ς���
		Bound,      //�@�e����΂���Ă���
		Lose,       //�@���Œ�
		Dash,       //  �_�b�V��
		DashCt,     //�@�_�b�V���N�[���^�C��
	};
	Motion			motion;			//	���݂̍s���������t���O
	int				animCnt;		//	�A�j���[�V�����J�E���^
	float			jumpPow;		//	�W�����v����
	float			maxFallSpeed;	//	�����ő呬�x
	float			gravity;		//	�t���[���P�ʂ̉��Z��
	float			maxSpeed;		//	���E�����ւ̈ړ��̉��Z��
	float			addSpeed;		//	���E�����ւ̈ړ��̉��Z��
	float			decSpeed;		//	���E�����ւ̈ړ��̌�����
	float           dashSpeed;      //  �_�b�V�����ɉ��Z�����ړ���
	int             unHitTime;      //  ���G����
	int				jumpMax;		//	�W�����v�����
	int				dashMax;		//	�_�b�V�������
	int				attackCnt;		//
	int				attackMax;		//
	int				WeaponLevel;	//	����	Lv

	//�����o�ϐ��ɍŒ���̏��������s��
	//���������o�ϐ���ǉ�������K�����������ǉ����鎖����
	BChara()
		: pos(0, 0)
		, hitBase(0, 0, 0, 0)
		, moveVec(0, 0)
		, moveCnt(0)
		, angle_LR(Angle_LR::Right)
		, motion(Motion::Stand)
		, jumpPow(0)
		, maxFallSpeed(0)
		, gravity(0)
		, maxSpeed(0)
		, addSpeed(0)
		, decSpeed(0)
		, dashSpeed(0)
		, hp(1)
		, atk(0)
		, unHitTime(0)
		, jumpMax(1)
		, dashMax(1)
		, attackCnt(0)
		, WeaponLevel(0)
	{
	}
	virtual  ~BChara() {}

	//�L�����N�^���ʃ��\�b�h
	//�߂荞�܂Ȃ��ړ�����
	virtual  void  CheckMove(ML::Vec2&  est_);
	//���ʐڐG����(�T�C�h�r���[�Q�[����p)
	virtual bool CheckFront_LR();
	//�����ڐG����
	bool  CheckFoot();
	//����ڐG����
	virtual  bool  CheckHead();
	//���[�V�������X�V�i�ύX�Ȃ��̏ꍇ	false)
	bool  UpdateMotion(Motion  nm_);

	//	�A�j���[�V�������\����
	struct DrawInfo {
		ML::Box2D		draw, src;
		ML::Color		color;
	};
	//�U�����
	struct AttackInfo {
		int power;//�U���̈З�
		int hit;//�������x
		int element;//�U���̑���
		//���̑��K�v�ɉ�����
	};
	//�ڐG���̉�������(���ꎩ�̂̓_�~�[�̂悤�Ȃ���)
	virtual void Received(BChara* from_, AttackInfo at_);
	//�ڐG����
	virtual bool CheckHit(const ML::Box2D& hit_);
};
