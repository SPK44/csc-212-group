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


void KDTree::insert(double lat, double lon, const char *desc) {
	
	KDNode * p = root;
	KDNode * parent = root;
	int depth = 0;

	while(1) {
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
		parent = p;
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
}



unsigned int KDTree::printHelper(KDNode * p, double * latMax, double * latMin, double * lonMax, double * lonMin, double * lat, double * lon, double  * rad, const char *filter) {

		if (!p)
			return 0;
			
		int count = 0;


		if (p->distance(*(lat), *(lon)) < *(rad) && p->description.find(filter) != std::string::npos) {
			std::cout << "\t[\"" << p->description << "\", " << p->latitude << ", " << p->longitude << "],\n";
			count++;
		}


		if (p->depth % 2) {
			if (p->latitude <= *(latMax))
				count += printHelper(p->right, latMax, latMin, lonMax, lonMin, lat, lon, rad, filter);
			if (p->latitude >= *(latMin))
				count += printHelper(p->left, latMax, latMin, lonMax, lonMin, lat, lon, rad, filter);
		}
		else {	
			if (p->longitude <= *(lonMax))
				count += printHelper(p->right, latMax, latMin, lonMax, lonMin, lat, lon, rad, filter);
			if (p->longitude >= *(lonMin))
				count += printHelper(p->left, latMax, latMin, lonMax, lonMin, lat, lon, rad, filter);
		}

		return count;
}


unsigned int KDTree::printNeighbors(double lat, double lon, double rad, const char *filter) {
	
	std::cout << "\t[\"" << "CENTER" << "\", " << lat << ", " << lon << "],\n";
	
	double latMax = lat + rad;
	double latMin = lat - rad;
	double lonMax = lon + rad;
	double lonMin = lon - rad;
	int count = 0;

	count += printHelper(root, &latMax, &latMin, &lonMax, &lonMin, &lat, &lon, &rad, filter);

	std::cout << "];\n";
	
	return count;
}

unsigned int KDTree::getSize() {
	return size;
}
