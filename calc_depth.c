/*
 * PROJ1-1: YOUR TASK A CODE HERE
 *
 * Feel free to define additional helper functions.
 */

#include "calc_depth.h"
#include "utils.h"
#include <math.h>
#include <limits.h>


/* Implements the normalized displacement function */
unsigned char normalized_displacement(int dx, int dy,
        int maximum_displacement) {

    double squared_displacement = dx * dx + dy * dy;
    double normalized_displacement = round(255 * sqrt(squared_displacement) / sqrt(2 * maximum_displacement * maximum_displacement));
    return (unsigned char) normalized_displacement;

}


/* Defined to keep track of the candidate feature giving 
the minimum squared euclidean distance

min_SED - the minimized distance between the feature (in 
the left image) and the candidate under observation

location - keeps track of the best candidate feature's
center */
typedef struct {
	unsigned int min_SED;
	int location;
} cand;


/* Takes in left feature array and start and end positions 
of features in right image, and returns the Squared Euclidean Distance */
unsigned int square_euc_dist(unsigned char feature_arr[], unsigned char cand_arr[], int length){
	unsigned int difference, squared_diff;
	unsigned int total_sum = 0;
	for (int n = 0; n < length; n++){
		difference = feature_arr[n] - cand_arr[n];
		squared_diff = difference*difference;
		total_sum += squared_diff;
	}
	return total_sum;
}

