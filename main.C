#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <GL/gl.h>
#include <GL/glx.h>

#include "config.h"
#include "Cube.h"

#define GLX_CONTEXT_MAJOR_VERSION_ARB    0x2091
#define GLX_CONTEXT_MINOR_VERSION_ARB    0x2092
typedef GLXContext (*glXCreateContextAttribsARBProc)(Display*, GLXFBConfig, GLXContext, Bool, const int*);

// Helper to check for extension string presence.  Adapted from:
//   http://www.opengl.org/resources/features/OGLextensions/
// 'extList' is a string containing all extension strings, separated by spaces.
// 'extension' is a string containing the extension string being queried.
static bool isExtensionSupported(const char *extList, const char *extension) {
  const char *start;
  const char *where, *terminator;
  
  // Extension names do not contain spaces.
  // Make sure an extension name is surrounded
  //  by spaces, or is at the beginning or end of
  //  the list of extensions.

  where = strchr(extension, ' ');
  if ( where || *extension == '\0' )
    return false;

  for ( start = extList; ; ) {
    where = strstr( start, extension );
    if ( !where )
      break;
    terminator = where + strlen( extension );
    if ((where == start     || *(where - 1) == ' ')  &&
	(*terminator == ' ' || *terminator == '\0'))
      return true;
    start = terminator;
  }

  return false;
}

// Helper function to gracefully handle context creation errors.
// NOTE: This handler is not thread-safe!
static bool ctxErrorOccurred = false;
static int ctxErrorHandler( Display *dpy, XErrorEvent *ev )
{
  ctxErrorOccurred = true;
  return 0;
}


