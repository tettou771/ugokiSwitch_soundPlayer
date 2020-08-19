#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
	ofLogToConsole();
	ofSetFrameRate(60);
    ofSetEscapeQuitsApp(false);
#ifdef TARGET_OSX
	ofSetDataPathRoot("../data");
#endif

	int graphNum = 60;
	int rowNum = 30;
	int margin = 10;
	ofVec2f last;

	for (int i = 0; i < graphNum; ++i) {
		string name = ofToString(i);
		ofRectangle rect(
			margin + (i / rowNum) * 440,
			margin + (i % rowNum) * 24,
			200,
			20
		);
		graphs.push_back(new Graph(name, rect));

		rect.x += rect.width + margin;
		sounds.push_back(new SoundPlayer(graphs.back(), rect));

		last.x = MAX(last.x, rect.x + rect.width);
		last.y = MAX(last.y, rect.y + rect.height);
	}

	ofSetWindowShape(last.x + margin, last.y + margin);

	loadConfig();
	oscReceiver.setup(oscPort);
    
    oscSender.setup("localhost", 6001);
}

//--------------------------------------------------------------
void ofApp::update(){
	while (oscReceiver.hasWaitingMessages()) {
		ofxOscMessage m;
		oscReceiver.getNextMessage(m);

		if (m.getAddress() == "/activity") {
			int index = m.getArgAsInt(0);
			float activity = m.getArgAsFloat(1);

			if (0 <= index && index < graphs.size()) {
				graphs[index]->addValue(activity, false);
			}
		}

		else if (m.getAddress() == "/bang") {
			int index = m.getArgAsInt(0);
			if (0 <= index && index < graphs.size()) {
				graphs[index]->setLastValueBang();
			}
            
            if (index >= 48) {
                ofxOscMessage msg;
                msg.setAddress("/flash");
                oscSender.sendMessage(msg);
            }
		}
	}
}

//--------------------------------------------------------------
void ofApp::draw(){
	ofClear(0);

	for (auto &g : graphs) {
		g->draw();
	}
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
	saveConfig();
}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

void ofApp::loadConfig() {
	oscPort = 6000;
		
	ofXml xml;
	if (xml) {
		xml.load("config.xml");
		auto configTag = xml.findFirst("config");
		if (configTag) {
			auto oscTag = configTag.findFirst("osc");
			if (oscTag) {
				oscPort = oscTag.getAttribute("port").getIntValue();
                ofSetWindowTitle("OSC port: " + ofToString(oscPort));
			}

		}

		auto soundsTag = configTag.findFirst("sounds");
		if (soundsTag) {
			int index = 0;
			for (auto &soundTag : soundsTag.getChildren("sound")) {
				string path = soundTag.getAttribute("path").getValue();
                SoundPlayer::ModurationMode mode = (SoundPlayer::ModurationMode)(soundTag.getAttribute("mode").getIntValue());
				if (path != "") {
					sounds[index]->load(path);
                    sounds[index]->setMode(mode);
				}
				index++;
				if (index >= sounds.size()) break;
			}
		}
	}
}

void ofApp::saveConfig() {
	ofXml xml;
	auto configTag = xml.appendChild("config");
	{
		auto oscTag = configTag.appendChild("osc");
		{
			oscTag.setAttribute("port", ofToString(oscPort));
		}

		auto soundsTag = configTag.appendChild("sounds");
		{
			for (auto &s : sounds) {
				auto soundTag = soundsTag.appendChild("sound");
				soundTag.setAttribute("path", s->getPath());
                soundTag.setAttribute("mode", (int)s->getMode());
			}
		}
	}

	xml.save("config.xml");
}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){
    int pastLoadSound = -1;
	for (auto &path : dragInfo.files) {
        if (pastLoadSound == -1) {
            for (int i=0;i<sounds.size(); ++i) {
                auto &s = sounds[i];
                if (s->getRect().inside(dragInfo.position.x, dragInfo.position.y)) {
                    s->load(path);
                    pastLoadSound = i;
                    break;
                }
            }
        }else{
            for (int i=pastLoadSound + 1; i<sounds.size(); ++i) {
                auto &s = sounds[i];
                if (!s->isLoaded()) {
                    s->load(path);
                    pastLoadSound = i;
                    break;
                }
            }
        }
	}

	saveConfig();
}
