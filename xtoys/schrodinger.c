 /* Schrodinger equation explorer  */

/* Michael Creutz          */
/* creutz@bnl.gov */

/* to compile:             */
/* cc -O -L/usr/X11R6/lib schrodinger.c -lX11 -lm  */

/* this solves the Schrodinger equation in a potential           */
/* displays amplitude squared and the potential superposed       */
/* the left mouse button perturbs the wave function              */
/* the right mouse button perturbs the potential                 */
/* the damp button evolves system towards the ground state           */

/* version of July 1997, adds hbar button */

# include <X11/Xlib.h>
# include <X11/Xutil.h>
# include <X11/Xatom.h> 
# include <X11/cursorfont.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <malloc.h>
# include <math.h>
# include <select.h>

/* NSITES=number of lattice sites 
   fft (as implemented here) REQUIRES a power of two */
# define NSITES 64
# define NPOINTS 128

double repsi[NSITES],impsi[NSITES];   /* real and imaginary parts of psi   */
double rekpsi[NSITES],imkpsi[NSITES]; /* field in momentum space           */
double potential[NSITES];             /* potential in Schrodinger equation */
double repfactor[NSITES],impfactor[NSITES]; /* for updating potential term */ 
double rekfactor[NSITES],imkfactor[NSITES]; /* for updating kinetic term   */
double cn[NSITES],sn[NSITES];         /* for cosine and sin of angles      */
double pi;                            /* 3.14...                           */
int iamp[NPOINTS+1],ipot[NPOINTS+1];
XPoint xpoints[2*NPOINTS];             /* for drawing waves                 */
struct timeval timeout;               /* timer for speed control           */

/* dimensions for buttons */
# define BUTTONWIDTH 84
# define BUTTONHEIGHT 18

int paused=0;             /* is the system paused?          */
int damped=0;             /* is the system damped?          */
int speed=1; /* updating delay proportional to speed; between 0 and 10   */
int hbar=5; /*  between 1 and 10 */
int delay=0; /* counter for implementing speed control */
struct timeval timeout; /* timer for speed control */

void drawbutton(),openwindow(),makebuttons(),update(),repaint(),
      cleanup(),drawit(),renorm(),oldupdate(),newupdate(),
      makefactor(),fastfourier();

# define line(x1,y1,x2,y2) XDrawLine(display,window,gc,x1,y1,x2,y2)
# define color(i) XSetForeground(display,gc,translate[i])

/* various window stuff */
Display *display;
int screen;
static char *progname;
Window window,quitbutton,pausebutton,vconstbutton,
    dampbutton,speedbutton,hbarbutton,makebutton();
XColor xcolor,colorcell;
Colormap cmap;
GC gc;
int windowwidth,windowheight,playwidth,playheight;
XFontStruct *font=NULL;
int font_height,font_width;
XSizeHints size_hints;
int darkcolor,lightcolor,black,white;
long translate[256];    /* for converting colors   */

