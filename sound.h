#pragma once
//----------------------------------------------
//音楽再生ライブラリ
// Ci-Lib派生 汎用関数
//      k.koyanagi
// 2021.07.07 SEボリューム調整バグ修正
//----------------------------------------------

#include <assert.h> 
#include <string>
#	include <dshow.h>
#	include <dsound.h>

namespace bgm{
	//BGM再生関数
	void Init();
	void Del();
	void LoadFile(const std::string& resname, const std::string& filename);
	bool Play(const std::string& resname);
	void RateControl(const std::string& resname, float rate);
	void EndCheck();
	void Stop(const std::string& resname);
	void AllStop();
	LONGLONG GetCurrentPos(const std::string& resname);
	void SetStartPos(const std::string& resname);
	void VolumeControl(const std::string& resname, int volume);
	void Pause(const std::string& resname);
	LONGLONG GetEndPosition(const std::string& resname);
}
namespace se{
	//SE再生関数
	void Init(HWND);
	bool Create(HWND);
	void CreateSecondaryBuffer();
	bool CreatePrimaryBuffer();
	bool Del();
	void Play(const std::string& resname);
	void PlayLoop(const std::string& resname);
	bool Del();
	void LoadFile(const std::string& resname, const std::string& filename);
	void AllStop();
	void Stop(const std::string& resname);
	void EndCheck();
	enum class RATE_SET{NORMAL,DOUBLE,HALF};
	void SetFrequency(const std::string& resname, RATE_SET f);
	enum class RATE_CONTROL { NORMAL, FASTER, LATER };
	void SetFrequency(const std::string& resname, RATE_CONTROL f);
	DWORD SetFrequency(const std::string& resname, int f);
	long SetVolume(const std::string& resname, int Vol);
}


