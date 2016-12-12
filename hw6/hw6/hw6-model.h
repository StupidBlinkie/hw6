//update 12/8
// updated gameState class, now board processing
// methods are memeber functions of the class

#ifndef _HW5_MODEL_H_
#define _HW5_MODEL_H_


using namespace std;

extern "C"{
	#include "../hw2/array2d.h"
    	#include "../jansson/include/jansson.h"
}
#include <iostream>


//----------------Candy Class-------------------//
class candy{
	public:
	    candy(int t, int c);
	    int get_type() const {return type;}
	    int get_color() const {return color;}
	    void set_type(int t)  { type = t;}
	    void set_color(int c) { color = c;}
	private:
		int type;
		int color;
	};
	inline candy::candy(int t, int c){
	  type = t;
	  color = c;
}




class gameDef{
 private:
 	int gameID;
	int movesAllowed;
 	int colors;
   
   int maxScore;

 	int* internal_extencolor; //copies data read from json
 	int* internal_boardstate; //copies data read from json
 	Array2dPtr extensionColor; // collection of pointers to internal data storage
	Array2dPtr boardState;
	int extensionColor_row, extensionColor_col;
	int boardState_row, boardState_col;

 public:
 	//simple get/set
   int get_max_score() {return maxScore;}
   
	void set_gameID(int id){gameID = id;}
	void set_movesAllowed(int m) {movesAllowed = m;}
	void set_colors(int c) {colors = c;}
	int get_gameID(){return gameID;}
	int get_movesAllowed(){return movesAllowed;}
	int get_colors(){return colors;}
	int get_extensionColor_rows(){return extensionColor_row;}
 	int get_extensionColor_cols(){return extensionColor_col;}
 	int get_boardState_rows(){return boardState_row;}
 	int get_boardState_cols(){return boardState_col;}
 	
 	//long get/set, defined below with inline
 	void set_extensionColor(int rows, int cols, int* data_from_json);
 	void set_boardState(int rows, int cols,int* data_from_json);
 	void* get_extensionColor_element(int row, int col);
 	void* get_boardState_element(int row, int col);

	~gameDef(void);
};


inline void gameDef::set_extensionColor(int rows, int cols,int* data_from_json){
	//deep copy data. (to-do: try std::vetor<int>? check if data is valid)
	extensionColor_row = rows;
	extensionColor_col = cols;
	internal_extencolor = (int*)malloc(rows * cols * sizeof(int));

	for(int i=0; i<rows*cols; i++){
		internal_extencolor[i] = data_from_json[i];
	}

	//fill 2d array
	extensionColor = A2d_AllocateArray2d(rows, cols, sizeof(void*));
	
	for (int r=0; r<rows; r++) {
	  for (int c=0; c<cols; c++) {
	       A2d_FillArray2d(extensionColor, r, c, &internal_extencolor[r * cols + c]);
	   }
	}
}

inline void gameDef::set_boardState(int rows, int cols,int* data_from_json){
	boardState_row = rows;
	boardState_col = cols;
	internal_boardstate = (int*)malloc(rows * cols * sizeof(int));
   
   maxScore = 0;
	for(int i=0; i<rows*cols; i++){
		internal_boardstate[i] = data_from_json[i];
      maxScore += data_from_json[i];
	}

	boardState = A2d_AllocateArray2d(rows, cols, sizeof(void*));
	
	//fill 2d array
	for (int r=0; r<rows; r++) {
	  for (int c=0; c<cols; c++) {
	       A2d_FillArray2d(boardState, r, c, &internal_boardstate[r * cols + c]);
	   }
	}
}

inline void* gameDef::get_extensionColor_element(int row, int col){
	return A2d_GetElement(extensionColor, row, col);
}

inline void* gameDef::get_boardState_element(int row, int col){
	return A2d_GetElement(boardState, row, col);
}


inline gameDef::~gameDef(void){
  //cout << "gameDef Object is being deleted" << endl;
  free(extensionColor->storage);
  free(extensionColor);
  free(internal_extencolor);
  free(boardState->storage);
  free(boardState);
  free(internal_boardstate);
}




