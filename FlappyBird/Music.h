#pragma once

#include <process.h>
#include <string>

class Music
{
protected:
    friend class MusicManager;

    int	m_ID;				    //编号，用于判断是否是同一个对象
    static int m_nNewID;		//下一个可用编号

    std::wstring filepath;	    //为""表示不改变路径，否则改变音乐文件路径
    int Volume;	                //音量，为负数表示不改变
    long StartPosition;	        //音乐开始位置，为负数表示不改变
    long EndPosition;	        //音乐结束位置，为负数表示不改变
    int NowPosition;		    //用于记录暂停时的音乐位置

    bool StopPlayFlag;	        //暂停播放的标志
    bool RepeatFlag;	        //循环播放的标志
    bool KillFocusFlag;		    //失去焦点标志
    bool SetFocusFlag;		    //获取焦点标志

                                //标志位，用于获取音乐状态
    bool Playing;	            //是否正在播放
    bool Stopped;	            //是否停止
    bool Opened;	            //是否打开了音乐文件

public:
    Music();
    Music(std::wstring filepath, bool repeat = false);
    ~Music();

    ////////////////////////////////////////////////////////////////////////////////////////////
    /*给主线程使用的方法*/
    //获取编号
    int GetID();

    //获取音乐状态
    bool IsPlaying();
    bool IsStopped();
    bool IsOpened();

protected:
    ////////////////////////////////////////////////////////////////////////////////////////////
    /*给音乐线程使用的方法*/
    bool GetPlayState();    //获取音乐播放状态
    bool GetOpenState();    //获取音乐文件打开状态
    bool GetStopState();    //获取音乐停止状态
    long GetNowPosition();  //获取音乐当前位置

    bool Open(std::wstring filepath);
    bool Close();
    void SetVolume(int nVolume);
    bool Stop();
    bool Play(bool ReStart, long from, long to);

    //只接受字符串命令的播放方式
    bool Play(std::wstring Command);

    static bool SendToMCI(std::wstring command);        //发送信息给MCI
    static std::wstring GetFromMCI(std::wstring command);//从MCI获取信息
};

