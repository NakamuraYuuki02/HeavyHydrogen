//-------------------------------------------------------------------
//�^�C�g�����
//-------------------------------------------------------------------
#include  "MyPG.h"
#include  "Task_Title.h"
#include  "Task_Game.h"
#include  "Task_Select.h"
#include "Task_Map2D.h"

namespace  Title
{
	Resource::WP  Resource::instance;
	//-------------------------------------------------------------------
	//���\�[�X�̏�����
	bool  Resource::Initialize()
	{

		this->img = DG::Image::Create("./data/image/title.png");
		this->titleImage = DG::Image::Create("./data/image/ff8bbf23761c1c62.png");
		this->start = DG::Image::Create("./data/image/space.png");
		this->player = DG::Image::Create("./data/image/Fumiko.png");
		return true;
	}
	//-------------------------------------------------------------------
	//���\�[�X�̉��
	bool  Resource::Finalize()
	{
		this->titleImage.reset();
		this->start.reset();
		this->player.reset();
		return true;
	}
	//-------------------------------------------------------------------
	//�u�������v�^�X�N�������ɂP�񂾂��s������
	bool  Object::Initialize()
	{
		//�X�[�p�[�N���X������
		__super::Initialize(defGroupName, defName, true);
		//���\�[�X�N���X����or���\�[�X���L
		this->res = Resource::Create();

		//���f�[�^������
		this->cnt = 0;
		this->start = true;
		int d[2] = { 1, 3 };
		int anim[4] = { 4, 3, 4, 5 };
		for (int i = 0; i < 2; ++i) {
			int y = d[i];
			for (int j = 0; j < 4; ++j) {
				int x = anim[j];
				playerImage[i][j] = ML::Box2D(24 * x, 32 * y, 24, 32);
			}
		}
		this->pos.x = 50;
		this->pos.y = 220;
		this->angle = 0;
		//���^�X�N�̐���

		return  true;
	}
	//-------------------------------------------------------------------
	//�u�I���v�^�X�N���Ŏ��ɂP�񂾂��s������
	bool  Object::Finalize()
	{
		//���f�[�^���^�X�N���
		ge->KillAll_G("Title");
		ge->KillAll_G("Field");

		if (!ge->QuitFlag() && this->nextTaskCreate) {
			//if(ge->ns == MyPG::MyGameEngine::NextScene::Select)
			//�������p���^�X�N�̐���
			auto  next = Select::Object::Create(true);
		}

		return  true;
	}
	//-------------------------------------------------------------------
	//�u�X�V�v�P�t���[�����ɍs������
	void  Object::UpDate()
	{
		if (this->cnt < 30) {
			this->start = true;
		}
		else if (this->cnt < 60) {
			this->start = false;
		}
		else {
			this->cnt = 0;
		}
		this->cnt++;
		this->animCnt++;

		if (this->angle == 0) {
			this->pos.x++;
		}
		else
		{
			this->pos.x--;
		}

		if (pos.x == 400) {
			this->angle = 1;
		}
		else if (pos.x == 50) {
			this->angle = 0;
		}

		auto inp = ge->in1->GetState();

		if (inp.S1.down)
		{
			//�^�C�g���̎��̃Z���N�g�V�[�����w��B
			ge->ns = MyPG::MyGameEngine::NextScene::Select;
			//���g�ɏ��ŗv��
			this->Kill();
		}
	}
	//-------------------------------------------------------------------
	//�u�Q�c�`��v�P�t���[�����ɍs������
	void  Object::Render2D_AF()
	{
		ML::Box2D  draw(0, 0, 480, 270);
		ML::Box2D  src(0, 0, 846, 852);

		this->res->img->Draw(draw, src);

		ML::Box2D draw2(ge->screen2DWidth / 2 - 100, ge->screen2DHeight / 4, 200, 40);
		ML::Box2D src2(0, 0, 207, 32);

		this->res->titleImage->Draw(draw2, src2);

		if (this->start) {
			ML::Box2D draw3(ge->screen2DWidth / 2 - 75, ge->screen2DHeight / 2 + 50, 150, 20);
			ML::Box2D src3(0, 0, 241, 32);

			this->res->start->Draw(draw3, src3);
		}

		ML::Box2D draw4(0, 0, 24, 32);
		draw4.Offset(this->pos);
		ML::Box2D src4 = playerImage[this->angle][(this->animCnt / 10) % 4];

		this->res->player->Draw(draw4, src4);
		//ML::Box2D src5(24 * 4, 32, 24, 32);	//���s1				
		//ML::Box2D src6(24 * 3, 31, 24, 32);	//���s�Q				
		//ML::Box2D src7(24 * 4, 32, 24, 32);	//���s�R				
		//ML::Box2D src8(24 * 5, 32, 24, 32);	//���s�S				
	}
	//������������������������������������������������������������������������������������
	//�ȉ��͊�{�I�ɕύX�s�v�ȃ��\�b�h
	//������������������������������������������������������������������������������������
	//-------------------------------------------------------------------
	//�^�X�N��������
	Object::SP  Object::Create(bool  flagGameEnginePushBack_)
	{
		Object::SP  ob = Object::SP(new  Object());
		if (ob) {
			ob->me = ob;
			if (flagGameEnginePushBack_) {
				ge->PushBack(ob);//�Q�[���G���W���ɓo�^
			}
			if (!ob->B_Initialize()) {
				ob->Kill();//�C�j�V�����C�Y�Ɏ��s������Kill
			}
			return  ob;
		}
		return nullptr;
	}
	//-------------------------------------------------------------------
	bool  Object::B_Initialize()
	{
		return  this->Initialize();
	}
	//-------------------------------------------------------------------
	Object::~Object() { this->B_Finalize(); }
	bool  Object::B_Finalize()
	{
		auto  rtv = this->Finalize();
		return  rtv;
	}
	//-------------------------------------------------------------------
	Object::Object() {	}
	//-------------------------------------------------------------------
	//���\�[�X�N���X�̐���
	Resource::SP  Resource::Create()
	{
		if (auto sp = instance.lock()) {
			return sp;
		}
		else {
			sp = Resource::SP(new  Resource());
			if (sp) {
				sp->Initialize();
				instance = sp;
			}
			return sp;
		}
	}
	//-------------------------------------------------------------------
	Resource::Resource() {}
	//-------------------------------------------------------------------
	Resource::~Resource() { this->Finalize(); }
}