//----------------game state class-------------------//
class gameState{
	private: 
      int prevMoveRow;
      int prevMoveCol;
      int prevMoveDir;
      
      int potentialScore; //used to show the score that is possible in future moves
      
		int rows, cols;
		int gameID;
		int movesMade = 0;
		int currScore = 0;
		int* extensionOffset;
		//internal storage
		int* internal_boardState;
		candy** internal_boardCandies; // internal candy array 
		//2d Array representation of internal storage
		Array2dPtr boardCandies;
		Array2dPtr boardState;

		gameDef * g_def_ref; //reference to g_def, used for copying gameState
      
      gameState * nextBestState;

		//previous move
		//prev_move_row , col
		//prev_move_dir

	public:
		//constructors
		gameState(void);
		gameState(gameState* &obj);
      
      void set_next_best(gameState* nb) {nextBestState = nb;};
      
      gameState* get_next_best() {return nextBestState;}

		//accessing data in class
      void set_prev_move_row(int r) {prevMoveRow = r;}
      void set_prev_move_col(int c) {prevMoveCol = c;}
      void set_prev_move_dir(int d) {prevMoveDir = d;}
      
      int get_prev_move_row() {return prevMoveRow;}
      int get_prev_move_col() {return prevMoveCol;}
      int get_prev_move_dir() {return prevMoveDir;}
      
      void set_potential_score(int score) {potentialScore = score;}
      
      int get_potential_score() {return potentialScore;}
      
		void set_gameID(int id) { gameID = id; }
		void incre_movesMade()  { movesMade += 1; }
		int get_movesMade() {return movesMade; }
		int get_currScore() {return currScore; }
		int get_gameID()	{return gameID; }	   
	    int get_rows()  const {return rows;}
	    int get_cols()  const {return cols;}
	    gameDef* get_g_def_ref() const {return g_def_ref;}    
	    void* get_boardState_element(int row, int col);
	    void set_boardState_element(int row, int col, int value);	      
	    void* get_candy_element(int row, int col);
	    void set_candy_element(int row, int col, void* value);       
        void swap_candy_elements(int row1, int col1, int row2, int col2);
        int get_candy_color(int row, int col);
        int get_candy_type(int row, int col);   
      	void set_candy_color(int row, int col, int color);
      	void set_candy_type(int row, int col, int type);
      	void decrement_boardState_element(int row, int col);
      	int get_extensionOffset(int col);
      	void set_extensionOffset(int col, int value);      
		void initialize(gameDef* &g_def, json_t *boardstate_json);	
		void free_gameState();
		~gameState(void);

		//gamestate processing methods
		void applyGravity();
		bool applyTemplate();
		bool VFour();
		bool checkVFour(int row, int col);
		bool HFour();
		bool checkHFour(int row, int col);
		bool VThree();
		bool checkVThree(int row, int col);
		bool HThree();
		bool checkHThree(int row, int col);

		void printSummary();
};

//constructors
inline gameState::gameState(void){}
inline gameState::gameState(gameState* &obj){
	// cout<< "constructing gameState with copy constructor" << endl;
	rows = obj->get_rows();
	cols = obj->get_cols();
	gameID = obj->get_gameID();
	movesMade = obj->get_movesMade();
	currScore = obj->get_currScore();
	g_def_ref = obj->get_g_def_ref();
	extensionOffset = (int*)malloc(rows * sizeof(int));
	internal_boardCandies = (candy**)malloc(rows * cols * sizeof(candy*));
	internal_boardState= (int*)malloc(rows * cols * sizeof(int));
	boardCandies = A2d_AllocateArray2d(rows, cols, sizeof(void*));
	boardState = A2d_AllocateArray2d(rows, cols, sizeof(void*));

	int rows = obj->get_rows();
	int cols = obj->get_cols();

	for (int i =0; i< cols; i++){
		extensionOffset[i] = obj->get_extensionOffset(i);
	}

	for (int r =0; r< rows; r++){
		for (int c= 0; c<cols; c++){
			int color = obj->get_candy_color(r, c);
			int type = obj->get_candy_type(r,c);
			internal_boardCandies[r * cols + c] = new candy(type, color);
			internal_boardState[r * cols + c] = *(int*)(obj->get_boardState_element(r, c));
		    A2d_FillArray2d(boardCandies, r, c, internal_boardCandies[r * cols + c]);
	        A2d_FillArray2d(boardState, r, c, &internal_boardState[r * cols + c]);
		}
	}
}

