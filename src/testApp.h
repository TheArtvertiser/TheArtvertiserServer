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

#ifndef _TEST_APP
#define _TEST_APP

#include "ofMain.h"

#include <queue>
#include <set>
#include <map>

#include "ofxHTTPServer.h"
#include "ofxOpenCv.h"
#include "Detector.h"
#include "Artvert.h"
#include "LocationDB.h"

class testApp : public ofBaseApp{

	public:

		void setup();
		void update();
		void draw();

		void keyPressed  (int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);

		void getRequest(ofxHTTPServerResponse & response);
		void postRequest(ofxHTTPServerResponse & response);
		void fileNotFound(ofxHTTPServerResponse & response);

		void analizeNext();

        ofxHTTPServer * server;

        Detector detector;

        queue<Artvert> adverts;
        map<Artvert,Artvert> redirections;
        ofMutex mutex;

        Artvert currentAdvert;
        ofImage currentImg;

        LocationDB locationDB;
};

#endif