int main(argc,argv)
int argc;
char **argv;
{int i,j,k,p;
 double psipeg,w;
 unsigned int width, height;
 XEvent report;
 progname=argv[0];
 openwindow(argc,argv);
 makebuttons();
  /* set up trig stuff */
 pi=4*atan(1.0);
 for (i=0;i<NSITES;i++)
   {cn[i]=cos(2*pi*i/(1.*NSITES));
    sn[i]=sin(2*pi*i/(1.*NSITES));
   }
  /* initial configuration */
 for (i=0;i<NSITES;i++)
  {
   repsi[i]=1+cn[i]+cn[(2*i)%NSITES];
   impsi[i]=0;
   potential[i]=0.5;
  }    
 renorm();
 makefactor();     
  /* loop forever, looking for events */
 while(1)
  {if ((0==XPending(display))&&(0==paused))
    {if (delay) /* don't update yet */
       {delay--;
        /* this use of select() seems a kludge to me; 
           why can't usleep() be more standard? */
        timeout.tv_sec=0;
        timeout.tv_usec=50000; /* .05 sec per delay unit */
        select(0,NULL,NULL,NULL,&timeout); 
       }
     else
      {delay=speed;
       update();
      }
    }
   else
    {XNextEvent(display,&report); 
     switch (report.type)
      {case Expose: /* the window is exposed and needs to be redrawn */
        if ((report.xexpose.window)!=window) break; /* cuts down flashing,
           but you might remove this line if things aren't being redrawn */
        if (report.xexpose.count!=0) break; /* more in queue, wait for them */
        repaint();  
        break;
       case ConfigureNotify: /* the window has been resized */
        width=report.xconfigure.width;
        height=report.xconfigure.height;
        if ((width<size_hints.min_width)||(height<size_hints.min_height))
            {fprintf(stderr,"%s: window too small to proceed.\n",progname);
             cleanup();
            } 
        else if ((width!=windowwidth)||(height!=windowheight))
            {windowwidth=width;
             windowheight=height;
             playwidth=windowwidth;
             playheight=(windowheight-3*BUTTONHEIGHT);
             makebuttons();
            } 
        break; 
       case ButtonPress:  /* which button was pressed? */
        if (report.xbutton.window==quitbutton)
             cleanup();
        else if (report.xbutton.window==pausebutton)
            {paused=1-paused;
             drawbutton(pausebutton,0,0,BUTTONWIDTH,BUTTONHEIGHT,
               "pause",2-4*paused);
            }
        else if (report.xbutton.window==dampbutton)
            {damped=1-damped;
             drawbutton(dampbutton,0,0,BUTTONWIDTH,BUTTONHEIGHT,
               "damp",2-4*damped);
             makefactor();
            } 
        else if (report.xbutton.window==vconstbutton)
            {drawbutton(vconstbutton,0,0,BUTTONWIDTH,BUTTONHEIGHT,
                     "V=const",-2);
             for (i=0;i<NSITES;i++)
              {potential[i]=0.5;
              }
             drawit();
             drawbutton(vconstbutton,0,0,BUTTONWIDTH,BUTTONHEIGHT,
                     "V=const", 2);
	     makefactor();
            } 
        else if (report.xbutton.window==speedbutton)
          {  /* reset speed */
           speed=10-(11*report.xbutton.x)/BUTTONWIDTH;
           if (speed<0) speed=0;
           if (speed>10) speed=10;
           delay=speed;
           drawbutton(speedbutton,0,0,BUTTONWIDTH,BUTTONHEIGHT,"speed",-2);
           drawbutton(speedbutton,1+((10-speed)*(BUTTONWIDTH-2))/11,1,
                      (BUTTONWIDTH-2)/11,BUTTONHEIGHT-2,"",2);
          }
        else if (report.xbutton.window==hbarbutton)
          {  /* reset hbar */
           hbar=1+(10*report.xbutton.x)/BUTTONWIDTH;
           if (hbar<1) hbar=1;
           if (hbar>10) hbar=10;
           makefactor();
           drawbutton(hbarbutton,0,0,BUTTONWIDTH,BUTTONHEIGHT,"hbar",-2);
           drawbutton(hbarbutton,1+((hbar-1)*(BUTTONWIDTH-2))/10,1,
                      (BUTTONWIDTH-2)/11,BUTTONHEIGHT-2,"",2);
          }
        else
         /* button one changes wave function, two updates, and three 
            modifies potential */
         {i=report.xbutton.x;
          j=report.xbutton.y;
          if (j<playheight) /* add in Lorentzian where clicked */
           {p=(i*NSITES)/playwidth; /* site where clicked */
            psipeg=5*(j-playheight)/(1.0*playheight);
            if (report.xbutton.button==Button1)
              {for (k=0;k<NSITES;k++)
                {
                 w=.5*(1+cos(2*pi*(p-k)/(1.*NSITES))); /* peaks at p */
                 w*=w;  /* sharpen the peak */
                 w*=w;
                 w*=w;
                 w*=w;
                 repsi[k]=psipeg*w+repsi[k]*(1-w);
                }
               renorm();
	      }
            else if (report.xbutton.button==Button3)
              {for (k=0;k<NSITES;k++)
                {
                 w=.5*(1+cos(2*pi*(p-k)/(1.*NSITES))); /* peaks at p */
                 w*=w;  /* sharpen the peak */
                 w*=w;
                 w*=w;
                 w*=w;
                 potential[k]=-.2*psipeg*w+potential[k]*(1-w);
                }
               makefactor();
              }            
            else if (report.xbutton.button==Button2) update();
            drawit(); 
	   }
          else update(); /* do a sweep when mouse clicked outside play area*/
         }
        break;
       default:
        break;
      } /* end of switch */
    } /* end of if XPending */
  } /* end of while(1) */
} /* end of main */

