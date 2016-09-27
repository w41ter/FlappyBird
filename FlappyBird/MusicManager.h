#pragma once

#include "Music.h"
#include <vector>
#include <queue>

//音乐播放参数，用于主线程对音乐线程的通信
typedef struct tagMusicParam {
    int ID;		        //要执行命令的音乐对象ID

                        //优先级最高
    bool Delete;	    //要求删除音乐对象
    bool Exit;		    //要求音乐线程退出

                        //优先级第二
    std::wstring filepath;	//为""表示不改变路径，否则改变音乐文件路径
    int Volume;	        //音量，为负数表示不改变
    long StartPosition;	//音乐开始位置，为负数表示不改变
    long EndPosition;	//音乐结束位置，为负数表示不改变
    bool EnableRepeat;	//设为true时才用Repeat设置音乐的循环播放标志
    bool Repeat;	    //设置音乐的循环播放标志

                        //优先级最低
    bool ReStart;	    //为true表示要求重放
    bool Play;		    //为true表示要求播放一次，仅在音乐停止时有效
    bool Stop;		    //为true表示要求停止

                        //游戏窗口失去焦点、重新获得焦点的标志
    bool KillFocus;
    bool SetFocus;

    tagMusicParam()
    {
        this->ID = -1;
        this->Delete = false;
        this->Exit = false;
        this->filepath = L"";
        this->Volume = -1;
        this->StartPosition = -1;
        this->EndPosition = -1;
        this->EnableRepeat = false;
        this->Repeat = false;
        this->ReStart = false;
        this->Play = false;
        this->Stop = false;
        this->KillFocus = false;
        this->SetFocus = false;
    }

}MusicParam;

//与音乐线程进行通信的消息队列
static std::queue<MusicParam> g_MusicMessageQueue;

//音乐线程的句柄
static HANDLE g_hThreadForMusic;

////////////////////////////////////////////////////////////////////////////////////////////////////////
class MusicManager
{
protected:
    std::vector<Music*> vMusic;	//音乐容器
    static MusicManager* Mmanager;	//指向自己的指针
    bool RunFlag;	//判断音乐线程是否启动，用于约束增加音乐方法

public:
    MusicManager();
    ~MusicManager();

    ////////////////////////////////////////////////////////////////////////////////////////////
    //增加音乐，只能在音乐线程未开始的情况下使用
    bool AddMusic(Music* music);

protected:
    ////////////////////////////////////////////////////////////////////////////////////////////
    //线程才可进行的操作
    //释放资源
    void Release(bool Del = false);

    //删除指定ID的音乐
    void DeleteMusic(Music* music, bool Del);

    //压缩容器
    void Compress();

    //根据ID查找音乐是否存在，存在就返回指针，否则返回NULL
    Music* FindMusic(int ID);

    //获取指向该对象的指针
    static MusicManager* GetPointer() {
        return Mmanager;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////
    //加载到线程循环中的方法
    void Run(MusicParam MP, bool Change = false);

    //初始化音乐
    void Initialize();

    //播放
    void Play(Music* music, bool Play, bool ReStart, bool Stop);

    //修改参数
    void Change(Music* music, MusicParam MP);

    //设置状态
    void SetStatus(Music* music);

public:
    ////////////////////////////////////////////////////////////////////////////////////////////
    //音乐线程的方法
    static unsigned __stdcall ThreadForPlayMusic(LPVOID lpParameter)
    {
        MusicManager* g_pMmanager = MusicManager::GetPointer();
        g_pMmanager->RunFlag = true;
        g_pMmanager->Initialize();
        while (true)
        {
            Sleep(1);
            MusicParam MP;
            if (g_MusicMessageQueue.empty() != true)
            {
                MP = g_MusicMessageQueue.front();
                g_MusicMessageQueue.pop();
                if (MP.Exit == true)
                    break;
                g_pMmanager->Run(MP, true);
            }
            else
                g_pMmanager->Run(MP);
        }
        delete g_pMmanager;		//退出时线程会清理音乐相关的所有资源
        return 0;
    }
};
