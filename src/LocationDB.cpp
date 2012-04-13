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

#include "LocationDB.h"
#include <set>

LocationDB::LocationDB() {
	xml.loadFile("locations.xml");
	int root = xml.getNumTags("locations");
	if(root==0){
		xml.addTag("locations");
	}
	xml.pushTag("locations");

}

LocationDB::~LocationDB() {
	xml.saveFile();
}

void LocationDB::addLocation(Artvert & artvert){
	ofxLocation loc = artvert.getLocation();
	ofxXmlSettings address = geo.getAddressFor(loc);

	string road = address.getValue("reversegeocode:addressparts:road","");
	string city = address.getValue("reversegeocode:addressparts:city","");
	string country = address.getValue("reversegeocode:addressparts:country","");

	int numLoc = xml.addTag("location");
	xml.addAttribute("location","uid",artvert.getUID(),numLoc);
	xml.addAttribute("location","lon",loc.longitude,numLoc);
	xml.addAttribute("location","lat",loc.latitude,numLoc);

	xml.addAttribute("location","road",road,numLoc);
	xml.addAttribute("location","city",city,numLoc);
	xml.addAttribute("location","country",country,numLoc);
	xml.saveFile();
}

vector<string> LocationDB::listCountries(){
	set<string> countries;
	int numLocations = xml.getNumTags("location");
	for(int i=0;i<numLocations;i++){
		string country = xml.getAttribute("location","country","",i);
		if ( country == "" )
			country = "unknown country";
		countries.insert(country);
	}
	vector<string> countries_vec;
	countries_vec.assign(countries.begin(),countries.end());
	return countries_vec;
}

vector<string> LocationDB::listCities(string country){
	set<string> cities;
	int numLocations = xml.getNumTags("location");
	for(int i=0;i<numLocations;i++){
		if(country=="" || country=="unknown country" || xml.getAttribute("location","country","",i)==country)
		{
			string city = xml.getAttribute("location","city","",i);
			if ( city == "" )
				city = "unknown city";
			cities.insert(city);
		}
	}
	vector<string> cities_vec;
	cities_vec.assign(cities.begin(),cities.end());
	return cities_vec;
}

vector<string> LocationDB::listRoads(string country, string city){
	set<string> roads;
	int numLocations = xml.getNumTags("location");
	for(int i=0;i<numLocations;i++){
		if((country=="" || country=="unknown country" || xml.getAttribute("location","country","",i)==country)
			&& (city=="" || city=="unknown city" || xml.getAttribute("location","city","",i)==city))
		{
			string road = xml.getAttribute("location","road","",i);
			if ( road == "" )
				road = "unknown road";
			roads.insert(road);
		}
	}
	vector<string> roads_vec;
	roads_vec.assign(roads.begin(),roads.end());
	return roads_vec;
}

vector<string> LocationDB::listArtverts(string country, string city, string road){
	set<string> uids;
	int numLocations = xml.getNumTags("location");
	for(int i=0;i<numLocations;i++){
		if((country=="" || country=="unknown country" || xml.getAttribute("location","country","unknown country",i)==country)
				&& (city=="" || city=="unknown city" || xml.getAttribute("location","city","unknown city",i)==city)
				&& (road=="" || road=="unknown road" ||  xml.getAttribute("location","road","unknown road",i)==road))
			uids.insert(xml.getAttribute("location","uid","",i));
	}
	vector<string> uids_vec;
	uids_vec.assign(uids.begin(),uids.end());
	return uids_vec;
}