void update()
{int i,iteration;
 double temp;
 for (iteration=0;iteration<5;iteration++)
  {  /* update the potential term */
   for (i=0;i<NSITES;i++)
    {
     temp    =impsi[i]*repfactor[i]+repsi[i]*impfactor[i];
     repsi[i]=repsi[i]*repfactor[i]-impsi[i]*impfactor[i];
     impsi[i]=temp;
    }
     /* go to momentum space and update the kinetic term */
   fastfourier(repsi,impsi,rekpsi,imkpsi,NSITES,1,1,1);
   for (i=0;i<NSITES;i++)
    {
     temp     =imkpsi[i]*rekfactor[i]+rekpsi[i]*imkfactor[i];
     rekpsi[i]=rekpsi[i]*rekfactor[i]-imkpsi[i]*imkfactor[i];
     imkpsi[i]=temp;
    }
    /* go back to position space */
   fastfourier(rekpsi,imkpsi,repsi,impsi,NSITES,1,1,-1);
  }
 if (damped) renorm();
 drawit();
 return;
}

# define size 10
# define dt .05 
 /* pfactor and kfactor normalize potential and kinetic terms   */
 /* their only purpose here is to make it easy to change things */
# define pfactor (.6*5./hbar)
# define kfactor (1.0*hbar/(5.*size))
# define dx (size/(1.*NSITES))
# define dampfactor .5

void makefactor() /* make the diagonal matrices used in updating */
{int i,j;
 double temp,nsitesinv;
 nsitesinv=1.0/NSITES;
 for (i=0;i<NSITES;i++)
   {temp=pfactor*dt*potential[i];
    repfactor[i]=cos(temp);
    impfactor[i]=sin(temp);
    if (damped)
     {temp=exp(-dampfactor*temp);
      repfactor[i]*=temp;
      impfactor[i]*=temp;
     }
    j=i;
    if (j>=NSITES/2) j-=NSITES;
    temp=kfactor*dt*j*j;
    rekfactor[i]=nsitesinv*cos(temp);
    imkfactor[i]=nsitesinv*sin(temp);
    if (damped)
     {temp=exp(-dampfactor*temp);
      rekfactor[i]*=temp;
      imkfactor[i]*=temp;
     }
   }
 return;
}

void renorm() /* normalizes the wave function */
{int i;
 double norm;
 norm=0.0;
 for(i=0;i<NSITES;i++)
   norm+=(repsi[i]*repsi[i]+impsi[i]*impsi[i]); 
 if (0.00001<norm) 
  {norm=1/sqrt(norm*dx);
   for (i=0;i<NSITES;i++)
     {repsi[i]*=norm;
      impsi[i]*=norm;
     }
  }  
 return;
}

#define min(a,b) ((a>b)? b : a)
#define max(a,b) ((a>b)? a : b)

