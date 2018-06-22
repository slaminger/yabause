#include <exception>
#include <functional>
#include <string>
#include <vector>

#include <SDL2/SDL.h>
#define GL_GLEXT_PROTOTYPES 1
#include <SDL2/SDL_opengles2.h>

extern "C" {
#include "../config.h"
#include "yabause.h"
#include "vdp2.h"
#include "scsp.h"
#include "vidsoft.h"
#include "vidogl.h"
#include "peripheral.h"
#include "m68kcore.h"
#include "sh2core.h"
#include "sh2int.h"
#include "cdbase.h"
#include "cs2.h"
#include "debug.h"
#include "sndal.h"
#include "sndsdl.h"
#include "osdcore.h"
#include "ygl.h"

static char biospath[256] = "/home/pigaming/RetroPie/BIOS/saturn/bios.bin";
static char cdpath[256] = "/home/pigaming/RetroPie/roms/saturn/nights.cue";
static char buppath[256] = "./back.bin";
static char mpegpath[256] = "\0";
static char cartpath[256] = "\0";

M68K_struct * M68KCoreList[] = {
  &M68KDummy,
  #ifdef HAVE_C68K
  &M68KC68K,
  #endif
  #ifdef HAVE_Q68
  &M68KQ68,
  #endif
#ifdef HAVE_MUSASHI
  &M68KMusashi,
#endif
  NULL
};

SH2Interface_struct *SH2CoreList[] = {
  &SH2Interpreter,
  &SH2DebugInterpreter,
#ifdef SH2_DYNAREC
  &SH2Dynarec,
#endif
#if DYNAREC_DEVMIYAX
  &SH2Dyn,
  &SH2DynDebug,
#endif
  NULL
};

PerInterface_struct *PERCoreList[] = {
  &PERDummy,
  NULL
};

CDInterface *CDCoreList[] = {
  &DummyCD,
  &ISOCD,
  NULL
};

SoundInterface_struct *SNDCoreList[] = {
  &SNDDummy,
#ifdef HAVE_LIBSDL
  &SNDSDL,
#endif
  NULL
};

VideoInterface_struct *VIDCoreList[] = {
  &VIDDummy,
  &VIDOGL,
  NULL
};

#ifdef YAB_PORT_OSD
#include "nanovg/nanovg_osdcore.h"
OSD_struct *OSDCoreList[] = {
  &OSDNnovg,
  NULL
};
#endif

static SDL_Window* wnd;
static SDL_GLContext glc;

void DrawDebugInfo()
{
}

int g_EnagleFPS = 0;

void YuiErrorMsg(const char *string)
{
  printf("%s",string);
}

void YuiSwapBuffers(void)
{
  SDL_GL_SwapWindow(wnd);
  SetOSDToggle(1);
}

int YuiRevokeOGLOnThisThread(){
  printf("revoke thread\n");
    SDL_GL_MakeCurrent(wnd,nullptr);
    return 0;
}

int YuiUseOGLOnThisThread(){
  printf("use thread\n");
    SDL_GL_MakeCurrent(wnd,glc);
    return 0;
}

}

int g_resolution_mode = 0;
int g_keep_aspect_rate = 0;
int g_scsp_sync = 1;
int g_frame_skip = 0;
int g_emulated_bios = 1;

int yabauseinit()
{
  int res;
  yabauseinit_struct yinit;
  void * padbits;

  yinit.m68kcoretype = M68KCORE_MUSASHI;
  yinit.percoretype = PERCORE_DUMMY;
#ifdef SH2_DYNAREC
    yinit.sh2coretype = 2;
#else
  //yinit.sh2coretype = 0;
#endif
  yinit.sh2coretype = 3;
  //yinit.vidcoretype = VIDCORE_SOFT;
  yinit.vidcoretype = 1;
  yinit.sndcoretype = SNDCORE_SDL;
  //yinit.sndcoretype = SNDCORE_DUMMY;
  //yinit.cdcoretype = CDCORE_DEFAULT;
  yinit.cdcoretype = CDCORE_ISO;
  yinit.carttype = CART_NONE;
  yinit.regionid = 0;
  if( g_emulated_bios ){
    yinit.biospath = NULL;
  }else{
    yinit.biospath = biospath;
  }
  yinit.cdpath = cdpath;
  yinit.buppath = buppath;
  yinit.mpegpath = mpegpath;
  yinit.cartpath = cartpath;
  yinit.videoformattype = VIDEOFORMATTYPE_NTSC;
  yinit.frameskip = g_frame_skip;
  yinit.usethreads = 0;
  yinit.skip_load = 0;    
  yinit.video_filter_type = 0;
  yinit.polygon_generation_mode = PERSPECTIVE_CORRECTION; ////GPU_TESSERATION;
  yinit.use_new_scsp = 1;
  yinit.resolution_mode = g_resolution_mode;
  yinit.rotate_screen = 0;
  yinit.scsp_sync_count_per_frame = g_scsp_sync;
  yinit.extend_backup = 1;

    res = YabauseInit(&yinit);
    if( res == -1)
    {
        return -1;
    }
  PerPortReset();
  padbits = PerPadAdd(&PORTDATA1);

  PerSetKey(SDLK_UP, PERPAD_UP, padbits);
  PerSetKey(SDLK_RIGHT, PERPAD_RIGHT, padbits);
  PerSetKey(SDLK_DOWN, PERPAD_DOWN, padbits);
  PerSetKey(SDLK_LEFT, PERPAD_LEFT, padbits);
  PerSetKey(SDLK_q, PERPAD_RIGHT_TRIGGER, padbits);
  PerSetKey(SDLK_e, PERPAD_LEFT_TRIGGER, padbits);
  PerSetKey(SDLK_RETURN, PERPAD_START, padbits);
  PerSetKey(SDLK_z, PERPAD_A, padbits);
  PerSetKey(SDLK_x, PERPAD_B, padbits);
  PerSetKey(SDLK_c, PERPAD_C, padbits);
  PerSetKey(SDLK_a, PERPAD_X, padbits);
  PerSetKey(SDLK_s, PERPAD_Y, padbits);
  PerSetKey(SDLK_d, PERPAD_Z, padbits);

  OSDInit(0);
  OSDChangeCore(OSDCORE_NANOVG);
  
  LogStart();
  LogChangeOutput(DEBUG_CALLBACK, NULL);

  return 0;
}

