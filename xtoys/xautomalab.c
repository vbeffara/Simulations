 /*  X-automalab, a totalistic cellular automaton simulator for X windows.
           
      Michael Creutz   
  creutz@bnl.gov 

 compiling:  cc -O -o xautomalab -L/usr/X11R6/lib xautomalab.c -lX11 

 version 2.0 
 24 Nov 1995 
 LZW disabled, 10 October 1998
 minor modifications to eliminate gcc -Wall warnings, March 1999
 left-right error depending on bit order fixed, June 1999

 This program is still evolving; the latest release can be found via

   http://thy.phy.bnl.gov/www/xtoys/xtoys.html

  version history
 1.1 adds save/restore as a gif file
 1.2 allows window resizing 
 1.3 allows block selection while running and improves picture loading
 1.4 supports monochrome 
 1.5 uses my own gif routines
 1.6 handles unusual screen depths and comments things better 
 1.7 prettier buttons and colors 
 1.8 fixed a color table error in the gif save routine
 2.0 adds speed gadget (it seemed too fast on an alpha)   
 */

/* because of the unisys patent on LZW, it may be illegal to
   distribute this program with the following "if" changed to "while" */
#define LZW if

# include <X11/Xlib.h>
# include <X11/Xutil.h>
# include <X11/Xatom.h> 
# include <X11/cursorfont.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <malloc.h>
# include <select.h>

# define BPW (sizeof(long))

/* size and position parameters for buttons, etc. */
# define PLAYTOP 130
# define PLAYLEFT 10
# define BOUNDHEIGHT 72
# define TRACEHEIGHT 36
# define BOUNDWIDTH  112
# define BARHEIGHT 45
# define BARWIDTH  120
# define BARTOP 2
# define BARLEFT 2
# define XHEIGHT 36

int nrows,ncols,volume; /* lattice dimensions (plus two for boundaries) */
     /* ncols will be truncated to 
        a multiple of bytes per long word (BPW),
        and then two units are lost for boundaries    */
int block,           /* size of cells displayed on the screen          */
    blockshift;      /* log_2(block)                                   */
unsigned char *field[2]={NULL,NULL}; /* pointers to old and new system */
int old=0,new=1;                 /* which field is current?            */
int table[2][256];               /* for the rule table                 */
int nbrhd[3][3];                 /* to tag active neighbors            */
int birth[9],survivor[9];        /* when are there births or survivors */
  /* things for regulating the updating speed */
int speed=0; /* updating delay proportional to speed   */
int delay=0; /* counter for implementing speed control */
struct timeval timeout; /* timer for speed control */

int paused=0,        /* is the system paused?                  */
    boundary,        /* 0,1,2,3 for open, live, periodic, flow */
    pencolor=1,      /* for sketching                          */
    trace,           /* is the tracer on?                      */
    xpast,           /* are we to xor with the past?           */
    neighbors,       /* how many live neighbors                */
    colshift;        /* how to shift to get the next column    */
long mask0x13,mask0x01,mask0x03,mask0x10; /* some useful masks */
char stringbuffer[256];              /* generally useful       */
unsigned char *work=NULL;            /* points to a work array */

/* various window stuff */
Display *display;
int screen;
static char *progname;
Window window,quitbutton,pausebutton,playground,boundwindow,rulebar,
       fillbutton,tracewindow,xpastbutton,nhood,srbutton,blockbutton,
       speedbutton,makebutton();
XColor xcolor,colorcell;
Colormap cmap;
GC gc,gcpen;
int windowwidth,windowheight;
XFontStruct *font=NULL;
int font_height,font_width;
XSizeHints size_hints;
int darkcolor,lightcolor,black,white;
XImage *spinimage=NULL;
long translate[256];                /* for converting colors   */
char * boundtext[4]={"open","live","periodic","flow"};

void settable(),openwindow(),makebuttons(),update(),repaint(),cleanup(),
     fixboundary(),showpic(),drawbutton(),setboundary(),
     settrace(),setrule(),sketch(),setnbrhd(),savepic(),loadpic(),
     setxpast(),putneighbors();

