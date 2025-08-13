/* (c) copyright 2025 Lawrence D. Kern /////////////////////////////////////// */

// NOTE: This file is the entry point for the Wayland-based Linux build. Each
// supported platform will have its code constrained to a single file, which
// will quarantine the underlying platform from the renderer.

#include <wayland-client.h>
#include <wayland-egl.h>
#include <EGL/egl.h>
#include <GL/gl.h>

#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "xdg-shell-client-protocol.h"
#include "xdg-decoration-unstable-v1-client-protocol.h"
#include "shared.h"

typedef struct {
   struct wl_display *Display;
   struct wl_compositor *Compositor;
   struct wl_surface *Surface;
   struct wl_egl_window *Window;
   struct wl_registry *Registry;

   struct xdg_wm_base *Desktop_Base;
   struct xdg_surface *Desktop_Surface;
   struct xdg_toplevel *Desktop_Toplevel;

   EGLDisplay Opengl_Display;
   EGLConfig Opengl_Configuration;
   EGLContext Opengl_Context;
   EGLSurface Opengl_Surface;

   int Window_Width;
   int Window_Height;
   bool Running;

   struct zxdg_decoration_manager_v1 *Decoration_Manager;
   struct zxdg_toplevel_decoration_v1 *Toplevel_Decoration;
} wayland_context;

static void Global_Registry(void *Data, struct wl_registry *Registry, u32 ID, const char *Interface, u32 Version)
{
   wayland_context *Wayland = Data;
   if(strcmp(Interface, wl_compositor_interface.name) == 0)
   {
      Wayland->Compositor = wl_registry_bind(Registry, ID, &wl_compositor_interface, 4);
   }
   else if(strcmp(Interface, xdg_wm_base_interface.name) == 0)
   {
      Wayland->Desktop_Base = wl_registry_bind(Registry, ID, &xdg_wm_base_interface, 1);
   }
   else if(strcmp(Interface, zxdg_decoration_manager_v1_interface.name) == 0)
   {
      Wayland->Decoration_Manager = wl_registry_bind(Registry, ID, &zxdg_decoration_manager_v1_interface, 1);
   }
}
static void Remove_Global_Registry(void *Data, struct wl_registry *Registry, u32 ID)
{
}
static const struct wl_registry_listener Registry_Listener =
{
   .global = Global_Registry,
   .global_remove = Remove_Global_Registry,
};

static void Ping_Desktop_Base(void *Data, struct xdg_wm_base *Base, u32 Serial)
{
   xdg_wm_base_pong(Base, Serial);
}
static const struct xdg_wm_base_listener Desktop_Base_Listener =
{
   .ping = Ping_Desktop_Base,
};

static void Configure_Desktop_Surface(void *Data, struct xdg_surface *Surface, u32 Serial)
{
   xdg_surface_ack_configure(Surface, Serial);
}
static const struct xdg_surface_listener Desktop_Surface_Listener =
{
   .configure = Configure_Desktop_Surface,
};

static void Configure_Desktop_Toplevel(void *Data, struct xdg_toplevel *Toplevel, s32 Width, s32 Height, struct wl_array *States)
{
   wayland_context *Wayland = Data;
   if(Width > 0 && Height > 0)
   {
      Wayland->Window_Width = Width;
      Wayland->Window_Height = Height;

      if(Wayland->Window)
      {
         wl_egl_window_resize(Wayland->Window, Wayland->Window_Width, Wayland->Window_Height, 0, 0);
      }
   }
}
static void Close_Desktop_Toplevel(void *Data, struct xdg_toplevel *Toplevel)
{
   wayland_context *Wayland = Data;
   Wayland->Running = false;
}
static const struct xdg_toplevel_listener Desktop_Toplevel_Listener =
{
   .configure = Configure_Desktop_Toplevel,
   .close = Close_Desktop_Toplevel,
};

