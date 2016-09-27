#include "main.h"
#include "MusicManager.h"

/* 初始化音乐管理静态指针 */
MusicManager *MusicManager::Mmanager = NULL;

////////////////////////////////////////////////////////////////////////////////////////////////////////

MusicManager::MusicManager()
{
    this->Mmanager = this;
    this->RunFlag = false;
}

MusicManager::~MusicManager()
{
    this->Release(true);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
//增加音乐，只能在音乐线程未开始的情况下使用
bool MusicManager::AddMusic(Music* music)
{
    if (this->RunFlag == true)
        return false;
    if (music != NULL)
    {
        this->vMusic.push_back(music);
        return true;
    }
    return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
//释放资源
void MusicManager::Release(bool Del)
{
    if (!this->vMusic.empty())
    {
        std::vector<Music*>::iterator i;
        for (i = this->vMusic.begin();i != this->vMusic.end();i++)
        {
            (*i)->Close();
            if (Del)
                delete *i;
        }
        this->vMusic.clear();
        this->Compress();
    }
}

//删除指定的音乐
void MusicManager::DeleteMusic(Music* music, bool Del)
{
    if (!this->vMusic.empty())
    {
        std::vector<Music*>::iterator i;
        for (i = this->vMusic.begin();i != this->vMusic.end();i++)
        {
            if ((*i)->GetID() == music->GetID())
            {
                music->Close();
                if (Del)
                    delete music;
                this->vMusic.erase(i);
                break;
            }
        }
        this->Compress();
    }
}

//压缩容器
void MusicManager::Compress()
{
    std::vector<Music*> temp(this->vMusic);
    this->vMusic.swap(temp);
    temp.~vector();
}

//根据ID查找音乐是否存在，存在就返回指针，否则返回NULL
Music* MusicManager::FindMusic(int ID)
{
    if (!this->vMusic.empty())
    {
        std::vector<Music*>::iterator i;
        for (i = this->vMusic.begin();i != this->vMusic.end();i++)
        {
            if ((*i)->GetID() == ID)
                return *i;
        }
    }
    return NULL;;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
//加载到线程循环中的方法
void MusicManager::Run(MusicParam MP, bool Change)
{
    std::vector<Music*>::iterator i;
    if (Change == true)
        for (i = this->vMusic.begin();i != this->vMusic.end();i++)
            if (MP.ID == (*i)->GetID())
            {
                if (MP.Delete == true)
                    this->DeleteMusic(*i, true);
                else
                    this->Change(*i, MP);
            }
    for (i = this->vMusic.begin();i != this->vMusic.end();i++)
    {
        //失去焦点时暂停播放
        if (MP.KillFocus == true)
        {
            (*i)->KillFocusFlag = true;
            //(*i)->SetVolume(0);	//也可以简单处理成设置音量为0，但是这对midi无效
        }
        //获得焦点时恢复播放
        if (MP.SetFocus == true)
        {
            (*i)->KillFocusFlag = false;
            (*i)->SetFocusFlag = true;
            //(*i)->SetVolume((*i)->Volume);	//重设音量
        }
        if (MP.ID == (*i)->GetID())
            this->Play(*i, MP.Play, MP.ReStart, MP.Stop);
        else
            this->Play(*i, false, false, false);
    }
}

//初始化音乐
void MusicManager::Initialize()
{
    std::vector<Music*>::iterator i;
    for (i = this->vMusic.begin();i != this->vMusic.end();i++)
    {
        if ((*i)->filepath == L"")
            continue;
        (*i)->Open((*i)->filepath);
        (*i)->SetVolume((*i)->Volume);
    }
}

//播放
void MusicManager::Play(Music* music, bool Play, bool ReStart, bool Stop)
{
    if (music->KillFocusFlag == true)
    {
        //失去焦点时停止播放
        if (music->GetStopState() == false)
        {
            music->Stop();
            music->NowPosition = music->GetNowPosition();	//记录位置
        }
        return;	//不修改状态，保留现场
    }
    else if (music->SetFocusFlag == true)
    {
        //获取焦点时，根据之前保留的状态恢复播放
        music->SetFocusFlag = false;
        if (music->Opened == true && music->Playing == true)
            music->Play(false, music->NowPosition, music->EndPosition);
    }
    else if (Stop == true)
    {
        //停止音乐
        music->Stop();
        music->NowPosition = music->GetNowPosition();	//记录位置
        music->StopPlayFlag = true;	//设置停止播放标志
    }
    else if (ReStart == true)
    {
        //重放
        music->Play(true, music->StartPosition, music->EndPosition);
        music->StopPlayFlag = false;
    }
    else if (music->RepeatFlag == true && music->StopPlayFlag == false)
    {
        //循环播放，可以响应重放的要求
        if (music->GetStopState() == true)
            music->Play(false, music->StartPosition, music->EndPosition);
    }
    else
    {
        //继续播放
        if (Play == true && music->StopPlayFlag == true)
        {
            music->Play(false, music->NowPosition, music->EndPosition);
            music->StopPlayFlag = false;
        }
        //播放一次，停止播放时才有效
        else if (Play == true && music->GetStopState() == true)
            music->Play(false, music->StartPosition, music->EndPosition);
    }
    //设置状态
    this->SetStatus(music);
}

//修改参数
void MusicManager::Change(Music* music, MusicParam MP)
{
    if (MP.filepath != L"")
        if (music->filepath != MP.filepath)
        {
            //音乐文件名被变更，重新读入音乐文件
            music->Open(MP.filepath);
        }
    if (MP.Volume >= 0)
        if (music->Volume != MP.Volume)
        {
            //音量改变
            music->Volume = MP.Volume;
            music->SetVolume(MP.Volume);
        }
    if (MP.StartPosition >= 0)
        if (MP.StartPosition != music->StartPosition)
            music->StartPosition = MP.StartPosition;	//音乐开始位置改变
    if (MP.EndPosition >= 0)
        if (MP.EndPosition != music->EndPosition)
            music->EndPosition = MP.EndPosition;	//音乐结束位置改变
    if (MP.EnableRepeat == true)
        music->RepeatFlag = MP.Repeat;	//改变循环播放标志
}

//设置状态
void MusicManager::SetStatus(Music* music)
{
    music->Opened = music->GetOpenState();
    music->Playing = music->GetPlayState();
    music->Stopped = music->GetStopState();
}