void drawit() /* draws the wave function */
{int i,k1,k2,j,jmax;
 double mag;
  /* calculate potential and amplitude to display as integers */
 j=0;
 for (i=0;i<NSITES;i++)
   {mag=sqrt(repsi[i]*repsi[i]+impsi[i]*impsi[i]);
    k1=playheight*(1-mag);
    k2=playheight*(1-potential[i]);
    iamp[j]=max(0,min(playheight,k2));
    ipot[j]=max(0,min(playheight,k1));
    j+=(NPOINTS/NSITES);
   }
 iamp[NPOINTS]=iamp[0];
 ipot[NPOINTS]=ipot[0];
  /* interpolate between NSITES to NPOINTS */
 jmax=NPOINTS/NSITES;
 for (j=1;j<jmax;j++)
  for (i=0;i<NSITES;i++)
    {
     iamp[i*jmax+j]=((jmax-j)*iamp[i*jmax]+j*iamp[(i+1)*jmax])/jmax;
     ipot[i*jmax+j]=((jmax-j)*ipot[i*jmax]+j*ipot[(i+1)*jmax])/jmax;
    }

  /* now draw it        */
  /* first the red part */
 for (i=0;i<NPOINTS;i++)
  {
   k1=iamp[i];
   k2=ipot[i];
   xpoints[i].x=xpoints[2*NPOINTS-1-i].x=(i*playwidth)/(NPOINTS-1);
   xpoints[i].y=max(k1,k2); 
   xpoints[2*NPOINTS-1-i].y=max(k2,min(k1,k2)); /* for later use */
  }
 xpoints[NPOINTS  ].x=playwidth;
 xpoints[NPOINTS  ].y=playheight; 
 xpoints[NPOINTS+1].x=0;
 xpoints[NPOINTS+1].y=playheight;
 color(0);
 XFillPolygon(display,window,gc,xpoints,NPOINTS+2,Nonconvex,CoordModeOrigin);

  /* fix the two points for the magenta part */
 for (i=NPOINTS-2;i<NPOINTS;i++)
  {
   k1=iamp[i];
   k2=ipot[i];
   xpoints[2*NPOINTS-1-i].x=(i*playwidth)/(NPOINTS-1);
   xpoints[2*NPOINTS-1-i].y=max(k2,min(k1,k2));
  }
 color(12);
 XFillPolygon(display,window,gc,xpoints,NPOINTS*2,Nonconvex,CoordModeOrigin);

  /* now the green */
 for (i=0;i<NPOINTS;i++)
  {
   k1=iamp[i];
   k2=ipot[i];
   xpoints[i].y=min(k1,k2);
  }
 color(2);
 XFillPolygon(display,window,gc,xpoints,NPOINTS*2,Nonconvex,CoordModeOrigin);

  /* and the cyan on top */
 xpoints[NPOINTS  ].x=playwidth;
 xpoints[NPOINTS  ].y=0; 
 xpoints[NPOINTS+1].x=0;
 xpoints[NPOINTS+1].y=0;
 color(3);
 XFillPolygon(display,window,gc,xpoints,NPOINTS+2,Nonconvex,CoordModeOrigin);
 return;
}

void repaint()  /* this fixes the window up whenever it is uncovered */
{color(9);
 drawbutton(pausebutton,  0,0,BUTTONWIDTH,BUTTONHEIGHT,"pause",  2-4*paused);
 drawbutton(dampbutton,   0,0,BUTTONWIDTH,BUTTONHEIGHT,"damp",   2-4*damped);  
 drawbutton(quitbutton,   0,0,BUTTONWIDTH,BUTTONHEIGHT,"quit",   2);
 drawbutton(vconstbutton,  0,0,BUTTONWIDTH,BUTTONHEIGHT,"V=const", 2);
 drawbutton(speedbutton,0,0,BUTTONWIDTH,BUTTONHEIGHT,"speed",-2);
 drawbutton(speedbutton,1+((10-speed)*(BUTTONWIDTH-2))/11,1,
                      (BUTTONWIDTH-2)/11,BUTTONHEIGHT-2,"",2);
 drawbutton(hbarbutton,0,0,BUTTONWIDTH,BUTTONHEIGHT,"hbar",-2);
 drawbutton(hbarbutton,1+((hbar-1)*(BUTTONWIDTH-2))/10,1,
                      (BUTTONWIDTH-2)/11,BUTTONHEIGHT-2,"",2);
 drawbutton(window,0,playheight+BUTTONHEIGHT,playwidth,BUTTONHEIGHT,
  "left mouse button perturbs wave function (psi^2=magenta)",0);
 drawbutton(window,0,playheight+2*BUTTONHEIGHT,playwidth,BUTTONHEIGHT,
  "right mouse button perturbs potential (green)",0);
 color(4);
 XFillRectangle(display,window,gc,0,playheight,windowwidth,BUTTONHEIGHT);
  drawit();
 return;
}

