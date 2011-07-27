/*
 Copyright 2011 Arturo Castro <arturo@arturocastro.net>,

 Distributed under the terms of the GNU General Public License v3.

 This file is part of The Artvertiser Server.

 The Artvertiser Server is free software: you can redistribute
 it and/or modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation, either version 3 of
 the License, or (at your option) any later version.

 The Artvertiser Server is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU Lesser General Public License
 along with The Artvertiser for Android.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef LOCATIONDB_H_
#define LOCATIONDB_H_

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