static void Destroy_Wayland(wayland_context *Wayland)
{
   // NOTE: Destroy OpenGL.
   if(Wayland->Opengl_Surface != EGL_NO_SURFACE)
   {
      eglDestroySurface(Wayland->Opengl_Display, Wayland->Opengl_Surface);
   }
   if(Wayland->Opengl_Context != EGL_NO_CONTEXT)
   {
      eglDestroyContext(Wayland->Opengl_Display, Wayland->Opengl_Context);
   }
   if(Wayland->Opengl_Display != EGL_NO_DISPLAY)
   {
      eglTerminate(Wayland->Opengl_Display);
   }
   if(Wayland->Window)
   {
      wl_egl_window_destroy(Wayland->Window);
   }

   // NOTE: Destroy desktop.
   if(Wayland->Desktop_Toplevel)
   {
      xdg_toplevel_destroy(Wayland->Desktop_Toplevel);
   }
   if(Wayland->Desktop_Surface)
   {
      xdg_surface_destroy(Wayland->Desktop_Surface);
   }
   if(Wayland->Desktop_Base)
   {
      xdg_wm_base_destroy(Wayland->Desktop_Base);
   }

   // NOTE: Destroy Wayland.
   if(Wayland->Surface)
   {
      wl_surface_destroy(Wayland->Surface);
   }
   if(Wayland->Compositor)
   {
      wl_compositor_destroy(Wayland->Compositor);
   }
   if(Wayland->Registry)
   {
      wl_registry_destroy(Wayland->Registry);
   }
   if(Wayland->Display)
   {
      wl_display_disconnect(Wayland->Display);
   }

   // NOTE: Destroy decorations.
   if(Wayland->Toplevel_Decoration)
   {
      zxdg_toplevel_decoration_v1_destroy(Wayland->Toplevel_Decoration);
   }
}

static bool Initialize_Wayland_Opengl(wayland_context *Wayland, int Width, int Height)
{
   bool Result = false;

   EGLint Configuration_Attributes[] =
      {
         EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
         EGL_RED_SIZE, 8,
         EGL_GREEN_SIZE, 8,
         EGL_BLUE_SIZE, 8,
         EGL_ALPHA_SIZE, 8,
         EGL_RENDERABLE_TYPE, EGL_OPENGL_BIT,
         EGL_NONE,
      };

   EGLint Context_Attributes[] =
      {
         EGL_CONTEXT_MAJOR_VERSION, 3,
         EGL_CONTEXT_MINOR_VERSION, 3,
         EGL_CONTEXT_OPENGL_PROFILE_MASK, EGL_CONTEXT_OPENGL_CORE_PROFILE_BIT,
         EGL_NONE,
      };

   Wayland->Opengl_Display = eglGetDisplay(Wayland->Display);
   if(Wayland->Opengl_Display != EGL_NO_DISPLAY)
   {
      EGLint Major, Minor;
      if(eglInitialize(Wayland->Opengl_Display, &Major, &Minor))
      {
         if(eglBindAPI(EGL_OPENGL_API))
         {
            EGLint Configuration_Count;
            if(eglChooseConfig(Wayland->Opengl_Display, Configuration_Attributes, &Wayland->Opengl_Configuration, 1, &Configuration_Count))
            {
               Wayland->Opengl_Context = eglCreateContext(Wayland->Opengl_Display, Wayland->Opengl_Configuration, EGL_NO_CONTEXT, Context_Attributes);
               if(Wayland->Opengl_Context != EGL_NO_CONTEXT)
               {
                  Wayland->Window = wl_egl_window_create(Wayland->Surface, Width, Height);
                  if(Wayland->Window)
                  {
                     Wayland->Opengl_Surface = eglCreateWindowSurface(Wayland->Opengl_Display, Wayland->Opengl_Configuration, Wayland->Window, 0);
                     if(Wayland->Opengl_Surface != EGL_NO_SURFACE)
                     {
                        if(eglMakeCurrent(Wayland->Opengl_Display, Wayland->Opengl_Surface, Wayland->Opengl_Surface, Wayland->Opengl_Context))
                        {
                           Result = true;
                        }
                        else
                        {
                           fprintf(stderr, "EGL failed to make the OpenGL context current.\n");
                        }
                     }
                     else
                     {
                        fprintf(stderr, "EGL failed to create a surface.\n");
                     }
                  }
                  else
                  {
                     fprintf(stderr, "EGL failed to create a window.\n");
                  }
               }
               else
               {
                  fprintf(stderr, "EGL failed to create an OpenGL context.\n");
               }
            }
            else
            {
               fprintf(stderr, "EGL failed to choose a configuration.\n");
            }
         }
         else
         {
            fprintf(stderr, "EGL failed to bind OpenGL API.\n");
         }
      }
      else
      {
         fprintf(stderr, "EGL failed to initialize.\n");
      }
   }
   else
   {
      fprintf(stderr, "EGL failed to get a display.\n");
   }

   return(Result);
}

