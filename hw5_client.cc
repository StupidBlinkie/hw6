//client-----model

#include "ClientSocket.h"
extern "C"{
   #include "../hw2/array2d.h"
   #include "../jansson/include/jansson.h"
}
#include "hw5_model.h"

#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>

using namespace std;


// gameDef* g_def;
// gameState* g_state;

void usage(const char *exeName) {
  cout << "Usage: " << exeName << " host port" << endl;
  cout << "  Connect to host::port then send data and read replies" << endl;
  exit(1);
}

char* generate_hello_message(){
  json_t* json_final = json_object();
  json_object_set(json_final, "action", json_string("hello"));
  return json_dumps(json_final, JSON_COMPACT); 
}


int main(int argc, char *argv[]) {

  if ( argc != 3 ) usage(argv[0]);

  int serverPort;
  try {
    serverPort = stoi(argv[2]);
  } catch (...) {
    usage(argv[0]);
  }

  try {
    string serverName(argv[1]);
    hw5_net::ClientSocket clientSocket(serverName, serverPort);
    
    char buf[2048];

    char* hello_message = generate_hello_message();
    clientSocket.WrappedWrite(hello_message, 2047);   


    int readCount = clientSocket.WrappedRead(buf, 2047);  //waits for server write helloack
    buf[readCount] = '\0';

    

    json_t * gamedata = json_loads(buf, JSON_COMPACT, NULL);
    
    gameDef* g_def = new gameDef();
    gameState* g_state = new gameState();

    deserialize(gamedata, g_def, g_state);  //loads g_def & g_state
    g_state->applyTemplate();

    string first_update(serialize(g_state));
    clientSocket.WrappedWrite(first_update.c_str(),first_update.length());

    int bye = 0;
    while (bye == 0){
      readCount = clientSocket.WrappedRead(buf, 2047);
      buf[readCount] = '\0';
      if(readCount > 1){   //read count is 1 if read ''

      
      //decode update message
      json_t* message_json = json_loads(buf, JSON_COMPACT, NULL);
      json_t* action_json = json_object_get(message_json, "action");
      const char* action = json_string_value(action_json);


      const char* temp = "bye";
      int match = 1;
      for (int i = 0; i< 4; i++){
        if(action[i] != temp[i]){
          match = 0;
        }
      }
      if (match == 1){
        bye = 1;
        break;
      }


      int row = json_integer_value(json_object_get(message_json, "row"));
      int col = json_integer_value(json_object_get(message_json, "col"));
      int dir = json_integer_value(json_object_get(message_json, "direction"));


      if (dir == 0){  //left
        g_state->swap_candy_elements(row, col, row , col - 1); 
        if (!g_state->applyTemplate()) {g_state->swap_candy_elements(row, col, row , col - 1); } else {g_state->incre_movesMade();}
      }
      else if (dir == 1){  //right
        g_state->swap_candy_elements(row, col, row , col + 1);
        if (!g_state->applyTemplate()) {g_state->swap_candy_elements(row, col, row , col + 1); } else {g_state->incre_movesMade();}
      }
      else if (dir == 2){  //up
        g_state->swap_candy_elements(row, col, row + 1 , col);
        if (!g_state->applyTemplate()) {g_state->swap_candy_elements(row, col, row + 1 , col); } else {g_state->incre_movesMade();}
      }
      else if (dir == 3){  //down
        g_state->swap_candy_elements(row, col, row - 1, col);
        if (!g_state->applyTemplate()) {g_state->swap_candy_elements(row, col, row - 1, col); } else {g_state->incre_movesMade();}
      }
      else{
        cout << "invalid direction received" << endl;
      }

      char* update_message = serialize(g_state);
      clientSocket.WrappedWrite(update_message,2047);
    }


    }


  } catch(string errString) {
    cerr << errString << endl;
    return EXIT_FAILURE;
  }



  cout << "Exiting..." << endl;

  return EXIT_SUCCESS;
}