inline void* gameState::get_boardState_element(int row, int col){
	return A2d_GetElement(boardState, row, col);
}

inline void gameState::set_boardState_element(int row, int col, int value){
	*((int*)get_boardState_element(row, col)) = value;
}

inline void* gameState::get_candy_element(int row, int col){
	return A2d_GetElement(boardCandies, row, col);
}
inline void gameState::set_candy_element(int row, int col, void* value){
	A2d_FillArray2d(boardCandies, row, col, value);
}
inline int gameState::get_candy_color(int row, int col) {
   return ((candy*)get_candy_element(row, col))->get_color();
}
inline void gameState::set_candy_color(int row, int col, int color) {
   ((candy*)get_candy_element(row, col))->set_color(color);
}
inline int gameState::get_candy_type(int row, int col) {
   return ((candy*)get_candy_element(row, col))->get_type();
}
inline void gameState::set_candy_type(int row, int col, int type) {
   ((candy*)get_candy_element(row, col))->set_type(type);
}

//decre board state and incre score
inline void gameState::decrement_boardState_element(int row, int col) {
   int num = *(int*) get_boardState_element(row, col);
   if (num > 0) {
      set_boardState_element(row, col, num - 1);
      currScore++;
   }
}

inline int gameState::get_extensionOffset(int col) {
   return extensionOffset[col];
}

inline void gameState::set_extensionOffset(int col, int value) {
   extensionOffset[col] = value;
}

inline void gameState::swap_candy_elements(int row1, int col1, int row2, int col2){
	A2d_Swap(boardCandies, row1, col1, row2, col2);
}

inline void gameState::initialize(gameDef* &g_def, json_t *gamestate_json){
	g_def_ref = g_def;

	rows = g_def->get_boardState_rows();
	cols = g_def->get_boardState_cols();


	extensionOffset = (int*)malloc(rows * sizeof(int));
	internal_boardCandies = (candy**)malloc(rows * cols * sizeof(candy*));
	internal_boardState= (int*)malloc(rows * cols * sizeof(int));
	boardCandies = A2d_AllocateArray2d(rows, cols, sizeof(void*));
	boardState = A2d_AllocateArray2d(rows, cols, sizeof(void*));
   

	if (gamestate_json == NULL) {
	   for (int i = 0; i < cols; i++) {
	      extensionOffset[i] = rows;
	   }

		for(int r=0; r<rows; r++){
			for (int c = 0; c<cols; c++){
			  int color = *(int*)g_def->get_extensionColor_element(r, c);
			  int boardstate_element = *(int*)g_def->get_boardState_element(r,c);
			  
			  //allocate memeory on heap for each candy, remember to free
			  internal_boardCandies[r * cols + c] = new candy(0, color); 
			  internal_boardState[r * cols + c] = boardstate_element;
			  //cout << "[gameState initialize]-- internal candy array.. type & color ---" << internal_boardCandies[ r * cols + c]->get_type() << " "<< internal_boardCandies[ r * cols + c]->get_color() << endl;
			}
		}		
	} 
	else {
		json_t *candy_json = json_object_get(gamestate_json, "boardcandies");
		json_t *boardstate_json = json_object_get(gamestate_json, "boardstate");
		json_t *movesMade_json = json_object_get(gamestate_json, "movesmade");
		json_t *score_json = json_object_get(gamestate_json, "currentscore");
		json_t *extension_json = json_object_get(gamestate_json, "extensionoffset");
		json_t *boardstate_elements_json = json_object_get(boardstate_json, "data");
		json_t *candy_elements_data = json_object_get(candy_json, "data");
		movesMade = json_integer_value(movesMade_json);
		currScore = json_integer_value(score_json);

		//set extensionOffset
		for (int c=0; c <cols; c++) {
			extensionOffset[c] = json_integer_value(json_array_get(extension_json, c));
		}

		for (int r=0; r<rows; r++) {
			for (int c=0; c<cols; c++) {
				internal_boardState[r * cols + c] = json_integer_value(json_array_get(boardstate_elements_json, r * cols + c));
				json_t *candy_json_element = json_array_get(candy_elements_data, r * cols + c);
				json_t *color_json = json_object_get(candy_json_element, "color");
				json_t *type_json = json_object_get(candy_json_element, "type");
				int color = json_integer_value(color_json);
				int type = json_integer_value(type_json);

				internal_boardCandies[r * cols + c] = new candy(type, color); 
			}
		}
	}

	for (int r=0; r<rows; r++) {
		for (int c=0; c<cols; c++) {
	       A2d_FillArray2d(boardCandies, r, c, internal_boardCandies[r * cols + c]);
	       A2d_FillArray2d(boardState, r, c, &internal_boardState[r * cols + c]);
		}
	}
}

