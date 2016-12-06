#include "KDTree.h"
#include <iostream>
#include <math.h>
#include <string>

KDNode::KDNode(double lat, double lon, const char *desc) {
	left = NULL;
	right = NULL;
	description = desc;
	latitude = lat;
	longitude = lon;
}

KDNode::~KDNode() {
}

double KDNode::distance(double lat, double lon) {
	double param = M_PI / 180.0; // required for conversion from degrees to radians
	double rad = 3956.0;  // radius of earth in miles
	double d_lat = (lat - latitude) * param;
	double d_lon = (lon - longitude) * param;
	double dist = sin(d_lat/2) * sin(d_lat/2) + cos(latitude*param) * cos(lat*param) * sin(d_lon/2) * sin(d_lon/2);
	dist = 2.0 * atan2(sqrt(dist), sqrt(1-dist));
	return rad * dist;
}

KDTree::KDTree() {
	root = NULL;
	size = 0;
}

KDTree::~KDTree() {
	destroy(root);	
}



// Simple postorder traversal, deleting nodes along the way
void KDTree::destroyHelper(KDNode *p) {
	if (!p)
		return;
	destroyHelper(p->left);
	destroyHelper(p->right);
	delete p;

}

// Calls helper function to purge data starting at a specific node
void KDTree::destroy(KDNode *p) {
	size = 0;
	destroyHelper(p);
	root = NULL;
}

// Function to insert into a kd tree. Needs, the latitude and longitude
void KDTree::insert(double lat, double lon, const char *desc) {
	
	//Starts at the root, keeps track of depth
	KDNode * p = root;
	KDNode * parent = root;
	int depth = 0;

	// Loops while the node is non null
	while(p) {

		// Sets the parent to p
		parent = p;
		
		//Checks the current depth. Based on depth the pointer go left or right based on latitude or longitude
		if (depth % 2) {
			if (lat < p->latitude)
				p = p->left;
			else
				p = p->right;
		}
		else {	
			if (lon < p->longitude)
				p = p->left;
			else
				p = p->right;
		}
		depth++;
	}
	
	// Creating the new node
	KDNode *node = new KDNode(lat, lon, desc);
	
	// Checks if the parent is p, which is the case when the root is null
	if (p == parent){
		root = node;
		node->depth = 0;
	}
	else {
		(parent->left == p) ? parent->left = node : parent->right = node;
		node->depth = depth;
	}
	size++;
	return;
}


// Helper function for printing out the nodes, returns the count of nodes, in the tree below p that fit the filter
unsigned int KDTree::printHelper(KDNode * p, double * latMax, double * latMin, double * lonMax, double * lonMin, double * lat, double * lon, double  * rad, const char *filter) {

		// Base case, when there is no node
		if (!p)
			return 0;
		
		// Keeps track of the count
		int count = 0;
		
		// Determines if the node is within the radius, and the filter word is in the description, then prints it if so
		if (p->distance(*(lat), *(lon)) < *(rad) && p->description.find(filter) != std::string::npos) {
			std::string str = p->description;
			str.erase(str.find_last_not_of(" \n\r\t")+1);
			std::cout << "\t[\"" << str << "\", " << p->latitude << ", " << p->longitude << "],\n";
			count++;
		}

		// Smart pathing algorithm based off of a bounding box
		if (p->depth % 2) {
			
			// Checks if the latitude is less than the max, in this case it will recurse to the right (Its still able to be above the node)
			if (p->latitude <= *(latMax))
				count += printHelper(p->right, latMax, latMin, lonMax, lonMin, lat, lon, rad, filter);
			// Checks if the latitude is more than the min, in this case it will recurse to the left (Its still able to be below the node)
			if (p->latitude >= *(latMin))
				count += printHelper(p->left, latMax, latMin, lonMax, lonMin, lat, lon, rad, filter);
			
			// In the case that the node is in the bounding box, both conditionals will trip (left/right), therefore, both tree will be checked
		}
		else {	
		
			// Same as above,except it checks longitude, because of the current depth
			if (p->longitude <= *(lonMax))
				count += printHelper(p->right, latMax, latMin, lonMax, lonMin, lat, lon, rad, filter);
			if (p->longitude >= *(lonMin))
				count += printHelper(p->left, latMax, latMin, lonMax, lonMin, lat, lon, rad, filter);
		}

		return count;
}

// Function for formating the output, will also create pointers to the values of the boungding box
unsigned int KDTree::printNeighbors(double lat, double lon, double rad, const char *filter) {

	std::cout << "var markers = [\n";	
	std::cout << "\t[\"" << "CENTER" << "\", " << lat << ", " << lon << "],\n";
	
	double param = M_PI / 180.0; // required for conversion from degrees to radians
	double latMax = lat + (rad/69.172);
	double latMin = lat - (rad/69.172);
	double lonMax = lon + rad*cos(param*lat);
	double lonMin = lon - rad*cos(param*lat);
	int count = 0;

	// Recursive call to helper
	count += printHelper(root, &latMax, &latMin, &lonMax, &lonMin, &lat, &lon, &rad, filter);

	std::cout << "];\n";
	
	return count;
}

unsigned int KDTree::getSize() {
	return size;
}
