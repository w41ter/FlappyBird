#include "main.h"
#include "Animation.h"
#include "TimeCounter.h"

namespace games
{
    void Animation::addSpriteFrameWithFileName(wchar_t * filename)
    {
        assert(filename != NULL);
        frames_.push_back(filename);
    }

    void Animation::setDelayPerUnit(unsigned int delay)
    {
        delay_ = delay;
    }

    void Animation::setRestoreOriginalFrame(bool status)
    {
        resotreOriginalFrame_ = status;
    }

    void Animation::setOnAllFrameDone(Callback callback)
    {
        callback_ = std::move(callback);
    }

    void Animation::begin()
    {
        if (isPlay_)
            return;
        curFrames_ = 0;
        id_ = TimeCounter::instance().Schedule(1, delay_, [this] { nextFrame(); });
        isPlay_ = true;
    }

    void Animation::end()
    {
        if (!isPlay_)
            return;
        TimeCounter::instance().Release(id_);
        isPlay_ = false;
    }

    const std::wstring & Animation::getCurrentFrameFileName()
    {
        return frames_[curFrames_];
    }

    void Animation::nextFrame()
    {
        if (curFrames_ + 1 != frames_.size())
        {
            curFrames_++;
            return;
        }
        if (resotreOriginalFrame_)
        {
            curFrames_ = 0;
        }
        if (callback_)
            callback_();
    }

    Animation::Animation()
        : delay_(30)
        , resotreOriginalFrame_(false)
        , curFrames_(0)
        , isPlay_(false)
    {
    }


    Animation::~Animation()
    {
        end();
    }
}