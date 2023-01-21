#ifndef __SSD1306_TEST_H__
#define __SSD1306_TEST_H__

#include <_ansi.h>

#include "ssd1306_conf.h"

_BEGIN_STD_C

void ssd1306_TestBorder(void);
void ssd1306_TestFonts(void);
void ssd1306_TestAll(void);
void ssd1306_TestLine(void);
void ssd1306_TestRectangle(void);
void ssd1306_TestCircle(void);
void ssd1306_TestArc(void);
void ssd1306_TestPolyline(void);
void ssd1306_TestDrawBitmap(void);

#if defined(SSD1306_INCLUDE_TIMED_TESTS)
void ssd1306_TestFPS(void);
void ssd1306_SysTick_Handler();
#endif

_END_STD_C

#endif  // __SSD1306_TEST_H__
