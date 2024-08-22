/****************************************************************************
  Copyright (c) 2005 Vasco Alexandre da Silva Costa
  Portions copyright (c) 2005 Jens Granseuer

  Please read the license terms contained in the LICENSE and
  publiclicensecontract.doc files which should be contained with this
  distribution.
 ****************************************************************************/

#include "base/base.h"
#include "inphdl/inphdl.h"
#include "gfx/scaling.h"

#include "SDL.h"

const int JOY_AXIS_DEAD_ZONE = 8000;

struct IHandler {
    S32 ul_XSensitivity;
    S32 ul_YSensitivity;
    U32 ul_WaitTicks;

    /* us_KeyCode; */

    bool MouseExists;
    bool EscStatus;
    bool FunctionKeyStatus;
    bool MouseStatus;

    bool JoyExists;
    SDL_Joystick *Joysticks[256];
};

struct IHandler IHandler;

void gfxWaitTOF(void);

static void inpDoPseudoMultiTasking(void)
{
    animator();
}

static void addJoystick(Sint32 which)
{
    IHandler.Joysticks[which] = SDL_JoystickOpen(which);

    if (!IHandler.Joysticks[which]) {
        DebugMsg(ERR_DEBUG, ERROR_MODULE_INPUT,
                 "Failed to open Joystick %d", which);
    }
}

static void removeJoystick(Sint32 which)
{
    if (IHandler.Joysticks[which]) {
        SDL_JoystickClose(IHandler.Joysticks[which]);
    }
}

void inpOpenAllInputDevs(void)
{
    Sint32 joyNum;

    inpSetKeyRepeat((1 << 5) | 10);

    IHandler.EscStatus = true;
    IHandler.FunctionKeyStatus = true;

    IHandler.MouseExists = true;
    IHandler.MouseStatus = true;

    if (SDL_InitSubSystem(SDL_INIT_JOYSTICK) == 0) {
        for (joyNum = 0; joyNum < SDL_NumJoysticks(); joyNum++) {
           addJoystick(joyNum);
        }
    } else {
        DebugMsg(ERR_DEBUG, ERROR_MODULE_INPUT,
                 "SDL_InitSubSystem: %s", SDL_GetError());
    }
    inpClearKbBuffer();
}

void inpCloseAllInputDevs(void)
{
    Sint32 joyNum;
    for (joyNum = 0; joyNum < SDL_NumJoysticks(); joyNum++) {
        removeJoystick(joyNum);
    }

    if (SDL_WasInit(SDL_INIT_JOYSTICK) != 0) {
        SDL_QuitSubSystem(SDL_INIT_JOYSTICK);
    }
}

void inpMousePtrOn(void)
{
/*      SDL_ShowCursor(SDL_ENABLE); */
}

void inpMousePtrOff(void)
{
/*      SDL_ShowCursor(SDL_DISABLE); */
}

