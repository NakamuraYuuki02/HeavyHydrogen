//-------------------------------------------------------------------
//�G���f�B���O
//-------------------------------------------------------------------
#include  "MyPG.h"
#include  "Task_Ending.h"
#include  "Task_Title.h"

namespace  Ending
{
	Resource::WP  Resource::instance;
	//-------------------------------------------------------------------
	//���\�[�X�̏�����
	bool  Resource::Initialize()
	{
		this->endBack = DG::Image::Create("./data/image/endBack.png");
		this->pressQ = DG::Image::Create("./data/image/pressQ.png");
		this->clear = DG::Image::Create("./data/image/clear.png");
		this->number = DG::Image::Create("./data/image/UItext.png");
		this->rankA = DG::Image::Create("./data/image/rankA.png");
		this->rankB = DG::Image::Create("./data/image/rankB.png");
		this->rankC = DG::Image::Create("./data/image/rankC.png");
		return true;
	}
	//-------------------------------------------------------------------
	//���\�[�X�̉��
	bool  Resource::Finalize()
	{
		this->endBack.reset();
		this->pressQ.reset();
		this->clear.reset();
		this->number.reset();
		this->rankA.reset();
		this->rankB.reset();
		this->rankC.reset();
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
		this->time = 0;
		this->timeCnt = 0;

		//���^�X�N�̐���

		return  true;
	}
	//-------------------------------------------------------------------
	//�u�I���v�^�X�N���Ŏ��ɂP�񂾂��s������
	bool  Object::Finalize()
	{
		//���f�[�^���^�X�N���


		if (!ge->QuitFlag() && this->nextTaskCreate) {
			//�������p���^�X�N�̐���
			auto  nextTask = Title::Object::Create(true);
		}

		return  true;
	}
	//-------------------------------------------------------------------
	//�u�X�V�v�P�t���[�����ɍs������
	void  Object::UpDate()
	{
		auto inp = ge->in1->GetState();

		this->timeCnt++;
		if (this->timeCnt == 45)
		{
			this->timeCnt = 0;
			this->time++;
		}

		if (inp.ST.down || inp.S1.down)
		{
			ge->ns = MyPG::MyGameEngine::NextScene::Title;
			//���g�ɏ��ŗv��
			this->Kill();
		}
	}
	//-------------------------------------------------------------------
	//�u�Q�c�`��v�P�t���[�����ɍs������
	void  Object::Render2D_AF()
	{
		//�G���f�B���O�w�i
		ML::Box2D draw(0, 0, ge->screen2DWidth, ge->screen2DHeight);
		ML::Box2D src(50, 50, 800, 400);
		this->res->endBack->Draw(draw, src);

		//pressQkey
		ML::Box2D Qdraw(150, 220, 192, 32);
		ML::Box2D Qsrc(0, 0, 192, 32);
		if (this->time % 2 == 0)
		{
			this->res->pressQ->Draw(Qdraw, Qsrc);
		}
		
		//�N���A�����X�e�[�W��
		ML::Box2D Cdraw(100, 10, 150, 32);
		ML::Box2D Csrc(0, 0, 225, 32);
		this->res->clear->Draw(Cdraw, Csrc);
		
		//�N���A�����X�e�[�W����number
		char mapNum[10];
		sprintf(mapNum, "%02d", ge->elapsedNum - 1);
		for (int i = 0; i < 2; ++i) 
		{
			int num = mapNum[i] - '0';//'0'== 48
			ML::Box2D numDraw(250, 50, 50, 50);
			ML::Box2D numSrc(num * 30, 0, 30, 52);
			numDraw.Offset(i * 35, 0);//�����Ԋu
			this->res->number->Draw(numDraw, numSrc);
		}

		//�X�e�[�W�N���A���ɂ���Č��t��ς���
		if (ge->elapsedNum - 1 <= 2)
		{
			ML::Box2D cAdraw(155, 150, 208, 64);
			ML::Box2D cAsrc(0, 0, 104, 32);
			this->res->rankC->Draw(cAdraw, cAsrc);
		}
		else if (ge->elapsedNum - 1 <= 4)
		{
			ML::Box2D cBdraw(150, 150, 208, 64);
			ML::Box2D cBsrc(0, 0, 104, 32);
			this->res->rankB->Draw(cBdraw, cBsrc);
		}
		else if (ge->elapsedNum - 1 == 5)
		{
			ML::Box2D cCdraw(150, 150, 208, 64);
			ML::Box2D cCsrc(0, 0, 104, 32);
			this->res->rankA->Draw(cCdraw, cCsrc);
		}
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