static void Initialize_Wayland(wayland_context *Wayland, int Width, int Height)
{
   Wayland->Display = wl_display_connect(0);
   if(Wayland->Display)
   {
      Wayland->Registry = wl_display_get_registry(Wayland->Display);
      wl_registry_add_listener(Wayland->Registry, &Registry_Listener, Wayland);

      wl_display_dispatch(Wayland->Display);
      wl_display_roundtrip(Wayland->Display);

      if(Wayland->Compositor && Wayland->Desktop_Base)
      {
         xdg_wm_base_add_listener(Wayland->Desktop_Base, &Desktop_Base_Listener, Wayland);

         Wayland->Surface = wl_compositor_create_surface(Wayland->Compositor);
         if(Wayland->Surface)
         {
            Wayland->Desktop_Surface = xdg_wm_base_get_xdg_surface(Wayland->Desktop_Base, Wayland->Surface);
            if(Wayland->Desktop_Surface)
            {
               xdg_surface_add_listener(Wayland->Desktop_Surface, &Desktop_Surface_Listener, Wayland);

               Wayland->Desktop_Toplevel = xdg_surface_get_toplevel(Wayland->Desktop_Surface);
               if(Wayland->Desktop_Toplevel)
               {
                  xdg_toplevel_add_listener(Wayland->Desktop_Toplevel, &Desktop_Toplevel_Listener, Wayland);
                  xdg_toplevel_set_title(Wayland->Desktop_Toplevel, "OpenGL Window");

                  if(Wayland->Decoration_Manager)
                  {
                     Wayland->Toplevel_Decoration = zxdg_decoration_manager_v1_get_toplevel_decoration(Wayland->Decoration_Manager, Wayland->Desktop_Toplevel);
                     zxdg_toplevel_decoration_v1_set_mode(Wayland->Toplevel_Decoration, ZXDG_TOPLEVEL_DECORATION_V1_MODE_SERVER_SIDE);
                  }

                  wl_surface_commit(Wayland->Surface);
                  wl_display_dispatch(Wayland->Display);

                  if(Initialize_Wayland_Opengl(Wayland, Width, Height))
                  {
                     Wayland->Running = true;
                  }
               }
               else
               {
                  fprintf(stderr, "Failed to create XDG toplevel\n");
               }
            }
            else
            {
               fprintf(stderr, "Failed to create XDG surface\n");
            }
         }
         else
         {
            fprintf(stderr, "Failed to create Wayland surface\n");
         }
      }
      else
      {
         fprintf(stderr, "Failed to bind required Wayland interfaces\n");
      }
   }
   else
   {
      fprintf(stderr, "Failed to connect to Wayland display\n");
   }
}

static void Display_Wayland_With_Opengl(wayland_context *Wayland)
{
   glClearColor(0, 0, 1, 1);
   glClear(GL_COLOR_BUFFER_BIT);

   eglSwapBuffers(Wayland->Opengl_Display, Wayland->Opengl_Surface);
}

int main(void)
{
   wayland_context Wayland = {0};
   Initialize_Wayland(&Wayland, 640, 480);

   while(Wayland.Running)
   {
      wl_display_dispatch_pending(Wayland.Display);

      Display_Wayland_With_Opengl(&Wayland);

      wl_display_flush(Wayland.Display);
   }

   Destroy_Wayland(&Wayland);

   return(0);
}