using std::string;

int main(int argc, char** argv)
{

  std::string current_exec_name = argv[0]; // Name of the current exec program
  std::vector<std::string> all_args;
  if (argc > 1) {
    all_args.assign(argv + 1, argv + argc);
    if( all_args[0] == "-h" || all_args[0] == "--h" ){
      printf("Usage:\n");
      printf("  -b STRING  --bios STRING                 bios file\n");
      printf("  -i STRING  --iso STRING                  iso/cue file\n");
      printf("  -r NUMBER  --resolution_mode NUMBER      0 .. Native, 1 .. 4x, 2 .. 2x, 3 .. Original\n");
      printf("  -a         --keep_aspect_rate\n");
      printf("  -s NUMBER  --scps_sync_per_frame NUMBER\n");
      printf("  -f         --frame_skip\n");    
      printf("  -v         --version\n");    
      exit(0);
    }
  }

  for( int i=0; i<all_args.size(); i++ ){
    string x = all_args[i];
		if(( x == "-b" || x == "--bios") && (i+1<all_args.size() ) ) {
      g_emulated_bios = 0;
      strncpy(biospath, all_args[i+1].c_str(), 256);
    }
		else if(( x == "-i" || x == "--iso") && (i+1<all_args.size() ) ) {
      strncpy(cdpath, all_args[i+1].c_str(), 256);
    }
		else if(( x == "-r" || x == "--resolution_mode") && (i+1<all_args.size() ) ) {
      g_resolution_mode = std::stoi( all_args[i+1] );
    }
		else if(( x == "-a" || x == "--keep_aspect_rate") ) {
      g_keep_aspect_rate = 1;
    }
		else if(( x == "-s" || x == "--g_scsp_sync")&& (i+1<all_args.size() ) ) {
      g_scsp_sync = std::stoi( all_args[i+1] );
    }
		else if(( x == "-f" || x == "--frame_skip") ) {
      g_frame_skip = 1;
    }
		else if(( x == "-v" || x == "--version") ) {
      printf("YabaSanshiro version %s(%s)\n",YAB_VERSION, GIT_SHA1 );
      exit(0);
    }
	}
  SDL_DisplayMode dsp;
  int width = 800;
  int height = 600;
  wnd = SDL_CreateWindow("test", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
      width, height, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_FULLSCREEN_DESKTOP);
  SDL_GetWindowDisplayMode(wnd,&dsp);
  dsp.refresh_rate = 60;
  SDL_SetWindowDisplayMode(wnd,&dsp);
  SDL_GetWindowSize(wnd,&width,&height);
  SDL_SetWindowInputFocus(wnd);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
  SDL_GL_SetSwapInterval(0);
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
  glc = SDL_GL_CreateContext(wnd);
  printf("context renderer string: \"%s\"\n", glGetString(GL_RENDERER));
  printf("context vendor string: \"%s\"\n", glGetString(GL_VENDOR));
  printf("version string: \"%s\"\n", glGetString(GL_VERSION));
  printf("Extentions: %s\n",glGetString(GL_EXTENSIONS));

  if( yabauseinit() == -1 ) {
      return -1;
  }

  if( g_keep_aspect_rate ){
    int originx = 0;
    int originy = 0;
    int specw = width;
    int spech = height;
    float specratio = (float)specw / (float)spech;
    int saturnw = 4;
    int saturnh = 3;
    float saturnraito = (float)saturnw/ (float)saturnh;
    float revraito = (float) saturnh/ (float)saturnw;
    if( specratio > saturnraito ){
            width = spech * saturnraito;
            height = spech;
            originx = (dsp.w - width)/2.0;
            originy = 0;
    }else{
        width = specw ;
        height = specw * revraito;
        originx = 0;
        originy = spech - height;
    }
    VIDCore->Resize(originx,originy,width,height,0);
  }else{
    VIDCore->Resize(0,0,width,height,0);
  }
  SDL_GL_MakeCurrent(wnd,nullptr);
  glClearColor(0.0,0.0,0.0,1.0);
  glClear(GL_COLOR_BUFFER_BIT);
  YabThreadSetCurrentThreadAffinityMask(0x00);
  while(true) {
    SDL_Event e;
    while(SDL_PollEvent(&e)) {
      if(e.type == SDL_QUIT) std::terminate();
      switch( e.type ){
        /* Keyboard event */
        /* Pass the event data onto PrintKeyInfo() */
      case SDL_KEYDOWN:
        PerKeyDown( e.key.keysym.sym );
        break;
      case SDL_KEYUP:
        PerKeyUp( e.key.keysym.sym );
        break;
      default:
        break;
      }
    }
    YabauseExec(); // exec one frame
  }
  YabauseDeInit();
  SDL_Quit();
  return 0;
}
