#pragma once

#include "ofMain.h"
#include "ofxSoundObjects.h"
#include "ofxSoundObjectMatrixMixerRenderer.h"

class ofApp : public ofBaseApp{
	
public:
	void setup();
	void update();
	void draw();
	void exit();
	
	void keyPressed(int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y );
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void mouseEntered(int x, int y);
	void mouseExited(int x, int y);
	void windowResized(int w, int h);
	void dragEvent(ofDragInfo dragInfo);
	void gotMessage(ofMessage msg);
	
    ofxPanel gui_main;
    ofParameter<bool> bShowGui;
    ofParameter<int> inChannels, outChannels;
    ofParameter<int> sampleRate;
    ofParameter<string> inDeviceName, outDeviceName;
    ofParameter<int> inDeviceIndex, outDeviceIndex;
    int old_inDeviceIndex, old_outDeviceIndex;

    
	ofSoundStream stream;

	ofxSoundMatrixMixer mixer;
	ofxSoundMatrixMixerRenderer mixerRenderer;

	ofxSoundInput input;
	
	vector<shared_ptr<ofxSoundPlayerObject>> players;

    vector<ofSoundDevice> inDevices;
    vector<ofSoundDevice> outDevices;
    
	string mixerSettingsXmlPath;
	bool bSettingsLoaded = false;
	
	string loadPath;
	
	void loadFolder(const string& path);
	
//    ofFbo testFBO;
    
};
