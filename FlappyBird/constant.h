#pragma once


// 度量参数
#define GAMES_SIZE_W 800
#define GAMES_SIZE_H 600

#define MS_GROUNDWIDTH      336
#define MS_GROUNDHEIGHT     112
#define MS_GROUNDDRAWCOUNT  (GAMES_SIZE_W / MS_GROUNDWIDTH + 1)
#define MS_PIPEWIDTH        52
#define MS_PIPEHEIGHT       420
#define MS_BIRDIMGSIZE_W    40
#define MS_BIRDIMGSIZE_H    40

#define MS_STAGEMOVESPEED 150   // 移动速度
#define MS_PIPECREATETIME 1.7   // 管子创建时间间隔
#define MS_PIPEGAPHEIGHT  100   // 管子间隙高度
#define MS_PIPECREATEYMINRESERVED  10.f
#define MS_PIPECREATEYMAXRESERVED  50.f
#define MS_PIPECREATEYMIN (GAMES_SIZE_H - MS_PIPEHEIGHT - (MS_PIPEGAPHEIGHT / 2.f) + MS_PIPECREATEYMINRESERVED) // 管子间隙中心最小Y值
#define MS_PIPECREATEYMAX (MS_PIPEHEIGHT + (MS_PIPEGAPHEIGHT / 2.f) - MS_PIPECREATEYMAXRESERVED)              // 管子间隙中心最大Y值

#define MS_BIRDCENTERX  200.f            // 鸟的水平位置
#define MS_BIRDBOUNDINGCIRCLESIZE 10.f   // 鸟的包围圆半径
#define MS_GRAVITY  980.f                // 重力加速度
#define MS_BIRDJUMPV 260.f               // 按下按键后鸟的速度
#define MS_BIRDMAXV  600.f               // 鸟的最大速度
#define MS_BIRDDROPV 400.f               // 旋转完成速度
#define MS_BIRD_BEGIN_DROP 250.F         // 开始旋转速度

#define MS_BIRDMAXROTATION (3.1415926f / 8.f)  // 朝上的最大角度
#define MS_BIRDMINROTATION (3.1415926f / 2.f)   // 朝下的最大角度
#define MS_BIRDROTATIONWEIGHT   1.25f        // 旋转的插值权重

#define MS_PATHLOGTIME          0.1f         // 路径记录时间间隔
#define MS_PATHFADEOUTTIME      2.5f         // 路径点消失时间
#define MS_PATHSTARTFADEOUTTIME 1.5f         // 路径点开始消失时间
#define MS_PATHCOLOR            0xFFE0E0E0   // 路径点颜色
#define MS_PATHSIZE             3.f          // 路径点大小