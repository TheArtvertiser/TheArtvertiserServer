#include "testApp.h"
#include "PersistanceEngine.h"


//--------------------------------------------------------------
void testApp::setup(){
	ofSetVerticalSync(true);
	ofBackground(255,255,255);
	ofSetLogLevel(OF_LOG_VERBOSE);

	//TODO: save artverts xml in the corresponding folder
	vector<Artvert> artverts = Artvert::listAll("uploads");
	for(int i=0; i<(int)artverts.size(); i++){
		cout << "loading already uploaded " << artverts[i].getUID() << endl;
		if(!Artvert(artverts[i].getUID(),"www/").isReady()){
			adverts.push(artverts[i]);
		}
	}

	server = ofxHTTPServer::getServer(); // get the instance of the server
	server->setServerRoot("www");		 // folder with files to be served
	server->setUploadDir("uploads");		 // folder to save uploaded files
	server->setCallbackExtension("of");	 // extension of urls that aren't files but will generate a post or get event
	ofAddListener(server->getEvent,this,&testApp::getRequest);
	ofAddListener(server->postEvent,this,&testApp::postRequest);
	ofAddListener(server->fileNotFoundEvent,this,&testApp::fileNotFound);
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
	}else if(response.url=="/listcountries.of"){
		vector<string> countries = locationDB.listCountries();
		for(int i=0;i<(int)countries.size();i++){
			response.response+=countries[i]+"\n";
		}
	}else if(response.url=="/listcities.of"){
		vector<string> cities = locationDB.listCities(response.requestFields["country"]);
		for(int i=0;i<(int)cities.size();i++){
			response.response+=cities[i]+"\n";
		}
	}else if(response.url=="/listroads.of"){
		vector<string> roads = locationDB.listRoads(response.requestFields["country"],response.requestFields["city"]);
		for(int i=0;i<(int)roads.size();i++){
			response.response+=roads[i]+"\n";
		}
	}
}

void testApp::postRequest(ofxHTTPServerResponse & response){
	ofLog(OF_LOG_VERBOSE,"new request: " + response.url);
	if(response.url=="/postadvert.of"){
		Artvert artvert(response.requestFields["uid"],"uploads/");
		mutex.lock();
		adverts.push(artvert);
		artvert.save();
		PersistanceEngine::save();
		locationDB.addLocation(artvert.getLocation());
		mutex.unlock();

		response.response = "<html> <head> oF http server </head> <body> image " + response.uploadedFiles["artvert"] + " received correctly <body> </html>";

		//cout <<  "image " + response.uploadedFiles[0] + " received correctly" << endl;
	}else if(response.url=="/listcountries.of"){
		vector<string> countries = locationDB.listCountries();
		for(int i=0;i<(int)countries.size();i++){
			response.response+=countries[i]+"\n";
		}
	}else if(response.url=="/listcities.of"){
		vector<string> cities = locationDB.listCities(response.requestFields["country"]);
		for(int i=0;i<(int)cities.size();i++){
			response.response+=cities[i]+"\n";
		}
	}else if(response.url=="/listroads.of"){
		vector<string> roads = locationDB.listRoads(response.requestFields["country"],response.requestFields["city"]);
		for(int i=0;i<(int)roads.size();i++){
			response.response+=roads[i]+"\n";
		}
	}else if(response.url=="/checkanalized.of"){
		mutex.lock();
		ofLog(OF_LOG_VERBOSE,"ArtvertiserServer: checking analized: " + response.requestFields["uid"]);
		Artvert artvert(response.requestFields["uid"],"www/");
		ofLog(OF_LOG_VERBOSE,"ArtvertiserServer: detector data: " + artvert.getDetectorData().path());
		if(artvert.isReady()){
			response.response = "<html> <head> oF http server </head> <body> image " + response.requestFields["uid"] + " already analyzed <body> </html>";
		}else{
			response.errCode = 404;
			response.response = "<html> <head> oF http server </head> <body> the requested image " + response.requestFields["uid"] + " hasn't been analyzed yet<body> </html>";
		}
		mutex.unlock();
	}else if(response.url=="/checkuploaded.of"){
		mutex.lock();
		Artvert advert(response.requestFields["uid"],"uploads/");
		Artvert artvert(response.requestFields["uid"],"www");
		if((advert.getROIFile().exists() && advert.getCompressedImage().exists()) || (artvert.getROIFile().exists() && artvert.getCompressedImage().exists())){
			response.response = "<html> <head> oF http server </head> <body> image " + response.requestFields["uid"] + " already uploaded <body> </html>";
		}else{
			response.errCode = 404;
			response.response = "<html> <head> oF http server </head> <body> the requested image " + response.requestFields["uid"] + " hasn't been uploaded yet<body> </html>";
		}
		mutex.unlock();
	}
}