int main (int argc, char ** argv) {
  // Get a reference to the display.
  Display *display = XOpenDisplay(0);
  if (!display) {
    fprintf(stderr, "ERROR: Failed to open X display\n");
    exit(1);
  }

  // Identify the "best" framebuffer config that meets minimum requirements.
  // First, specify the minimum requirements in visual_attribs.
  // Next, identify the "best" one meeting requirements, in this case the "best"
  //   has the most number of samples per pixel.
  // Finally, store a reference to the "best", and free the storage for others.
  static int visual_attribs[] = {
    GLX_X_RENDERABLE,  True,
    GLX_DRAWABLE_TYPE, GLX_WINDOW_BIT,
    GLX_RENDER_TYPE,   GLX_RGBA_BIT,
    GLX_X_VISUAL_TYPE, GLX_TRUE_COLOR,
    GLX_RED_SIZE,      8,
    GLX_GREEN_SIZE,    8,
    GLX_BLUE_SIZE,     8,
    GLX_ALPHA_SIZE,    8,
    GLX_DEPTH_SIZE,    24,
    GLX_STENCIL_SIZE,  8,
    GLX_DOUBLEBUFFER,  True,
    None
  };
  int fbcount  =  0;
  int best_fbc = -1;
  int best_num_samp = -1;
  GLXFBConfig *fbc = glXChooseFBConfig(display, DefaultScreen(display), visual_attribs, &fbcount);
  if (!fbc) {
    fprintf(stderr, "ERROR: Failed to retrieve a framebuffer config.\n");
    exit(1);
  }
  for (int i = 0; i < fbcount; ++i) {
    XVisualInfo *vi = glXGetVisualFromFBConfig(display, fbc[i]);
    if (vi) {
      int samp_buf, samples;
      glXGetFBConfigAttrib(display, fbc[i], GLX_SAMPLE_BUFFERS, &samp_buf);
      glXGetFBConfigAttrib(display, fbc[i], GLX_SAMPLES,        &samples );
      
      if (best_fbc < 0 || (samp_buf && samples > best_num_samp)) {
	best_fbc = i;
	best_num_samp = samples;
      }
      XFree(vi);
      vi = 0;
    }
  }
  GLXFBConfig bestFbc = fbc[best_fbc];
  XFree(fbc);
  fbc = 0;
  XVisualInfo *vi = glXGetVisualFromFBConfig(display, bestFbc);
  fprintf(stderr, "INFO: Using visualID = 0x%x\n", (unsigned int)vi->visualid);

  // Create a colormap and window.
  XSetWindowAttributes swa;
  Colormap cmap;
  swa.colormap = cmap = XCreateColormap
    (display, RootWindow(display, vi->screen), vi->visual, AllocNone);
  swa.background_pixmap = None;
  swa.border_pixel = 0;
  swa.event_mask = StructureNotifyMask;
  Window win = XCreateWindow
    (display, RootWindow(display, vi->screen),
     0, 0, RENDER_WIDTH, RENDER_HEIGHT, 0, vi->depth,
     InputOutput, vi->visual, CWBorderPixel|CWColormap|CWEventMask, &swa);
  if (!win) {
    fprintf(stderr, "ERROR: Failed to create the window.\n");
    exit(1);
  }
  XFree(vi);
  vi = 0;

  // Set the window name, and make it active.
  XStoreName(display, win, "GL 4.1 Window");
  XMapWindow(display, win);

  // Get the default screen's GLX extension list
  const char *glxExts = glXQueryExtensionsString(display, DefaultScreen(display));

  // Install an X error handler so the application won't exit if context
  // creation fails. Note that the error handler is not thread safe, so don't
  // allow other threads to interact with X until the handler is uninstalled.
  ctxErrorOccurred = false;
  int (*oldHandler)(Display*, XErrorEvent*) = XSetErrorHandler(&ctxErrorHandler);

  // TODO where better to put this?
  GLXContext ctx = 0;

  // Check for the GLX_ARB_create_context extension string and corresponding function.
  // If either is not present, use GLX 1.3 context creation method.
  glXCreateContextAttribsARBProc glXCreateContextAttribsARB = 0;
  glXCreateContextAttribsARB = (glXCreateContextAttribsARBProc)
    glXGetProcAddressARB( (const GLubyte *) "glXCreateContextAttribsARB" );
  if (!isExtensionSupported(glxExts, "GLX_ARB_create_context") ||
      !glXCreateContextAttribsARB) {
    fprintf(stderr, "WARNING: glXCreateContextAttribsARB() not found. Failing back to compatibility.\n");
    ctx = glXCreateNewContext(display, bestFbc, GLX_RGBA_TYPE, 0, True);
  }
  // If it does, try to get a core GL context!
  else {
    int context_attribs[] = {
      GLX_CONTEXT_MAJOR_VERSION_ARB, 4,
      GLX_CONTEXT_MINOR_VERSION_ARB, 1,
      None
    };
    ctx = glXCreateContextAttribsARB(display, bestFbc, 0, True, context_attribs);
    
    // Sync to ensure any errors generated are processed.
    XSync(display, False);
    if (!ctxErrorOccurred && ctx) {
      fprintf(stderr, "INFO: Created GL 4.1 context.\n");
    }
    else {
      // Couldn't create a core GL context.  Fall back to old-style 2.x context.
      // When a context version below 4.0 is requested, implementations will
      // return the newest context version compatible with OpenGL versions less
      // than version 4.0.
      // GLX_CONTEXT_MAJOR_VERSION_ARB = 1
      context_attribs[1] = 1;
      // GLX_CONTEXT_MINOR_VERSION_ARB = 0
      context_attribs[3] = 0;

      ctxErrorOccurred = false;
      fprintf(stderr, "WARNING: Failed to create core GL context. Failing back to compatibility.\n");
      ctx = glXCreateContextAttribsARB(display, bestFbc, 0, True, context_attribs);
    }
  }
  XSync(display, False);
  
  // Restore the original error handler.
  XSetErrorHandler(oldHandler);
  if (ctxErrorOccurred || !ctx) {
    fprintf(stderr, "ERROR: Failed to create an OpenGL context.\n");
    exit(1);
  }
    
  // Verifying that context is a direct context
  if (!glXIsDirect(display, ctx)) {
    fprintf(stderr, "WARNING: GLX rendering context is Indirect.\n");
  }
  else {
    fprintf(stderr, "INFO: GLX rendering context is Direct.\n");
  }
  glXMakeCurrent(display, win, ctx);

  //---------------------------------------------------------------------------
  // INITIALIZATION IS COMPLETE
  // OPENGL CODE GOES BELOW
  //---------------------------------------------------------------------------  

  Cube cube;
  while (true) {
    cube.render(0.001f); // HACK: hardcoded value for now.
    glXSwapBuffers (display, win);
  }

  //---------------------------------------------------------------------------
  // RENDERING IS COMPLETE
  // OPENGL CODE GOES ABOVE
  //---------------------------------------------------------------------------  

  // Destroy context
  glXMakeCurrent( display, 0, 0 );
  glXDestroyContext( display, ctx );

  XDestroyWindow( display, win );
  XFreeColormap( display, cmap );
  XCloseDisplay( display );
}


