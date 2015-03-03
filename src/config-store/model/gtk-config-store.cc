/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 *  This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 * 
 * Authors: Faker Moatamri <faker.moatamri@sophia.inria.fr>
 *          Mathieu Lacage <mathieu.lacage@sophia.inria.fr>
 */

#include "gtk-config-store.h"
#include "raw-text-config.h"
#include "display-functions.h"
#include "ns3/log.h"
#include <fstream>


namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("GtkconfigStore");

GtkConfigStore::GtkConfigStore ()
{
}

std::string
get_sort_key (ModelTypeid * node)
{
  switch (node->type)
    {
    case ModelTypeid::NODE_TYPEID:
      return node->tid.GetName ();
    case ModelTypeid::NODE_ATTRIBUTE:
      // We just return an empty string here so that all attributes compare
      // as equal.  This way they come up in the order they were defined.
      // If we wanted to alphebetize them, we'd return node->name.
      return "";
      //return node->name;
    }
  return ""; // Shouldn't happen
}

gint
compare_modeltypeid (GtkTreeModel *model,
                     GtkTreeIter  *a,
                     GtkTreeIter  *b,
                     gpointer     userdata)
{
  ModelTypeid *n1, *n2;

  gtk_tree_model_get(model, a, 0, &n1, -1);
  gtk_tree_model_get(model, b, 0, &n2, -1);

  if (!n1 || !n2)
    {
      if (!n1 && !n2) return 0;
      return (n2) ? -1 : 1;
    }

  return get_sort_key(n1).compare(get_sort_key(n2));
}

void
GtkConfigStore::ConfigureDefaults (void)
{
  //this function should be called before running the script to enable the user
  //to configure the default values for the objects he wants to use
  GtkWidget *window;
  GtkWidget *view;
  GtkWidget *scroll;

  gtk_init (0, 0);
  window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title (GTK_WINDOW (window), "ns-3 Default attributes.");
  gtk_window_set_default_size (GTK_WINDOW (window), 600, 600);

  g_signal_connect (window, "delete_event", (GCallback)delete_event_callback, window);
  GtkTreeStore *model = gtk_tree_store_new (COL_LAST, G_TYPE_POINTER);
  ModelTypeidCreator creator;
  creator.Build (model);

  GtkTreeSortable *sortable = GTK_TREE_SORTABLE(model);
  gtk_tree_sortable_set_sort_func(sortable, 0, compare_modeltypeid, 0, NULL);
  gtk_tree_sortable_set_sort_column_id(sortable, 0, GTK_SORT_ASCENDING);

  view = create_view_config_default (model);
  scroll = gtk_scrolled_window_new (0, 0);
  gtk_container_add (GTK_CONTAINER (scroll), view);

  GtkWidget *vbox = gtk_vbox_new (FALSE, 5);
  gtk_box_pack_start (GTK_BOX (vbox), scroll, TRUE, TRUE, 0);
  gtk_box_pack_end (GTK_BOX (vbox), gtk_hseparator_new (), FALSE, FALSE, 0);
  GtkWidget *hbox = gtk_hbox_new (FALSE, 5);
  gtk_box_pack_end (GTK_BOX (vbox), hbox, FALSE, FALSE, 0);
  GtkWidget *save = gtk_button_new_with_label ("Save");
  g_signal_connect (save, "clicked",  (GCallback) save_clicked_default, window);
  gtk_box_pack_end (GTK_BOX (hbox), save, FALSE, FALSE, 0);
  GtkWidget *load = gtk_button_new_with_label ("Load");
  g_signal_connect (load, "clicked",  (GCallback) load_clicked_default, window);
  gtk_box_pack_end (GTK_BOX (hbox), load, FALSE, FALSE, 0);
  GtkWidget *exit = gtk_button_new_with_label ("Run Simulation");
  g_signal_connect (exit, "clicked",  (GCallback) exit_clicked_callback, window);
  gtk_box_pack_end (GTK_BOX (hbox), exit, FALSE, FALSE, 0);

  gtk_container_add (GTK_CONTAINER (window), vbox);

  gtk_widget_show_all (window);

  gtk_main ();

  gtk_tree_model_foreach (GTK_TREE_MODEL (model), 
                          clean_model_callback_config_default,
                          0);

  gtk_widget_destroy (window); 
}

void 
GtkConfigStore::ConfigureAttributes (void)
{
  GtkWidget *window;
  GtkWidget *view;
  GtkWidget *scroll;

  gtk_init (0, 0);

  window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title (GTK_WINDOW (window), "ns-3 Object attributes.");
  gtk_window_set_default_size (GTK_WINDOW (window), 600, 600);

  g_signal_connect (window, "delete_event", (GCallback)delete_event_callback, window);


  GtkTreeStore *model = gtk_tree_store_new (COL_LAST, G_TYPE_POINTER);
  ModelCreator creator;
  creator.Build (model);

  view = create_view (model);
  scroll = gtk_scrolled_window_new (0, 0);
  gtk_container_add (GTK_CONTAINER (scroll), view);

  GtkWidget *vbox = gtk_vbox_new (FALSE, 5);
  gtk_box_pack_start (GTK_BOX (vbox), scroll, TRUE, TRUE, 0);
  gtk_box_pack_end (GTK_BOX (vbox), gtk_hseparator_new (), FALSE, FALSE, 0);
  GtkWidget *hbox = gtk_hbox_new (FALSE, 5);
  gtk_box_pack_end (GTK_BOX (vbox), hbox, FALSE, FALSE, 0);
  GtkWidget *save = gtk_button_new_with_label ("Save");
  g_signal_connect (save, "clicked",  (GCallback) save_clicked, window);
  gtk_box_pack_end (GTK_BOX (hbox), save, FALSE, FALSE, 0);
  GtkWidget *load = gtk_button_new_with_label ("Load");
  g_signal_connect (load, "clicked",  (GCallback) load_clicked, window);
  gtk_box_pack_end (GTK_BOX (hbox), load, FALSE, FALSE, 0);
  GtkWidget *exit = gtk_button_new_with_label ("Run Simulation");
  g_signal_connect (exit, "clicked",  (GCallback) exit_clicked_callback, window);
  gtk_box_pack_end (GTK_BOX (hbox), exit, FALSE, FALSE, 0);

  gtk_container_add (GTK_CONTAINER (window), vbox);

  gtk_widget_show_all (window);

  gtk_main ();

  gtk_tree_model_foreach (GTK_TREE_MODEL (model), 
                          clean_model_callback,
                          0);

  gtk_widget_destroy (window);
}

} // namespace ns3
