#include "Graph.h"



Graph::Graph() {
	lastTimeStamp = 0;
}


Graph::Graph(string _name, ofRectangle _rect) :name(_name), rect(_rect) {
	lastTimeStamp = 0;
}

Graph::~Graph() {
}

void Graph::draw() {
	ofPushStyle();
	ofPushMatrix();
	ofTranslate(rect.x, rect.y);

	ofNoFill();
	ofSetColor(255, 100);
	ofDrawRectangle(0, 0, rect.width, rect.height);

	// activity graph
	if (values.size() > 1) {
		ofBeginShape();
		for (int i = 0; i < values.size(); ++i) {
			float magnitude = 1;

			float x = i * rect.width / maxValueNum;
			float y = (1.0 - values[i].activity * magnitude) * rect.height;

			ofVertex(x, y);

			// bang
			if (values[i].bang) {
				ofSetColor(255, 255, 0);
				ofDrawLine(x, 0, x, rect.height);
			}
		}

		ofSetColor(0, 255, 0);
		ofEndShape();
	}

	ofDrawBitmapString(name, 4, 16);

	ofPopMatrix();
	ofPopStyle();
}

void Graph::setName(string _name) {
	name = _name;
}

void Graph::setPosition(ofVec2f _pos) {
	rect.x = _pos.x;
	rect.y = _pos.y;
}

void Graph::addValue(float _activity, bool _bang) {
	values.push_back(ActivityAndBang(_activity, _bang));
	if (values.size() > maxValueNum) {
		values.erase(values.begin());
	}
    if (_bang) bangStack++;

	lastTimeStamp = ofGetElapsedTimef();
}

void Graph::setLastValueBang() {
	if (!values.empty()) {
		values.back().bang = true;
        bangStack++;
	}
}

vector<ActivityAndBang> & Graph::getValues() {
    return values;
}

ActivityAndBang Graph::getLastValue() {
	if (values.empty()) {
		return ActivityAndBang(0, false);
	}
	else {
		return values.back();
	}
}

float Graph::getLastTimeStamp() {
	return lastTimeStamp;
}

int Graph::getBangStack(){
    return bangStack;
}

void Graph::flushBangStack(){
    bangStack = 0;
}
