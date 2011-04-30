/*
 * LocationDB.h
 *
 *  Created on: 30/04/2011
 *      Author: arturo
 */

#ifndef LOCATIONDB_H_
#define LOCATIONDB_H_

#include "ofxAndroidGPS.h"
#include "Geo.h"
#include "ofConstants.h"

class LocationDB {
public:
	LocationDB();
	virtual ~LocationDB();

	void addLocation(const ofxLocation & loc);
	vector<string> listCountries();
	vector<string> listCities(string country="");
	vector<string> listRoads(string country="", string city="");

private:
	ofxXmlSettings xml;
	Geo geo;
};

#endif /* LOCATIONDB_H_ */