/*---------------------------serialize/deserilaize--------------------------------------*/
void deserialize2dArray(json_t *json, gameDef* &g_def, bool reading_first_array){
    //read rows
    json_t* json_row = json_object_get(json, "rows");
    int rows = json_integer_value(json_row); 
    //read cols
    json_t* json_col = json_object_get(json, "columns");
    int cols = json_integer_value(json_col);
  
    //read json array
    json_t* json_data = json_object_get(json, "data");

    //store in an temporary int array
    int* data = (int*)malloc(rows * cols *sizeof(int));
        for (size_t i = 0; i < json_array_size(json_data); i++) {
            data[i] = json_integer_value(json_array_get(json_data, i));
    }
    

    //load to different fileds of g_def
    if (reading_first_array){
      g_def->set_extensionColor(rows, cols, data);
    }
    else {
      g_def->set_boardState(rows,cols,data);
    }
    
    //g_def set functions deep copied the data, so free immediately after use
    free(data); 
}

//read json and load into 2d array arr
void deserialize(json_t* json, gameDef* &g_def, gameState* &g_state){
    //cout << "starting deserialize" << endl;
    json_t* gameinstance_json = json_object_get(json, "gameinstance");

    json_t* gamedef_json = json_object_get(gameinstance_json, "gamedef");
    if (gamedef_json == NULL) {
      cout << "did not find gamedef" << endl;
    }

    json_t *extension = json_object_get(gamedef_json, "extensioncolor");
    if (extension == NULL) {
      cout << "did not find extensioncolor" << endl;
    }
    deserialize2dArray(extension, g_def, 1); //1 and 0 to indicate if reading first or second array
    //iterate to boardstate field
    json_t *boardstate = json_object_get(gamedef_json, "boardstate");
    deserialize2dArray(boardstate, g_def, 0);
    
    //iterate to moves allowed
    int movesAllowed = json_integer_value(json_object_get(gamedef_json, "movesallowed"));

    //iterate to id
    int id = json_integer_value(json_object_get(gamedef_json, "gameid"));
    //iterate to color
    int colors = json_integer_value(json_object_get(gamedef_json, "colors"));

    g_def->set_gameID(id);
    g_def->set_movesAllowed(movesAllowed);
    g_def->set_colors(colors);

    json_t *gamestate_json = json_object_get(gameinstance_json, "gamestate");


    g_state->initialize(g_def, gamestate_json);
    json_decref(json);

}


