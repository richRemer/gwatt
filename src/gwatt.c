#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>

#define GWATT_REFRESH_INTERVAL 15
// #define GWATT_EVENTS

extern unsigned char gwatt_css[];
extern unsigned int gwatt_css_len;

static int read_file_value(const char* file) {
  FILE* f;
  long len;
  char* contents;

  f = fopen(file, "r");
  fseek(f, 0, SEEK_END);
  len = ftell(f);
  fseek(f, 0, SEEK_SET);

  contents = malloc(len + 1);
  fread(contents, len, 1, f);
  fclose(f);
  contents[len] = 0;

  int result = atoi(contents);
  free(contents);
  return result;
}

static int charge_full() {
  return read_file_value("/sys/class/power_supply/BAT0/charge_full");
}

static int charge_now() {
  return read_file_value("/sys/class/power_supply/BAT0/charge_now");
}

static double charge_percent() {
  return (double)charge_now() / (double)charge_full();
}

static gboolean refresh_meter(gpointer data) {
  GtkWidget* meter = (GtkWidget*)data;
  gtk_level_bar_set_value(GTK_LEVEL_BAR(meter), charge_percent());
  return TRUE;
}

void init_styles() {
    GtkCssProvider* styles;
    GdkScreen* screen;
    GdkDisplay* display;

    display = gdk_display_get_default();
    screen = gdk_display_get_default_screen(display);
    styles = gtk_css_provider_new();

    gtk_css_provider_load_from_data(styles, gwatt_css, gwatt_css_len, NULL);
    gtk_style_context_add_provider_for_screen(
        screen,
        GTK_STYLE_PROVIDER(styles),
        GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
    g_object_unref(styles);
}

int main(int argc, char* argv[]) {
  GtkWidget* window;
  GtkWidget* ebox;
  GtkWidget* meter;

  gtk_init(&argc, &argv);

  init_styles();

  meter = gtk_level_bar_new();
  gtk_level_bar_set_value(GTK_LEVEL_BAR(meter), 0.0);
  gtk_level_bar_set_min_value(GTK_LEVEL_BAR(meter), 0.0);
  gtk_level_bar_set_max_value(GTK_LEVEL_BAR(meter), 1.0);

#ifdef GWATT_EVENTS
  ebox = gtk_event_box_new();
  gtk_container_add(GTK_CONTAINER(ebox), meter);
#endif

  window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title(GTK_WINDOW(window), "Power Meter");
  gtk_window_set_default_size(GTK_WINDOW(window), 250, 50);
  gtk_window_set_decorated(GTK_WINDOW(window), FALSE);
#ifdef GWATT_EVENTS
  gtk_container_add(GTK_CONTAINER(window), ebox);
#else
  gtk_container_add(GTK_CONTAINER(window), meter);
#endif
  g_signal_connect(G_OBJECT(window), "destroy", gtk_main_quit, NULL);

  g_timeout_add_seconds(GWATT_REFRESH_INTERVAL, refresh_meter, meter);
  refresh_meter(meter);

  gtk_widget_show_all(window);
  gtk_main();

  return 0;
}
