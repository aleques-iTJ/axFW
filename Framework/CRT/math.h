#pragma once
#include "API.inl"


// 
#define M_PI	3.14159265358979323846264338327950288419716939937510582097494459
#define M_PI_2	1.57079632679489661923132169163975144209858469968755291048747229
#define M_PI_4	0.78539816339744830961566084581987572104929234984377645524373614

// Trig
// http://www.gamedev.net/topic/681723-faster-sin-and-cos/


AX_API double sin(double radians);
AX_API double cos(double radians);
AX_API double tan(double radians);

AX_API double cot(double radians);