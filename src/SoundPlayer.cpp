#include "SoundPlayer.h"

SoundPlayer::SoundPlayer(Graph * _graph, ofRectangle _rect) {
	volume = 1.0;
	targetVolume = 1.0;
	speed = 1.0;
	targetSpeed = 1.0;
    machineGunThreshold = 0.05;
	value = 0;
	listenerAdded = false;
	mode = VolumeModuration;
	graph = nullptr;

	rect.set(_rect);
	graph = _graph;

	if (!listenerAdded) {
		ofAddListener(ofEvents().update, this, &SoundPlayer::update);
		ofAddListener(ofEvents().draw, this, &SoundPlayer::draw);
		ofAddListener(ofEvents().mouseMoved, this, &SoundPlayer::mouseMoved);
		ofAddListener(ofEvents().mousePressed, this, &SoundPlayer::mousePressed);
		ofAddListener(ofEvents().mouseDragged, this, &SoundPlayer::mouseDragged);
		ofAddListener(ofEvents().mouseReleased, this, &SoundPlayer::mouseReleased);
		ofAddListener(ofEvents().messageEvent, this, &SoundPlayer::messageReceived);
	}
}

SoundPlayer::~SoundPlayer() {
	if (listenerAdded) {
		ofRemoveListener(ofEvents().update, this, &SoundPlayer::update);
		ofRemoveListener(ofEvents().draw, this, &SoundPlayer::draw);
		ofRemoveListener(ofEvents().mouseMoved, this, &SoundPlayer::mouseMoved);
		ofRemoveListener(ofEvents().mousePressed, this, &SoundPlayer::mousePressed);
		ofRemoveListener(ofEvents().mouseDragged, this, &SoundPlayer::mouseDragged);
		ofRemoveListener(ofEvents().mouseReleased, this, &SoundPlayer::mouseReleased);
		ofRemoveListener(ofEvents().messageEvent, this, &SoundPlayer::messageReceived);
	}
}

void SoundPlayer::update(ofEventArgs & args) {
	if (!sound.isLoaded()) return;

	if (graph != nullptr) {
		float magnitude = 1.0;
		value = graph->getLastValue().activity * magnitude;

		switch (mode) {
            case VolumeModuration: targetVolume = MIN(value * 3, 1.0); break;
            case VolumeModuration_Sensitive: targetVolume = MIN(value * 3 * sensitiveFactor, 1.0); break;
            case SpeedModuration: targetSpeed = MIN(value * 5, 1.0); break;
            case SpeedModuration_Sensitive: targetSpeed = MIN(value * 5 * sensitiveFactor, 1.0); break;
            case TriggerPlay:
                if (graph->getBangStack() > 0 && !sound.isPlaying()) {
                    sound.play();
                    graph->flushBangStack();
                }
                break;
            case TriggerPlayMulti:
                if (graph->getBangStack() > 0) {
                    sound.play();
                    graph->flushBangStack();
                }
                break;
            case TriggerToggle:
                if (graph->getBangStack() > 0) {
                    if (sound.isPlaying()) {
                        soundPaused = !soundPaused;
                        sound.setPaused(soundPaused);
                    }
                    else  {
                        sound.play();
                        soundPaused = false;
                    }
                    graph->flushBangStack();
                }
                break;
            case MachineGun:
                    if (ofGetElapsedTimef() - graph->getLastTimeStamp() < 1.0) {
                        for (int i=graph->getValues().size() - 10; i < graph->getValues().size(); ++i) {
                            auto &value = graph->getValues()[i];
                            if (value.activity > machineGunThreshold && !sound.isPlaying()) {
                                sound.play();
                                break;
                            }
                        }
                    }
                break;
            
            default: break;
		}

		if (sound.isLoaded()) {
			float elapsed = ofGetElapsedTimef() - graph->getLastTimeStamp();
			switch (mode) {
                // one "break;" because same behaviour
                case VolumeModuration:
                case VolumeModuration_Sensitive:
                case SpeedModuration:
                case SpeedModuration_Sensitive:
                    if (sound.isPlaying() && elapsed > 1) {
                        sound.stop();
                    }
                    else if (!sound.isPlaying() && elapsed <= 1) {
                        sound.play();
                        sound.setLoop(true);
                    }
                    break;
                    
                // one "break;" because same behaviour
                case TriggerPlay:
                case TriggerPlayMulti:
                case TriggerToggle:
                    break;
                    
                default: break;
			}
		}
	}

	volume = MIN(1.0, volume + (targetVolume - volume) * 0.3);
	sound.setVolume(volume - 0.1);
	speed = MIN(1.0, speed + (targetSpeed - speed) * 0.3);
	sound.setSpeed(speed);
}

