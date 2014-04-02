 /*  Xpotts, a Potts model simulator for X windows.
           
      Michael Creutz   
      creutz@bnl.gov 

 compiling: cc -O -L/usr/X11R6/lib xpotts.c -lm -lX11 

 latest version kept at:
   http://thy.phy.bnl.gov/www/xtoys/xtoys.html

 version 1.4
 24 Nov 1995  
 LZW disabled, 10 October 1998

 minor modifications to eliminate gcc -Wall warnings, March 1999

 */

/* because of the unisys patent on LZW, it may be illegal to
   distribute this program with the following "if" modified to "while"*/
#define LZW if

# include <X11/Xlib.h>
# include <X11/Xutil.h>
# include <X11/Xatom.h> 
# include <X11/cursorfont.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <malloc.h>
# include <math.h>
# include <sys/select.h>

/* dimensions for control boxes */
# define HEATERHEIGHT 80
# define HEATERWIDTH 96
# define MHEIGHT 80
# define MWIDTH 96
# define QWIDTH 162
# define PLAYLEFT 20
# define PLAYTOP 80
# define XHEIGHT 40
# define BUTTONWIDTH 66
# define SRHEIGHT (40)
# define BBWIDTH 96
# define INFOLEFT (block*ncols+56)

typedef unsigned char spin;
spin *field=NULL; /* pointer to system */
int nrows,ncols,volume; /* lattice dimensions (plus two for boundaries) */
int q=8; /* the `q' state potts model, default to 8 */
int block,           /* size of cells displayed on the screen          */
    blockshift;      /* log_2(block)                                   */
int heatervalue=2, magvalue=3; /* start with cooling and free magnitization */
int demon=0,mdemon=0;       /* the energy and magnetism demons, respectively */
# define DEMONMASK 0xf      /* how big the demon can get */
int iteration=0,energycount=0,mcount=0,volumecount=0;
long mrand48(),lrand48(); /* generates a random word, lrand is non-negative */
int paused=0;             /* is the system paused?     */
char stringbuffer[256];   /* generally useful       */
void drawbutton(),openwindow(),makebuttons(),update(),repaint(),cleanup(),
    showpic(),setheater(),setmag(),setq(),savepic(),loadpic();
int energy();
  /* things for regulating the updating speed */
int speed=0; /* updating delay proportional to speed   */
int delay=0; /* counter for implementing speed control */
struct timeval timeout; /* timer for speed control */

/* various window stuff */
Display *display;
int screen;
static char *progname;
Window window,quitbutton,pausebutton,srbutton,heatwindow,magwindow,
       speedbutton,qbutton,blockbutton,makebutton();
XColor xcolor,colorcell;
Colormap cmap;
GC gc;
int windowwidth,windowheight;
XFontStruct *font=NULL;
int font_height,font_width;
XSizeHints size_hints;
int darkcolor,lightcolor,black,white;
XImage *spinimage=NULL;
long translate[256];    /* for converting colors   */
 /* text for some of the buttons */
char *srtext[2]={"save","restore"};
char *heatertext[4]={"conserve E","heat","cool","free E"};
char *magtext[4]={"conserve M","increase M","decrease M","free M"};
char *qtext[3]={"<","q",">"};