void openwindow(argc,argv)
/* a lot of this is taken from basicwin in the Xlib Programming Manual */
int argc;
char **argv;
{char *window_name="Schrodinger";
 char *icon_name="waves";
 long event_mask;
 Pixmap icon_pixmap;
 char *display_name=NULL;
 int i;
# define icon_bitmap_width 16
# define icon_bitmap_height 16
 static unsigned char icon_bitmap_bits[] = {
   0x1f, 0xf8, 0x1f, 0x88, 0x1f, 0x88, 0x1f, 0x88, 0x1f, 0x88, 0x1f, 0xf8,
   0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0xff, 0xff,
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};

/* open up the display */
 if ((display=XOpenDisplay(display_name))==NULL)
  {fprintf(stderr,"%s: cannot connect to X server %s\n",
    progname,XDisplayName(display_name));
   exit(-1);
  }
 screen=DefaultScreen(display);
 cmap=DefaultColormap(display,screen);

 darkcolor=black=BlackPixel(display,screen);
 lightcolor=white=WhitePixel(display,screen);
 translate[0]=white;
 translate[1]=black;
 translate[2]=lightcolor;
 translate[3]=darkcolor;
 for(i=4;i<256;i++)
   translate[i]=translate[i%4]; 
  /* a bunch of colors to play with */
 if (XAllocNamedColor(display,cmap,"salmon",&colorcell,&xcolor))
              darkcolor=colorcell.pixel;
 if (XAllocNamedColor(display,cmap,"wheat",&colorcell,&xcolor))
              lightcolor=colorcell.pixel;
 if (XAllocNamedColor(display,cmap,"red",&colorcell,&xcolor))
              translate[0]=colorcell.pixel;
 if (XAllocNamedColor(display,cmap,"blue",&colorcell,&xcolor))
              translate[1]=colorcell.pixel;
 if (XAllocNamedColor(display,cmap,"green",&colorcell,&xcolor))
              translate[2]=colorcell.pixel;
 if (XAllocNamedColor(display,cmap,"cyan",&colorcell,&xcolor))
              translate[3]=colorcell.pixel;
 if (XAllocNamedColor(display,cmap,"orange",&colorcell,&xcolor))
              translate[4]=colorcell.pixel;
 if (XAllocNamedColor(display,cmap,"purple",&colorcell,&xcolor))
              translate[5]=colorcell.pixel;
 if (XAllocNamedColor(display,cmap,"yellow",&colorcell,&xcolor))
              translate[6]=colorcell.pixel;
 if (XAllocNamedColor(display,cmap,"pink",&colorcell,&xcolor))
              translate[7]=colorcell.pixel;
 if (XAllocNamedColor(display,cmap,"brown",&colorcell,&xcolor))
              translate[8]=colorcell.pixel;
 if (XAllocNamedColor(display,cmap,"grey",&colorcell,&xcolor))
              translate[9]=colorcell.pixel;
 if (XAllocNamedColor(display,cmap,"turquoise",&colorcell,&xcolor))
              translate[10]=colorcell.pixel;
 if (XAllocNamedColor(display,cmap,"gold",&colorcell,&xcolor))
              translate[11]=colorcell.pixel;
 if (XAllocNamedColor(display,cmap,"magenta",&colorcell,&xcolor))
              translate[12]=colorcell.pixel;
 if (XAllocNamedColor(display,cmap,"navy",&colorcell,&xcolor))
              translate[13]=colorcell.pixel;
 if (XAllocNamedColor(display,cmap,"tan",&colorcell,&xcolor))
              translate[14]=colorcell.pixel;
 if (XAllocNamedColor(display,cmap,"violet",&colorcell,&xcolor))
              translate[15]=colorcell.pixel;
 if (XAllocNamedColor(display,cmap,"white",&colorcell,&xcolor))
              translate[16]=colorcell.pixel;
 if (XAllocNamedColor(display,cmap,"black",&colorcell,&xcolor))
              translate[17]=colorcell.pixel;
  /* feel free to type in more colors, I got bored */
 for(i=18;i<256;i++)
   translate[i]=translate[i%18];

    /* make the main window */
 windowwidth=6*BUTTONWIDTH;
 windowheight=300;
 playwidth=windowwidth;
 playheight=(windowheight-3*BUTTONHEIGHT);
 window=XCreateSimpleWindow(display,RootWindow(display,screen),
   0,0,windowwidth,windowheight,4,translate[14],lightcolor);
    /* make the icon */
 icon_pixmap=XCreateBitmapFromData(display,window,
   icon_bitmap_bits,icon_bitmap_width,icon_bitmap_height);

 size_hints.flags=PPosition | PSize | PMinSize;
 size_hints.min_width=windowwidth;
 size_hints.min_height=300;
 /* this is probably silly, and not really tested, but here goes */
#ifdef X11R3
 size_hints.x=x;
 size_hints.y=y;
 size_hints.width=windowwidth;
 size_hints.height=windowheight;
 XSetStandardProperties(display,window,window_name,icon_name,
    icon_pixmap,argv,argc,&size_hints);
#else
 {XWMHints wm_hints;
  XClassHint class_hints;
  XTextProperty windowName, iconName;
  if (XStringListToTextProperty(&window_name,1,&windowName)==0)
   {fprintf(stderr,"%s: structure allocation for windowName failed.\n"
      ,progname);
    exit(-1);
   }
  if (XStringListToTextProperty(&icon_name,1,&iconName)==0)
   {fprintf(stderr,"%s: structure allocation for iconName failed.\n"
       ,progname);
    exit(-1);
   }
  wm_hints.initial_state=NormalState;
  wm_hints.input=True;
  wm_hints.icon_pixmap=icon_pixmap;
  wm_hints.flags=StateHint|IconPixmapHint|InputHint;
  class_hints.res_name=progname;
  class_hints.res_class="Basicwin";
  XSetWMProperties(display,window,&windowName,&iconName,
       argv,argc,&size_hints,&wm_hints,&class_hints);
 }
#endif

   /* pick the events to look for */
 event_mask=ExposureMask|ButtonPressMask|StructureNotifyMask;
 XSelectInput(display,window,event_mask);
   /* pick font: 9x15 is supposed to almost always be there */
 if ((font=XLoadQueryFont(display,"9x15"))==NULL) 
   if ((font=XLoadQueryFont(display,"fixed"))==NULL)
     {fprintf(stderr,"%s: Sorry, having font problems.\n",progname);
      exit(-1);
     }
 font_height=font->ascent+font->descent;
 font_width=font->max_bounds.width;

  /* make graphics context: */
 gc=XCreateGC(display,window,0,NULL);
 XSetFont(display,gc,font->fid);
 XSetForeground(display,gc,black);
 XSetBackground(display,gc,lightcolor); 

   /* show the window */
 XMapWindow(display,window);
 return;
}

