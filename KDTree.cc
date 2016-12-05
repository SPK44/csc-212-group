#include "KDTree.h"
#include <iostream>
#include <math.h>


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

void KDTree::destroyHelper(KDNode *p) {
	if (!p)
		return;
	destroyHelper(p->left);
	destroyHelper(p->right);
	delete p;

}

void KDTree::destroy(KDNode *p) {
	size = 0;
	destroyHelper(root);
	root = NULL;
}

void KDTree::insertHelper(KDNode *p, KDNode *parent, int depth, double lat, double lon, const char *desc) {
	
	if (!p) {
		KDNode *node = new KDNode(lat, lon, desc);
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
	if (depth % 2) {
		if (lat >= p->latitude)
			insertHelper(p->right, p, depth + 1, lat, lon, desc);
		if (lat < p->latitude)
			insertHelper(p->left, p, depth + 1, lat, lon, desc);
	}
	else {	
		if (lon >= p->longitude)
			insertHelper(p->right, p, depth + 1, lat, lon, desc);
		if (lon < p->longitude)
			insertHelper(p->left, p, depth + 1, lat, lon, desc);
	}

}

void KDTree::insert(double lat, double lon, const char *desc) {
	insertHelper(root, root, 0, lat, lon, desc);
}

void KDTree::printNode(KDNode *p){
	
	std::cout << "\t[\"" << p->description << "\", " << p->latitude << ", " << p->longitude << "],\n";
	
}

unsigned int KDTree::printNeighborsHelp(double lat, double lon, double rad, const char *filter, KDNode *p){
	unsigned int count = 0;
	if(!p){ 
	
	return count;
	
	}
	else if(!(p->depth % 2)){
		
		if( p->latitude > lat + rad){
		
			count += printNeighborsHelp(lat, lon, rad, filter, p->left);
		
		}
		else if( p->latitude > lat - rad){
		
			count += printNeighborsHelp(lat, lon, rad, filter, p->right);
		
		}
		else if(p->distance(lat, lon) <= rad){
			
			printNode(p);
			count++;
			count += printNeighborsHelp(lat, lon, rad, filter, p->right);
			count += printNeighborsHelp(lat, lon, rad, filter, p->left);
		}
		else{
		
			count += printNeighborsHelp(lat, lon, rad, filter, p->right);
			count += printNeighborsHelp(lat, lon, rad, filter, p->left);
			
		}
	
	}
	else{
		
		if(p->longitude > lon + rad){
			
			count += printNeighborsHelp(lat, lon, rad, filter, p->left); 
			
		}
		else if( p->longitude > lon - rad){
		
			count += printNeighborsHelp(lat, lon, rad, filter, p->right);
		
		}
		else if(p->distance(lat, lon) <= rad){
			
			printNode(p);
			count++;
			count += printNeighborsHelp(lat, lon, rad, filter, p->right);
			count += printNeighborsHelp(lat, lon, rad, filter, p->left);
		}
		else{
			
			count += printNeighborsHelp(lat, lon, rad, filter, p->right);
			count += printNeighborsHelp(lat, lon, rad, filter, p->left);	
			
		}
	
	}
	
	return count;
}

unsigned int KDTree::printNeighbors(double lat, double lon, double rad, const char *filter) {
	
	KDNode *p = root;
	std::cout << "\t[\"" << "CENTER" << "\", " << lat << ", " << lon << "],\n";
	unsigned int count = printNeighborsHelp(lat, lon, rad, filter, p);
	std::cout << "];\n";
	
	return count;
}

unsigned int KDTree::getSize() {
	return size;
}
