/*
 * Standard display driver - uses SDL
 */

#include <vb/Image.h>

namespace vb {
  void Image::events () {
    while (( (paused==0)&&SDL_PollEvent(&event) ) ||
	   ( (paused==1)&&SDL_WaitEvent(&event) )) {
      switch (event.type) {
      case SDL_QUIT:
	exit (1);
	break;
      case SDL_KEYDOWN:
	switch (event.key.keysym.sym) {
	case SDLK_ESCAPE:
	case SDLK_q:
	  std::cout << (*this);
	  exit (0);
	  break;
        case SDLK_x:
          exit (1);
          break;
	case SDLK_SPACE:
	  paused = 1-paused;
	  break;
	default:
	  break;
	}
	break;
      }
    }
  }
  
  int Image::onscreen () {
    SDL_Init(SDL_INIT_VIDEO);
    
    screen=SDL_SetVideoMode(width,height,8,SDL_SWSURFACE);
    if (!screen) {
      std::cerr << "libvb : error : "
		<< "Couldn't map it ! Continuing without.\n";
      return 0;
    }
    
    SDL_WM_SetCaption (title.c_str(),"Simulation");
    
    SDL_Color sdl_palette[256];
    
    int pstep = 255 / ((1<<depth)-1);
    for (int i=0;i<(1<<depth);i++) {
      sdl_palette[i].r = i*pstep;
      sdl_palette[i].g = i*pstep;
      sdl_palette[i].b = i*pstep;
    }    

    SDL_SetColors (screen,sdl_palette,0,1<<depth);

    is_onscreen = 1;
    
    char *bufp = (char*) screen->pixels;
    for (int i=0;i<width;i++)
      for (int j=0;j<height;j++)
	bufp[i+screen->pitch*j] = pic[i+pitch*j];

    delete[] pic;
    pic = bufp;
    pic_is_original = 0;
    pitch = screen->pitch;

    update();  
    
    return 1;
  }
  
  void Image::update () {
    if (is_onscreen) {
      SDL_UpdateRect(screen,0,0,0,0);
      if ( (snapshot_period>0) && (time(0)>=snapshot_next) ) {
        char buffer[100];
        sprintf (buffer,"snapshot%06d.bmp",snapshot_number++);
        SDL_SaveBMP (screen,buffer);
        snapshot_next = time(0) + snapshot_period;
      }
      events();
    }
  }  
}
