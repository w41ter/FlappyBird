#pragma once

namespace games
{
    class Collision
    {
    public:
        static bool PointPointHitTest(const D2D1_POINT_2F &p1, const D2D1_POINT_2F &p2, float radius);
        static bool PointRectHitTest(const D2D1_POINT_2F &p, const D2D1_RECT_F &r);
        static bool PointLineHitTest(const D2D1_POINT_2F &p1, const D2D1_POINT_2F &p2, const D2D1_POINT_2F p);
        static bool CircleLineHitTest(const D2D1_ELLIPSE &e, const D2D1_POINT_2F &p1, const D2D1_POINT_2F &p2);
        static bool OBBCircleHitTest(const D2D1_RECT_F &r1, const D2D1_ELLIPSE &e1);
    };

}