void SoundPlayer::draw(ofEventArgs & args) {
	ofPushMatrix();
	ofTranslate(rect.x, rect.y);

	// background
	ofFill();
	ofSetColor(50);
	ofDrawRectangle(0, 0, rect.width, rect.height);

	// value bar
	ofFill();
	ofSetColor(0, 200, 50);
	ofDrawRectangle(0, 0, rect.width * value, rect.height);

	// mode icon
	if (sound.isLoaded()) {
		ofSetColor(255);
		string modeStr = "";
		switch (mode) {
            case VolumeModuration: modeStr = "V"; break;
            case VolumeModuration_Sensitive: modeStr = "Vs"; break;
            case SpeedModuration: modeStr = "S"; break;
            case SpeedModuration_Sensitive: modeStr = "Ss"; break;
            case TriggerPlay: modeStr = "T1"; break;
            case TriggerPlayMulti: modeStr = "Tm"; break;
            case TriggerToggle: modeStr = "Tt"; break;
            case MachineGun: modeStr = "M"; break;

            case ModurationModeNum: break;
		}

		ofDrawBitmapString(modeStr, 2, rect.height / 2 + 4);
	}

	// play or stop icon
	if (sound.isLoaded()) {
		ofPushMatrix();
		ofTranslate(rect.height, 0);
		ofSetColor(190);
		ofFill();
		if (sound.isPlaying()) {
			float margin = 4;
			ofDrawTriangle(margin, margin, rect.height - margin, rect.height / 2, margin, rect.height - margin);
		}
		else {
			/*
			float size = 10;
			float offset = (rect.height - size) / 2;
			ofDrawRectangle(offset, offset, size, size);
			*/
		}
		ofPopMatrix();
	}

	// name
	ofSetColor(255);
    ofDrawBitmapString(name, rect.height * 2, rect.height / 2 + 4);

	// batten
	if (sound.isLoaded() && rect.inside(ofGetMouseX(), ofGetMouseY())) {
		float r = 6;
		ofPushMatrix();
		ofTranslate(rect.width - rect.height / 2, rect.height / 2);
		ofSetLineWidth(1);
		ofSetColor(255);
		ofDrawLine(-r, -r, r, r);
		ofDrawLine(r, -r, -r, r);
		ofPopMatrix();
	}

	ofPopMatrix();

	if (dragging) {
		ofSetColor(255, 100);
		ofDrawBitmapString(name, ofGetMouseX(), ofGetMouseY() + 4);
	}
}

void SoundPlayer::mouseMoved(ofMouseEventArgs & mouse) {
}

void SoundPlayer::mousePressed(ofMouseEventArgs & mouse) {
	// batten
	if (rect.inside(mouse.x, mouse.y)) {
		if (mouse.x < rect.x + rect.height) {
			toggleMode();
            modeDragging = true;
            modeDragDistance = 0;
		}
		else if (rect.x + rect.width - rect.height < mouse.x) {
			unload();		
		}
		else {
			dragging = true;
		}
	}
}

void SoundPlayer::mouseDragged(ofMouseEventArgs & mouse) {
    if (modeDragging) {
        ofVec2f move(mouse.x - ofGetPreviousMouseX(), mouse.y - ofGetPreviousMouseY());
        modeDragDistance += move.x;
        float modeChangeDistnace = 10;
        
        while (modeDragDistance <= -modeChangeDistnace) {
            reverseToggleMode();
            modeDragDistance += modeChangeDistnace;
        }
        while (modeDragDistance >= modeChangeDistnace) {
            toggleMode();
            modeDragDistance -= modeChangeDistnace;
        }
    }
}

void SoundPlayer::mouseReleased(ofMouseEventArgs & mouse) {
	if (dragging) {
		string delimiter = ",";
		string message = "dropEvent"
			+ delimiter + ofToString(mouse.x)
			+ delimiter + ofToString(mouse.y)
			+ delimiter + ofToString(long(this))
			;

		ofSendMessage(message);
	}

	dragging = false;
    modeDragging = false;
}

void SoundPlayer::messageReceived(ofMessage & message) {
	auto data = ofSplitString(message.message, ",");

	// swap this and draggedObject if dropped on this
	if (data[0] == "dropEvent" && data.size() >= 4) {
		int x = ofToInt(data[1]);
		int y = ofToInt(data[2]);
		if (rect.inside(x, y)) {
			auto other = (SoundPlayer *)(ofToInt64(data[3]));
			if (other != this) {
				string tmpPath = "";
				ModurationMode tmpMode = VolumeModuration;
				if (sound.isLoaded()) {
					tmpPath = path;
					tmpMode = mode;
				}
				unload();

				load(other->path);
				setMode(other->getMode());
				if (tmpPath != "") {
					other->load(tmpPath);
					other->setMode(tmpMode);
				}
				else {
					other->unload();
				}
			}
		}
	}
}

void SoundPlayer::load(string _path) {
	path = _path;
#ifdef WIN32
	name = ofSplitString(path, "\\").back();
#else
	name = ofSplitString(path, "/").back();
#endif
	sound.load(path);

	setMode(VolumeModuration);

	if (sound.isLoaded()) {
	}
	else {
		ofLogError("Load fault " + path);
	}
}

void SoundPlayer::unload() {
	path = "";
	name = "";
	sound.unload();
}

void SoundPlayer::setRect(ofRectangle _rect) {
	rect = _rect;
}

string SoundPlayer::getPath() {
	return path;
}

void SoundPlayer::setMode(ModurationMode _mode) {
    soundPaused = false;

	mode = _mode;
	switch (mode) {
        case VolumeModuration:
        case VolumeModuration_Sensitive:
            targetSpeed = 1.0;
            sound.setLoop(true);
            break;

        case SpeedModuration:
        case SpeedModuration_Sensitive:
            targetVolume = 1.0;
            sound.setLoop(true);
            break;

        case TriggerPlay:
            targetSpeed = 1.0;
            targetVolume = 1.0;
            sound.setLoop(false);
            break;

        case TriggerPlayMulti:
            targetSpeed = 1.0;
            targetVolume = 1.0;
            sound.setLoop(false);
            break;
            
        case TriggerToggle:
            targetSpeed = 1.0;
            targetVolume = 1.0;
            sound.setLoop(false);
            break;

        case MachineGun:
            targetSpeed = 1.0;
            targetVolume = 1.0;
            sound.setLoop(false);
            break;
	}
}

void SoundPlayer::toggleMode() {
	setMode(ModurationMode(((int)mode + 1) % ModurationModeNum));
}

void SoundPlayer::reverseToggleMode() {
    setMode(ModurationMode(((int)mode + (int)ModurationModeNum - 1) % ModurationModeNum));
}
