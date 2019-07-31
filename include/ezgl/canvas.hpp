/*
 * Copyright 2019 University of Toronto
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * Authors: Mario Badr, Sameh Attia and Tanner Young-Schultz
 */

#ifndef EZGL_CANVAS_HPP
#define EZGL_CANVAS_HPP

#include "ezgl/camera.hpp"
#include "ezgl/rectangle.hpp"
#include "ezgl/graphics.hpp"
#include "ezgl/color.hpp"

#include <cairo.h>
#include <cairo-pdf.h>
#include <cairo-svg.h>
#include <gtk/gtk.h>

#include <string>

namespace ezgl {

/**** Functions in this class are for ezgl internal use; application code doesn't need to call them ****/

class renderer;

/**
 * create_and_generate_pdf and create_and_generate_svg both create a surface and generate an file output.
 * 
 * @param widget        a widget which specifies the width and the height of the 
 *                      surface generated. Can use any widget — button, window, etc. 
 *                      Should be using the MainCanvas (GtkDrawingArea) to save the
 *                      graphical content shown in the main canvas.
 * @param file_name:    a filename for the PDF output (must be writable), NULL may 
 *                      be used to specify no output. This will generate a PDF/SVG 
 *                      surface that may be queried and used as a source, without 
 *                      generating a temporary file.
 * @return              a pointer to the newly created surface. The caller owns 
 *                      the surface and should call cairo_surface_destroy() when 
 *                      done with it.
 * 
 * sample code: http://zetcode.com/gfx/cairo/cairobackends/
 */
cairo_surface_t *create_and_generate_pdf(GtkWidget *widget, const char *file_name);
cairo_surface_t *create_and_generate_svg(GtkWidget *widget, const char *file_name);

/**
 * create_png and generate_png work the same way as create_and_generate_pdf and create_and_generate_svg,
 * while the process is broken into two separate functions due to the corresponding cairo functions. 
 * Please refer to the comments for create_and_generate_pdf/create_and_generate_svg above.
 * Pass the return value of create_png into generate_png to generate an output png file.
 */
cairo_surface_t *create_png(GtkWidget *widget);
cairo_public cairo_status_t generate_png(cairo_surface_t *p_surface, const char *file_name);

/**
 * The signature of a function that draws to an ezgl::canvas.
 */
using draw_canvas_fn = void (*)(renderer &);

/**
 * Responsible for creating, destroying, and maintaining the rendering context of a GtkWidget.
 *
 * Underneath, the class relies on a GtkDrawingArea as its GUI widget along with cairo to provide the rendering context.
 * The class connects to the relevant GTK signals, namely configure and draw events, to remain responsive.
 *
 * Each canvas is double-buffered. A draw callback (see: ezgl::draw_canvas_fn) is invoked each time the canvas needs to
 * be redrawn. This may be caused by the user (e.g., resizing the screen), but can also be forced by the programmer.
 */
class canvas {
public:
  /**
   * Destructor.
   */
  ~canvas();

  /**
   * Get the name (identifier) of the canvas.
   */
  char const *id() const
  {
    return m_canvas_id.c_str();
  }

  /**
   * Get the width of the canvas in pixels.
   */
  int width() const;

  /**
   * Get the height of the canvas in pixels.
   */
  int height() const;

  /**
   * Force the canvas to redraw itself.
   *
   * This will invoke the ezgl::draw_canvas_fn callback and queue a redraw of the GtkWidget.
   */
  void redraw();

  /**
   * Get an immutable reference to this canvas' camera.
   */
  camera const &get_camera() const
  {
    return m_camera;
  }

  /**
   * Get a mutable reference to this canvas' camera.
   */
  camera &get_camera()
  {
    return m_camera;
  }

  /**
   * Create a temporary renderer that can be used to draw on top of the current canvas
   *
   * The created renderer should be used only in the same callback in which it was created
   */
  renderer create_temporary_renderer();

protected:
  // Only the ezgl::application can create and initialize a canvas object.
  friend class application;

  /**
   * Create a canvas that can be drawn to.
   */
  canvas(std::string canvas_id, draw_canvas_fn draw_callback, rectangle coordinate_system, color background_color);

  /**
   * Lazy initialization of the canvas class.
   *
   * This function is required because GTK will not send activate/startup signals to an ezgl::application until control
   * of the program has been reliquished. The GUI is not built until ezgl::application receives an activate signal.
   */
  void initialize(GtkWidget *drawing_area);

private:
  // Name of the canvas in XML.
  std::string m_canvas_id;

  // The function to call when the widget needs to be redrawn.
  draw_canvas_fn m_draw_callback;

  // The transformations between the GUI and the world.
  camera m_camera;

  // The background color of the drawing area
  color m_background_color;

  // A non-owning pointer to the drawing area inside a GTK window.
  GtkWidget *m_drawing_area = nullptr;

  // The off-screen surface that can be drawn to.
  cairo_surface_t *m_surface = nullptr;

  // The off-screen cairo context that can be drawn to
  cairo_t *m_context = nullptr;

private:
  // Called each time our drawing area widget has changed (e.g., in size).
  static gboolean configure_event(GtkWidget *widget, GdkEventConfigure *event, gpointer data);

  // Called each time we need to draw to our drawing area widget.
  static gboolean draw_surface(GtkWidget *widget, cairo_t *context, gpointer data);
};
}

#endif //EZGL_CANVAS_HPP
