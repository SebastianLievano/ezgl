#include "ezgl/canvas.hpp"

#include <gtk/gtk.h>

#include <cassert>

namespace ezgl {

cairo_surface_t *create_surface(GtkWidget *widget)
{
  GdkWindow *parent_window = gtk_widget_get_window(widget);
  int const width = gtk_widget_get_allocated_width(widget);
  int const height = gtk_widget_get_allocated_height(widget);

  return gdk_window_create_similar_surface(parent_window, CAIRO_CONTENT_COLOR_ALPHA, width, height);
}

gboolean canvas::configure_event(GtkWidget *widget, GdkEventConfigure *, gpointer data)
{
  // User data should have been set during the signal connection.
  g_return_val_if_fail(data != nullptr, FALSE);

  auto ezgl_canvas = static_cast<canvas *>(data);
  auto &surface = ezgl_canvas->m_surface;

  if(surface == nullptr) {
    cairo_surface_destroy(surface);
  }

  // Something has changed, recreate the surface.
  surface = create_surface(widget);

  // Draw to the newly created surface.
  ezgl_canvas->redraw();

  g_info("canvas::configure_event has been handled.");
  return TRUE; // the configure event was handled
}

gboolean canvas::draw_surface(GtkWidget *, cairo_t *context, gpointer data)
{
  // Assume context and data are non-null.
  auto &surface = static_cast<canvas *>(data)->m_surface;

  // Assume surface is non-null.
  cairo_set_source_surface(context, surface, 0, 0);
  cairo_paint(context);

  return FALSE;
}

canvas::canvas(char const *canvas_id, draw_canvas_fn draw_callback)
    : m_canvas_id(canvas_id), m_draw_callback(draw_callback)
{
}

void canvas::initialize(GtkWidget *drawing_area)
{
  g_return_if_fail(drawing_area != nullptr);

  m_drawing_area = drawing_area;
  m_surface = create_surface(m_drawing_area);

  // Draw to the newly created surface for the first time.
  redraw();

  // Connect to configure events in case our widget changes shape.
  g_signal_connect(m_drawing_area, "configure-event", G_CALLBACK(configure_event), this);
  // Connect to draw events so that we draw our surface to the drawing area.
  g_signal_connect(m_drawing_area, "draw", G_CALLBACK(draw_surface), this);

  // GtkDrawingArea objects need mouse button button presses enabled explicitly.
  gtk_widget_add_events(GTK_WIDGET(m_drawing_area), GDK_BUTTON_PRESS_MASK);
  gtk_widget_add_events(GTK_WIDGET(m_drawing_area), GDK_BUTTON_RELEASE_MASK);

  g_info("canvas::initialize successful.");
}

void canvas::redraw()
{
  cairo_t *context = cairo_create(m_surface);
  m_draw_callback(context);
  cairo_destroy(context);

  gtk_widget_queue_draw(m_drawing_area);

  g_info("The canvas will be redrawn.");
}
}