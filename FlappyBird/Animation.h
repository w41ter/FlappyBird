#pragma once

namespace games
{
    class Animation
    {
        using Callback = std::function<void()>;

    public:
        Animation();
        ~Animation();

        void addSpriteFrameWithFileName(wchar_t *filename);
        void setDelayPerUnit(unsigned int delay);
        void setRestoreOriginalFrame(bool status);
        void setOnAllFrameDone(Callback callback);

        void begin();
        void end();

        const std::wstring &getCurrentFrameFileName();

    private:

        void nextFrame();

    private:
        unsigned int id_;
        unsigned int delay_;

        bool isPlay_;
        bool resotreOriginalFrame_;

        size_t curFrames_;

        Callback callback_;
        std::vector<std::wstring> frames_;
    };

}