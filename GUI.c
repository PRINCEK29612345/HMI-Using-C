#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>
#include <time.h>

typedef struct {
  int x,y,x1,y1;

}coordinate;

void DrawData(Display *d,Window w,GC gc,int *Data,coordinate *g,int y) {

  for(int i = 0; i < 1023; i++) {
    if(Data[i]>30 && Data[i]<60){
      XSetForeground(d,gc,0x00FF00);
    }
    else if(Data[i]>60) {
      XSetForeground(d,gc,0xFF0000);
    }
    else {
      XSetForeground(d,gc,0xFF0000);
    }
    XFillRectangle(d,w,gc,g->x1+1+(i),g->y1+(y),1,1);
  }

}

void DrawBG(Display *d,Window w,GC gc,coordinate *g,int s) {

  char *msg = {"Active Display"};
  int len = strlen(msg);

  XSetForeground(d,gc,BlackPixel(d,s));
  XFillRectangle(d,w,gc,0,0,g->x,g->y);
  XSetForeground(d,gc,WhitePixel(d,s));
  XFillRectangle(d,w,gc,g->x1,g->y1,1024,1);
  XFillRectangle(d,w,gc,g->x1,g->y1,1,400);
  XFillRectangle(d,w,gc,g->x1,g->y1+400,1024,1);
  XFillRectangle(d,w,gc,g->x1+1024,g->y1,1,401);

  XSetForeground(d,gc,0xFF0000);
  XDrawString(d,w,gc,g->x1+512,g->y1-25,msg,len);
  XSetForeground(d,gc,WhitePixel(d,s));
  XFillRectangle(d,w,gc,g->x1,g->y1-20,60,15);

}

void CrossLine(Display *d,Window w,GC gc,coordinate *g,int x,int y) {
    XSetFunction(d,gc,GXxor);
    XSetForeground(d,gc,0x00FFFFFF);
    for (int i = 1;i < 1024; i = i+3) XFillRectangle(d,w,gc,g->x1+i,y,1,1);
    for (int i = 1;i < 400; i = i+3) XFillRectangle(d,w,gc,x,g->y1+1+i,1,1);
    XSetFunction(d,gc,GXcopy);
}

void ShowMsg(Display *d,Window w,GC gc,coordinate *g,int x,int y,int s) {
  char xind[10],yind[10];

  sprintf(xind,"%d",x);
  sprintf(yind,"%d",y);

  int xlen,ylen;
  xlen = strlen(xind);
  ylen = strlen(yind);

  XSetForeground(d,gc,WhitePixel(d,s));
  XFillRectangle(d,w,gc,g->x1,g->y1-20,60,15);

  XSetForeground(d,gc,0x0000FF);
  XDrawString(d,w,gc,g->x1+3,g->y1-10,xind,xlen);

  XSetForeground(d,gc,0x0000FF);
  XDrawString(d,w,gc,g->x1+30,g->y1-10,yind,ylen);  
}
      
void DrawHrLine(Display *d, Window w, GC gc, int y, int thickness) {
  XSetForeground(d,gc,0xFFFF00);
  XFillRectangle(d,w,gc,50,101+y,1024,thickness);
}


int main() {
  
  Display *d;
  Window w;
  XEvent e;
  int s;
  coordinate arg;
  arg.x = 1126;
  arg.y = 552;
  arg.x1 = 50;
  arg.y1 = 100;
  int xprev = -1,yprev = -1,yval = 398;
  int mouse_x = -1,mouse_y = -1,mouse_in = 0;
  int *data = (int*)malloc(1024*sizeof(int));


  d = XOpenDisplay(NULL);
  if (d==NULL) {
    printf("Can't open display\n");
    return 0;
  }

  s = DefaultScreen(d);
  int run = 1;

  w = XCreateSimpleWindow(d,RootWindow(d,s),100,100,arg.x,arg.y,1,BlackPixel(d,s),WhitePixel(d,s));
  XSelectInput(d,w,ExposureMask | KeyPressMask | PointerMotionMask | ButtonPressMask);
  XMapWindow(d,w);

  GC gc = XCreateGC(d,w,0,NULL);

  
  Pixmap back = XCreatePixmap(d, w, arg.x, arg.y, DefaultDepth(d, s));
  GC bg_gc = XCreateGC(d, back, 0, NULL);

  DrawBG(d,back,bg_gc,&arg,s);

  while(run) {
    while(XPending(d)) {

      XNextEvent(d,&e);

      if(e.type == Expose) {
        XCopyArea(d, back, w, gc, 0, 0, arg.x, arg.y, 0, 0);
      }

      if (e.type == MotionNotify) {
        int x = e.xmotion.x;
        int y = e.xmotion.y;

        if(x > arg.x1 && x < (arg.y1+1024) && y > arg.y1 && arg.y1 < y + 400) {
          mouse_x = x;
          mouse_y = y;
          mouse_in = 1;
        }
        else {
          mouse_in = 0;
        }
      }

      if (e.type == ButtonPress) {

        int x = e.xbutton.x;
        int y = e.xbutton.y;
        if(x > arg.x1 && x < (arg.x1+1024) && y > arg.y1 && y < arg.y1 + 400) {
          
          if(xprev != -1 && yprev != -1) {
            CrossLine(d,w,gc,&arg,x,y);
          }

          xprev = x;
          yprev = y;
          CrossLine(d,w,gc,&arg,x,y);
        }
      }

      if (e.type == KeyPress) {
        KeySym key = XLookupKeysym(&e.xkey,0);
        if(key == XK_Escape) {
          run = 0;
        }
      }
    }

  for (int i = 0; i < 1024; i++) {
    data[i] = rand() % 65;
  }

  DrawData(d,back,bg_gc,data,&arg,yval+1);

  XCopyArea(d,back,w,gc,0,0,arg.x,arg.y,0,0);
  
  DrawHrLine(d, w, gc,yval, 1);
  srand(time(NULL));


  if(xprev != -1 && yprev != -1) {
    CrossLine(d,w,gc,&arg,xprev,yprev);
  }

  if(mouse_in) {
    ShowMsg(d,w,gc,&arg,mouse_x,mouse_y,s);
  }

  XFlush(d);
  yval = yval - 1;
  if(yval == 0) yval = 398;
  usleep(10000);
  }

  XCloseDisplay(d);
return 0;
}
