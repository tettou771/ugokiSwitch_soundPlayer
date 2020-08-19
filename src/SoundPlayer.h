#pragma once
#include "ofMain.h"
#include "Graph.h"

class SoundPlayer {
public:
	SoundPlayer(Graph * _graph, ofRectangle _rect);
	~SoundPlayer();

	void update(ofEventArgs &args);
	void draw(ofEventArgs &args);

	void mouseMoved(ofMouseEventArgs &mouse);
	void mousePressed(ofMouseEventArgs &mouse);
	void mouseDragged(ofMouseEventArgs &mouse);
	void mouseReleased(ofMouseEventArgs &mouse);
	void messageReceived(ofMessage &message);

	void load(string _path);
	void unload();
	void setRect(ofRectangle _rect);
	ofRectangle & getRect() { return rect; }
	string getPath();

	enum ModurationMode {
		VolumeModuration,
        VolumeModuration_Sensitive,
		SpeedModuration,
        SpeedModuration_Sensitive,
		TriggerPlay,
		TriggerPlayMulti,
        TriggerToggle,
        MachineGun,
		ModurationModeNum
	};
    const float sensitiveFactor = 5;
	void setMode(ModurationMode _mode);
	void toggleMode();
    void reverseToggleMode();
	ModurationMode getMode() { return mode; }
    bool isLoaded() {return sound.isLoaded();}

private:
	ofRectangle rect;
	Graph * graph;

	ModurationMode mode;

	string path, name;
	ofSoundPlayer sound;
	float volume, targetVolume;
	float speed, targetSpeed;
	float value;
    float machineGunThreshold;
    bool soundPaused;
    
	bool dragging = false;
    bool modeDragging = false;
    float modeDragDistance = 0;

	bool listenerAdded;
};

