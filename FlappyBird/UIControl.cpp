#include "main.h"
#include "UIControl.h"

#include "Graphics.h"
#include "UIBase.h"

namespace games
{
    bool UIControl::Button(
        UIState &state, 
        UIID id, 
        FontHandle font, 
        const wchar_t *text, 
        const D2D1_RECT_F &rect
        )
    {
        D2D1_RECT_F rc = rect;
        // Check whether the button should be hot
        if (RegionHit(state, rc))
        {
            state.hotitem = id;
            if (state.activeitem == 0 && state.mousedown)
                state.activeitem = id;
        }

        // Render button 
        auto &gdi = Graphics::Instance();

        // draw background
        D2D1_ROUNDED_RECT buttonrc = D2D1::RoundedRect(
            D2D1::RectF(rc.left + 2.f, rc.top + 2.f, rc.right + 2.f, rc.bottom + 2.f),
            3.0f,
            3.0f
            );
        gdi.DrawRoundedRectangle(buttonrc, 0x111111);
        UINT32 rgb;
        D2D1_ROUNDED_RECT frontrc;
        if (state.hotitem == id)
        {
            if (state.activeitem == id)
            {
                // Button is both 'hot' and 'active'
                frontrc = D2D1::RoundedRect(
                    D2D1::RectF(rc.left + 1.f, rc.top + 1.f, rc.right - 0.f, rc.bottom - 0.f),
                    3.0f,
                    3.0f
                    );
                rgb = 0xdadada;
            }
            else
            {
                // Button is merely 'hot'
                frontrc = D2D1::RoundedRect(
                    D2D1::RectF(rc.left, rc.top, rc.right, rc.bottom),
                    3.0f,
                    3.0f
                    );
                rgb = 0xefefef;
            }
        }
        else
        {
            // button is not hot, but it may be active    
            frontrc = D2D1::RoundedRect(
                D2D1::RectF(rc.left, rc.top, rc.right, rc.bottom),
                3.0f,
                3.0f
                );
            rgb = 0xdcdcdc;
        }

        gdi.FillRoundedRectangle(frontrc, rgb);
        //wchar_t *hello = L"Hello world!";
        //buttonrc.rect.left += 10;
        unsigned int len = wcslen(text);
        gdi.DrawText(text, len, font, frontrc.rect, 0x000000);


        // If button is hot and active, but mouse button is not
        // down, the user must have clicked the button.
        if (state.mousedown == 0 &&
            state.hotitem == id &&
            state.activeitem == id)
            return true;

        // Otherwise, no clicky.
        return false;
    }

    bool UIControl::Button(
        UIState &state, 
        UIID id, 
        FontHandle font, 
        const wchar_t *text, 
        float left, 
        float top, 
        float right, 
        float bottom
        )
    {
        return Button(state, id, font, text, D2D1::RectF(left, top, right, bottom));
    }

    bool UIControl::TextButton(
        UIState &state, 
        UIID id, 
        FontHandle font, 
        const wchar_t * text, 
        const D2D1_RECT_F & rect,
        int32_t rgb
        )
    {
        D2D1_RECT_F rc = rect;

        if (RegionHit(state, rc))
        {
            state.hotitem = id;
            if (state.activeitem == 0 && state.mousedown)
                state.activeitem = id;
        }

        // Render button 
        auto &gdi = Graphics::Instance();

        if (state.hotitem == id)
        {
            // Button is merely 'hot'
            D2D1_ROUNDED_RECT frontrc = D2D1::RoundedRect(
                D2D1::RectF(rc.left, rc.top, rc.right, rc.bottom),
                3.0f,
                3.0f
                );
            if (state.activeitem == id) 
            {
                rgb -= 0x101010;
            }
            gdi.FillRoundedRectangle(frontrc, rgb, 0.3f);
        }

        unsigned int len = wcslen(text);
        gdi.DrawText(text, len, font, D2D1::RectF(
            rc.left,
            rc.top,
            rc.right,
            rc.bottom
            ), 0x000000);

        // If button is hot and active, but mouse button is not
        // down, the user must have clicked the button.
        if (state.mousedown == 0 &&
            state.hotitem == id &&
            state.activeitem == id)
            return true;

        // Otherwise, no clicky.
        return false;
    }
    
    bool UIControl::TextButton(
        UIState &state, 
        UIID id, 
        FontHandle font, 
        const wchar_t * text, 
        float left, 
        float top, 
        float right, 
        float bottom,
        int32_t rgb
        )
    {
        return TextButton(state, id, font, text, D2D1::RectF(left, top, right, bottom), rgb);
    }
}