#include <iostream>
#include <string>
#include <cstdlib>
#include <memory>

#include "ServerSocket.h"
#include "ClientSocket.h"
#include "hw5_server.h"


extern "C"{
    #include "../jansson/include/jansson.h"
    #include <gtk/gtk.h>
}
using namespace std;

const char* color_state1_files[7] = {"../images/regular/state1/blue.png", "../images/regular/state1/green.png", "../images/regular/state1/orange.png",
                              "../images/regular/state1/purple.png", "../images/regular/state1/red.png", "../images/regular/state1/yellow.png",
                              "../images/regular/state1/nocolor.png"};
const char* color_state0_files[7] = {"../images/regular/state0/blue.png", "../images/regular/state0/green.png", "../images/regular/state0/orange.png",
                              "../images/regular/state0/purple.png", "../images/regular/state0/red.png", "../images/regular/state0/yellow.png",
                              "../images/regular/state0/nocolor.png"};
long selected_row;
long selected_col;
long selected_dir;
int selected_candy_bool = 0;
int rows, cols;

//shared ptr to reserve peerSocket in gtk callbacks
std::shared_ptr<hw5_net::ClientSocket> peerSocket_ptr;

GtkWidget *grid;
GtkWidget *window;
GtkWidget *moves_label;
GtkWidget *score_label;






void activate (GtkApplication *app, gpointer user_data) {

   window = gtk_application_window_new (app);
   gtk_window_set_title (GTK_WINDOW (window), "Window");
   gtk_window_set_default_size (GTK_WINDOW (window), -1, -1);
   gtk_container_set_border_width (GTK_CONTAINER (window), 10);
   grid = gtk_grid_new();
   gtk_container_add (GTK_CONTAINER (window), grid);

   moves_label = gtk_label_new (NULL);
   score_label = gtk_label_new(NULL);

   view_construct_grid();

   GtkButton *button;
   button = (GtkButton*) gtk_button_new_with_label (NULL);
   gtk_button_set_image(button, gtk_image_new_from_file ("../images/direction/up.png"));
   g_signal_connect (button, "clicked", G_CALLBACK (clicked_up), NULL);
   gtk_grid_attach (GTK_GRID (grid), (GtkWidget*) button, cols, 0, 1, 1);

   button = (GtkButton*) gtk_button_new_with_label (NULL);
   gtk_button_set_image(button, gtk_image_new_from_file ("../images/direction/down.png"));
   g_signal_connect (button, "clicked", G_CALLBACK (clicked_down), NULL);
   gtk_grid_attach (GTK_GRID (grid), (GtkWidget*) button, cols, 1, 1, 1);

   button = (GtkButton*) gtk_button_new_with_label (NULL);
   gtk_button_set_image(button, gtk_image_new_from_file ("../images/direction/left.png"));
   g_signal_connect (button, "clicked", G_CALLBACK (clicked_left), NULL);
   gtk_grid_attach (GTK_GRID (grid), (GtkWidget*) button, cols, 2, 1, 1);

   button = (GtkButton*) gtk_button_new_with_label (NULL);
   gtk_button_set_image(button, gtk_image_new_from_file ("../images/direction/right.png"));
   g_signal_connect (button, "clicked", G_CALLBACK (clicked_right), NULL);
   gtk_grid_attach (GTK_GRID (grid), (GtkWidget*) button, cols, 3, 1, 1);


   gtk_grid_attach (GTK_GRID (grid), moves_label, cols, 4, 1, 1);
   gtk_grid_attach (GTK_GRID (grid), score_label, cols, 5, 1, 1);

   gtk_widget_show_all (window);

}