//destructor
inline gameState::~gameState(void){
	free(extensionOffset);	

	free(boardCandies->storage);
	free(boardCandies);
	for (int r=0; r<rows; r++) {
	  for (int c=0; c<cols; c++) {
	  	delete internal_boardCandies[r * cols + c];  //use delete instead of free for objects
      }
	}
	free(internal_boardCandies);
	
	free(boardState->storage);
	free(boardState);
	free(internal_boardState);
	//cout << "gamestate Object is being deleted" << endl;
}



//---------------------------gamestate processing methods-----------------
inline bool gameState::applyTemplate() {
   bool possibleFire = true;
   int numberTemplatesFired = 0;
   while (possibleFire && numberTemplatesFired < 1000) {
      possibleFire = false;
      possibleFire = VFour() || possibleFire;
      possibleFire = HFour() || possibleFire;
      possibleFire = VThree() || possibleFire;
      possibleFire = HThree() || possibleFire;
      // possibleFire = (VFour() || HFour() || VThree() || HThree());
      //cout << "end firing, apply gravity" << endl << endl << endl << endl;
      if (possibleFire) {
         numberTemplatesFired++;
         gameState::applyGravity();
      }
      
   }
   
   return (numberTemplatesFired > 0);     
}

inline void gameState::applyGravity() {
    //cout <<"in class method applyGravity method being alled" << endl;
   for (int col = 0; col < cols; col++) {
      int lowestFiredRow = 0;
      bool foundFired = false;
      for (int row = 0;  row < rows; row++) {
         if (get_candy_color(row, col) == -1 && !foundFired) {
            foundFired = true;
            lowestFiredRow = row;
         } else if (get_candy_color(row, col) != -1 && foundFired) {
            swap_candy_elements(row, col, lowestFiredRow, col);
            lowestFiredRow++;
         }
      }    
      if (foundFired) {
         for (int row = lowestFiredRow; row < rows; row++) {
            int offset = get_extensionOffset(col) % g_def_ref->get_extensionColor_rows();
            int newColor = *(int*)g_def_ref->get_extensionColor_element(offset, col);
            set_candy_color(row, col, newColor);
            int newOffset = get_extensionOffset(col) + 1;
            set_extensionOffset(col, newOffset);
            //newOffset = get_extensionOffset(col) % g_def_ref->get_extensionColor_rows();
         }
      }
   }
}

inline bool gameState::VFour() {
   bool foundFire = false;
   for (int row = 0; row <= get_rows() - 4; row++) {
      for (int col = 0; col < get_cols(); col++) {
        foundFire = foundFire || checkVFour(row, col);
         
      }
   }
   return foundFire;
}

