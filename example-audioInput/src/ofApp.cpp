#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){	 
	
	ofSetVerticalSync(true);
    //        bool setup(int outChannels, int inChannels, int sampleRate, int bufferSize, int nBuffers));
    //    stream.setup(2, 2, 44100, 256, 1);
    
    
    gui_main.setup();
    gui_main.setName("audioInput");
    gui_main.setPosition(10,10);
    gui_main.setDefaultHeaderBackgroundColor(ofColor(255,0,0));
    gui_main.add(bShowGui.set("showGui",true));
    gui_main.add(deviceIndex.set("deviceIndex",0,0,10));
    gui_main.add(inChannels.set("inCHannels",2,0,16));
    gui_main.add(outChannels.set("outChannels",2,0,16));
    gui_main.add(sampleRate.set("sampleRate",48000,44100,96000));
    gui_main.loadFromFile("gui_main.xml");
    
    
    // Setup the sound stream.
    ofSoundStreamSettings settings;
    settings.bufferSize = 256;
    settings.numBuffers = 1;
    settings.numInputChannels =  inChannels;
    settings.numOutputChannels = outChannels;
    settings.sampleRate = sampleRate;
    
    stream.setup(settings);
//    stream.setup(inChannels,outChannels, sampleRate, 256, 1);
    wave.setup(0, 0, ofGetWidth(), ofGetHeight());
    
    stream.setInput(input);
    stream.setOutput(output);
    
    input.connectTo(wave).connectTo(output);
    
}

//--------------------------------------------------------------
void ofApp::update(){
}

//--------------------------------------------------------------
void ofApp::draw(){
    ofBackground(0);
    wave.draw();
    
    if(bShowGui) gui_main.draw();
}

//--------------------------------------------------------------
void ofApp::keyPressed  (int key){ }

//--------------------------------------------------------------
void ofApp::keyReleased(int key){ 
    if(key == 'g'){
        bShowGui = !bShowGui;
        if(bShowGui == false){
               gui_main.saveToFile("gui_main.xml");
        }
    }
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

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}

