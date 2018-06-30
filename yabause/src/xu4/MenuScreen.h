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
