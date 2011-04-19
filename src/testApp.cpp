#include "testApp.h"
#include "PersistanceEngine.h"


//--------------------------------------------------------------
void testApp::setup(){
	ofSetVerticalSync(true);
	ofBackground(255,255,255);
	ofSetLogLevel(OF_LOG_VERBOSE);

	vector<Artvert> artverts = Artvert::listAll("uploads");
	for(int i=0; i<(int)artverts.size(); i++){
		cout << "loading already uploaded " << artverts[i].getUID() << endl;
		adverts.push(artverts[i]);
	}

	server = ofxHTTPServer::getServer(); // get the instance of the server
	server->setServerRoot("www");		 // folder with files to be served
	server->setUploadDir("uploads");		 // folder to save uploaded files
	server->setCallbackExtension("of");	 // extension of urls that aren't files but will generate a post or get event
	ofAddListener(server->getEvent,this,&testApp::getRequest);
	ofAddListener(server->postEvent,this,&testApp::postRequest);
	server->start(8888);


	ofNoFill();
}

void testApp::getRequest(ofxHTTPServerResponse & response){
	if(response.url=="/showScreen.of"){
		response.response="<html> <head> <title>oF http server</title> \
				<script> \
				function beginrefresh(){ \
					setTimeout(\"window.location.reload();\",30); \
				}\
		window.onload=beginrefresh; \
		</script>\
				</head> <body> <img src=\"screen.jpg\"/> </body> </html>";
	}
}

void testApp::postRequest(ofxHTTPServerResponse & response){
	ofLog(OF_LOG_VERBOSE,"new request: " + response.url);
	if(response.url=="/postadvert.of"){
		Artvert artvert(response.requestFields["uid"],"uploads");
		mutex.lock();
		adverts.push(artvert);
		artvert.save();
		PersistanceEngine::save();
		mutex.unlock();

		response.response = "<html> <head> oF http server </head> <body> image " + response.uploadedFiles["artvert"] + " received correctly <body> </html>";

		//cout <<  "image " + response.uploadedFiles[0] + " received correctly" << endl;
	}else if(response.url=="/checkanalized.of"){
		mutex.lock();
		ofLog(OF_LOG_VERBOSE,"ArtvertiserServer: checking analized: " + response.requestFields["uid"]);
		Artvert artvert(response.requestFields["uid"],"www/");
		ofLog(OF_LOG_VERBOSE,"ArtvertiserServer: detector data: " + artvert.getDetectorData().path());
		if(artvert.getDetectorData().exists() && artvert.getTrackerData().exists()){
			response.response = "<html> <head> oF http server </head> <body> image " + response.requestFields["uid"] + " already analyzed <body> </html>";
		}else{
			response.errCode = 404;
			response.response = "<html> <head> oF http server </head> <body> the requested image " + response.requestFields["uid"] + " hasn't been analyzed yet<body> </html>";
		}
		mutex.unlock();
	}else if(response.url=="/checkuploaded.of"){
		mutex.lock();
		Artvert artvert(response.requestFields["uid"],"uploads");
		if(artvert.getROIFile().exists() && artvert.getCompressedImage().exists()){
			response.response = "<html> <head> oF http server </head> <body> image " + response.requestFields["uid"] + " already uploaded <body> </html>";
		}else{
			response.errCode = 404;
			response.response = "<html> <head> oF http server </head> <body> the requested image " + response.requestFields["uid"] + " hasn't been uploaded yet<body> </html>";
		}
		mutex.unlock();
	}
}

void testApp::analizeNext(){
	mutex.lock();
	if(!adverts.empty()){
		currentAdvert = adverts.front();
		adverts.pop();
		ofLog(OF_LOG_VERBOSE,"ArtvertiserServer: got new artvert: " + currentAdvert.getUID());
		currentImg.loadImage(currentAdvert.getCompressedImage());

		ofImage bmp;
		bmp.setUseTexture(false);
		bmp.loadImage(currentAdvert.getCompressedImage());
		bmp.saveImage(currentAdvert.getModel());

		cout << "got " + currentAdvert.getUID() << endl;
		detector.setupTrainOnly(currentAdvert.getModel().path());
	}else{
		if(detector.getState()!=Detector::Init) detector.close();
	}
	mutex.unlock();
}

//--------------------------------------------------------------
void testApp::update(){
	if(detector.getState()==Detector::Init){
		analizeNext();
	}
	if(detector.getState()==Detector::Finished){
		cerr << "finished analysis copying images to server folder" << endl;
		currentAdvert.getDetectorData().copyTo("www/"+currentAdvert.getDetectorData().getFileName());
		currentAdvert.getTrackerData().copyTo("www/"+currentAdvert.getTrackerData().getFileName());

		analizeNext();
	}
}

//--------------------------------------------------------------
void testApp::draw(){
	if(currentImg.bAllocated()){
		currentImg.draw(0,0);
		ofBeginShape();
			ofVertexes(currentAdvert.getROI());
		ofEndShape(true);
	}
}

//--------------------------------------------------------------
void testApp::keyPressed  (int key){

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){
}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){
}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){
}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){

}

