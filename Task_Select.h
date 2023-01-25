#pragma warning(disable:4996)
#pragma once
//-------------------------------------------------------------------
//�Z���N�g�V�[��
//-------------------------------------------------------------------
#include "GameEngine_Ver3_83.h"

namespace  Select
{
	//�^�X�N�Ɋ��蓖�Ă�O���[�v���ƌŗL��
	const  string  defGroupName("Select");	//�O���[�v��
	const  string  defName("NoName");	//�^�X�N��
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
		//���L����ϐ��͂����ɒǉ�����
		DG::Image::SP selectBack;
		DG::Image::SP selectUI;
		DG::Image::SP sword;
		DG::Image::SP axe;
		DG::Image::SP gun;
	};
	//-------------------------------------------------------------------
	class  Object : public  BTask
	{
		//�ύX�s����������������������������������������������������
	public:
		virtual  ~Object();
		typedef  shared_ptr<Object>		SP;
		typedef  weak_ptr<Object>		WP;
		//�������� ������true�Ń^�X�N�V�X�e���֎����o�^
		static  Object::SP  Create(bool flagGameEnginePushBack_);
		Resource::SP	res;
	private:
		Object();
		bool  B_Initialize();
		bool  B_Finalize();
		bool  Initialize();	//�u�������v�^�X�N�������ɂP�񂾂��s������
		void  UpDate()			override;//�u���s�v�P�t���[�����ɍs������
		void  Render2D_AF()		override;//�u2D�`��v�P�t���[�����ɍs������
		bool  Finalize();		//�u�I���v�^�X�N���Ŏ��ɂP�񂾂��s������
	//�ύX������������������������������������������������������
	public:
		//�ǉ��������ϐ��E���\�b�h�͂����ɒǉ�����

		//�I�΂ꂽ�X�L���A��̑I�񂾃X�L���̔ԍ����i�[����B

		//�I��p
		MyPG::MyGameEngine::SelectedSkill s;

		//�I�����
		enum class SelectionState
		{
			Before,		//�I��O
			Weapon,		//����I��
			Skill,		//�X�L���I��
			Stage,		//�X�e�[�W�I��
			After		//�I����
		};
		SelectionState state;

		//State�X�V���\�b�h
		bool UpDateSelectionState(SelectionState state_);

		//�I�����\�b�h �X�L���A�X�e�[�W��I���A����͕�����I��
		void Select();
		bool SelectWeapon();
		bool SelectSkill();
		bool SelectStage();

		//�I�����ꂽ�ԍ�
		int selectedNumber;

		//�X�e�[�W�I���A�\���p�ϐ�
		int stage[3];

		//�I��UI
		//�\���ŏ��l
		ML::Vec2 posMin;
		//�\���ʒu
		ML::Vec2 pos;
		//�\���ő�l
		ML::Vec2 posMax;
		//�ړ�����
		ML::Vec2 moveVec;
		//�`��T�C�Y
		ML::Box2D drawBase;
	};
}