#pragma once

#include "ofMain.h"
#include "ofxSoundObjects.h"
#include "ofxSoundObjectMatrixMixerRenderer.h"

#define USE_WAVES
#ifdef USE_WAVES
#include "waveformDraw.h"
#endif
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
	
    void setAllMicsCrossPoints(float _level);
	ofxSoundMatrixMixer mixer; 
	ofxSoundMatrixMixerRenderer mixerRenderer;
	

	ofSoundStream stream;
	
	ofxSoundInputMultiplexer input;
	ofxSoundOutputMultiplexer output;
//	ofxSoundMixer mixer;
	
    int inChannels;
    int outChannels;
    
	vector< unique_ptr<ofxSoundRecorderObject>> recorders;
 
    float myTimer;
	bool toggleChannelRecording(int chan);
#ifdef USE_WAVES
	vector< waveformDraw > waves;
#endif
};

