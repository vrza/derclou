/*
 * UI scaling functions for "Der Clou!"
 *
 * Copyright (c) 2024 Vladimir Vrzić
 * 
 * Please read the license terms contained in the LICENSE and
 * publiclicensecontract.txt files which should be contained with this
 * distribution.
 */

#ifndef MODULE_GFX_SCALING
#define MODULE_GFX_SCALING

double scaleFromWindowSize(int w, int h);
CoordinatesU16 getBoxedMouseCoordinates(int w, int h, double scale);
CoordinatesDouble getScaledBoxedMouseCoordinates(void);

#endif