int main(int argc, char *argv[]) {
  if ( argc != 2 ) usage(argv[0]);
  int port = 0;


  try {
    int socketFd;
    hw5_net::ServerSocket sock(port);
    sock.BindAndListen(AF_INET, &socketFd);
    cout << "Created bound socket. port = " << sock.port() << endl;

    // wait for client connection
    int acceptedFd;
    string clientAddr;
    uint16_t clientPort;
    string clientDNSName;
    string serverAddress;
    string serverDNSName;

    sock.Accept( &acceptedFd,
     &clientAddr,
     &clientPort,
     &clientDNSName,
     &serverAddress,
     &serverDNSName );

    cout << "Got connection from: " << endl
   << "\tacceptedFd\t" << acceptedFd << endl
   << "\tclientPort\t" << clientPort << endl
   << "\tclientDNSName\t" << clientDNSName << endl
   << "\tserverAddress\t" << serverAddress << endl
   << "\tserverDNSName\t" << serverDNSName << endl;

    // wrap connection to peer with a CientSocket
    //hw5_net::ClientSocket peerSocket(acceptedFd);


    // std::shared_ptr<hw5_net::ClientSocket> 
    //               peerSocket_ptr (new hw5_net::ClientSocket(acceptedFd));

   peerSocket_ptr =   std::shared_ptr<hw5_net::ClientSocket>  (new hw5_net::ClientSocket(acceptedFd));
    cout << "Reading" << endl;

    char buf[2048];
    int readCount;
    bool said_hello = 0;
    //keep reading until gets hello
    while (said_hello == 0) {
      // write to stdout
      readCount = (*peerSocket_ptr).WrappedRead(buf, 2047); //waits for client to write hello
      buf[readCount] = '\0'; // make sure buf holds a c style string
      cout << "Got '" << buf << "'" << endl;
      json_t* hello_json = json_loads(buf, JSON_COMPACT, NULL);
      json_t* action_json = json_object_get(hello_json, "action");
      const char* hello = json_string_value(action_json);

      const char* temp = "hello";
      bool match_hello = 1;
      for (int i = 0; i<6; i++){
        if(hello[i] != temp[i]){
          cout<< hello[i] << endl;
          match_hello = 0;
        }
      }
      if (match_hello == 1){
        cout << "received hello from client.." << endl;
        
        said_hello = 1;
        match_hello = 0;

        char* helloack_message = generate_helloack_message(argv[1]);
        (*peerSocket_ptr).WrappedWrite(helloack_message, 2047);  //TO-DO: not safe here if gameboard is large, convert to string then use str.length()?
        cout << "sending helloack >>" << helloack_message << endl;
      } 
      
    } //end of while }


   GtkApplication *app;
   // int status;
   app = gtk_application_new ("candy.crush", G_APPLICATION_FLAGS_NONE);
   g_signal_connect (app, "activate", G_CALLBACK(activate), (gpointer) &peerSocket_ptr);
   g_application_run (G_APPLICATION (app), 0, argv);

 
  } catch (string errString) {         //end of try }
    cerr << errString << endl;
    return EXIT_FAILURE;
  }
  
  char* bye_message = generate_bye_message();
  (*peerSocket_ptr).WrappedWrite(bye_message, 2047); 

  cout << "Closing" << endl;


  
  return EXIT_SUCCESS;
}



///---------------server helper methods----------------------///
void usage(const char *exeName) {
  cout << "Usage: " << exeName << " filename" << endl;
  exit(1);
}

char* generate_helloack_message(char* file){
  json_t* json_from_file = json_load_file(file, JSON_COMPACT, NULL);
  json_t* json_final = json_object();
  json_object_set(json_final, "action", json_string("helloack"));
  json_object_set(json_final, "gameinstance", json_from_file);

  return json_dumps(json_final, JSON_COMPACT); 
}

char* generate_bye_message(){
  json_t* json_final = json_object();
  json_object_set(json_final, "action", json_string("bye"));
  return json_dumps(json_final, JSON_COMPACT); 
}

//----------------new view methods-------------------------///

