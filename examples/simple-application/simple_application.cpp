/**
 * @file
 *
 * This example shows you how to create an XML-Based application using the library.
 */

#include <ezgl/application.hpp>

#include <iostream>

/**
 * Reacts to events where the user has pressed a key on the keyboard.
 *
 * @param event The keyboard event.
 */
void press_key(GdkEventKey *event)
{
  // see: https://developer.gnome.org/gdk3/stable/gdk3-Keyboard-Handling.html
  std::cout << gdk_keyval_name(event->keyval) << " was pressed.\n";
}

/**
 * React to events where the mouse has been clicked.
 *
 * @param event The click event.
 */
void click_mouse(GdkEventButton *event)
{
  if(event->type == GDK_BUTTON_PRESS) {
    std::cout << "User clicked mouse at " << event->x << ", " << event->y << "\n";
  } else if(event->type == GDK_BUTTON_RELEASE) {
    std::cout << "User released mouse button at " << event->x << ", " << event->y << "\n";
  }
}

/**
 * Draws different graphics to the screen.
 *
 * This function is called by the event loop very frequently and should finish quickly.
 *
 * @param g The graphics object used to draw onto the screen.
 * @param width The width of the screen.
 * @param height The height of the screen.
 */
void draw_screen(ezgl::graphics g, int, int)
{
  // change the next draw calls to use the colour red
  g.set_colour(ezgl::RED);

  // draw rectangle outlines...
  g.draw_rectangle({100, 100}, {400, 300}); // from one point to another
  g.draw_rectangle({10, 10}, 50, 50);       // from one point with a width and height

  // draw 3/4 transparent blue text
  g.set_colour(ezgl::BLUE, 0.6);
  g.format_font("monospace", ezgl::font_slant::oblique, ezgl::font_weight::normal, 24);
  g.draw_text({100, 100}, "Hello World!");

  // change the next draw calls to use green with half transparency
  g.set_colour(0, 1, 0, 0.5);
  // draw filled in rectangles...
  g.fill_rectangle({500, 50}, {600, 300}); // from one point to another
  g.fill_rectangle({500, 50}, 50, 50);     // from one point with a width and height
}

/**
 * The start point of the program.
 *
 * This function initializes an ezgl application and runs it.
 *
 * @param argc The number of arguments provided.
 * @param argv The arguments as an array of c-strings.
 *
 * @return the exit status of the application run.
 */
int main(int argc, char **argv)
{
  // create the EZGL application we wish to run
  ezgl::application application;

  application.set_callback(draw_screen);
  application.set_callback(press_key);
  application.set_callback(click_mouse);

  // run the application until the user quits
  return application.run(argc, argv);
}