char* serialize(gameState* &g_state){ 
  json_t* obj = json_object();

  //------------------serialize gamedef--------------------
  json_t* gamedef = json_object();
  //serialize extensioncolor
  json_t* extensioncolor = json_object();

  gameDef* g_def = g_state->get_g_def_ref();  

  json_t* json_exten_row = json_integer(g_def->get_extensionColor_rows());
  json_t* json_exten_col = json_integer(g_def->get_extensionColor_cols());
  json_t* json_exten_data = json_array();
  for (int r=0; r < g_def->get_extensionColor_rows(); r++){
    for(int c=0; c < g_def->get_extensionColor_cols(); c++){
      json_t* element =  json_integer(*(int*)g_def->get_extensionColor_element(r, c));
      json_array_append(json_exten_data, element);
      json_decref(element);
    }
  }
  json_object_set(extensioncolor, "rows", json_exten_row);
  json_object_set(extensioncolor, "columns", json_exten_col);
  json_object_set(extensioncolor, "data", json_exten_data);
  
  //serialize boardstate
  json_t* boardstate = json_object();
  json_t* json_board_row = json_integer(g_def->get_boardState_rows());
  json_t* json_board_col = json_integer(g_def->get_boardState_cols());  
  json_t* json_board_data = json_array();
  for (int r=0; r < g_def->get_boardState_rows(); r++){
    for(int c=0; c < g_def->get_boardState_cols(); c++){
      json_t* element =  json_integer(*(int*)g_def->get_boardState_element(r, c));
      json_array_append(json_board_data, element);
      json_decref(element);
    }
  }
  json_object_set(boardstate, "rows", json_board_row);
  json_object_set(boardstate, "columns", json_board_col);
  json_object_set(boardstate, "data", json_board_data);

  json_t* gameid = json_integer(g_def->get_gameID());
  json_t* movesallowed = json_integer(g_def->get_movesAllowed());
  json_t* colors = json_integer(g_def-> get_colors());
  json_object_set(gamedef, "extensioncolor", extensioncolor);
  json_object_set(gamedef, "boardstate", boardstate);
  json_object_set(gamedef, "movesallowed", movesallowed);
  json_object_set(gamedef, "gameid", gameid);
  json_object_set(gamedef, "colors", colors);

  //-------END --- serialize gamedef-----------------------------



  //---------serialize gamestate----------------------------------
  json_t* gamestate = json_object();

  //serialize boardcandies
  json_t* boardcandies = json_object();
  json_t* json_gamestate_row = json_integer(g_state->get_rows());
  json_t* json_gamestate_col = json_integer(g_state->get_cols());  
  json_t* json_boardcandies_data = json_array(); 
  for (int r=0; r < g_state->get_rows(); r++){
    for(int c=0; c < g_state->get_cols(); c++){
      json_t* candy = json_object();
      json_t* candy_color = json_integer(g_state->get_candy_color(r, c));
      json_t* candy_type = json_integer(g_state->get_candy_type(r, c));
      json_object_set(candy, "color", candy_color);
      json_object_set(candy, "type", candy_type);
      //append candy element to data
      json_array_append(json_boardcandies_data, candy);
      json_decref(candy); 
      json_decref(candy_color); 
      json_decref(candy_type); 
    }
  }
  json_object_set(boardcandies, "rows", json_gamestate_row);
  json_object_set(boardcandies, "cols", json_gamestate_col);
  json_object_set(boardcandies, "data", json_boardcandies_data);

  //serialize boardstate
  json_t* gstate_boardstate = json_object(); 
  json_t* json_boardstate_data = json_array();
  for (int r=0; r < g_state->get_rows(); r++){
    for(int c=0; c < g_state->get_cols(); c++){
      json_t* element =  json_integer(*(int*)g_state->get_boardState_element(r, c));
      json_array_append(json_boardstate_data, element);
      json_decref(element);
    }
  }
  json_object_set(gstate_boardstate, "rows", json_gamestate_row);
  json_object_set(gstate_boardstate, "columns", json_gamestate_col);
  json_object_set(gstate_boardstate, "data", json_boardstate_data);

  //serialize extensionoffset
  json_t* extensionoffset = json_array();
  for (int i=0; i < g_state->get_cols(); i++){
      json_t* element =  json_integer(g_state->get_extensionOffset(i));
      json_array_append(extensionoffset, element);
      json_decref(element);
  }
  
  json_t* movesmade = json_integer(g_state->get_movesMade());
  json_t* currentscore = json_integer(g_state->get_currScore());
  json_object_set(gamestate, "boardcandies", boardcandies);
  json_object_set(gamestate, "boardstate", gstate_boardstate);
  json_object_set(gamestate, "movesmade", movesmade);
  json_object_set(gamestate, "currentscore", currentscore);
  json_object_set(gamestate, "extensionoffset", extensionoffset);

  //END ------------------------- serialize gamestate----------------------


  json_object_set(obj, "gamedef", gamedef);
  json_object_set(obj, "gamestate", gamestate);

  json_t* json_final = json_object();
  json_object_set(json_final, "action", json_string("update"));
  json_object_set(json_final, "gameinstance", obj);


  char* temp = json_dumps(json_final , JSON_COMPACT);



  //decref obj
  json_decref(obj);
  //decref gamedef
  json_decref(gamedef);
  json_decref(extensioncolor);
  json_decref(json_exten_row);
  json_decref(json_exten_col);
  json_decref(json_exten_data);
  json_decref(boardstate);
  json_decref(json_board_row);
  json_decref(json_board_col);
  json_decref(json_board_data);
  json_decref(gameid);
  json_decref(movesallowed);
  json_decref(colors);
  //decref game state
  json_decref(gamestate);
  json_decref(boardcandies);
  json_decref(json_gamestate_row);
  json_decref(json_gamestate_col);
  json_decref(json_boardcandies_data);
  json_decref(gstate_boardstate);
  json_decref(json_boardstate_data);
  json_decref(extensionoffset);
  json_decref(movesmade);
  json_decref(currentscore);

  return temp;

}
