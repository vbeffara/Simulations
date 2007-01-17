/*
 * X11 display driver
 */

#include <vb/Image.h>

namespace vb {

int Image::onscreen () {
  if (!(display=XOpenDisplay(NULL))) {
    std::cerr << "Error: cannot connect to X server "
	      << XDisplayName(NULL) << "\n";
    return 0;
  }

  Screen *scr = DefaultScreenOfDisplay(display);
  if ((width > scr->width-100) || (height > scr->height-100)) {
    std::cerr << "Image too big to fit on the screen ...\n";
    return 0;
  }    
  
  screen=DefaultScreen(display);
  black=BlackPixel(display,screen);
  white=WhitePixel(display,screen);

  int pstep = 255 / ((1<<depth)-1);

  for (int i=0;i<(1<<depth);i++) {
    palette[i]= i * pstep * 0x010101;
  }

  window=XCreateSimpleWindow(display,RootWindow(display,screen),
			     0,0,width,height,4,black,black);

  /// @bug (X11 version) Why does it return a BadMatch sometimes ?

  XmbSetWMProperties (display, window, title.c_str(), NULL, NULL, 0,
		      NULL, NULL, NULL);

  /// @todo (X11 version) Make the image non-resizable.

  gc=XCreateGC(display,window,0,NULL);
  XSetForeground(display,gc,black);
  XSetBackground(display,gc,white); 

  XMapWindow(display,window);

  while (XPending(display)) {
    XNextEvent(display,NULL); 
  }

  ximage = XGetImage (display,window, 0,0, width,height, AllPlanes, ZPixmap);

  if (!(pic))
    return 0;

  is_onscreen = 1;
  update ();
  return 1;
}

void Image::update () {
  if (is_onscreen) {
    for (int row=0;row<height;row++)
      for (int col=0;col<width;col++)
	XPutPixel(ximage,col,row,palette[(int)(*this)(col,row)]);

    XPutImage(display,window,gc,ximage,0,0,0,0,width,height); 
  }
}

}
