/*  Copyright 2018 devMiyax(smiyaxdev@gmail.com)

This file is part of YabaSanshiro.

Yabause is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

Yabause is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with YabaSanshiro; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA
*/

#include <SDL2/SDL.h>
#define GL_GLEXT_PROTOTYPES 1
#include <SDL2/SDL_opengles2.h>
#undef Success 
#include "nanogui/screen.h"
#include "nanovg.h"
#include <string.h>

using namespace nanogui;


class MenuScreen : public nanogui::Screen
{
public:
    Widget *tools;
    Button *bAnalog;

    int imageid_ = 0;
    int imgw_ = 0;
    int imgh_ = 0;
    NVGpaint imgPaint_ = {};     
    void setBackGroundImage( const std::string & fname );

    nanogui::Window *window;
    
    
    MenuScreen( SDL_Window* pwindow, int rwidth, int rheight );
    virtual bool keyboardEvent( std::string & keycode , int scancode, int action, int modifiers);
    virtual void draw(NVGcontext *ctx);

	uint32_t reset_ = 0;
	void setResetMenuEventCode( uint32_t type ){ reset_ = type; }

	uint32_t pad_ = 0;
	void setTogglePadModeMenuEventCode( uint32_t type ){ pad_ = type; }
};
