//Kellan Grant & Yifan Gu
//kwg5 & jackgu

#include <stdio.h>
#include <stdlib.h>
#include "array2d.h"

Array2dPtr A2d_AllocateArray2d(int row, int col, int datasize){
    
    if(row<0 || col<0 || datasize<0){
        return NULL;
    }

    Array2dPtr arr;
    //allocate memory for arr structure
    arr = (Array2dPtr)malloc(sizeof(Array2d));
    
    if(arr == NULL){
        return NULL;
    }
    //allocate memory for arr's storage
    arr->storage = malloc(row * col * datasize);
    
    arr->row = row;
    arr->col = col;
    return arr;
}


bool A2d_FillArray2d(Array2dPtr arr, int index_r, int index_c, void* data){
    //check for index 
    if(index_r < 0 || index_c < 0){
        return false;
    }
    if(index_r >= arr->row || index_c >= arr->col){
        return false;
    }
    
    //check for null 
    if (arr == NULL || data == NULL){
        printf("FILL ARRAY NULL para");
        return false;
    }
    

    *(arr->storage + arr->col * index_r + index_c) = data;

    return true;
}


bool A2d_Swap(Array2dPtr arr, 
            int from_row, int from_col, int to_row, int to_col){

    //check for index
    if(from_col >= arr->col || from_row >= arr->row ||
         to_col >= arr->col || to_row>= arr->row ||
         from_col<0 || from_col<0 || to_col<0 || to_row<0){

        return false;
    }
    
    if (arr==NULL){
        return false;
    }
    
    //store values in temp variables and fill array
    int col = arr->col;
    void* from = *(arr->storage + col * from_row + from_col);
    void* to = *(arr->storage + col * to_row + to_col);

    A2d_FillArray2d (arr, from_row, from_col, to);
    A2d_FillArray2d (arr, to_row, to_col, from);

    return true;
}


void* A2d_GetElement (Array2dPtr arr, int row, int col){
    if (arr != NULL){
        if (row>=0 && row< arr->row && col>=0 && col< arr->col){
            return arr->storage[row * arr->col + col];
        }
    } 

    return NULL;
}


void A2d_FreeArray2d(Array2dPtr arr, 
                    A2d_Client_FreeFuncPtr clientFree){

    if (arr != NULL && clientFree != NULL){
        //callback to clinet's free function before freeing 2d array
        clientFree(); 
        free(arr->storage);
        free(arr);
    }
}