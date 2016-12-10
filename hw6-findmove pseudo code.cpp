
 map  findMoveSequence(gameState initial_state){

 	Queue<gameStates> q;
 	enqueue(initial_state);

 	Map <gamestate, gamestate> pathmap;
 	pathmap.add(initial_state, NULL);


 	while (q is not empty){
 		gamestate cur = q.pop();


 		if(cur->score = desired score){
 			return pathmap;
 		}

 		for (int r = 0; r<rows; r++){
 			for (int c = 0; c< cols; c++){
 				gamestate cur_copy = cur;
 				gamestate cur_copy2 = cur;

 				//apply move: up
 				cur_copy->swapCandy(r, c, r+1, c);
 				if (cur_copy->applyTemplate()){
 					//record moves made in gamestate object, cur_copy->setMovesMade_row(), setMovesMade_col(), setMovesMade_dir()
 					enqueue (cur_copy);
 					pathmap.add(cur_copy, cur); //cur copy now is a modified board after applying move to cur
 				}

 				//apply move: up 
 				cur_copy2-> swapCandy();
 				//....
 				
 			}
 		}
 	}

 	return pathmap;

 }



 TO-DOs:
 1. figure out how to use Map 
 2. figure out how to use Queue
 3. when to stop? (idea: use a main thread listening for requestMove message from server,
 	multiple child thread doing findMove. Once main thread received request, broadcast to 
 	all children threads to stop)
 4. others..