void testApp::fileNotFound(ofxHTTPServerResponse & response){
	ofLog(OF_LOG_VERBOSE,"file not found, redirecting?");
	string uid = response.url.substr(1,response.url.find('.')-1);
	Artvert artvert(uid,"www");
	string file = response.url.substr(response.url.find('.'));
	if(artvert.hasAlias()){
		if(file == ".bmp.detector_data")
			response.location = "/"+artvert.getAlias().getDetectorData().getFileName();
		else if(file == ".bmp.tracker_data")
			response.location = "/"+artvert.getAlias().getTrackerData().getFileName();
		else if(file == ".bmp.roi")
			response.location = "/"+artvert.getAlias().getROIFile().getFileName();
		else if(file == ".bmp")
			response.location = "/"+artvert.getAlias().getModel().getFileName();
		else if(file == ".jpg")
			response.location = "/"+artvert.getAlias().getCompressedImage().getFileName();
		response.errCode = 301;
	}
}

void testApp::analizeNext(){
	mutex.lock();
	if(!adverts.empty()){
		currentAdvert = adverts.front();
		adverts.pop();

		if(currentAdvert.isReady()){
			mutex.unlock();
			return;
		}

		ofLog(OF_LOG_VERBOSE,"ArtvertiserServer: got new artvert: " + currentAdvert.getUID());
		currentImg.loadImage(currentAdvert.getCompressedImage());

		ofImage bmp;
		bmp.setUseTexture(false);
		bmp.loadImage(currentAdvert.getCompressedImage());
		bmp.saveImage(currentAdvert.getModel());

		cout << "got " + currentAdvert.getUID() << endl;
		vector<Artvert> artverts = Artvert::listAll("www");
		for(int i=0;i<(int)artverts.size();i++){
			if(currentAdvert.getUID() == artverts[i].getUID() || artverts[i].hasAlias() || !artverts[i].isReady()) continue;
			ofImage img;
			img.loadImage(artverts[i].getModel());
			detector.setup(artverts[i].getModel().path(),img.getWidth(),img.getHeight(),artverts[i].getROI(),true);
			detector.newFrame(bmp.getPixelsRef());
			if(detector.isDetected() || detector.isTracked()){
				redirections[currentAdvert] = artverts[i];
				currentAdvert.setAliasUID(artverts[i].getUID());
				currentAdvert.save();
				PersistanceEngine::save();
				detector.close();
				cerr << "redirecting " << currentAdvert.getUID() << "->" << artverts[i].getUID() << endl;
				mutex.unlock();
				return;
			}
		}

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
	if(detector.getState()==Detector::Finished && !currentAdvert.hasAlias()){
		cerr << "finished analysis copying images to server folder" << endl;
		currentAdvert.getModel().moveTo("www/"+currentAdvert.getModel().getFileName());
		currentAdvert.getCompressedImage().copyTo("www/"+currentAdvert.getCompressedImage().getFileName());
		currentAdvert.getROIFile().moveTo("www/"+currentAdvert.getROIFile().getFileName());
		currentAdvert.getDetectorData().moveTo("www/"+currentAdvert.getDetectorData().getFileName());
		currentAdvert.getTrackerData().moveTo("www/"+currentAdvert.getTrackerData().getFileName());

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

