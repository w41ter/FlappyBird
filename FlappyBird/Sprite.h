#pragma once

namespace games
{
    class Animation;

    class Sprite
    {
    public:
        Sprite();
        virtual ~Sprite();

        void setRotation(float rotation, D2D1_POINT_2F point);
        void setPosition(int left, int top, int right, int bottom);
        virtual void draw();

        void runAction(Animation *animation);

    protected:
        int delay_;

        float rotation_;
        D2D1_POINT_2F point_;

        struct {
            int left_;
            int right_;
            int top_;
            int bottom_;
        };

        Animation *animation_;
    };

}