void calc_depth(unsigned char *depth_map, unsigned char *left,
        unsigned char *right, int image_width, int image_height,
        int feature_width, int feature_height, int maximum_displacement) {

    int left_edge_buff = (feature_height - 1)*(image_width) + image_width + feature_width; //Initialize left edge buffer to first allowable pixel
    int curr_row = feature_height;
    int curr_col = feature_width;
    int right_edge_buff = (feature_height)*(image_width) + image_width - 1 - feature_width;
    unsigned char *feat_scan_left = left;
    unsigned char *feat_scan_right = right;
    int i = left_edge_buff-1; //Starts off as 1 less than what it should be.
    int last_pixel = (image_height*image_width - 1) - feature_width - feature_height*image_width;
   	int length = (2*feature_width+1)*(2*feature_height+1);
   	int dx, dy, dx_min, dy_min;
   	unsigned char depth;
   	cand min_euc_dist_cand;
   	min_euc_dist_cand.min_SED = UINT_MAX;


    while (i < last_pixel){ //While i <= last allowable pixel in image
    	if (i == right_edge_buff){
    		i += 2*feature_width;
    		curr_row++;
    		curr_col = feature_width;
    		right_edge_buff += image_width;
    	}
    	i++;
       	
       	//At this point, i is what it needs to be.
       	int start_left = i - image_width*feature_height - feature_width; //Top left corner of feature
       	int end_left = i + image_width*feature_height + feature_width;	//Bottom right corner of feature
       	int j = start_left;	//j initialization, which will be the object looping through the feature
       	int count = 0;
       	unsigned char feature_arr[length]; //array holding all characters within a left feature


       	//LEFT FEATURE
       	//Obtain distances for left image, to find the minimal candidate feature in right image.
       	while (j <= end_left){
       		if (j > start_left + 2*feature_width){
       			j = start_left + image_width;
       			start_left += image_width;
       		}
       		feat_scan_left = left+j;

       		//Build the array
       		feature_arr[count] = *feat_scan_left;
			
       		j++;
       		count++;
       	}

    	//SPACE SEARCH
    	//NOTE TO SELF: Search Spaces are *NOT* always square!

    	//Some object initializations, and cleaning up edge cases for space search.
    	int offset_x_top = maximum_displacement + feature_height - curr_row;
    	int offset_y_left = maximum_displacement + feature_width - curr_col;
    	int offset_x_bot = maximum_displacement - (image_height - feature_height - curr_row - 1);
    	int offset_y_right =  maximum_displacement - (image_width - feature_width - curr_col - 1);

    	int start_search = i - image_width*(maximum_displacement+feature_height) - (maximum_displacement+feature_width); //Search space feature start (top left-most pixel)
    	int end_search = i + image_width*(maximum_displacement+feature_height) + (maximum_displacement+feature_width);	//Search space end (bottom right-most pixel)


		//Fix the search space dimensions if the space is near the edge.
    	if (offset_x_top > 0){
    		start_search += (offset_x_top)*image_width;
    	}if (offset_x_bot > 0){
    		end_search -= (offset_x_bot)*image_width;
    	}if (offset_y_left > 0){
    		start_search += offset_y_left;
    	}if (offset_y_right > 0){
    		end_search -= offset_y_right;
    	}

    	int i_search = start_search + feature_width + feature_height*image_width; //Center of feature in search space (initialized to first center)
    	int i_search_end = end_search - feature_width - feature_height*image_width; //End center position
    	int k = i_search;


    	int count_search = 0;
    	unsigned int dist;
    	unsigned char cand_arr[length]; //array holding characters within a feature candidate

    	//Loop through possible center feature locations in right image. The 'k' represents the center of the candidate feature
    	while (k <= i_search_end){
    		if ((k % image_width) > (i_search_end % image_width)){ //If the current feature center is past the last feature center, skip to next row.
    			i_search += image_width;
    			k = i_search;
    			if (k > i_search_end){continue;}
    		}
			while ((k % image_width) < (i_search % image_width)){
				k++;
    		}

       		int start_feat = k - image_width*feature_height - feature_width; //Top left corner of feature
       		int end_feat = k + image_width*feature_height + feature_width;	//Bottom right corner of feature
       		int l = start_feat;

       		//Loop through every pixel surrounding the center 'k'
    		while (l <= end_feat){
    			if (l > start_feat + 2*feature_width){
    				l = start_feat + image_width;
    				start_feat += image_width;
    			}
       			feat_scan_right = right + l;

       			//Build the array.
       			cand_arr[count_search] = *feat_scan_right;

       			l++;
       			count_search++;
       		}
       		count_search =  0;
       		dist = square_euc_dist(feature_arr, cand_arr, length);

       		//Minimize distance. If the previous minimum distance is equal to the calculated distance, 
       		//pick the one with the smallest normalized displacement.
       		if (dist == min_euc_dist_cand.min_SED){
       			dx = k % image_width - i % image_width;
       			dy = (k - i - dx)/image_width;
       			dx_min = min_euc_dist_cand.location % image_width - i % image_width;
       			dy_min = (min_euc_dist_cand.location - i - dx_min)/image_width;

       			//Take feature candidate with smaller normalized displacement if there's a tie in distance
       			if (normalized_displacement(dx, dy, maximum_displacement) < normalized_displacement(dx_min, dy_min, maximum_displacement)){
       				min_euc_dist_cand.min_SED = dist;
       				min_euc_dist_cand.location = k;
       			}
       		}else if (dist < min_euc_dist_cand.min_SED){
       			min_euc_dist_cand.min_SED = dist;
       			min_euc_dist_cand.location = k;
       		}
       		k++;
    	}

    	min_euc_dist_cand.min_SED = UINT_MAX;
    	curr_col++;



    	//Found feature match, now find depth!
    	dx = min_euc_dist_cand.location % image_width - i % image_width;
    	dy = (min_euc_dist_cand.location - i - dx)/image_width;

    	depth = normalized_displacement(dx, dy, maximum_displacement);
    	*(depth_map + i) = depth;
    	

    }
    left_edge_buff = feature_height*image_width+feature_width;
    for (int m = 0; m < image_width*image_height; m++){

    	if (image_height - feature_height <= (m - m%image_width)/image_width){
    		left_edge_buff = -1;
    	}
    	if (m == left_edge_buff){
    		m += image_width - 1 - 2*feature_width;
    		left_edge_buff+=image_width;
    	}
    	else{ //Set the border pixels to 0
    		*(depth_map + m) = 0;
    	}
    }
    return;
}


