#pragma once
#include "ofMain.h"

struct ActivityAndBang {
	ActivityAndBang(float _activity, bool _bang)
		:activity(_activity), bang(_bang) {
	}
	float activity;
	bool bang;
};

class Graph {
public:
	Graph();
	Graph(string _name, ofRectangle _rect);
	~Graph();

	void draw();
	void setName(string _name);
	void setPosition(ofVec2f _pos);
	void addValue(float _activity, bool _bang);
	void setLastValueBang();

    vector<ActivityAndBang> & getValues();
	ActivityAndBang getLastValue();
	float getLastTimeStamp();
    
    int getBangStack();
    void flushBangStack();
private:
	string name;
	ofRectangle rect;
	float lastTimeStamp;
	vector<ActivityAndBang> values;
	const int maxValueNum = 200;
    int bangStack = 0;
};

