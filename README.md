depth_map
=========

A program simulating depth perception on a computer

###### Programs I worked on:

##### calc_depth.c:

This module takes in two 8-bit stereoimages (one from the left, the other from the right) and outputs a depth map. 
It works by iterating through both images and matching similar features. After successfully matching the features, 
the algorithm takes the square euclidean distance between the pixel locations of the left and right images. Then, 
it writes that distance into a pixel of the depth map corresponding to the center of the feature in the left image
(the left image is arbitrarily taken as the single source of truth).

##### make_qtree.c:

The make_qtree module creates a quadtree to store the depth map values. This quadtree mapping assumes that the depth
map is square, so it only covers that special case.  It scans the depth map looking for square sections of the same
pixel values. It then stores the whole square section as a node, noting the dimensions, location of the square section,
and pixel value within the section. This is convenient for depth maps because they tend to have many homogeneous sections.