void makebuttons()
{    /* first destroy any old buttons */
 XDestroySubwindows(display,window);
     /* now make the new buttons */
 quitbutton =makebutton(0,windowheight-3*BUTTONHEIGHT,BUTTONWIDTH,BUTTONHEIGHT);
 vconstbutton=makebutton(BUTTONWIDTH,windowheight-3*BUTTONHEIGHT,
                        BUTTONWIDTH,BUTTONHEIGHT);
 pausebutton=makebutton(2*BUTTONWIDTH,windowheight-3*BUTTONHEIGHT,
                        BUTTONWIDTH,BUTTONHEIGHT);
 dampbutton=makebutton(3*BUTTONWIDTH,windowheight-3*BUTTONHEIGHT,
                        BUTTONWIDTH,BUTTONHEIGHT);
 speedbutton=makebutton(windowwidth-BUTTONWIDTH,
                  windowheight-3*BUTTONHEIGHT,BUTTONWIDTH,BUTTONHEIGHT);
 hbarbutton=makebutton(windowwidth-2*BUTTONWIDTH,
                  windowheight-3*BUTTONHEIGHT,BUTTONWIDTH,BUTTONHEIGHT);
 return;
}

void cleanup()
{XUnloadFont(display,font->fid);
 XFreeGC(display,gc); 
 XCloseDisplay(display);
 exit(0);
} 