void view_redraw_grid() {
   view_destroy_grid();
   view_construct_grid();
}
void view_destroy_grid() {
   for (int row = 0; row < rows; row++) {
      for (int column = 0; column < cols; column++){
         view_destroy_candy(row, column);
      }
   }
}
//also constructes move and score labels
void view_construct_grid(){
   //reterive update message
    selected_candy_bool = 0;
   int readCount1 = 0;
   char buff[2048];
   readCount1 = (*peerSocket_ptr).WrappedRead(buff, 2047);
   cout << "Got update >>>>" << buff  << endl << endl;
   buff[readCount1] = '\0'; // make sure buf holds a c style string
   
   //encode to json object
   json_t* message_json = json_loads(buff, JSON_COMPACT, NULL);  //remeber to json_decref()
   json_t* gameinstance_json = json_object_get(message_json, "gameinstance");
    json_t* gamedef_json = json_object_get(gameinstance_json, "gamedef");
    json_t* gamestate_json = json_object_get(gameinstance_json, "gamestate");
      json_t* boardstate_json = json_object_get(gamestate_json, "boardstate");
      json_t* boardcandies_json = json_object_get(gamestate_json, "boardcandies");

   rows = json_integer_value(json_object_get(boardcandies_json, "rows"));
   cols = json_integer_value(json_object_get(boardcandies_json, "cols"));



   json_t* boardcandies_data_json = json_object_get(boardcandies_json, "data");
   json_t* boardstate_data_json = json_object_get(boardstate_json, "data");

   //cout<< "checking decoding gameinstance, boardcandies_data size should be 36 -------output: " << json_array_size(boardcandies_data_json)<< endl;
   for (int row = 0; row < rows; row++){
    for (int col = 0; col < cols; col++){
      int curr = row * rows + col;
      json_t* candy = json_array_get(boardcandies_data_json, curr);
      int color = json_integer_value(json_object_get(candy, "color"));
      int state = json_integer_value(json_array_get(boardstate_data_json, curr));
      
      view_create_candy(color, state, row, col);
    }
   }
   int movesallowed = json_integer_value(json_object_get(gamedef_json, "movesallowed"));
   int movesmade = json_integer_value(json_object_get(gamestate_json, "movesmade"));
   int score = json_integer_value(json_object_get(gamestate_json, "currentscore"));
   //draw score and moves label
   view_update_moves_label(movesallowed - movesmade);
   
   view_update_score_label(score);
   //cout << "(inside construct grid---) after write" << endl;
   //socket gets deleted after this
}


void view_create_candy(int candy, int boardStateNum, int row, int col){
   GtkButton *button = (GtkButton*) gtk_toggle_button_new();
   if (boardStateNum > 0) {
      gtk_button_set_image(button, gtk_image_new_from_file (color_state1_files[candy]));
   } else {
      gtk_button_set_image(button, gtk_image_new_from_file (color_state0_files[candy]));
   }
   gtk_button_set_relief (button, GTK_RELIEF_NONE);

   g_signal_connect (button, "clicked", G_CALLBACK (set_row), (gpointer) row);
   g_signal_connect (button, "clicked", G_CALLBACK (set_col), (gpointer) col);  
   gtk_grid_attach (GTK_GRID (grid), (GtkWidget*) button, col, rows - 1 - row, 1, 1);   //col,row <--> left offset, top offset

   gtk_widget_show_all (window);
}

void view_destroy_candy(int row, int col){
  gtk_widget_destroy(gtk_grid_get_child_at(GTK_GRID (grid), row, col));
}

void view_update_moves_label(int m){
   gchar *label_text;
   label_text = g_strdup_printf("moves left: %d", m);
   gtk_label_set_text ((GtkLabel*) moves_label, label_text);
   free(label_text);

}
void view_update_score_label(int s){
   gchar *label_text;
   label_text = g_strdup_printf("score: %d", s);
   gtk_label_set_text ((GtkLabel*) score_label, label_text);
   free(label_text);
}





