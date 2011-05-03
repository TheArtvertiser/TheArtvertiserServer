/*
 * LocationDB.h
 *
 *  Created on: 30/04/2011
 *      Author: arturo
 */

#ifndef LOCATIONDB_H_
#define LOCATIONDB_H_

#include "ofxAndroidGPS.h"
#include "ofxGeoLocation.h"
#include "ofConstants.h"
#include "Artvert.h"

class LocationDB {
public:
	LocationDB();
	virtual ~LocationDB();

	void addLocation(Artvert & artvert);
	vector<string> listCountries();
	vector<string> listCities(string country="");
	vector<string> listRoads(string country="", string city="");
	vector<string> listArtverts(string country="", string city="", string road="");

private:
	ofxXmlSettings xml;
	ofxGeoLocation geo;
};

#endif /* LOCATIONDB_H_ */
