#include "testApp.h"
#include "PersistanceEngine.h"


//--------------------------------------------------------------
void testApp::setup(){
	ofSetVerticalSync(true);
	ofSetFrameRate(10);
	ofBackground(255,255,255);
	ofSetLogLevel("artvertiserServer",OF_LOG_VERBOSE);

	//TODO: save artverts xml in the corresponding folder
	vector<Artvert> artverts = Artvert::listAll("uploads");
	for(int i=0; i<(int)artverts.size(); i++){
		if(artverts[i].checkIntegrity()){
			if(!Artvert(artverts[i].getUID(),"www/").isReady()){
				ofLogVerbose("artvertiserServer") << "loading already uploaded " << artverts[i].getUID();
				adverts.push(artverts[i]);
			}
		}else{
			ofLogError("artvertiserServer") << "found corrupt or incomplete advert " << artverts[i].getUID() << " deleting";
			artverts[i].remove();
		}
	}

	artverts = Artvert::listAll("www");
	for(int i=0; i<(int)artverts.size(); i++){
		if(!artverts[i].checkIntegrity()){
			ofLogError("artvertiserServer") << "found corrupt or incomplete advert " << artverts[i].getUID() << " deleting";
			artverts[i].remove();
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
		ofLogVerbose("artvertiserServer") << "/listcountries.of";
		vector<string> countries = locationDB.listCountries();
		for(int i=0;i<(int)countries.size();i++){
			ofLogVerbose("artvertiserServer") << countries[i];
			response.response+=countries[i]+"\n";
		}
	}else if(response.url=="/listcities.of"){
		ofLogVerbose("artvertiserServer") << "/listcities.of";
		vector<string> cities = locationDB.listCities(response.requestFields["country"]);
		for(int i=0;i<(int)cities.size();i++){
			ofLogVerbose("artvertiserServer") << cities[i];
			response.response+=cities[i]+"\n";
		}
	}else if(response.url=="/listroads.of"){
		ofLogVerbose("artvertiserServer") << "/listroads.of";
		vector<string> roads = locationDB.listRoads(response.requestFields["country"],response.requestFields["city"]);
		for(int i=0;i<(int)roads.size();i++){
			ofLogVerbose("artvertiserServer") << roads[i];
			response.response+=roads[i]+"\n";
		}
	}else if(response.url=="/listartverts.of"){
		ofLogVerbose("artvertiserServer") << "/listartverts.of";
		vector<string> uids = locationDB.listArtverts(response.requestFields["country"],response.requestFields["city"],response.requestFields["road"]);
		for(int i=0;i<(int)uids.size();i++){
			ofLogVerbose("artvertiserServer") << uids[i];
			response.response+=uids[i]+"\n";
		}
	}
}

void testApp::postRequest(ofxHTTPServerResponse & response){
	ofLogVerbose("artvertiserServer","new request: " + response.url);
	if(response.url=="/postadvert.of"){
		ofLogVerbose("artvertiserServer","receiving advert " + response.requestFields["uid"]);
		Artvert artvert(response.requestFields["uid"],"uploads/");
		if(!Artvert(artvert.getUID(),"www/").isReady()){
			if(!artvert.checkIntegrity()){
				ofLogError("artvertiserServer",response.requestFields["uid"] + " was corrupt removing");
				artvert.remove();
				response.errCode = 500;
				response.response = "<html> <head> oF http server </head> <body> the posted advert " + response.requestFields["uid"] + " was corrupt or some file was missing<body> </html>";
				return;
			}
			ofLogVerbose("artvertiserServer","artvert integrity ok: storing in uploads");
			mutex.lock();
			adverts.push(artvert);
			artvert.save();
			PersistanceEngine::save();
			mutex.unlock();
		}else{
			ofLogError("artvertiserServer","something is wrong, artvert was already analized, client should be checking before sending");
			response.errCode = 500;
			response.response = "<html> <head> oF http server </head> <body> the posted advert " + response.requestFields["uid"] + " was corrupt or some file was missing<body> </html>";
		}
		response.response = "<html> <head> oF http server </head> <body> image " + response.uploadedFiles["artvert"] + " received correctly <body> </html>";

		//cout <<  "image " + response.uploadedFiles[0] + " received correctly" << endl;
	}else if(response.url=="/listcountries.of"){
		ofLogVerbose("artvertiserServer") << "/listcountries.of";
		vector<string> countries = locationDB.listCountries();
		for(int i=0;i<(int)countries.size();i++){
			ofLogVerbose("artvertiserServer") << countries[i];
			response.response+=countries[i]+"\n";
		}
	}else if(response.url=="/listcities.of"){
		ofLogVerbose("artvertiserServer") << "/listcities.of";
		vector<string> cities = locationDB.listCities(response.requestFields["country"]);
		for(int i=0;i<(int)cities.size();i++){
			ofLogVerbose("artvertiserServer") << cities[i];
			response.response+=cities[i]+"\n";
		}
	}else if(response.url=="/listroads.of"){
		ofLogVerbose("artvertiserServer") << "/listroads.of";
		vector<string> roads = locationDB.listRoads(response.requestFields["country"],response.requestFields["city"]);
		for(int i=0;i<(int)roads.size();i++){
			ofLogVerbose("artvertiserServer") << roads[i];
			response.response+=roads[i]+"\n";
		}
	}else if(response.url=="/listartverts.of"){
		ofLogVerbose("artvertiserServer") << "/listartverts.of";
		vector<string> uids = locationDB.listArtverts(response.requestFields["country"],response.requestFields["city"],response.requestFields["road"]);
		for(int i=0;i<(int)uids.size();i++){
			ofLogVerbose("artvertiserServer") << uids[i];
			response.response+=uids[i]+"\n";
		}
	}else if(response.url=="/checkanalized.of"){
		mutex.lock();
		ofLogVerbose("artvertiserServer","ArtvertiserServer: checking analized: " + response.requestFields["uid"]);
		Artvert artvert(response.requestFields["uid"],"www/");
		ofLogVerbose("artvertiserServer","ArtvertiserServer: detector data: " + artvert.getDetectorData().path());
		if(artvert.isReady()){
			ofLogVerbose("artvertiserServer",response.requestFields["uid"] + " is ready");
			response.response = "<html> <head> oF http server </head> <body> image " + response.requestFields["uid"] + " already analyzed <body> </html>";
		}else{
			ofLogVerbose("artvertiserServer",response.requestFields["uid"] + " is not ready yet");
			response.errCode = 404;
			response.response = "<html> <head> oF http server </head> <body> the requested image " + response.requestFields["uid"] + " hasn't been analyzed yet<body> </html>";
		}
		mutex.unlock();
	}else if(response.url=="/checkuploaded.of"){
		mutex.lock();
		ofLogVerbose("artvertiserServer","checking if " + response.requestFields["uid"] + " is uploaded");
		Artvert advert(response.requestFields["uid"],"uploads/");
		Artvert artvert(response.requestFields["uid"],"www");
		if((advert.getROIFile().exists() && advert.getCompressedImage().exists()) || (artvert.getROIFile().exists() && artvert.getCompressedImage().exists())){
			ofLogVerbose("artvertiserServer",response.requestFields["uid"] + " already uploaded");
			response.response = "<html> <head> oF http server </head> <body> image " + response.requestFields["uid"] + " already uploaded <body> </html>";
		}else{
			ofLogVerbose("artvertiserServer",response.requestFields["uid"] + " not uploaded yet");
			response.errCode = 404;
			response.response = "<html> <head> oF http server </head> <body> the requested image " + response.requestFields["uid"] + " hasn't been uploaded yet<body> </html>";
		}
		mutex.unlock();
	}
}

void testApp::fileNotFound(ofxHTTPServerResponse & response){
	ofLogVerbose("artvertiserServer","file not found, redirecting?");
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

		ofLogVerbose("artvertiserServer","ArtvertiserServer: got new artvert: " + currentAdvert.getUID());
		currentImg.loadImage(currentAdvert.getCompressedImage());

		ofImage bmp;
		bmp.setUseTexture(false);
		bmp.loadImage(currentAdvert.getCompressedImage());
		bmp.saveImage(currentAdvert.getModel());

		ofLogVerbose("artvertiserServer") << "got " + currentAdvert.getUID() << endl;
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
	if(detector.getState()==Detector::Finished){
		locationDB.addLocation(currentAdvert);
		if(!currentAdvert.hasAlias()){
			ofLogError("artvertiserServer") << "finished analysis copying images to server folder" << endl;
			currentAdvert.getModel().moveTo("www/"+currentAdvert.getModel().getFileName());
			currentAdvert.getCompressedImage().copyTo("www/"+currentAdvert.getCompressedImage().getFileName());
			currentAdvert.getROIFile().moveTo("www/"+currentAdvert.getROIFile().getFileName());
			currentAdvert.getDetectorData().moveTo("www/"+currentAdvert.getDetectorData().getFileName());
			currentAdvert.getTrackerData().moveTo("www/"+currentAdvert.getTrackerData().getFileName());
		}
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