Window makebutton(xoffset,yoffset,xsize,ysize)
int xoffset,yoffset,xsize,ysize;
/* Puts button of specified dimensions on window.  Nothing is drawn. */
{Window buttonwindow;
 long event_mask;
 buttonwindow=XCreateSimpleWindow(display,window,xoffset,yoffset,
        xsize,ysize,0,black,lightcolor);
 event_mask=ButtonPressMask|ExposureMask; /* look for mouse-button presses */
 XSelectInput(display,buttonwindow,event_mask);
 XMapWindow(display,buttonwindow);
 return buttonwindow;
}

void drawbutton(buttonwindow,xoffset,yoffset,xsize,ysize,text,state)
Window buttonwindow;
int xoffset,yoffset,xsize,ysize,state;
char * text;
/* Draw a button in buttonwindow of specified dimensions with text
   centered.  Color of button determined by sign of "state", 
   size of border determined by magnitude. 
   I don't need no stinking tookit. */
{int textlength,i,j;
 int cdark,clight,cup,cdown;
 int cleft,cright,cbutton,ctext;
 cup=lightcolor;
 cdown=darkcolor;
 cdark=black;
 clight=white;
 if (state<0)
  {cbutton=cdown;
   ctext=clight;
   cleft=cdark;
   cright=clight;
  } 
 else
  {cbutton=cup;
   ctext=cdark;
   cleft=clight;
   cright=cdark;
  }
 j=abs(state); /* width for button borders */
 XSetForeground(display,gc,cbutton);  
 XFillRectangle(display,buttonwindow,gc,xoffset+j,yoffset+j,
        xsize-2*j,ysize-2*j);
 XSetForeground(display,gc,cleft);
 XFillRectangle(display,buttonwindow,gc,xoffset,yoffset,xsize,j);
 XFillRectangle(display,buttonwindow,gc,xoffset,yoffset,j,ysize);
 XSetForeground(display,gc,cright);
 for (i=0;i<j;i++) 
  {XDrawLine(display,buttonwindow,gc,
     xoffset+i,yoffset+ysize-i-1,xoffset+xsize-i-1,yoffset+ysize-i-1);
   XDrawLine(display,buttonwindow,gc,
     xoffset+xsize-i-1,yoffset+i,xoffset+xsize-i-1,yoffset+ysize-i-1);
  }
 if (NULL!=text)
   {textlength=strlen(text);
    XSetForeground(display,gc,ctext);
    XDrawString(display,buttonwindow,gc,
          xoffset+(xsize-textlength*font_width)/2,
          yoffset+(ysize+font_height)/2-2,
          text,textlength);
   }
 XSetForeground(display,gc,black);
 return;
}

void fastfourier(double *rex,double *imx, double *ref,double *imf,
                 int n,int d1,int d2,int direction)
/* recursively calls itself on two halves of the vector */
/* (rex[],imx[]) is the input vector
   (ref[],imf[]) is the output
   n=vector length
   d1=spacing between elements of input vector
   d2= same for output
   direction= +/-1 for forward and reverse fourier transforms
   vector is not normalized */  
{int j,Ndn,nd2,i1,i2;
 double c,s,temp1,temp2,temp3;
 if (n<=2) /* do two elements case by hand */
   {*ref=*rex+*(rex+d1);
    *imf=*imx+*(imx+d1);
    *(ref+d2)=*rex-*(rex+d1);
    *(imf+d2)=*imx-*(imx+d1);
    return;
   }
 nd2=n>>1; /* half the vector length */
  /* recursively call fastfourier */
 fastfourier(rex   ,imx   ,ref       ,imf       ,nd2,2*d1,d2,direction);
 fastfourier(rex+d1,imx+d1,ref+nd2*d2,imf+nd2*d2,nd2,2*d1,d2,direction);
  /* combine the results */
 Ndn=NSITES/n;
 for (j=0;j<nd2;j++)
  {c=cn[j*Ndn];
   s=direction*sn[j*Ndn];
   i1=d2*j;
   i2=d2*(j+nd2);
   temp1  =ref[i1]+c*ref[i2]-s*imf[i2];
   temp2  =imf[i1]+c*imf[i2]+s*ref[i2];
   temp3  =ref[i1]-c*ref[i2]+s*imf[i2];
   imf[i2]=imf[i1]-c*imf[i2]-s*ref[i2];
   ref[i1]=temp1;
   imf[i1]=temp2;
   ref[i2]=temp3;
  }
 return;
}