inline bool gameState::checkVFour(int row, int col){
   int candy_color = get_candy_color(row, col);
   if (candy_color == -1) {
      return false;
   }
   for (int i = 1; i<4; i++){
      int curr_color = get_candy_color(row + i, col);
      if (candy_color != curr_color) {
         return false;
      }
   }
   //cout << "fired VFour" << endl;
   for (int i = 0; i < 4; i++) {
    //cout << "fired (" << row + i << ", " << col << ")" << endl;
      set_candy_color(row + i, col, -1);
      decrement_boardState_element(row + i, col);
   }
   return true;
}

inline bool gameState::HFour() {
   bool foundFire = false;
   for (int row = 0; row < get_rows(); row++) {
      for (int col = 0; col <= get_cols() - 4; col++) {
        foundFire = foundFire || checkHFour(row, col);
      }
   }
   return foundFire;
}

inline bool gameState::checkHFour(int row, int col){
   int candy_color = get_candy_color(row, col);
   if (candy_color == -1) {
      return false;
   }
   for (int i = 1; i<4; i++){
      int curr_color = get_candy_color(row, col + i);
      if (candy_color != curr_color) {
         return false;
      }
   }
  // cout << "fired HFour" << endl;
   for (int i = 0; i < 4; i++) {
    //cout << "fired (" << row << ", " << col + i << ")" << endl;
      set_candy_color(row, col + i, -1);
      decrement_boardState_element(row, col + i);
   }
   return true;
}

inline bool gameState::VThree() {
   bool foundFire = false;
   for (int row = 0; row <= get_rows() - 3; row++) {
      for (int col = 0; col < get_cols(); col++) {
        foundFire = foundFire || checkVThree(row, col);
      }
   }
   return foundFire;
}

inline bool gameState::checkVThree(int row, int col){
   int candy_color = get_candy_color(row, col);
   if (candy_color == -1) {
      return false;
   }
   for (int i = 1; i<3; i++){
      int curr_color = get_candy_color(row + i, col);
      if (candy_color != curr_color) {
         return false;
      }
   }
   //cout << "fired VThree" << endl;
   for (int i = 0; i < 3; i++) {
     //cout << "fired (" << row + i << ", " << col << ")" << endl;
      set_candy_color(row + i, col, -1);
      decrement_boardState_element(row + i, col);
   }
   return true;
}

inline bool gameState::HThree() {
   bool foundFire = false;
   for (int row = 0; row < get_rows(); row++) {
      for (int col = 0; col <= get_cols() - 3; col++) {
        foundFire = foundFire || checkHThree(row, col);
      }
   }
   return foundFire;
}

inline bool gameState::checkHThree(int row, int col){
   int candy_color = get_candy_color(row, col);
   if (candy_color == -1) {
      return false;
   }
   for (int i = 1; i<3; i++){
      int curr_color = get_candy_color(row, col + i);
      if (candy_color != curr_color) {
         return false;
      }
   }
   //cout << "fired HThree" << endl;
   for (int i = 0; i < 3; i++) {
    //cout << "fired (" << row << ", " << col + i << ")" << endl;
      set_candy_color(row, col + i, -1);
      decrement_boardState_element(row, col + i);
   }
   return true;
}

inline void gameState::printSummary(){
	cout<< "------gamestate summary------" << endl;
	cout<< "score = " << currScore << endl;
	cout<< "movesMade = " << movesMade << endl;
	cout << "internal boardstate is valid?" << endl;
	for (int i = 0; i<rows*cols ; i++){
		cout << internal_boardState[i] << ",";
	}
	cout<<endl;

	cout << endl << endl;
}





//-----------------model functions declaration---------------//
void deserialize2dArray(json_t *json, gameDef* &g_def, bool reading_first_array);
void deserialize(json_t* file, gameDef* &g_def, gameState* &g_state);
char* serialize(gameState* &g_state);









#endif // _HW5_MODEL_H_
