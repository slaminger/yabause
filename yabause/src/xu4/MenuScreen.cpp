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
#include "MenuScreen.h"
#include <nanogui/window.h>
#include <nanogui/layout.h>
#include <nanogui/label.h>
#include <nanogui/button.h>


MenuScreen::MenuScreen( SDL_Window* pwindow, int rwidth, int rheight )
: nanogui::Screen( pwindow, Eigen::Vector2i(rwidth, rheight), "Menu Screen"){
  
        window = new Window(this, "Yaba Sanshiro Menu");
        window->setPosition(Vector2i(0, 0));
        window->setLayout(new GroupLayout());

        tools = new Widget(window);
        tools->setLayout(new BoxLayout(Orientation::Vertical,Alignment::Middle, 0, 5));
        tools->setFixedWidth(256);

        Button *b0 = new Button(tools, "Exit");
        b0->setFixedWidth(248);
        b0->setCallback([this]() { 
          printf("Exit\n"); 
      		SDL_Event* quit = new SDL_Event();
			    quit->type = SDL_QUIT;
			    SDL_PushEvent(quit);  
        });

        Button *b1 = new Button(tools, "Reset");
        b1->setFixedWidth(248);
        b1->setCallback([this]() { 
          //printf("Reset\n");  
          SDL_Event event = {};
          event.type = reset_;
          event.user.code = 0;
          event.user.data1 = 0;
          event.user.data2 = 0;
          SDL_PushEvent(&event);          
        });

        //Button *b4 = new Button(tools, "Open CD Tray");
        //b4->setFixedWidth(120);
        //b4->setCallback([this]() { printf("test4");  });

        bAnalog = new Button(tools, "Switch Pad Mode");
        bAnalog->setFixedWidth(248);
        bAnalog->setCallback([this]() { 
          SDL_Event event = {};
          event.type = pad_;
          event.user.code = 0;
          event.user.data1 = 0;
          event.user.data2 = 0;
          SDL_PushEvent(&event);            
        });

        b0->focusEvent(true);
        b0->mouseEnterEvent(b0->absolutePosition(),true);
        mFocus = b0;

        performLayout();
        

}

bool MenuScreen::keyboardEvent( std::string & keycode , int scancode, int action, int modifiers){

  printf("%s %d %d\n",keycode.c_str(),scancode,action);
  if (action == 1) {
          
    if (keycode == "down") {
      nanogui::Button* btn = (nanogui::Button*)mFocus;
      btn->setPushed(false);
      mFocus->mouseEnterEvent(mFocus->position(), false);
      mFocus = tools->getNearestWidget(mFocus, 1);
      printf("%s is selected\n",((nanogui::Button*)mFocus)->caption().c_str());
      mFocus->mouseEnterEvent(mFocus->position(), true);
    }else if (keycode == "up") {
      nanogui::Button* btn = (nanogui::Button*)mFocus;
      btn->setPushed(false);
      mFocus->mouseEnterEvent(mFocus->position(), false);
      mFocus = tools->getNearestWidget(mFocus, 0);
      mFocus->mouseEnterEvent(mFocus->position(), true);
    }
    if (keycode == "a") {
            mFocus->mouseButtonEvent(mFocus->position(), SDL_BUTTON_LEFT, true, 0);
    }
  }
 
  if (action == 0 && keycode == "a" ) {
    mFocus->mouseButtonEvent(mFocus->position(), SDL_BUTTON_LEFT, false, 0);
  }    

  return false;
}

void MenuScreen::setBackGroundImage( const std::string & fname ){
  if(imageid_!=0){
    nvgDeleteImage(mNVGContext,imageid_);
  }
  imageid_ = nvgCreateImage(mNVGContext, fname.c_str(), 0 );
  nvgImageSize(mNVGContext, imageid_, &imgw_, &imgh_);
  //printf("imageid_:%d w:%d h:%d\n",imageid_,imgw_,imgh_);
	imgPaint_ = nvgImagePattern(mNVGContext, 0, 0, imgw_,imgh_, 0, imageid_, 0.5f);
}

void MenuScreen::draw(NVGcontext *ctx){

  if( imageid_ != 0 ){
    nvgSave(ctx);
		nvgBeginPath(ctx);
		nvgRect(ctx, 0, 0, imgw_,imgh_);
		nvgFillPaint(ctx, imgPaint_);
    //nvgFillColor(ctx, nvgRGBA(0,160,192,255));
		nvgFill(ctx);
    nvgRestore(ctx);
  }

  window->center();
  Screen::draw(ctx);
}