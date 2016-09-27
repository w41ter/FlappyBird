#include "main.h"
#include "Sprite.h"

#include "Animation.h"
#include "Graphics.h"

namespace games
{
    Sprite::Sprite()
        : animation_(nullptr), rotation_(0.f), point_(D2D1::Point2F(0, 0))
    {
    }


    Sprite::~Sprite()
    {
    }

    void Sprite::setRotation(float rotation, D2D1_POINT_2F point)
    {
        rotation_ = rotation;
        point_ = point;
    }

    void Sprite::setPosition(int left, int top, int right, int bottom)
    {
        left_ = left;
        right_ = right;
        top_ = top;
        bottom_ = bottom;
    }

    void Sprite::draw()
    {
        if (animation_ == nullptr)
            return;
        
        D2D1_RECT_F rect{ left_, top_, right_, bottom_ };
        Graphics::Instance().DrawRotationBitmap(
            animation_->getCurrentFrameFileName(),
            rect, rotation_, point_);
    }

    void Sprite::runAction(Animation * animation)
    {
        if (animation_ != nullptr)
        {
            animation_->end();
        }

        animation_ = animation;

        if (animation != nullptr)
            animation->begin();
    }

}
