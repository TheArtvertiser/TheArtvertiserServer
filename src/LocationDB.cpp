/*
 * LocationDB.cpp
 *
 *  Created on: 30/04/2011
 *      Author: arturo
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

void LocationDB::addLocation(const ofxLocation & loc){
	ofxXmlSettings address = geo.getAddressFor(loc);

	string road = address.getValue("reversegeocode:addressparts:road","");
	string city = address.getValue("reversegeocode:addressparts:city","");
	string country = address.getValue("reversegeocode:addressparts:country","");

	int numLoc = xml.addTag("location");
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
		countries.insert(xml.getAttribute("location","country","",i));
	}
	vector<string> countries_vec;
	countries_vec.assign(countries.begin(),countries.end());
	return countries_vec;
}

vector<string> LocationDB::listCities(string country){
	set<string> cities;
	int numLocations = xml.getNumTags("location");
	for(int i=0;i<numLocations;i++){
		if(country=="" || xml.getAttribute("location","country","",i)==country)
			cities.insert(xml.getAttribute("location","city","",i));
	}
	vector<string> cities_vec;
	cities_vec.assign(cities.begin(),cities.end());
	return cities_vec;
}

vector<string> LocationDB::listRoads(string country, string city){
	set<string> roads;
	int numLocations = xml.getNumTags("location");
	for(int i=0;i<numLocations;i++){
		if((country=="" || xml.getAttribute("location","country","",i)==country)
			&& (city=="" || xml.getAttribute("location","city","",i)==city))
			roads.insert(xml.getAttribute("location","road","",i));
	}
	vector<string> roads_vec;
	roads_vec.assign(roads.begin(),roads.end());
	return roads_vec;
}