int main(argc,argv)
int argc;
char **argv;
{unsigned int width, height;
 int i,y;
 XEvent report;
 progname=argv[0];
  /* initial lattice size */
 block=1;
 nrows=192/block;
 ncols=192/block;
 ncols&=(~(sizeof(long)-1));
 if (!((nrows-2)%29)) nrows-=1; /* because demons hop 29 rows */
 if (!((ncols-2)%29)) ncols-=sizeof(long);
 volume=nrows*ncols;
 openwindow(argc,argv);
 makebuttons();

/* loop forever, looking for events */
 while(1)
  {if ((0==XPending(display))&&(0==paused))
    {if (delay) /* don't update yet */
       {delay--;
        /* this use of select() seems a kludge to me; 
           why can't usleep() be more standard? */
        timeout.tv_sec=0;
        timeout.tv_usec=100000; /* .1 sec per delay unit */
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
      {case Expose:
        if ((report.xexpose.window)!=window) break; /* cuts down flashing,
           but you might remove this line if things aren't being redrawn */
        if (report.xexpose.count!=0) break; /* more in queue, wait for them */
        repaint();  
        break;
       case ConfigureNotify:
        width=report.xconfigure.width;
        height=report.xconfigure.height;
        if ((width<size_hints.min_width)||(height<size_hints.min_height))
            {fprintf(stderr,"%s: window too small to proceed.\n",progname);
             cleanup();
            } 
        else if ((width!=windowwidth)||(height!=windowheight))
            {windowwidth=width;
             windowheight=height;  
             nrows=(windowheight-PLAYTOP-20)/block;
             ncols=(windowwidth-82-190)/block;
             ncols&=(~(sizeof(long)-1));
             if (!((nrows-2)%29)) nrows-=2;
             if (!((ncols-2)%29)) ncols-=sizeof(long);
             volume=nrows*ncols;
             makebuttons();
             showpic();
            } 
        break; 
       case ButtonPress:
        if (report.xbutton.window==quitbutton)
             cleanup();
        else if (report.xbutton.window==pausebutton)
            {paused=1-paused;
             drawbutton(pausebutton,0,0,BUTTONWIDTH,18,"pause",1-2*paused);
            } 
        else if (report.xbutton.window==heatwindow)
            {setheater((report.xbutton.y*4)/HEATERHEIGHT);
            }
        else if (report.xbutton.window==magwindow)
            {setmag((report.xbutton.y*4)/MHEIGHT);
            }
        else if (report.xbutton.window==qbutton)
            {setq((report.xbutton.x*3)/QWIDTH);
            }
        else if (report.xbutton.window==srbutton)
            {y=(report.xbutton.y*2)/SRHEIGHT;
             drawbutton(srbutton,0,y*(SRHEIGHT/2),BUTTONWIDTH,SRHEIGHT/2,
                  srtext[y],-1);
             if (0==y)
                savepic(field,ncols,nrows);
             else if (1==y) /* load in gif image */
              {loadpic(field,ncols,nrows);
               for (i=0;i<volume;i++)
                 while (((int) field[i])>=q) field[i]-=q; /* mod q the image */
               showpic();
              }
             drawbutton(srbutton,0,y*(SRHEIGHT/2),BUTTONWIDTH,SRHEIGHT/2,
                  srtext[y],1);
            }
        else if (report.xbutton.window==blockbutton)
          {i=(1<<(4*report.xbutton.x/HEATERWIDTH));
           if (i!=block) /* reset for new block size */
             {block=i;
              nrows=(windowheight-PLAYTOP-20)/block;
              ncols=(windowwidth-82-190)/block;
              ncols&=(~(sizeof(long)-1));
              if (!((nrows-2)%29)) nrows-=2;
              if (!((ncols-2)%29)) ncols-=sizeof(long);
              volume=nrows*ncols;
              makebuttons();
              showpic();
             }
          }
        else if (report.xbutton.window==speedbutton)
          {  /* reset speed */
           speed=10-(11*report.xbutton.x)/HEATERWIDTH;
           if (speed<0) speed=0;
           if (speed>10) speed=10;
           delay=speed;
           drawbutton(speedbutton,0,0,HEATERWIDTH,18,"speed",-1);
           drawbutton(speedbutton,1+((10-speed)*(HEATERWIDTH-2))/11,1,
                      (HEATERWIDTH-2)/11,16,"",2);
          }
        else
         update(); /* do a sweep when mouse clicked */
        break;
       default:
        break;
      } /* end of switch */
    } /* end of if XPending */
  } /* end of while(1) */
} /* end of main */

void update()
{int newdemon,oldenergy,newenergy,direction;
 int rowloop,colloop,newmdemon;
 double beta,hfield,E,M;
 int i,row,col;
 spin i1,i2,i3,i4,trial,oldspin,*ptr,*ptr1;
 row=col=1;
 for (rowloop=2;rowloop<nrows;rowloop++)
    {row+=29;
     while (row>=nrows-1) row-=(nrows-2);
     direction=1+lrand48()%(q-1);
     if (heatervalue) /* if heater on, adjust demon */
      if (8&lrand48()) /* half the time */
       {if (2==heatervalue) demon=0;
        if ((1==heatervalue)&&(demon<DEMONMASK)) demon++;
        if (3==heatervalue) demon=DEMONMASK>>1;
       }
     if (magvalue)  /* adjust magnetic demon */
      if (0==(60&lrand48())) /* one in 16 chance since 60 has 4 bits set */
       {if (2==magvalue) mdemon=0;
        if (1==magvalue) mdemon=DEMONMASK;
        if (3==magvalue) mdemon=DEMONMASK>>1;
       }
     ptr1=field+ncols*row;
     for (colloop=2;colloop<ncols;colloop++)
      {col+=29;
       while (col>=ncols-1) col-=(ncols-2);
       ptr=ptr1+col;
       oldspin=(*ptr);
       i1=(*(ptr+1));     /* right neighbor  */
       i2=(*(ptr-1));     /* left neighbor   */
       i3=(*(ptr+ncols)); /* top neighbor    */
       i4=(*(ptr-ncols)); /* bottom neighbor */
       trial=oldspin+direction;
       if (((int) trial)>=q) trial-=q;
       oldenergy=(oldspin==i1)
                +(oldspin==i2)
                +(oldspin==i3)
                +(oldspin==i4);
       newenergy=(trial==i1)
                +(trial==i2)
                +(trial==i3)
                +(trial==i4);
       newdemon=demon-oldenergy+newenergy;
       newmdemon=mdemon-(0==trial)+(0==oldspin);
       if (0==((newdemon|newmdemon)&(~DEMONMASK))) /* accept changes */
        {demon=newdemon;
         mdemon=newmdemon;  
         *ptr=trial;
         direction=q-direction;
         /* fix vertical boundary */
         if (1==col)
           *(ptr+ncols-2)=(*ptr); 
         else if ((ncols-2)==col) 
           *(ptr-ncols+2)=(*ptr);
        }
       volumecount++;
       energycount+=(2&demon); /* use second bit so works for q=2 */
       mcount+=(1&mdemon);
      }
        /* fix horizontal boundary */
     if (1==row) 
       for (i=0;i<ncols;i++)
        field[volume-ncols+i]=field[i+ncols];
     else if ((nrows-2)==row) 
       for (i=0;i<ncols;i++)
        field[i]=field[volume-2*ncols+i];
    }
 iteration++;
 if (20==iteration) /* display information */
  {if ((2*volumecount>energycount)&&(0!=energycount))
    {beta=.5*log((double)(2*volumecount/(1.0*energycount)-1.0));
     sprintf(stringbuffer," beta=%6.3f    ",beta);
    }
   else
     sprintf(stringbuffer," beta divergent");
   XDrawImageString(display,window,gc,INFOLEFT+6,150
     ,stringbuffer,strlen(stringbuffer));
   if ((volumecount>mcount)&&(0!=mcount))
    {hfield=(-log((double) (volumecount/(1.0*mcount)-1.0)));
     sprintf(stringbuffer," h   =%6.3f  ",hfield);
    }
   else
     sprintf(stringbuffer," h divergent ");
   XDrawImageString(display,window,gc,INFOLEFT+6,168
     ,stringbuffer,strlen(stringbuffer));
   iteration=volumecount=energycount=mcount=0;
   energy(&E,&M);
   sprintf(stringbuffer," E   =%6.3f  ",E);
   XDrawImageString(display,window,gc,INFOLEFT+6,186
     ,stringbuffer,strlen(stringbuffer));
   sprintf(stringbuffer," M   =%6.3f  ",M);
   XDrawImageString(display,window,gc,INFOLEFT+6,202
     ,stringbuffer,strlen(stringbuffer));
  }
 showpic(); 
 return;
}

int energy(e,m)
double *e,*m;
{int i,row,col;
 int etot=0,mtot=0;
 for (row=1;row<nrows-1;row++)
 for (col=1;col<ncols-1;col++)
    {i=col+ncols*row;
     etot+= (field[i]==field[i+1])
           +(field[i]==field[i+ncols]);
     mtot+=(0==field[i]);
    }
 *e=(1.0-etot/(2.0*(nrows-2)*(ncols-2)))/(1-1.0/q);
 *m=(mtot/(1.0*(nrows-2)*(ncols-2))-1.0/q)/(1-1.0/q);
 return etot;
}

void showpic() /* display the field */
{int row,col,i1,i2,color,j,j1,j2,blocktop=block;
 char *picture=(*spinimage).data;
 if (block>4) blocktop=block-1;
 if (8==(*spinimage).depth) 
  {if (block>1) /* I wish I knew how to do this faster */
     for (row=0;row<nrows;row++)
      for (col=0;col<ncols;col++)
       {color=translate[field[row*ncols+col]];
        j=block*(col+block*ncols*row);
        if (color!=picture[j])
         for (i1=0;i1<blocktop;i1++)
          {j1=i1*block*ncols+j;
           for (i2=0;i2<blocktop;i2++)
            picture[j1+i2]=color;
          }
       }
   else
    {for (j=0;j<volume;j++)
      picture[j]=translate[field[j]];
    }
  }
 else /* depth is not 8, use xputpixel (this is really ugly) */
  {if (block>1) /* I wish I knew how to do this faster */
    for (row=0;row<nrows;row++)
     for (col=0;col<ncols;col++)
      {color=translate[field[row*ncols+col]];
       if (color!=XGetPixel(spinimage,j1=block*col,j2=block*row))
         for (i2=0;i2<blocktop;i2++)
           for (i1=0;i1<blocktop;i1++)
            XPutPixel(spinimage,j1+i1,j2+i2,color);
      }
   else
    for (row=0;row<nrows;row++)
     {j1=row*ncols; 
      for (col=0;col<ncols;col++)
        XPutPixel(spinimage,col,row,translate[field[j1+col]]);
     }
  }
 XPutImage(display,window,gc,spinimage,0,0,PLAYLEFT,PLAYTOP,
        block*ncols,block*nrows); 
 return;
}

void repaint()
   /* this fixes the window up whenever it is uncovered */
{int i;
 XSetForeground(display,gc,translate[14]);
 XFillRectangle(display,window,gc,0,0,windowwidth,windowheight);
 XSetForeground(display,gc,darkcolor);
 drawbutton(pausebutton,0,0,BUTTONWIDTH,18,"pause",1-2*paused);
 drawbutton(quitbutton,0,0,BUTTONWIDTH,18,"quit",1);
 drawbutton(window,PLAYLEFT-4,PLAYTOP-4,block*ncols+8,block*nrows+8,NULL,4);
 drawbutton(window,INFOLEFT,132,208,120,NULL,2);
 drawbutton(speedbutton,0,0,HEATERWIDTH,18,"speed",-1);
 drawbutton(speedbutton,1+((10-speed)*(HEATERWIDTH-2))/11,1,
               (HEATERWIDTH-2)/11,16,"",2);
 for (i=0;i<2;i++)
  drawbutton(srbutton,0,i*(SRHEIGHT/2),BUTTONWIDTH,SRHEIGHT/2,srtext[i],1);
  /* fix the block buttons */
 for (i=1;i<=4;i++)
  {sprintf(stringbuffer,"%d",(1<<i)/2);
   drawbutton(blockbutton,(i-1)*BBWIDTH/4,0,BBWIDTH/4,18,
         stringbuffer,1-2*(i==(blockshift+1)));
  } 
  /* fix the q button */
 for (i=0;i<3;i+=2)
  drawbutton(qbutton,i*(QWIDTH/3),0,QWIDTH/3,18,qtext[i],1);
 setq(1);
  /* draw the heater buttons */
 for (i=0;i<4;i++)
  drawbutton(heatwindow,0,i*HEATERHEIGHT/4,HEATERWIDTH,HEATERHEIGHT/4,
     heatertext[i],1-2*(i==heatervalue));
  /* draw the magnitization buttons */
 for (i=0;i<4;i++)
  drawbutton(magwindow,0,i*MHEIGHT/4,MWIDTH,MHEIGHT/4,
     magtext[i],1-2*(i==magvalue));
          /* write various strings */
 sprintf(stringbuffer,"%d by %d lattice   ",ncols-2,nrows-2);
 XDrawImageString(display,window,gc,INFOLEFT+10,242
     ,stringbuffer,strlen(stringbuffer));
 XDrawString(display,window,gc,
        windowwidth-3*HEATERWIDTH/2-4,HEATERHEIGHT+42,
        "cell size",9);
 XDrawString(display,window,gc,windowwidth-50,windowheight-5,"MJC",3); 
 showpic();  
 return;
}

void setheater(value)
/* acts when heater buttons hit */
int value;
{value&=3;
 drawbutton(heatwindow,0,(HEATERHEIGHT*heatervalue)/4,
          HEATERWIDTH,HEATERHEIGHT/4,heatertext[heatervalue],1);
 heatervalue=value;
 drawbutton(heatwindow,0,(HEATERHEIGHT*heatervalue)/4,
          HEATERWIDTH,HEATERHEIGHT/4,heatertext[heatervalue],-1);
 return;
}

void setmag(value)
/* acts when magnitization buttons hit */
int value;
{value&=3;
 drawbutton(magwindow,0,(MHEIGHT*magvalue)/4,MWIDTH,MHEIGHT/4,
       magtext[magvalue],1);
 magvalue=value;
 drawbutton(magwindow,0,(MHEIGHT*magvalue)/4,MWIDTH,MHEIGHT/4,
       magtext[magvalue],-1);
 return;
}

void setq(value) 
int value;
{int i; 
 if ((0==value)&&(q>2)) 
  {q--;
   for (i=0;i<volume;i++)
     if (((int) field[i])>=q) field[i]--; 
   showpic();
  }           
 if ((2==value)&&(q<256)) q++;
 sprintf(stringbuffer,"q=%d",q);
 drawbutton(qbutton,QWIDTH/3,0,QWIDTH/3,18,stringbuffer,1);
 sprintf(stringbuffer,"(critical beta=%6.3f)",log((double)(1+sqrt(1.*q))));
 XDrawImageString(display,window,gc,INFOLEFT+6,224
     ,stringbuffer,strlen(stringbuffer));
 return;
}

void openwindow(argc,argv)
/* a lot of this is taken from basicwin in the Xlib Programming Manual */
int argc;
char **argv;
{char *window_name="Potts";
 char *icon_name="Potts";
 long event_mask;
 Pixmap icon_pixmap;
 char *display_name=NULL;
 int i;
# define icon_bitmap_width 16
# define icon_bitmap_height 16
 static char icon_bitmap_bits[] = {
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
 windowwidth=block*ncols+82+190;
 windowheight=PLAYTOP+block*nrows+20;
 window=XCreateSimpleWindow(display,RootWindow(display,screen),
   0,0,windowwidth,windowheight,4,black,lightcolor);
    /* make the icon */
 icon_pixmap=XCreateBitmapFromData(display,window,
   icon_bitmap_bits,icon_bitmap_width,icon_bitmap_height);

 size_hints.flags=PPosition | PSize | PMinSize;
 size_hints.min_width=windowwidth;
 size_hints.min_height=windowheight;
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
 XSetForeground(display,gc,darkcolor);
 XSetBackground(display,gc,lightcolor); 
   /* show the window */
 XMapWindow(display,window);
 return;
}

void makebuttons()
{int i;
 XEvent report;
 Cursor cursor;
     /* first destroy any old buttons */
 XDestroySubwindows(display,window);
     /* now make the new buttons */
 quitbutton  =makebutton(12,4,BUTTONWIDTH,18);
 pausebutton =makebutton(QWIDTH-BUTTONWIDTH,4,BUTTONWIDTH,18);
 srbutton    =makebutton(QWIDTH+14,4,BUTTONWIDTH,SRHEIGHT);
 heatwindow  =makebutton(windowwidth-HEATERWIDTH-6,4,HEATERWIDTH,HEATERHEIGHT);
 magwindow   =makebutton(windowwidth-MWIDTH-HEATERWIDTH-14,4,MWIDTH,MHEIGHT);
 qbutton     =makebutton(6,26,QWIDTH,18);
 blockbutton =makebutton(windowwidth-3*HEATERWIDTH/2-10,HEATERHEIGHT+10,
               BBWIDTH,18);
 speedbutton=makebutton(40,48,HEATERWIDTH,18);
   /* wait for window to be displayed befor proceeding */
 i=1; /* a flag */
 while (i)
    {XNextEvent(display,&report); 
     switch (report.type)
      {case Expose:
          if (report.xexpose.window!=window) i=0;
       default:
        break;
      }
    }
   /* destroy old image structure */
 if (NULL!=spinimage) 
  {XDestroyImage(spinimage); 
   spinimage=NULL;
  }
   /* clear out the play area and create the new image */
 XFillRectangle(display,window,gc,0,0,windowwidth,windowheight);
 spinimage=XGetImage((Display *) display, (Drawable) window,
           PLAYLEFT,PLAYTOP,block*ncols,block*nrows,
           AllPlanes,ZPixmap);
 if (NULL==spinimage)
       {fprintf(stderr,"trouble creating image structure\n");
        exit(-1);
       } 
    /* make special cursors to be cute */
 cursor=XCreateFontCursor(display,XC_hand2);
 XDefineCursor(display,pausebutton,cursor);
 XDefineCursor(display,quitbutton,cursor);
 XDefineCursor(display,srbutton,cursor);
 XDefineCursor(display,blockbutton,cursor);
 XDefineCursor(display,heatwindow,cursor);
 XDefineCursor(display,magwindow,cursor);
 XDefineCursor(display,qbutton,cursor); 
 XDefineCursor(display,blockbutton,cursor);
 XDefineCursor(display,speedbutton,cursor);
   /* reallocate field array */
 if (NULL!=field)  free((char *) field);
   if (NULL==( field= (spin *) malloc(sizeof(spin)*volume)))
     {fprintf(stderr,"allocation problems\n");
      cleanup();
     } 
   /* set initial state random */
 for (i=0;i<volume;i++)
   field[i]=lrand48()%q;
   /* set blockshift to log_2 of block */
 blockshift=0;
 i=block;
 while(i>>=1)
  blockshift++; 
  /* draw everything */
 repaint();
 return;
}

void cleanup()
{XUnloadFont(display,font->fid);
 XFreeGC(display,gc); 
 XCloseDisplay(display);
 XDestroyImage(spinimage);
 if (NULL!=field) free((char *) field);
 exit(1);
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
   centered.  Color of button determined by sign of "state." 
   size of border determined by magnitude. */
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
 j=abs(state);
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
 XSetForeground(display,gc,cdark);
 return;
}

/* from here on is the stuff for saving and restoring from a gif file */
/* for info on how gif works, see ftp://network.ucsd.edu/graphics/GIF.shar.Z */

char *picturename="xpotts.gif";
void compress(),decompress(),inittable();
int findcode();

void loadpic(data,xsize,ysize) /* load in a GIF image          */
unsigned char *data;      /* where the output data starts */
int xsize,ysize;          /* output picture dimensions    */
/* load in a gif image */
{int i,j,filesize,gwidth,gheight,gvolume;
 unsigned char *ptr, *ptr1, *rawgif;
 int colorbits,codesize;
 FILE *infile;
 if (NULL==(infile=fopen(picturename,"r")))
   {fprintf(stderr,"couldn't open input file\n");
    return;
   }
    /* find the file size */
 fseek(infile, 0L, 2);
 filesize = ftell(infile);
 fseek(infile, 0L, 0);
  /* make a place in memory for the file */
 if (NULL==(rawgif= (unsigned char *) malloc(filesize) ))
    {fprintf(stderr, "not enough memory to read gif file\n");
     return;
    }
 ptr=rawgif;
  /* read in the file */ 
 if (fread(ptr, filesize, 1, infile) != 1)
    {fprintf(stderr, "read failed\n");
     free((char *) rawgif);
     return;
    }
 fclose(infile);
  /* check for GIF signature */
 if (strncmp((char *) ptr,"GIF87a", 6))
     {fprintf(stderr, "not a GIF87a file\n");
      free((char *) rawgif);
      return;
     }
 ptr+=6;
 ptr+=4;                    /* skip over screen size  */
 colorbits=1+((*ptr)&0xf);  /* how many bits of color */
 ptr+=2;                    /* skip over background   */ 
 if (*ptr++)                /* should be zero         */
   {fprintf(stderr, "corrupt GIF file\n");
    free((char *) rawgif);
    return;
   }
 ptr+=(3*(1<<colorbits));   /* skip over colormap     */
 if (','!=(*ptr++))         /* image separator        */
   {fprintf(stderr, "corrupt GIF file\n");
    free((char *) rawgif);
    return;
   }
 ptr+=4;                    /* skip over image offset */
 gwidth=(*ptr)+0x100*(*(ptr+1));
 ptr+=2;
 gheight=(*ptr)+0x100*(*(ptr+1));
 ptr+=2;
 if (0x80&(*ptr++))           /* skip over local color map */
   ptr+=(3*(1<<colorbits));
      /* should now be at start of data */
 codesize=(*ptr++);
  /* make a place for the decompressed file */
 gvolume=gwidth*gheight;
 ptr1=(unsigned char *) malloc(gvolume);
 decompress(codesize,ptr,ptr1,gvolume);
 free((char *) rawgif);
  /* map picture into data, allowing for different dimensions */ 
 for (j=0;j<ysize;j++)
  {if (j>=gheight) break;
   for (i=0;i<xsize;i++)
    {if (i>=gwidth) break;
     data[i+j*xsize]=ptr1[i+j*gwidth];
    }
  }
 free((char *) ptr1);
 return;
}

void savepic(data,xsize,ysize)  /* save the field as a GIF image */
unsigned char *data;       /* where the input data starts   */
int xsize,ysize;           /* picture dimensions            */
{int i;
 int colorbits=5,codesize=5; /* assume a 32 color image */
 FILE *outfile;
 if (NULL==(outfile=fopen(picturename,"w")))
   {fprintf(stderr,"couldn't open output file\n");
    return;
   }
  /* GIF signature */
 fwrite("GIF87a",6,1,outfile);
  /* screen descriptor */
 stringbuffer[0]=xsize&0xff;   /* screen width  */
 stringbuffer[1]=(xsize>>8)&0xff;
 stringbuffer[2]=ysize&0xff;   /* screen height */
 stringbuffer[3]=(ysize>>8)&0xff;
 stringbuffer[4]=(0x80)             /* M=1; global color map follows */
                |((colorbits-1)<<4) /* -1+ bits of color reslution   */
                |(colorbits-1);     /* -1+bits per pixel in image    */
 stringbuffer[5]=0; /* background color */
 stringbuffer[6]=0; /* should be zero   */
 fwrite(stringbuffer,7,1,outfile);
  /* global color map */
 for (i=0;i<(1<<colorbits);i++)
  {colorcell.pixel=translate[i];
   XQueryColor(display,cmap,&colorcell);
   fputc(colorcell.red,outfile);
   fputc(colorcell.green,outfile);
   fputc(colorcell.blue,outfile);
  }
  /* image descriptor */
 stringbuffer[0]=','; /* image descriptor separator */
 stringbuffer[1]=0;   /* image offset               */
 stringbuffer[2]=0;
 stringbuffer[3]=0;
 stringbuffer[4]=0;
 stringbuffer[5]=xsize&0xff;         /* image width  */
 stringbuffer[6]=(xsize>>8)&0xff;
 stringbuffer[7]=ysize&0xff;         /* image height */
 stringbuffer[8]=(ysize>>8)&0xff;
 stringbuffer[9]=0; /* use global color map, no interlace  */ 
 fwrite(stringbuffer,10,1,outfile);
    /* start of image data */
 fputc(codesize,outfile); 
 compress(codesize,data,outfile,volume); 
    /* gif terminator */
 fputc(';',outfile);
 fclose(outfile);
 return;
}

/* LZW compression */
/* hash function assumes TABLELENGTH is a power of 2 */
# define TABLELENGTH (1<<13)
char **addresses=NULL;          /* where to find the string         */
int  *codes=NULL,               /* the code value                   */
     *linktonext=NULL,          /* the next index in the hash chain */
     *lengths=NULL,             /* the length of the coded string   */
     *codeindex=NULL;           /* the index for a given code       */ 
int nextcode;                   /* the next unused code             */

/* hashit is supposed to give a unique fairly random number in the table for
each length a and string b */

# define hashit(a,b) (51*a+53*(57*b[0]+59*(61*b[a-1]+b[a>>1])))&(TABLELENGTH-1)

void compress(initcodesize,ptr,outfile,size)
int initcodesize; /* the initial compression bits */
char * ptr;       /* where the data comes from    */
FILE * outfile;   /* where the output goes        */
int size;         /* how much data                */
{int currentcode=0,prefixcode=0,codesize,maxbits=12,maxcode;
 int clearcode,eoicode,currentplace=0,length,blocksize=0,bitoffset;
 int findcode();
 unsigned long outputword;
 unsigned char blockbuffer[256]; /* to hold data blocks before writing */
  /* allocate space for hash tables */
 if (NULL==(codes=(int *) malloc(sizeof(int)*TABLELENGTH)))
  {fprintf(stderr,"compress: trouble allocating tables\n");
   currentplace=size;
  }
 if (NULL==(linktonext=(int *) malloc(sizeof(int)*TABLELENGTH)))
  {fprintf(stderr,"compress: trouble allocating tables\n");
   currentplace=size;
  }
 if (NULL==(lengths=(int *) malloc(sizeof(int)*TABLELENGTH)))
  {fprintf(stderr,"compress: trouble allocating tables\n");
   currentplace=size;
  }
   /* need one extra place in codeindex for overflow before resetting: */
 if (NULL==(codeindex=(int *) malloc(sizeof(int)*4097)))
  {fprintf(stderr,"compress: trouble allocating tables\n");
   currentplace=size;
  }
 if (NULL==(addresses=(char **) malloc(sizeof(char *)*TABLELENGTH)))
  {fprintf(stderr,"compress: trouble allocating tables\n");
   currentplace=size;
  }
   /* set up initial code table */
 inittable(initcodesize);
 clearcode=(1<<initcodesize);
 eoicode=clearcode+1;
 codesize=initcodesize+1;
 maxcode=1<<codesize;
 nextcode=eoicode+1;
  /* start with a clear code */
 outputword=clearcode;
 bitoffset=codesize; 
   /* now do the compressing */
 while (currentplace<size)
  {   /* check if codesize needs increasing */
   if (nextcode>maxcode)
    {codesize++;
     maxcode=1<<codesize;
     /* if too big, then reset compressor */
     if (codesize>maxbits)
      {if (bitoffset) outputword|=(clearcode<<bitoffset);
       else outputword=clearcode;
       bitoffset+=maxbits;
       inittable(initcodesize);
       codesize=initcodesize+1;
       maxcode=1<<codesize;
       nextcode=eoicode+1;
      }
    }
    /* look for an unstored string */
   length=1;
   LZW (nextcode>
            (currentcode=findcode(length,(char *)(ptr+currentplace))))
    {prefixcode=currentcode;
     length++;
     if ((currentplace+length)>=size) break;
    }
   nextcode++;
   currentplace+=(length-1);
    /* output the prefix code */
   if (bitoffset)  outputword|=(prefixcode<<bitoffset);
   else outputword=prefixcode;
   bitoffset+=codesize;
    /* output finished bytes to blocks */
   while (bitoffset>=8)
     {blockbuffer[blocksize]=outputword&0xff;
      outputword>>=8;
      bitoffset-=8;
      blocksize++;
        /* output filled block */
      if (blocksize>=254)
        {fputc((char) blocksize, outfile);
         fwrite(blockbuffer,blocksize,1,outfile);
         blocksize=0;
        }
     }
  }
   /* output the end of information code */
 if (bitoffset) outputword|=(eoicode<<bitoffset);
 else outputword=eoicode;
 bitoffset+=codesize;
   /* finish outputting the data */
 while (bitoffset>=0) 
     {blockbuffer[blocksize]=(char) (outputword&0xff);
      outputword>>=8;
      bitoffset-=8;
      blocksize++;
      if (blocksize>=254)
        {fputc((char) blocksize, outfile);
         fwrite(blockbuffer,blocksize,1,outfile);
         blocksize=0;
        }
     }
   /* output the last block */
 if (blocksize) 
   {fputc((char) blocksize, outfile);
    fwrite(blockbuffer,blocksize,1,outfile);
   }
  /* a final zero block count */
 fputc(0, outfile);
  /* deallocate tables */
 if (NULL!=codes) free((char *) codes);
 if (NULL!=linktonext) free((char *) linktonext);
 if (NULL!=lengths) free((char *) lengths);
 if (NULL!=codeindex) free((char *) codeindex);
 if (NULL!=addresses) free((char *) addresses);
 codes=linktonext=lengths=codeindex=NULL;
 addresses=(char **) NULL;
 return;
}

void decompress(initcodesize,ptr,ptr1,size)
int initcodesize;
unsigned char *ptr, *ptr1; /* compressed data from ptr go to ptr1 */
int size;                  /* an upper limit purely as a check    */
{int i,currentcode,codesize=0,maxbits=12,blocksize;
 int clearcode,eoicode,codemask=0;
 int bitoffset=0,indx,oldindx=0;
 int currentplace=0,oldplace=0;
 int findcode();
 unsigned long inputword=0;
 unsigned char *p1, *p2;
   /* first deblock the data */
 p1=p2=ptr;
 blocksize=(*p1++);
 while (blocksize)
  {while (blocksize--)
    (*p2++)=(*p1++); /* a wonderful example of how abstruse C can be */
   blocksize=(*p1++);
  }
   /* set up initial code table */
 currentcode=clearcode=(1<<initcodesize);
 eoicode=clearcode+1;
  /* allocate space for hash table */
 if (NULL==(codes=(int *) malloc(sizeof(int)*TABLELENGTH)))
  {fprintf(stderr,"decompress: trouble allocating tables\n");
   currentcode=eoicode;
  }
 if (NULL==(linktonext=(int *) malloc(sizeof(int)*TABLELENGTH)))
  {fprintf(stderr,"decompress: trouble allocating tables\n");
   currentcode=eoicode;
  }
 if (NULL==(lengths=(int *) malloc(sizeof(int)*TABLELENGTH)))
  {fprintf(stderr,"decompress: trouble allocating tables\n");
   currentcode=eoicode;
  }
   /* need one extra place in codeindex for overflow before resetting: */
 if (NULL==(codeindex=(int *) malloc(sizeof(int)*4097)))
  {fprintf(stderr,"compress: trouble allocating tables\n");
   currentcode=eoicode;
  }
 if (NULL==(addresses=(char **) malloc(sizeof(char*)*TABLELENGTH)))
  {fprintf(stderr,"decompress: trouble allocating tables\n");
   currentplace=eoicode;
  }
 while (currentcode!=eoicode)
  {if (currentcode==clearcode) /* reset the decompressor */
      {inittable(initcodesize);
       codesize=initcodesize+1;
       nextcode=eoicode+1;
       codemask=(1<<codesize)-1;
       oldindx=(-1);
      }
   else /* code represents data */
    {indx=codeindex[currentcode]; /* where in table is currentcode */
     if (indx>=0) /* it is there */
      { /* put it into the output */
       for (i=0;i<lengths[indx];i++)
         ptr1[currentplace+i]=addresses[indx][i];
       if (oldindx>=0) /* first character treated differently */
        {findcode(lengths[oldindx]+1,(char *) (ptr1+oldplace));
         nextcode++;  /* add new code to table */
        }
       oldplace=currentplace;
       currentplace+=lengths[indx];
       oldindx=indx;
      }
     else /* not in table yet; must be old code plus last=first character */
      {for (i=0;i<lengths[oldindx];i++)
         ptr1[currentplace+i]=addresses[oldindx][i];
       ptr1[currentplace+lengths[oldindx]]=addresses[oldindx][0];
              /* store new code */
       findcode(lengths[oldindx]+1,(char *)(ptr1+currentplace)); 
       oldplace=currentplace;
       currentplace+=lengths[oldindx]+1;
       oldindx=codeindex[nextcode++];
      }
      /* crude error checking */
     if ((oldindx<0)||(currentplace>size))
      {fprintf(stderr,"gif file appears to be corrupt\n");
       break;
      }
    } 
     /* check if codesize needs increasing */     
   if (nextcode>codemask)
      if (codesize<maxbits)
        {codesize++;
         codemask=(1<<codesize)-1;
        }
   while (bitoffset<codesize) /* read some more data */
    {if (bitoffset) inputword|=(((int)(*ptr++))<<bitoffset);
     else inputword=(*ptr++);
     bitoffset+=8;
    } 
    /* strip off current code */
   currentcode=inputword&codemask;
   inputword>>=codesize;
   bitoffset-=codesize;
   if (currentcode>nextcode) 
      {fprintf(stderr,"gif file appears to be corrupt\n");
       break;
      }
  }
  /* deallocate tables */
 if (NULL!=codes) free((char *) codes);
 if (NULL!=linktonext) free((char *) linktonext);
 if (NULL!=lengths) free((char *) lengths);
 if (NULL!=codeindex) free((char *) codeindex);
 if (NULL!=addresses) free((char *) addresses);
 codes=linktonext=lengths=codeindex=NULL;
 addresses=(char **) NULL;
 return;
}

void inittable(size)
int size;
{int i;
 for (i=0;i<TABLELENGTH;i++)
  {linktonext[i]=(-1);
   codes[i]=(-1);
   lengths[i]=(-1);
  }
 for (i=0;i<4096;i++)
   codeindex[i]=(-1);
  /* store initial codes for raw characters */
 nextcode=0;
 for (i=0;i<(1<<size);i++)
  {stringbuffer[i]=i;
   findcode(1,(char *) (stringbuffer+i));
   nextcode++;
  }
 return;
}

int findcode(length,string)
char *string;
int length;
 /* return code for string of given length;  
    if not found, store it and return nextcode */
{int i,j,indx,previousindex;
 indx=hashit(length,string); 
  /* look for string in table */
 previousindex=indx;
 while (indx>0)
  {if (lengths[indx]==length) /* is the length right ? */
    {for (j=0;j<length;j++)
      if ((string[j]) != (addresses[indx][j])) break;
     if (j==length) return codes[indx]; 
    }
   previousindex=indx;
   indx=linktonext[indx];
  } 
  /* not found, so store it */
 indx=previousindex;
 i=indx;
 while (codes[i]>=0) /* find an unused slot in table */
  {i++;
   if (i>=TABLELENGTH) i-=TABLELENGTH;
  }
 if (i!=indx) 
     {linktonext[indx]=i;  /* link to it */
      indx=i;              /* move to it */
     }
 codes[indx]=nextcode; /* save the new code */
 lengths[indx]=length;
 addresses[indx]=string;
 codeindex[nextcode]=indx; 
 return nextcode;
}  