int main(argc,argv)
int argc;
char **argv;
{unsigned int width, height;
 int i,j,x,y;
 long bitcheck;
 XEvent report;
 progname=argv[0];

  /* initial lattice size */
 block=1;
 nrows=200/block;
 ncols=(~(BPW-1))&(200/block);
 volume=nrows*ncols;

 /* initial rule */
 for (i=0;i<3;i++)
  for (j=0;j<3;j++)
   nbrhd[i][j]=1;
 nbrhd[0][0]=nbrhd[2][1]=0;
 trace=1;
 xpast=0;
 boundary=1;
 for (i=0;i<9;i++)
   birth[i]=survivor[i]=0;
 birth[2]=birth[5]=birth[6]=1;
 survivor[4]=survivor[5]=1;
 settable();
 openwindow(argc,argv);
 makebuttons();

  /* set the masks for parallel updating */
 for (i=0;i<sizeof(long);i++) 
  {mask0x13=0x13|(mask0x13<<8);
   mask0x01=0x01|(mask0x01<<8);
   mask0x03=0x03|(mask0x03<<8);
   mask0x10=0x10|(mask0x10<<8);
  }
   /* check for byte order in a word */
 bitcheck=1;
 if (* (char*) (&bitcheck) )
   colshift=(-1); /* LSBfirst */
 else 
   colshift=1;    /* MSBfirst */

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
             nrows=(height-PLAYTOP-14)/block;
             ncols=(~(BPW-1))&((width-PLAYLEFT-BOUNDWIDTH-20)/block);
             volume=nrows*ncols;
             makebuttons();
             fixboundary();
             showpic();
            } 
        break; 
       case ButtonPress:
        if (report.xbutton.window==quitbutton)
             cleanup();
        else if (report.xbutton.window==pausebutton)
            {paused=1-paused;
             drawbutton(pausebutton,0,0,68,18,"pause",1-2*paused);
            } 
        else if (report.xbutton.window==fillbutton)
            {drawbutton(fillbutton,0,0, 68,18,"clear",-1);
             for (i=0;i<volume;i++)
               field[old][i]=0;
             fixboundary();
             showpic();
             drawbutton(fillbutton,0,0, 68,18,"clear",1);
            }
        else if (report.xbutton.window==boundwindow)
            {setboundary((report.xbutton.y*4)/BOUNDHEIGHT);
             showpic();
            }
        else if (report.xbutton.window==tracewindow)
             settrace((report.xbutton.y*2)/TRACEHEIGHT);
        else if (report.xbutton.window==rulebar)
             setrule((report.xbutton.x*(neighbors+1))/BARWIDTH+
                   ((report.xbutton.y*3)/BARHEIGHT)*10);
        else if (report.xbutton.window==playground) /* sketch */
            {x=report.xbutton.x/block;
             y=report.xbutton.y/block;
             sketch(x,y);
            }
        else if (report.xbutton.window==xpastbutton) 
             setxpast((report.xbutton.y*2)/XHEIGHT);
        else if (report.xbutton.window==nhood) 
            {x=(3*report.xbutton.x)/BARHEIGHT;
             y=(3*report.xbutton.y)/BARHEIGHT;
             setnbrhd(x,y);
            }
        else if (report.xbutton.window==srbutton)
            {if (2*report.xbutton.y<XHEIGHT)
               {drawbutton(srbutton,0,0,68,18,"save",-1);
                savepic(field[old],ncols,nrows);
                drawbutton(srbutton,0,0,68,18,"save",1);
	       }
             else
               {drawbutton(srbutton,0,XHEIGHT/2,68,18,"restore",-1);
                loadpic(field[old],ncols,nrows);
                drawbutton(srbutton,0,XHEIGHT/2,68,18,"restore",1);
	       }
            }
        else if (report.xbutton.window==blockbutton)
          {i=(1<<(4*report.xbutton.x/BOUNDWIDTH));
           if (i!=block) /* reset for new block size */
             {block=i;
              nrows=(windowheight-PLAYTOP-10)/block;
              ncols=(~(BPW-1))&((windowwidth-PLAYLEFT-BOUNDWIDTH-20)/block);
              volume=nrows*ncols;
                 /* clear old window */
              XSetForeground(display,gcpen,lightcolor);  
              XFillRectangle(display,window,gcpen,0,0,
                             windowwidth,windowheight);
              makebuttons();
              fixboundary();
              showpic();
             }
          }
        else if (report.xbutton.window==speedbutton)
          {  /* reset speed */
           speed=10-(11*report.xbutton.x)/BOUNDWIDTH;
           if (speed<0) speed=0;
           if (speed>10) speed=10;
           delay=speed;
           drawbutton(speedbutton,0,0,BOUNDWIDTH,18,"speed",-1);
           drawbutton(speedbutton,1+((10-speed)*(BOUNDWIDTH-2))/11,1,
                      (BOUNDWIDTH-2)/11,16,"",2);
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

void settable()  /* set up the rule table from birth and survivor arrays */
/* The use of a table in allows much more general rules
   with no loss in speed over these totalistic ones.
   Any ideas as to how to add easily selected further rules? */
{int i,j,bits,mask;
  /* mask out the unused neighbors */
 if (colshift>0) /* correct for byte order */
 mask=nbrhd[0][1]
     |nbrhd[2][1]<<1
     |nbrhd[1][2]<<2
     |nbrhd[1][0]<<3
     |nbrhd[0][2]<<4
     |nbrhd[2][2]<<5
     |nbrhd[0][0]<<6
     |nbrhd[2][0]<<7;
 else
 mask=nbrhd[2][1]
     |nbrhd[0][1]<<1
     |nbrhd[1][2]<<2
     |nbrhd[1][0]<<3
     |nbrhd[2][2]<<4
     |nbrhd[0][2]<<5
     |nbrhd[2][0]<<6
     |nbrhd[0][0]<<7;
 for (i=0;i<256;i++)
  {bits=0;  /* count the set bits in i */
   j=i&mask;
   while (j) 
    {bits+=(1&j);
     j>>=1;
    }
   table[0][i]=birth[bits];
   table[1][i]=survivor[bits];
  }
 return;
}

void sketch(x,y)   /* sketch in playground starting at point x,y */
int x,y;
{int sketching,i,oldx,oldy,deltax,deltay,newx,newy;
 Window root,child;
 XEvent sketchreport;
 unsigned int keys_buttons;
 int window_x,window_y;
 newx=x;
 newy=y;
 field[old][x+ncols*y]^=pencolor;
 sketching=1;
 while (sketching)
  {showpic();
   XNextEvent(display,&sketchreport); 
   switch (sketchreport.type)
    {case MotionNotify: 
      oldx=newx;
      oldy=newy;
      if (!XQueryPointer(display,playground,
        &root,&child,&window_x,&window_y,
        &newx,&newy,&keys_buttons))
          {sketching=0;
           break;
          }
      if (blockshift)
        {newx>>=blockshift;
         newy>>=blockshift;
        }
      if ((newx>ncols)|(newy>nrows)|(newx<0)|(newy<0))
        {sketching=0;
         break;
        }
      deltax=abs(newx-oldx);
      deltay=abs(newy-oldy);
      if (deltax>deltay)
         for (i=1;i<=deltax;i++)
          field [old][oldx+i*(newx-oldx)/deltax
                  +ncols*(oldy+i*(newy-oldy)/deltax)]^=pencolor;
      else 
         for (i=1;i<=deltay;i++)
          field [old][oldx+i*(newx-oldx)/deltay
                  +ncols*(oldy+i*(newy-oldy)/deltay)]^=pencolor;
      break;
     case ButtonRelease:
     default:
      sketching=0;
      break;
    } 
  }
 fixboundary();
 showpic();
 return;
}

void update()
{int i,topi=volume-ncols;
 putneighbors((long *) field[old],(long *) field[new]); 
 for (i=ncols;i<topi;i++) /* bottom and top will be fixed by boundary */
   field[new][i]^=table[field[old][i]][work[i]]; 
 old=new;
 new=1-new;
 fixboundary();
 showpic(); 
 return;
}

void putneighbors(source,destination)
/* initializes the new field and puts neighbors in work array */
/* this updating is done BPW spins at a time using multi-spin coding */
long *source, *destination;
{int i,rowshift=ncols/BPW,topi,cprshift,cmrshift;
 unsigned long * temp;
 temp=(unsigned long*) work;
 topi=(volume/BPW); 
  /* depending on xpast and trace, set up the destination */
 if (xpast&trace)
  for (i=0;i<topi;i++)
   {destination[i]=(source[i]    &mask0x10)   /* keep old trace bit      */
                 |((source[i]>>1)&mask0x01)   /* previous spin for xpast */
                 |((source[i]<<1)&mask0x03)   /* old state to second bit */
                 |((source[i]<<3)&mask0x10);  /* new trace bit           */
    source[i]&=mask0x01;  /* clean out the source */
   }
 else if (xpast) /* include previous state into destination */
  for (i=0;i<topi;i++)
   {destination[i]=(source[i]    &mask0x10)   /* keep trace bit          */
                 |((source[i]>>1)&mask0x01)   /* previous spin for xpast */
                 |((source[i]<<1)&mask0x03);  /* old state to second bit */
    source[i]&=mask0x01;  /* clean out the source */
   }
 else if (trace)
  for (i=0;i<topi;i++)
   {destination[i]=(source[i]    &mask0x10)   /* keep old trace bit      */
                 |((source[i]<<1)&mask0x03)   /* old state to second bit */
                 |((source[i]<<3)&mask0x10);  /* new trace bit           */
    source[i]&=mask0x01;  /* clean out the source */
   }
 else /* neither trace nor xpast */
  for (i=0;i<topi;i++)
   {destination[i]=(source[i]    &mask0x10)   /* keep old trace bit      */
                 |((source[i]<<1)&mask0x03);  /* old state to second bit */
    source[i]&=mask0x01;  /* clean out the source */
   }

  /* now put the neighbors into the work array */
 topi=(volume/BPW)-rowshift; 
 cprshift=colshift+rowshift;
 cmrshift=colshift-rowshift;
 for (i=rowshift;i<topi;i++)
  {temp[i]=(source[i         ]>>8)            
          |(source[i-colshift]<<(8*(BPW-1)))   /* W neighbor  */
          |(source[i         ]<<9)  
          |(source[i+colshift]>>(8*(BPW-1)-1)) /* E neighbor  */
          |(source[i+rowshift]<<2 )            /* S neighbor  */
          |(source[i-rowshift]<<3 )            /* N neighbor  */
          |(source[i+rowshift]>>4)
          |(source[i-cmrshift]<<(8*(BPW-1)+4)) /* SW neighbor */
          |(source[i+rowshift]<<13)         
          |(source[i+cprshift]>>(8*(BPW-1)-5)) /* SE neighbor */
          |(source[i-rowshift]>>2)
          |(source[i-cprshift]<<(8*(BPW-1)+6)) /* NW neighbor */
          |(source[i-rowshift]<<15)
          |(source[i+cmrshift]>>(8*(BPW-1)-7));/* NE neighbor */ 
  }   
} 

void showpic() /* display the field */
{int row,col,i1,i2,color,j,j1,j2,blocktop=block;
 char *picture=(*spinimage).data;
 if (block>4) blocktop=block-1;
 if (8==(*spinimage).depth) 
  {if (block>1) /* I wish I knew how to do this faster */
     for (row=0;row<nrows;row++)
      for (col=0;col<ncols;col++)
       {color=translate[field[old][row*ncols+col]];
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
      picture[j]=translate[field[old][j]];
    }
  }
 else /* depth is not 8, use xputpixel (this is really ugly) */
  {if (block>1) /* I wish I knew how to do this faster */
    for (row=0;row<nrows;row++)
     for (col=0;col<ncols;col++)
      {color=translate[field[old][row*ncols+col]];
       if (color!=XGetPixel(spinimage,j1=block*col,j2=block*row))
         for (i2=0;i2<blocktop;i2++)
           for (i1=0;i1<blocktop;i1++)
            XPutPixel(spinimage,j1+i1,j2+i2,color);
      }
   else
    for (row=0;row<nrows;row++)
     {j1=row*ncols; 
      for (col=0;col<ncols;col++)
        XPutPixel(spinimage,col,row,translate[field[old][j1+col]]);
     }
  }
 XPutImage(display,playground,gc,spinimage,0,0,0,0,block*ncols,block*nrows); 
 return;
}

void fixboundary()
{int i,bv;
 if (boundary<2) /* dead or alive */
  {bv=pencolor*boundary;
   for (i=0;i<ncols;i++)
    {field[old][i]=bv;
     field[old][volume-1-i]=bv;
    }
   for (i=0;i<volume;i+=ncols)
    {field[old][i]=bv;
     field[old][i+ncols-1]=bv;
    }
  }
 else if (2==boundary) /* periodic */
  {for (i=0;i<ncols;i++)
    {field[old][i]=field[old][volume-2*ncols+i];
     field[old][volume-ncols+i]=field[old][ncols+i];
    }
   for (i=0;i<volume;i+=ncols)
    {field[old][i]=field[old][i+ncols-2];
     field[old][i+ncols-1]=field[old][i+1];
    }
  }
 else /* flow: alive on top, dead on sides and bottom */
  {for (i=0;i<ncols;i++)
    {field[old][i]=pencolor;
     field[old][volume-1-i]=0;
    }
   for (i=0;i<volume;i+=ncols)
    {field[old][i]=0;
     field[old][i+ncols-1]=0;
    }
  }
 return;
} 

void repaint()
   /* this fixes the window up whenever it is uncovered */
{int i;
 drawbutton(quitbutton,0,0, 68,18,"quit",1);
 drawbutton(fillbutton,0,0, 68,18,"clear",1);
 drawbutton(srbutton,  0,0, 68,18,"save",1);
 drawbutton(srbutton,  0,18,68,18,"restore",1);
 drawbutton(pausebutton,0,0,68,18,"pause",1-2*paused);
 drawbutton(speedbutton,0,0,BOUNDWIDTH,18,"speed",-1);
 drawbutton(speedbutton,1+((10-speed)*(BOUNDWIDTH-2))/11,1,
               (BOUNDWIDTH-2)/11,16,"",2);
 drawbutton(window,PLAYLEFT-4,PLAYTOP-4,block*ncols+8,block*nrows+8,NULL,4);
 setxpast(-1);
 setnbrhd(-1,-1);

  /* fix the block buttons */
 for (i=1;i<=4;i++)
  {
   sprintf(stringbuffer,"%d",(1<<i)/2);
   drawbutton(blockbutton,(i-1)*BOUNDWIDTH/4,0,
         BOUNDWIDTH/4,18,
         stringbuffer,1-2*(i==(blockshift+1)));
  }

  /* draw the boundary condition buttons */
 for (i=0;i<4;i++)
  drawbutton(boundwindow,0,i*BOUNDHEIGHT/4,BOUNDWIDTH,BOUNDHEIGHT/4,
     boundtext[i],1-2*(i==boundary));

  /* draw the trace buttons */
 drawbutton(tracewindow,0,0,BOUNDWIDTH,TRACEHEIGHT/2,
        "tracer",1-2*trace);
 drawbutton(tracewindow,0,TRACEHEIGHT/2,BOUNDWIDTH,TRACEHEIGHT/2,
        "clear trace",1);

          /* write various strings */
 XDrawString(display,window,gc,BARWIDTH+BARLEFT+2,BARTOP+font_height-2,
        "births",6);
 XDrawString(display,window,gc,BARWIDTH+BARLEFT+2,BARTOP+BARHEIGHT-2,
        "survivors",9);
 XDrawString(display,window,gc,70,BARHEIGHT+BARTOP+2*font_height,
        "neighborhood",12);
 sprintf(stringbuffer,"%d by %d lattice    ",ncols-2,nrows-2);
 XDrawImageString(display,window,gc,5,BARHEIGHT+BARTOP+5*font_height,
               stringbuffer,strlen(stringbuffer));
 if ((*spinimage).depth>1)
  {XSetForeground(display,gcpen,translate[1]);
   XDrawString(display,window,gcpen,5,BARTOP+BARHEIGHT+4*font_height,
         "young",5); 
   XSetForeground(display,gcpen,translate[3]);
   XDrawString(display,window,gcpen,60,BARTOP+BARHEIGHT+4*font_height,
         "old",3); 
   XSetForeground(display,gcpen,translate[2]);
   XDrawString(display,window,gcpen,98,BARTOP+BARHEIGHT+4*font_height,
         "moribund",8); 
  }
 XDrawString(display,window,gc,windowwidth-BOUNDWIDTH,BOUNDHEIGHT+font_height,
               "  Boundary  ",12);
 XDrawString(display,window,gc,
        windowwidth-BOUNDWIDTH+12,BOUNDHEIGHT+TRACEHEIGHT+XHEIGHT+76,
        "cell size",9);
 XDrawString(display,window,gc,windowwidth-50,windowheight-5,"MJC",3); 
 showpic();  
 return;
}
 
void setboundary(value)
/* take action if boundary buttons hit */
int value;
{drawbutton(boundwindow,0,(BOUNDHEIGHT*boundary)/4,BOUNDWIDTH,BOUNDHEIGHT/4,
                       boundtext[boundary],1);
 boundary=value;
 drawbutton(boundwindow,0,(BOUNDHEIGHT*boundary)/4,BOUNDWIDTH,BOUNDHEIGHT/4,
                       boundtext[boundary],-1);
 fixboundary();
 return;
}
 
void settrace(value)
/* take action if trace buttons hit */
int value;
{int i;
 if (1==value) /* clear the trace bit */
  {drawbutton(tracewindow,0,TRACEHEIGHT/2,BOUNDWIDTH,TRACEHEIGHT/2,
        "clear trace",-1);
   for (i=0;i<volume;i++)
     field[old][i]&=0x3;
   showpic();
   drawbutton(tracewindow,0,TRACEHEIGHT/2,BOUNDWIDTH,TRACEHEIGHT/2,
        "clear trace",1);
  } 
 else 
  {trace=1-trace;
   drawbutton(tracewindow,0,0,BOUNDWIDTH,TRACEHEIGHT/2,
          "tracer",1-2*trace);
  }
 return;
}

void setrule(value)
/* take action if rule buttons hit */
/* call with negative value just to redraw but not change rule */
int value;
{int i;  
 if (value>=0) 
   {if (value<10)
      birth[value]=1-birth[value];
    else if (value>=20)
      survivor[value-20]=1-survivor[value-20];
   }
      /* fix the rulebar */
 XSetForeground(display,gcpen,lightcolor);  
 XFillRectangle(display,rulebar,gcpen,0,0,BARWIDTH,BARHEIGHT);
 for (i=0;i<(neighbors+1);i++)
  {drawbutton(rulebar,i*(BARWIDTH/(neighbors+1)),0,
              BARWIDTH/(neighbors+1),BARHEIGHT/3,NULL,1-2*birth[i]);
   drawbutton(rulebar,i*(BARWIDTH/(neighbors+1)),2*BARHEIGHT/3,
              BARWIDTH/(neighbors+1),BARHEIGHT/3,NULL,1-2*survivor[i]);
   sprintf(stringbuffer,"%d",i);
   XDrawString(display,rulebar,gc,
          18-2*neighbors+BARWIDTH*i/(neighbors+1),2*BARHEIGHT/3-2,
          stringbuffer,1);
  }
 settable();
 return;
}

void setnbrhd(vx,vy)  /* fix the neighborhood buttons */
int vx,vy;
{int i,j;
 neighbors=0;
 if ((vx>=0)&(vy>=0))  /* toggle this neighbor */
  nbrhd[vx][vy]=1-nbrhd[vx][vy];
 nbrhd[1][1]=0;
 for (i=0;i<3;i++)
  for (j=0;j<3;j++)
   {drawbutton(nhood,i*BARHEIGHT/3,j*BARHEIGHT/3,
           BARHEIGHT/3,BARHEIGHT/3,NULL,1-2*nbrhd[i][j]);
    neighbors+=nbrhd[i][j];
   }
 setrule(-1); /* redraw the rule bar */
 return;
}

void setxpast(value)   /* fix up the xpast button */
int value;
{int i;
 if ((1==value)&xpast) /* swap past and present */
  {drawbutton(xpastbutton,0,XHEIGHT/2,BOUNDWIDTH,XHEIGHT/2,"reverse",-1);
   for (i=0;i<volume;i++)
    field[old][i]=((field[old][i]>>1)&1)|((field[old][i]&1)<<1);
   showpic();
   drawbutton(xpastbutton,0,XHEIGHT/2,BOUNDWIDTH,XHEIGHT/2,"reverse",1);
  }
 if ((0==value)|((1==value)&(0==xpast))) xpast=1-xpast;
 /* repaint the button */
 if (xpast) 
  {drawbutton(xpastbutton,0,0,BOUNDWIDTH,XHEIGHT/2,"xor past",-1);
   drawbutton(xpastbutton,0,XHEIGHT/2,BOUNDWIDTH,XHEIGHT/2,"reverse",1);
  }
 else
  {drawbutton(xpastbutton,0,0,BOUNDWIDTH,XHEIGHT/2,"xor past",1);
   drawbutton(xpastbutton,0,XHEIGHT/2,BOUNDWIDTH,XHEIGHT/2,NULL,0);
  }
 return;
}

void openwindow(argc,argv)
/* a lot of this is taken from basicwin in the Xlib Programming Manual */
int argc;
char **argv;
{char *window_name="Automalab";
 char *icon_name="Automalab";
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
 translate[2]=white;
 translate[3]=black;
 for(i=4;i<256;i++)
   translate[i]=translate[i%4]; 
  /* for monochrome, invert black and white when traced */
 translate[8]=black;
 translate[17]=white;
 translate[18]=black;
 translate[19]=white;
if (DefaultDepth(display,screen)>1)
{   /* set the various colors */
 if (XAllocNamedColor(display,cmap,"steel blue",&colorcell,&xcolor))
              darkcolor=colorcell.pixel; /* pressed button color */
 if (XAllocNamedColor(display,cmap,"pale turquoise",&colorcell,&xcolor))
              lightcolor=colorcell.pixel; /* background color */
 if (XAllocNamedColor(display,cmap,"light gray",&colorcell,&xcolor))
              translate[0]=colorcell.pixel; /* dead cells */
 if (XAllocNamedColor(display,cmap,"red",&colorcell,&xcolor))
              translate[1]=colorcell.pixel; /* newborn    */
 if (XAllocNamedColor(display,cmap,"sea green",&colorcell,&xcolor))
              translate[2]=colorcell.pixel; /* moribund   */
 if (XAllocNamedColor(display,cmap,"blue",&colorcell,&xcolor))
              translate[3]=colorcell.pixel; /* living     */
 if (XAllocNamedColor(display,cmap,"orange",&colorcell,&xcolor))
              translate[4]=colorcell.pixel;
 if (XAllocNamedColor(display,cmap,"gold",&colorcell,&xcolor))
              translate[5]=colorcell.pixel;
 if (XAllocNamedColor(display,cmap,"yellow",&colorcell,&xcolor))
              translate[6]=colorcell.pixel;
 if (XAllocNamedColor(display,cmap,"white",&colorcell,&xcolor))
              translate[7]=colorcell.pixel;
 if (XAllocNamedColor(display,cmap,"grey",&colorcell,&xcolor))
              translate[8]=colorcell.pixel; /* tracing color */
  /* fill out the color table for future uses */
 for(i=9;i<256;i++)
   translate[i]=translate[i%8]; 
}
 translate[16]=translate[8]; /* for tracing */

   /* make the main window */
 windowwidth=(block*ncols+PLAYLEFT+BOUNDWIDTH+20);
 windowheight=(PLAYTOP+block*nrows+14);
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

/* pick font: 9x15 is supposed to almost always be there,
     the fixed font option put in by Peter Chang */
 if ((font=XLoadQueryFont(display,"9x15"))==NULL) 
   if ((font=XLoadQueryFont(display,"fixed"))==NULL)
     {fprintf(stderr,"%s: Sorry, having font problems.\n",progname);
      exit(-1);
     }
 font_height=font->ascent+font->descent;
 font_width=font->max_bounds.width;

  /* make graphics contexts: 
      gc for black on background
      gcpen for varying purposes */
   
 gc=XCreateGC(display,window,0,NULL);
 XSetFont(display,gc,font->fid);
 XSetForeground(display,gc,black);
 XSetBackground(display,gc,lightcolor); 
  /* the following might speed things up on some displays? */
 XSetPlaneMask(display,gc,black|white|translate[0]|translate[1]|translate[2]
       |translate[3]|translate[16]|translate[4]);

 gcpen=XCreateGC(display,window,0,NULL); 
 XSetFont(display,gcpen,font->fid);
 XSetForeground(display,gcpen,translate[pencolor]);
 XSetBackground(display,gcpen,lightcolor); 

/* show the window */
 XMapWindow(display,window);
 return;
}

void makebuttons()
/* makes buttons and reallocates everything */
{int i,j;
 long event_mask;
 XEvent report;
 Cursor cursor;
     /* first destroy any old buttons */
 XDestroySubwindows(display,window);
     /* now make the new buttons */
 quitbutton =makebutton(windowwidth-92,windowheight-42 ,68,18);
 pausebutton=makebutton(windowwidth-92,windowheight-60 ,68,18);
 fillbutton =makebutton(windowwidth-92,windowheight-78 ,68,18);
 srbutton   =makebutton(windowwidth-92,windowheight-114,68,36);
 speedbutton=makebutton(windowwidth-BOUNDWIDTH-4,
           BOUNDHEIGHT+TRACEHEIGHT+XHEIGHT+24,
           BOUNDWIDTH,18);
 rulebar    =makebutton(BARLEFT,BARTOP,BARWIDTH,BARHEIGHT);
 nhood      =makebutton(20,BARHEIGHT+BARTOP+4,BARHEIGHT,BARHEIGHT);
 boundwindow=makebutton(windowwidth-BOUNDWIDTH-4,2,
                        BOUNDWIDTH,BOUNDHEIGHT);
 tracewindow=makebutton(windowwidth-BOUNDWIDTH-4,BOUNDHEIGHT+20,
                        BOUNDWIDTH,TRACEHEIGHT);
 xpastbutton=makebutton(windowwidth-BOUNDWIDTH-4,BOUNDHEIGHT+TRACEHEIGHT+22,
                        BOUNDWIDTH,XHEIGHT);
 blockbutton=makebutton(windowwidth-BOUNDWIDTH-4,
            BOUNDHEIGHT+TRACEHEIGHT+XHEIGHT+46,BOUNDWIDTH,18);

 playground=XCreateSimpleWindow(display,window,
    PLAYLEFT,PLAYTOP,block*ncols,block*nrows,0,black,translate[4]);
 event_mask=ExposureMask|ButtonReleaseMask|ButtonPressMask|
            PointerMotionHintMask|ButtonMotionMask; 
 XSelectInput(display,playground,event_mask);
 XMapWindow(display,playground);

   /* wait for playground to be displayed befor proceeding */
 i=1; /* a flag */
 while (i)
    {XNextEvent(display,&report); 
     switch (report.type)
      {case Expose:
          if (report.xexpose.window!=playground) i=0;
       default:
        break;
      }
    }

   /* make image structure, first destroying any old one */
 if (NULL!=spinimage) 
  {XDestroyImage(spinimage); 
   spinimage=NULL;
  }
 spinimage=XGetImage((Display *) display, (Drawable) playground,
           0,0,block*ncols,block*nrows,
           AllPlanes,ZPixmap);
 if (NULL==spinimage)
       {fprintf(stderr,"trouble creating image structure\n");
        exit(-1);
       } 

  /* make special cursors to be cute */
 cursor=XCreateFontCursor(display,XC_sb_up_arrow); 
 XDefineCursor(display,playground,cursor);
 cursor=XCreateFontCursor(display,XC_exchange);
 XDefineCursor(display,xpastbutton,cursor);
 cursor=XCreateFontCursor(display,XC_hand2);
 XDefineCursor(display,nhood,cursor);
 XDefineCursor(display,rulebar,cursor);
 XDefineCursor(display,boundwindow,cursor);
 XDefineCursor(display,tracewindow,cursor);
 XDefineCursor(display,srbutton,cursor);
 XDefineCursor(display,quitbutton,cursor);
 XDefineCursor(display,pausebutton,cursor);
 XDefineCursor(display,fillbutton,cursor);
 XDefineCursor(display,blockbutton,cursor);
 XDefineCursor(display,speedbutton,cursor);

   /* reallocate various arrays */
 if (NULL!=work) free((char *) work);
 if (NULL==( work=(unsigned char *) malloc(volume)))
     {fprintf(stderr,"allocation problems\n");
      cleanup();
     } 
   /* clear working array */
 for (i=0;i<volume;i++)
   work[i]=0;
   /* allocate a few bytes of extra space at ends of field array */
 for (i=0;i<2;i++)
  {if (NULL!=field[i])  free((char *) field[i]-BPW);
   if (NULL==( field[i]= (unsigned char *) malloc(volume+2*BPW)))
     {fprintf(stderr,"allocation problems\n");
      cleanup();
     } 
   /* clear initial state */
   for (j=0;j<(volume+2*BPW);j++)
     field[i][j]=0;
   field[i]+=BPW;
  } 
  /* set blockshift to log_2 of block */
 blockshift=0;
 i=block;
 while(i>>=1)
  blockshift++; 
  /* draw everything */
 repaint();
 return;
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
 XSetForeground(display,gcpen,cbutton);  
 XFillRectangle(display,buttonwindow,gcpen,xoffset+j,yoffset+j,
        xsize-2*j,ysize-2*j);
 XSetForeground(display,gcpen,cleft);
 XFillRectangle(display,buttonwindow,gcpen,xoffset,yoffset,xsize,j);
 XFillRectangle(display,buttonwindow,gcpen,xoffset,yoffset,j,ysize);
 XSetForeground(display,gcpen,cright);
 for (i=0;i<j;i++) 
  {XDrawLine(display,buttonwindow,gcpen,
     xoffset+i,yoffset+ysize-i-1,xoffset+xsize-i-1,yoffset+ysize-i-1);
   XDrawLine(display,buttonwindow,gcpen,
     xoffset+xsize-i-1,yoffset+i,xoffset+xsize-i-1,yoffset+ysize-i-1);
  }
 if (NULL!=text)
   {textlength=strlen(text);
    XSetForeground(display,gcpen,ctext);
    XDrawString(display,buttonwindow,gcpen,
          xoffset+(xsize-textlength*font_width)/2,
          yoffset+(ysize+font_height)/2-2,
          text,textlength);
   }
 return;
}

void cleanup()
{XUnloadFont(display,font->fid);
 XFreeGC(display,gc); 
 XFreeGC(display,gcpen); 
 XCloseDisplay(display);
 XDestroyImage(spinimage);
 if (NULL!=work)     free((char *) work);
 if (NULL!=field[0]) free((char *) field[0]-BPW);
 if (NULL!=field[1]) free((char *) field[1]-BPW);
 exit(1);
} 

/* from here on is the stuff for saving and restoring from a gif file */
/* for info on how gif works, see ftp://network.ucsd.edu/graphics/GIF.shar.Z */

char *picturename="xautomalab.gif";
void decompress(),compress(),inittable();
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
 colorbits=1+((*ptr)&0xf);    /* how many bits of color */
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
 fixboundary();
 showpic();
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
   fputc(colorcell.red>>8,outfile);
   fputc(colorcell.green>>8,outfile);
   fputc(colorcell.blue>>8,outfile);
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
{int currentcode,prefixcode=0,codesize,maxbits=12,maxcode;
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
   /* the LZW stuff */
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

