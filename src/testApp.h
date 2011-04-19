#ifndef _TEST_APP
#define _TEST_APP

#include "ofMain.h"

#include <queue>
#include <set>

#include "ofxHTTPServer.h"
#include "ofxOpenCv.h"
#include "Detector.h"
#include "Artvert.h"

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

		void analizeNext();

        ofxHTTPServer * server;

        Detector detector;

        queue<Artvert> adverts;
        ofMutex mutex;

        Artvert currentAdvert;
        ofImage currentImg;
};

#endif
