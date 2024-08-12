/*
 * UI scaling functions for "Der Clou!"
 *
 * Copyright (c) 2024 Vladimir Vrzić
 * 
 * Please read the license terms contained in the LICENSE and
 * publiclicensecontract.txt files which should be contained with this
 * distribution.
 */

#include "SDL.h"
#include "gfx/gfx.h"

CoordinatesDouble getScaledBoxedMouseCoordinates()
{
    int w, h;
    double scale;
    CoordinatesU16 boxedMouse;
    CoordinatesDouble scaledBoxedMouse;

    SDL_GetWindowSize(getMainSDLWindow(), &w, &h);
    scale = scaleFromWindowSize(w, h);
    boxedMouse = getBoxedMouseCoordinates(w, h, scale);

    scaledBoxedMouse.x = boxedMouse.x / scale;
    scaledBoxedMouse.y = boxedMouse.y / scale;

    return scaledBoxedMouse;
}

CoordinatesU16 getBoxedMouseCoordinates(int w, int h, double scale)
{
    int mouseX, mouseY = 0;
    double usedW, usedH, marginX, marginY;
    CoordinatesU16 boxedMouse;
    double logicalScreenAspectRatio = (double)SCREEN_WIDTH / (double)SCREEN_HEIGHT;
    double windowAscpectRatio = (double)w /(double)h;

    SDL_PumpEvents();
    SDL_GetMouseState(&mouseX, &mouseY);

    usedH = h;
    usedW = w;
    boxedMouse.x = mouseX;
    boxedMouse.y = mouseY;

    /* adjust for letterboxing */
    if (windowAscpectRatio > logicalScreenAspectRatio) {
        usedW = SCREEN_WIDTH * scale;
        marginX = (((double)w) - usedW) / 2;
        if (mouseX < marginX) {
            boxedMouse.x = 0;
        } else if (mouseX > w - marginX) {
            boxedMouse.x = (int)usedW;
        } else {
            boxedMouse.x = mouseX - marginX;
        }
    }

    if (windowAscpectRatio < logicalScreenAspectRatio) {
        usedH = SCREEN_HEIGHT * scale;
        marginY = (((double)h) - usedH) / 2;
        if (mouseY < marginY) {
            boxedMouse.y = 0;
        } else if (mouseY > h - marginY) {
            boxedMouse.y = (int)usedH;
        } else {
            boxedMouse.y = mouseY - marginY;
        }
    }

    return boxedMouse;
}

double scaleFromWindowSize(int w, int h)
{
    double logicalScreenAspectRatio = (double)SCREEN_WIDTH / (double)SCREEN_HEIGHT;
    double windowAscpectRatio = (double)w /(double)h;
    return (windowAscpectRatio > logicalScreenAspectRatio)
       ? (double)h / (double)SCREEN_HEIGHT
       : (double)w / (double)SCREEN_WIDTH;
}

void gfxResizeToFit()
{
    int w, h;
    SDL_GetWindowSize(getMainSDLWindow(), &w, &h);
    double windowAscpectRatio = (double)w /(double)h;
    double logicalScreenAspectRatio = (double)SCREEN_WIDTH / (double)SCREEN_HEIGHT;
    int newWidth = w;
    int newHeight = h;
    double scale = scaleFromWindowSize(w, h);
    if (windowAscpectRatio > logicalScreenAspectRatio) {
        newWidth = (int)round((double)SCREEN_WIDTH * scale);
    } else {
        newHeight = (int)round((double)SCREEN_HEIGHT * scale);
    }
    SDL_SetWindowSize(getMainSDLWindow(), newWidth, newHeight);
}