//----------------new controller methods-------------------------///
void unselect_current_active_button(){
   gtk_toggle_button_set_active(
      (GtkToggleButton*)gtk_grid_get_child_at(GTK_GRID (grid), selected_col, rows - 1 - selected_row), 
      false);   
}

void set_row(GtkWidget *widget, gpointer data) {
   unselect_current_active_button();
   selected_row = (long) data;
}

// connected to each candy to set col
void set_col(GtkWidget *widget, gpointer data) {
   selected_col = (long) data;
   selected_candy_bool = 1;
}



void clicked_up(){
  if (selected_candy_bool == 1){
    json_t* json_final = json_object();
    json_object_set(json_final, "action", json_string("move"));
    json_object_set(json_final, "row", json_integer(selected_row));
    json_object_set(json_final, "col", json_integer(selected_col));
    json_object_set(json_final, "direction", json_integer(2));
    char* updateMessage = json_dumps(json_final, JSON_COMPACT); 
    cout << "sending >>> "<< updateMessage << endl;
    (*peerSocket_ptr).WrappedWrite(updateMessage, 2048);
    view_redraw_grid();
  }
  else{
    cout<< "please select a candy !" << endl;
  }
}

void clicked_down(){
  if (selected_candy_bool == 1){
    json_t* json_final = json_object();
    json_object_set(json_final, "action", json_string("move"));
    json_object_set(json_final, "row", json_integer(selected_row));
    json_object_set(json_final, "col", json_integer(selected_col));
    json_object_set(json_final, "direction", json_integer(3));
    char* updateMessage = json_dumps(json_final, JSON_COMPACT); 
    cout << "sending >>> "<< updateMessage << endl;
    (*peerSocket_ptr).WrappedWrite(updateMessage, 2048);
    view_redraw_grid();
  }
  else{
    cout<< "please select a candy !" << endl;
  }
}

void clicked_left(){
  if (selected_candy_bool == 1){
    json_t* json_final = json_object();
    json_object_set(json_final, "action", json_string("move"));
    json_object_set(json_final, "row", json_integer(selected_row));
    json_object_set(json_final, "col", json_integer(selected_col));
    json_object_set(json_final, "direction", json_integer(0));
    char* updateMessage = json_dumps(json_final, JSON_COMPACT); 
    cout << "sending >>> "<< updateMessage << endl;
    (*peerSocket_ptr).WrappedWrite(updateMessage, 2048);
    view_redraw_grid();
  }
  else{
    cout<< "please select a candy !" << endl;
  }
}

void clicked_right(){
  if (selected_candy_bool == 1){
    json_t* json_final = json_object();
    json_object_set(json_final, "action", json_string("move"));
    json_object_set(json_final, "row", json_integer(selected_row));
    json_object_set(json_final, "col", json_integer(selected_col));
    json_object_set(json_final, "direction", json_integer(1));
    char* updateMessage = json_dumps(json_final, JSON_COMPACT); 
    cout << "sending >>> "<< updateMessage << endl;
    (*peerSocket_ptr).WrappedWrite(updateMessage, 2048);
    view_redraw_grid();
  }
  else{
    cout<< "please select a candy !" << endl;
  }
}

// void dosomething(gpointer user_data){

//  //if selected candy, generate a move message and send to client
//   cout << "in dosomething, user_data (peerSocket_ptr address) is " << &peerSocket_ptr << endl;
//   if (selected_candy_bool == 1){
//       cout << "in dosomething, user_data (peerSocket_ptr address) is " << user_data << endl;
//       cout << "(inside dosomething---) about to send an action to client" << endl;
//       (*peerSocket_ptr).WrappedWrite(fakemove.c_str(), fakemove.length());
//       cout << "(inside dosomething---) I just sent an action" << endl;
//   }else{
//     cout<< "please select a candy !" << endl;
//   }
// }