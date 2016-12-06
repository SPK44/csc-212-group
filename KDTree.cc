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
	destroyHelper(root);
	root = NULL;
}

//helper function that is called by insert using two nodes, p and its parent, as well as a depth value
//also calls itself recursively to traverse tree
void KDTree::insertHelper(KDNode *p, KDNode *parent, int depth, double lat, double lon, const char *desc) {
	
	//if pointer points to a null
	if (!p) {
		//creates a new node with given lat, lon and desc
		KDNode *node = new KDNode(lat, lon, desc);
		//if p is at root
		if (p == parent){
			//points root to node
			root = node;
			//sets depth of node to zero
			node->depth = 0;
		}
		else {
			//based on what child of parent p is
			//sets correct parent pointer to point to node
			(parent->left == p) ? parent->left = node : parent->right = node;
			//sets depth of node equal to deth value in recursive function
			node->depth = depth;
		}
		//increases size of Tree
		size++;
		return;
	}
	//if depth is odd
	if (depth % 2) {
		//uses latitude to determine which way to recurse
		//if inputted latitude is greater than or equal to latitude of p
		if (lat >= p->latitude)
			//calls itself recursively to the right
			insertHelper(p->right, p, depth + 1, lat, lon, desc);
		//if inputted latitude is less than latitude of p		
		if (lat < p->latitude)
			//calls itself recursively to the left			
			insertHelper(p->left, p, depth + 1, lat, lon, desc);
	}
	//if depth is even
	else {
		//uses longitude to determine which way to recurse
		//if inputted longitude is greater than or equal to longitude of p		
		if (lon >= p->longitude)
			//calls itself recursively to the right
			insertHelper(p->right, p, depth + 1, lat, lon, desc);
		//if inputted longitude is less than longitude of p		
		if (lon < p->longitude)
			//calls itself recursively to the left
			insertHelper(p->left, p, depth + 1, lat, lon, desc);
	}

}
// Function to insert into a kd tree given the latitude and longitude and a description
void KDTree::insert(double lat, double lon, const char *desc) {
	//calls helper function insertHelper
	insertHelper(root, root, 0, lat, lon, desc);
}

//helper function for printNeighbors to print a node in the correct format
void KDTree::printNode(KDNode *p){
	std::string str = p->description;
	str.erase(str.find_last_not_of(" \n\r\t")+1);
	std::cout << "\t[\"" << str << "\", " << p->latitude << ", " << p->longitude << "],\n";
	
}

//helper function for printNeighbors
//takes inputs of latitude and longitude of center point, radius around point, filter keyword, and a node
//uses recursive calls to traverse tree and print out matches within radius and containing the keyword
//returns number of hits
unsigned int KDTree::printNeighborsHelp(double lat, double lon, double rad, const char *filter, KDNode *p){
	//initializes count variable which holds number of hits
	unsigned int count = 0;
	double param = M_PI / 180.0; // required for conversion from degrees to radians
	//if KDNode pointer p points to a null
	//returns 0 and goes back up tree
	if(!p){ 
	
	return count;
	
	}
	//if even depth
	else if(p->depth % 2){
		//check latitude
		if( p->latitude > lat + (rad/69.172)){
			//call recursivly left and update count
			count += printNeighborsHelp(lat, lon, rad, filter, p->left);
		
		}
		else if( p->latitude < lat - (rad/69.172)){
			//call recursivly left and update count
			count += printNeighborsHelp(lat, lon, rad, filter, p->right);
		
		}
		else if(p->distance(lat, lon) <= rad){
		
			//checks if filter keyword is in the description of the node
			if(p->description.find(filter) != std::string::npos){
				//if so, prints node	
				printNode(p);
				//increment the count
				count++;
		
			}
			//recurse down the tree
			count += printNeighborsHelp(lat, lon, rad, filter, p->right);
			//recurse down the tree
			count += printNeighborsHelp(lat, lon, rad, filter, p->left);
		}
		else{
			//p latitude is in the range, then call resursivley right and left
			count += printNeighborsHelp(lat, lon, rad, filter, p->right);
			count += printNeighborsHelp(lat, lon, rad, filter, p->left);
			
		}
	
	}
	else{
		//if p's longitude is larger
		if(p->longitude > lon + rad*cos(param*lat)){
			//update count and call left
			count += printNeighborsHelp(lat, lon, rad, filter, p->left); 
			
		}
		//if p's longitude is smaller
		else if( p->longitude < lon - rad*cos(param*lat)){
			//updat count, call the function recurse right
			count += printNeighborsHelp(lat, lon, rad, filter, p->right);
		
		}
		else if(p->distance(lat, lon) <= rad){
		
			//checks if filter keyword is in the description of the node
			if(p->description.find(filter) != std::string::npos){
				//if so, prints node	
				printNode(p);
				//increment the count
				count++;
		
			}
			//recurse down the tree
			count += printNeighborsHelp(lat, lon, rad, filter, p->right);
			//recurse down the tree
			count += printNeighborsHelp(lat, lon, rad, filter, p->left);
		}
		else{
			//if the longitude is in the range, call both left and right recursivly.
			count += printNeighborsHelp(lat, lon, rad, filter, p->right);
			count += printNeighborsHelp(lat, lon, rad, filter, p->left);	
			
		}
	
	}
	
	return count;
}

//prints formatting for center point and calls helper function on the root
unsigned int KDTree::printNeighbors(double lat, double lon, double rad, const char *filter) {
	
	KDNode *p = root;
	std::cout << "var markers = [\n";	
	std::cout << "\t[\"" << "CENTER" << "\", " << lat << ", " << lon << "],\n";
	unsigned int count = printNeighborsHelp(lat, lon, rad, filter, p);
	std::cout << "];\n";
	
	return count;
}

//returns size of the KDTree
unsigned int KDTree::getSize() {
	return size;
}
