#ifndef _HW5_SERVER_H_
#define _HW5_SERVER_H_

extern "C"{
    #include "../jansson/include/jansson.h"
    #include <gtk/gtk.h>
}

char* generate_helloack_message(char* file);
char* generate_bye_message();

//void view_construct_grid();
void view_construct_grid();
void usage(const char *exeName);
void view_create_candy(int candy, int boardStateNum, int row, int col);
void unselect_current_active_button();
void view_update_moves_label(int m);
void view_update_score_label(int s);
void view_redraw_grid();
void view_destroy_grid();
void view_destroy_candy(int row, int col);
void activate (GtkApplication *app, gpointer user_data);


void set_row(GtkWidget *widget, gpointer data);
void set_col(GtkWidget *widget, gpointer data);

void clicked_up();
void clicked_down();
void clicked_left();
void clicked_right();


// void clicked_direction_btn( gpointer data );
// char* generate_update_message();

#endif // _HW5_SERVER_H_