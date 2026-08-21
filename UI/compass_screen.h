#ifndef COMPASS_SCREEN_H_
#define COMPASS_SCREEN_H_

#include <stdio.h>
#include <string.h>
#include <math.h>

#include "qmc5883l.h"
#include "st7789.h"




void UI_CompassScreen_DrawBackground(void);  
void UI_CompassScreen_DrawNeedle(float angle, uint16_t color);

#endif /* COMPASS_SCREEN_H_ */