// lv-tool - Libvisual commandline tool
//
// Copyright (C) 2012-2013 Libvisual team
//               2004-2006 Dennis Smit
//
// Authors: Daniel Hiepler <daniel@niftylight.de>
//          Chong Kai Xiong <kaixiong@codeleft.sg>
//          Dennis Smit <ds@nerds-incorporated.org>
//
// This file is part of lv-tool.
//
// lv-tool is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 2 of the License, or
// (at your option) any later version.
//
// lv-tool is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with lv-tool.  If not, see <http://www.gnu.org/licenses/>.

#include "config.h"
#include "stdout_driver.hpp"
#include "display.hpp"
#include "display_driver.hpp"
#include <libvisual/libvisual.h>
#include <string>
#include <unistd.h>

#if defined(HAVE_GL) && defined(HAVE_OSMESA)
#define USE_OSMESA 1
#endif

// MinGW unistd.h doesn't have *_FILENO or SEEK_* defined
#ifdef VISUAL_WITH_MINGW
#  define STDOUT_FILENO 1
#endif

#if USE_OSMESA
#include <GL/gl.h>
#include <GL/osmesa.h>
#endif

namespace {

  class StdoutDriver
      : public DisplayDriver
  {
  public:

      StdoutDriver (Display& display)
          : m_display (display)
      #if USE_OSMESA
          , m_gl_context (nullptr)
      #endif
      {}

      virtual ~StdoutDriver ()
      {
          close ();
      }

      virtual LV::VideoPtr create (VisVideoDepth depth,
                                   VisVideoAttrOptions const* vidoptions,
                                   unsigned int width,
                                   unsigned int height,
                                   bool resizable)
      {
      #if USE_OSMESA
          if (m_gl_context) {
              OSMesaDestroyContext (m_gl_context);
          }

          if (depth == VISUAL_VIDEO_DEPTH_GL) {
              // FIXME: Need to parse actor video GL attributes and
              // match them to what OSMesa supports

              m_gl_context   = OSMesaCreateContextExt (OSMESA_RGB, 16, 0, 0, nullptr);
              m_screen_video = LV::Video::create (width, height, VISUAL_VIDEO_DEPTH_24BIT);
              OSMesaMakeCurrent (m_gl_context, m_screen_video->get_pixels (), GL_UNSIGNED_BYTE, width, height);
          } else {
              m_screen_video = LV::Video::create (width, height, depth);
          }

          return m_screen_video;
      #else
          if (depth == VISUAL_VIDEO_DEPTH_GL)
          {
              visual_log (VISUAL_LOG_ERROR, "Cannot use stdout driver for OpenGL rendering");
              return nullptr;
          }

          m_screen_video = LV::Video::create (width, height, depth);

          return m_screen_video;
      #endif
      }

      virtual void close ()
      {
      #if USE_OSMESA
          if (m_gl_context) {
              OSMesaDestroyContext (m_gl_context);
          }
      #endif
          m_screen_video.reset ();
      }

      virtual void lock ()
      {
          // nothing to do
      }

      virtual void unlock ()
      {
          // nothing to do
      }

      virtual void set_fullscreen (bool fullscreen, bool autoscale)
      {
          // nothing to do
      }

      virtual LV::VideoPtr get_video () const
      {
          return m_screen_video;
      }

      virtual void set_title(std::string const& title)
      {
          // nothing to do
      }

      virtual void update_rect (LV::Rect const& rect)
      {
      #if USE_OSMESA
          // Flush command buffers and make sure rendering completes
          glFinish ();
      #endif

          if (write (STDOUT_FILENO, m_screen_video->get_pixels (), m_screen_video->get_size ()) == -1)
              visual_log (VISUAL_LOG_ERROR, "Failed to write pixels to stdout");
      }

      virtual void drain_events (VisEventQueue& eventqueue)
      {
          // nothing to do
      }

  private:

      Display&     m_display;
      LV::VideoPtr m_screen_video;

  #if USE_OSMESA
      OSMesaContext m_gl_context;
  #endif
  };

} // anonymous namespace

// creator
DisplayDriver* stdout_driver_new (Display& display)
{
    return new StdoutDriver (display);
}
