/*
 * Initial main.c file generated by Glade. Edit as required.
 * Glade will not overwrite this file.
 */

/*
#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif
*/
#  include <config.h>

#include <gtk/gtk.h>

#include "interface.h"
#include "support.h"
#include "callbacks.c"

#include "gui_throbber.h"
#include <nemesi/egui.h>
#include <nemesi/etui.h>
#include <nemesi/comm.h>

int gui(struct RTSP_args *rtsp_args, NMSUiHints *ui_hints, int argc, char *argv[])
{
  GtkWidget *nemesi;
  char *path;

  gtk_set_locale ();
  gtk_init (&argc, &argv);

  add_pixmap_directory (PACKAGE_DATA_DIR "/" PACKAGE "/pixmaps");
  // add_pixmap_directory (PACKAGE_DATA_DIR "/" PACKAGE "/throbber");
  path=getenv(NEMESI_THROBBER_DIR_ENV);
  if (!path) {
  	add_pixmap_directory(NEMESI_THROBBER_DIR_DEFAULT);
	nmsprintf(3, "NEMESI_THROBBER_DIR_DEFAULT: "NEMESI_THROBBER_DIR_DEFAULT"\n");
  } else {
  	add_pixmap_directory(path);
	nmsprintf(3, "NEMESI_THROBBER_DIR_ENV: %s\n", path);
  }

  /*
   * The following code was added by Glade to create one of each component
   * (except popup menus), just so that you see something after building
   * the project. Delete any components that you don't want shown initially.
   */
  nemesi = create_nemesi ();

  save_static_data(nemesi, rtsp_args); // must be done fist of all

  if (create_throbber(lookup_widget(nemesi, "hbox3")))
	  nmserror("no throbber available");

	update_toolbar();
	if (ui_hints->url) {
		nmsprintf(3, "Connect: Please wait, opening \"%s\"", ui_hints->url);
		send_open(rtsp_args, ui_hints->url);
		gui_throbber(&rtsp_args->rtsp_th->busy);
	} else
		nmsprintf(3, "Please, enter a command or press 'h' for help\n\n");
  gtk_widget_show (nemesi);

  gtk_main ();
  return 0;
}

