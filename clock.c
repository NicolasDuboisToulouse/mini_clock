#include <stdio.h>
#include <gtk/gtk.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#define MAX_DATE_STRING_LEN 50


//
// Clock 'class'
//
typedef struct {
  GtkWidget* widget;
  uint32_t   width;
  uint32_t   height;
  uint32_t   display_second;
  int        last_min_displayed;
} clock_widget_t;



//
// Tools
//
#define max(a, b) (((a)>(b))?(a):(b))


//
// Draw the clock
//
static gboolean clock_expose(GtkWidget *widget, GdkEventExpose *event, gpointer data)
{
  clock_widget_t* clock = (clock_widget_t*)data;
  cairo_t *cr = gdk_cairo_create(widget->window);

  // Draw background
  // TODO: option
  cairo_set_source_rgba (cr, 1.0, 1.0, 1.0, 0.5);
  cairo_set_operator (cr, CAIRO_OPERATOR_SOURCE);
  cairo_paint (cr);


  // Prepare text to draw
  time_t now = time(NULL);
  struct tm* now_tm = localtime(&now);

  char time_string[MAX_DATE_STRING_LEN];
  if (clock->display_second) {
    strftime(time_string, MAX_DATE_STRING_LEN - 1, "%T", now_tm);
  } else {
    strftime(time_string, MAX_DATE_STRING_LEN - 1, "%R", now_tm);
  }


  // Set text attribute (TODO: options)
  cairo_select_font_face(cr, "Serif", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
  cairo_set_font_size(cr, 13);
  cairo_set_source_rgb(cr, 0., 0., 0.); 


  // Check if we have enough space to draw the text,
  // Otherwise, resize the window.
  cairo_text_extents_t extents;
  cairo_text_extents(cr, time_string, &extents);

  if (extents.height                    > clock->height ||
      extents.width + extents.x_bearing > clock->width    )
  {
    clock->width  = max(extents.width + extents.x_bearing, clock->width);
    clock->height = max(-extents.y_bearing,                clock->height);

    gtk_window_resize(GTK_WINDOW(widget), clock->width, clock->height);
  }

  // Draw the text
  cairo_move_to(cr, 0, -extents.y_bearing);
  cairo_show_text(cr, time_string);

  // Done
  cairo_destroy(cr);
  return FALSE;
}


//
// Configure colormap
//
static void clock_screen_changed(GtkWidget *widget, GdkScreen *old_screen, gpointer userdata)
{
  GdkScreen *screen = gtk_widget_get_screen(widget);
  GdkColormap *colormap = gdk_screen_get_rgba_colormap(screen);

  if (colormap == NULL) {
    printf("Your screen does not support alpha channels!\n");
    exit(1);
  }

  gtk_widget_set_colormap(widget, colormap);
}


//
// Update drawing if time has changed
//
static gboolean clock_update(gpointer data)
{
  clock_widget_t* clock = (clock_widget_t*)data;

  // Sometime the clock become hidden by other widgets
  //  gtk_window_present(GTK_WINDOW(clock->widget));
  //  gtk_window_set_keep_above(GTK_WINDOW(clock->widget), TRUE);

  if (clock->display_second) {
    gtk_widget_queue_draw(clock->widget);
  } else {
    time_t now = time(NULL);
    struct tm* now_tm = localtime(&now);
    if (clock->last_min_displayed != now_tm->tm_min) {
      clock->last_min_displayed = now_tm->tm_min;
      gtk_widget_queue_draw(clock->widget);
    }
  }

  return TRUE;
}




//
// Create the clock
//
static clock_widget_t* clock_create()
{
  // alloc the widget (TODO: destroy)
  clock_widget_t* clock = (clock_widget_t*)malloc(sizeof(clock_widget_t));
  memset(clock, 0, sizeof(clock_widget_t));

  // Initial size
  clock->width = 1;
  clock->height = 1;
  clock->last_min_displayed = -1;
  clock->display_second = 0; // TODO: option


  clock->widget = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_type_hint(GTK_WINDOW(clock->widget), GDK_WINDOW_TYPE_HINT_DOCK);
  gtk_window_set_decorated(GTK_WINDOW(clock->widget), FALSE);
  gtk_window_set_keep_above(GTK_WINDOW(clock->widget), TRUE);
  gtk_window_set_accept_focus(GTK_WINDOW(clock->widget), FALSE);
  gtk_window_stick(GTK_WINDOW(clock->widget));
  gtk_widget_set_app_paintable(clock->widget, TRUE);
  gtk_window_set_default_size(GTK_WINDOW(clock->widget), clock->width, clock->height);

  // TODO: option
  gtk_window_move(GTK_WINDOW(clock->widget), 10, 13);



  g_signal_connect(GTK_WINDOW(clock->widget), "expose-event", G_CALLBACK(clock_expose), clock);
  g_signal_connect(GTK_WINDOW(clock->widget), "screen-changed", G_CALLBACK(clock_screen_changed), clock);

  clock_screen_changed(clock->widget, NULL, clock);
  g_timeout_add(1000, &clock_update, clock);

  gtk_widget_show_all(clock->widget);

  return clock;
}


//
// Some help
//
void help()
{
  printf("USAGE: TODO\n");
}

//
// Main
//
#define NEXT_ARG()                                                   \
  do {                                                               \
     arg_id++;                                                       \
     if (arg_id >= argc) {                                           \
       printf("Option '%s' need an argument! \n", argv[arg_id - 1]); \
       exit(1); }                                                    \
  } while (0)                                                        \


int main(int argc, char **argv)
{
  uint32_t arg_id;
  for (arg_id = 1; arg_id < argc; arg_id++)
  {
    if (strcmp(argv[arg_id], "-h") == 0) {
      help();
      exit(0);
    }

    else if (strcmp(argv[arg_id], "--help") == 0) {
      help();
      exit(0);
    }

    else if (strcmp(argv[arg_id], "--bg") == 0) {
      NEXT_ARG();
      
    }



    else {
      printf("Unknown option '%s'. Try --help.\n", argv[arg_id]);
      exit(1);
    }

  }




  gtk_init(&argc, &argv);
  clock_create();
  gtk_main();

  return 0;
}