S32 inpWaitFor(S32 l_Mask)
{
    SDL_Event ev;
    S32 action;
    U32 WaitTime = 0;

    if ((IHandler.EscStatus) && (!(l_Mask & INP_NO_ESC))) {
        l_Mask |= INP_ESC;
    }

    if ((IHandler.FunctionKeyStatus) && (!(l_Mask & INP_FUNCTION_KEY))) {
        l_Mask |= INP_FUNCTION_KEY;
    }

    action = 0;

    /* Nun wird auf den Event gewartet... */
    WaitTime = 0;

    while (action == 0) {
        gfxWaitTOF();

        WaitTime++;
        /* Abfrage des Zeit-Flags */
        if ((l_Mask & INP_TIME) && WaitTime >= IHandler.ul_WaitTicks) {
            action |= INP_TIME;
        }

        while (SDL_PollEvent(&ev)) {
            switch (ev.type) {
                case SDL_WINDOWEVENT:
                    switch (ev.window.event) {
                        case SDL_WINDOWEVENT_SIZE_CHANGED:
                        case SDL_WINDOWEVENT_EXPOSED:
                            if (setup.WindowKeepAspect)
                                gfxResizeToFit();
                            gfxRefreshArea(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
                            break;
                        default:
                            break;
                    }
                    break;
                case SDL_QUIT:
                    tcQuit();
                    break;
                case SDL_KEYDOWN:
                    switch (ev.key.keysym.sym) {
                        case SDLK_f:
                            if (setup.FullScreen) {
                                SDL_SetWindowFullscreen(getMainSDLWindow(), 0);
                                setup.FullScreen = false;
                            } else {
                                SDL_SetWindowFullscreen
                                    (getMainSDLWindow(), GFX_SDL_FULLSCREEN);
                                setup.FullScreen = true;
                            }
                            break;
                        case SDLK_k:
                            setup.WindowKeepAspect = !setup.WindowKeepAspect;
                            if (setup.WindowKeepAspect)
                                gfxResizeToFit();
                            break;
                        case SDLK_LEFT:
                            if ((l_Mask & INP_LEFT))
                                action |= INP_KEYBOARD + INP_LEFT;
                            break;

                        case SDLK_RIGHT:
                            if ((l_Mask & INP_RIGHT))
                                action |= INP_KEYBOARD + INP_RIGHT;
                            break;

                        case SDLK_UP:
                            if ((l_Mask & INP_UP))
                                action |= INP_KEYBOARD + INP_UP;
                            break;

                        case SDLK_DOWN:
                            if ((l_Mask & INP_DOWN))
                                action |= INP_KEYBOARD + INP_DOWN;
                            break;

                        case SDLK_SPACE:
                        case SDLK_RETURN:
                        case SDLK_KP_ENTER:
                            if ((l_Mask & (INP_LBUTTONP | INP_LBUTTONR)))
                                action |= INP_KEYBOARD + INP_LBUTTONP;
                            break;

                        case SDLK_ESCAPE:
                            if (IHandler.EscStatus)
                                action |= INP_KEYBOARD + INP_ESC;
                            break;

                        case SDLK_F1:
                        case SDLK_F2:
                        case SDLK_F3:
                        case SDLK_F4:
                        case SDLK_F5:
                        case SDLK_F6:
                        case SDLK_F7:
                        case SDLK_F8:
                        case SDLK_F9:
                        case SDLK_F10:
                        case SDLK_F11:
                            if (IHandler.FunctionKeyStatus)
                                action |= INP_KEYBOARD + INP_FUNCTION_KEY;
                            break;

                        case SDLK_F12:
                            gfxScreenshot();
                            break;

                        default:
                            break;
                    }
                    break;

                case SDL_MOUSEMOTION:
                    if (IHandler.MouseExists && IHandler.MouseStatus) {
                        if ((l_Mask & INP_LEFT) && (ev.motion.xrel < 0))
                            action |= INP_MOUSE + INP_LEFT;
                        if ((l_Mask & INP_RIGHT) && (ev.motion.xrel > 0))
                            action |= INP_MOUSE + INP_RIGHT;
                        if ((l_Mask & INP_UP) && (ev.motion.yrel < 0))
                            action |= INP_MOUSE + INP_UP;
                        if ((l_Mask & INP_DOWN) && (ev.motion.yrel > 0))
                            action |= INP_MOUSE + INP_DOWN;
                    }
                    break;

                case SDL_MOUSEBUTTONDOWN:
                    if (IHandler.MouseExists && IHandler.MouseStatus) {
                        if ((l_Mask & INP_LBUTTONP)
                            && ev.button.button == SDL_BUTTON_LEFT) {
                            action |= INP_MOUSE + INP_LBUTTONP;
                        }
                        if ((l_Mask & INP_RBUTTONP)
                            && ev.button.button == SDL_BUTTON_RIGHT) {
                            action |= INP_MOUSE + INP_RBUTTONP;
                        }
                    }
                    break;
                case SDL_JOYAXISMOTION:
                    if (ev.jaxis.axis == 0) {        // X axis
                        if ((l_Mask & INP_LEFT) && (ev.jaxis.value < -JOY_AXIS_DEAD_ZONE)) {
                            action |= INP_KEYBOARD | INP_LEFT;
                        } else if ((l_Mask & INP_RIGHT) && (ev.jaxis.value > JOY_AXIS_DEAD_ZONE)) {
                            action |= INP_KEYBOARD | INP_RIGHT;
                        }
                    } else if (ev.jaxis.axis == 1) { // Y axis
                        if ((l_Mask & INP_UP) && (ev.jaxis.value < -JOY_AXIS_DEAD_ZONE)) {
                            action |= INP_KEYBOARD | INP_UP;
                        } else if ((l_Mask & INP_DOWN) && (ev.jaxis.value > JOY_AXIS_DEAD_ZONE)) {
                            action |= INP_KEYBOARD | INP_DOWN;
                        }
                    }
                    break;
                case SDL_JOYBUTTONUP:
                    if ((l_Mask & (INP_LBUTTONP | INP_LBUTTONR)) && ev.jbutton.button <= SDL_CONTROLLER_BUTTON_Y) {
                        action |= INP_KEYBOARD | INP_LBUTTONP;
                    }
                    switch (ev.jbutton.button) {
                        case SDL_CONTROLLER_BUTTON_GUIDE:
                        case SDL_CONTROLLER_BUTTON_START:
                            if (IHandler.FunctionKeyStatus)
                                action |= INP_KEYBOARD + INP_FUNCTION_KEY;
                            break;
                        default:
                            break;
                    }
                    break;
                case SDL_JOYHATMOTION:
                    if ((l_Mask & INP_LEFT) && (ev.jhat.value == SDL_HAT_LEFT)) {
                        action |= INP_KEYBOARD | INP_LEFT;
                    } else if ((l_Mask & INP_RIGHT) && (ev.jhat.value == SDL_HAT_RIGHT)) {
                        action |= INP_KEYBOARD | INP_RIGHT;
                    } else if ((l_Mask & INP_UP) && (ev.jhat.value == SDL_HAT_UP)) {
                        action |= INP_KEYBOARD | INP_UP;
                    } else if ((l_Mask & INP_DOWN) && (ev.jhat.value == SDL_HAT_DOWN)) {
                        action |= INP_KEYBOARD | INP_DOWN;
                    }
                    break;
                case SDL_JOYDEVICEADDED:
                    addJoystick(ev.jdevice.which);
                    break;
                case SDL_JOYDEVICEREMOVED:
                    removeJoystick(ev.jdevice.which);
                    break;
                default:
                    break;
            }
        }

        inpClearKbBuffer();
        inpDoPseudoMultiTasking();
    }

    return action;
}

void inpSetWaitTicks(U32 l_Ticks)
{
    IHandler.ul_WaitTicks = l_Ticks;
}

void inpTurnESC(bool us_NewStatus)
{
    IHandler.EscStatus = us_NewStatus;
}

void inpTurnFunctionKey(bool us_NewStatus)
{
    IHandler.FunctionKeyStatus = us_NewStatus;
}

void inpTurnMouse(bool us_NewStatus)
{
    IHandler.MouseStatus = us_NewStatus;
}

void inpDelay(S32 l_Ticks)
{
    S32 i;

    for (i = 0; i < l_Ticks; i++) {
        gfxWaitTOF();
        inpDoPseudoMultiTasking();
    }
}

void inpSetKeyRepeat(unsigned char rate)
{
    int delay, interval;
    SDL_Event ev;

    delay = (rate >> 5);
    interval = (rate & 0x1f);

    if (delay == 0)
        delay = 1000;
    else
        delay = 750;

    if (interval == 0)
        interval = 30;
    else
        interval = 60;

/*    SDL_EnableKeyRepeat(delay, interval);*/

    /* flush event queue */
    while (SDL_PollEvent(&ev)) {
        /* do nothing. */
    }
}

void inpClearKbBuffer(void)
{
}
