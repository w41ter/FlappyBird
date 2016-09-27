#pragma once

#include "UIBase.h"
#include "Graphics.h"

namespace games
{
    class UIControl
    {
    public:
        static bool Button(
            UIState &state, 
            UIID id, 
            FontHandle font, 
            const wchar_t *text, 
            const D2D1_RECT_F &rect
            );
        static bool Button(
            UIState &state, 
            UIID id, 
            FontHandle font, 
            const wchar_t *text, 
            float left, 
            float top, 
            float right, 
            float bottom
            );

        static bool TextButton(
            UIState &state, 
            UIID id, 
            FontHandle font, 
            const wchar_t *text, 
            const D2D1_RECT_F &rect,
            int32_t rgb = 0xdfdfdf
            );
        static bool TextButton(
            UIState &state, 
            UIID id, 
            FontHandle font, 
            const wchar_t *text, 
            float left, 
            float top, 
            float right, 
            float bottom,
            int32_t rgb = 0xdfdfdf
            );
    };

}