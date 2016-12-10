//Kellan Grant & Yifan Gu
//kwg5 & jackgu


#ifndef array2d_h
#define array2d_h

#include <stdbool.h>


/* This structure represents a 2d array. A 2d array contains two ints 
 * for the number of rows and columns, and it contains an array of void*
 * that point to generic client data.
 */
typedef struct Array2d_t{
    int row;
    int col;
    void** storage;
}*Array2dPtr, Array2d;


/* When a customer frees a 2d arry, they need to pass a pointer to a
 * function that does any necessary free-ing of the data stored in 2d array
 */
typedef void (*A2d_Client_FreeFuncPtr)();



/* Allocates and returns a new Array2dPtr.
 * 
 * Arguments: 
 * -row: the number of rows for the 2d array.
 * -col: the number of columns for the 2d array.
 * -datasize: the size of data needed to store values (usually sizeof(void*),
 * however if data is smaller than 8 bytes, it can be directly stored in the
 * array instead of indirectly using pointers).
 *
 * Returns a pointer to the new 2d array.
 */
Array2dPtr A2d_AllocateArray2d (int row, int col, int datasize);


/* Fills the arr[index_r][index_c] with a data pointer.
 * 
 * Arguments: 
 * -arr: a pointer to array needed to fill.
 * -index_r: the row position that will be filled.
 * -index_c: the column position that will be filled.
 * -data: a pointer to the data that will fill the requested position in arr.
 *
 * Returns true if the fill was successful, and false if not.
 */
bool A2d_FillArray2d(Array2dPtr arr, int index_r, int index_c, void* data);

/* Swaps the values of arr[from_row][from_col] with arr[to_row][to_col].
 * 
 * Arguments: 
 * -arr: a pointer to array that you will do the swapping in.
 * -from_row: index of row of first element.
 * -from_col: index of column of first element.
 * -to_row: index of row of second element.
 * -from_col: index of column of second element.
 * Returns true if the swap was successful, and false if not.
 */
bool A2d_Swap(Array2dPtr arr, int from_row, int from_col, int to_row, int to_col);

/* Gets the value of arr[row][col]
 * 
 * Arguments: 
 * -arr: a pointer to array with the requested element.
 * -row: the row position of requested element.
 * -col: the column position of requested element.
 * Returns a void* pointing to the requested element.
 */
void* A2d_GetElement (Array2dPtr arr, int row, int col);





/* Free an 2d array as well as all the data stored inside, client should provide 
 * a custom call back function.
 * Arguments: 
 * -arr: a pointer to the 2d array to be freed.
 * -clientFree: a pointer to custom free function provided by the client.
*/
void A2d_FreeArray2d(Array2dPtr arr, A2d_Client_FreeFuncPtr clientFree);




#endif /* array2d_h */
