/*
 * PROJ1-1: YOUR TASK B CODE HERE
 *
 * Feel free to define additional helper functions.
 */

#include <stdlib.h>
#include <stdio.h>
#include "quadtree.h"
#include "make_qtree.h"
#include "utils.h"

#define ABS(x) (((x) < 0) ? (-(x)) : (x))

qNode *quadhelper(unsigned char *depth_map, int map_width, int x, int y, int section_width){
    qNode *root = malloc(sizeof(qNode));
    int zero_leaf = 0;
    int default_gray = 256;

    if (homogenous(depth_map, map_width, x, y, section_width) == 1){

        (*root).leaf = 1;
        (*root).size = section_width;
        (*root).x = x;
        (*root).y = y;
        (*root).gray_value = depth_map[(y*map_width)+x];
        return root;
    }else{
        (*root).leaf = zero_leaf;
        (*root).size = section_width;
        (*root).x = x;
        (*root).y = y;
        (*root).gray_value = default_gray;

        root->child_NW = quadhelper(depth_map, map_width, x, y, section_width/2);
        root->child_NE = quadhelper(depth_map, map_width, (x+section_width/2), y, section_width/2);
        root->child_SE = quadhelper(depth_map, map_width, (x+section_width/2), (y+section_width/2), section_width/2);
        root->child_SW = quadhelper(depth_map, map_width, x, (y+section_width/2), section_width/2);

    }
    return root;
}

//Homogeneous is handed an x,y and a section width, so it knows where to check for similar pixels
int homogenous(unsigned char *depth_map, int map_width, int x, int y, int section_width) {
    
    //convert x and y to actual position in array
    int curr_position = y * map_width + x;
    int start = curr_position;
    int prev_position = curr_position;
    int last = section_width*section_width;

    for (int count = 0; count < last; count++){
        if (curr_position == start + section_width){
            start += map_width;
            curr_position = start;
        }
        if (depth_map[curr_position] != depth_map[prev_position]){
            return 0;
        }
        prev_position = curr_position;
        curr_position++;

    }
    return 1;

}

qNode *depth_to_quad(unsigned char *depth_map, int map_width) {
    qNode *root = NULL; //Create root quadtree node

    root = (qNode *) malloc(sizeof(qNode)); //dynamically allocated memory for root quadtree
    if (root == NULL) {
        allocation_failed();
    }
    
    /*Populate the entire quadtree*/
    root = quadhelper(depth_map, map_width, 0, 0, map_width); 

    return root;

}

void free_qtree(qNode *qtree_node) {
    if(qtree_node) {
        if(!qtree_node->leaf){
            free_qtree(qtree_node->child_NW);
            free_qtree(qtree_node->child_NE);
            free_qtree(qtree_node->child_SE);
            free_qtree(qtree_node->child_SW);
        }
        free(qtree_node);
    }
}


