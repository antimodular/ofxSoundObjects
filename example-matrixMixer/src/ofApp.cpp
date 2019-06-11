#include "ofApp.h"

//#define USE_LOAD_DIALOG // comment this line if you dont want to use the system load dialog

//--------------------------------------------------------------
void ofApp::setup(){
	
	ofSetLogLevel(OF_LOG_VERBOSE);
	
		
    gui_main.setup();
    gui_main.setName("audioInput");
    gui_main.setPosition(10,10);
    gui_main.setDefaultHeaderBackgroundColor(ofColor(255,0,0));
    gui_main.add(bShowGui.set("showGui",true));
    gui_main.add(inDeviceIndex.set("inDeviceIndex",0,0,6));
    gui_main.add(inDeviceName.set("in",""));
    
    gui_main.add(outDeviceIndex.set("outDeviceIndex",0,0,6));
    gui_main.add(outDeviceName.set("out",""));

    gui_main.add(inChannels.set("inChannels",2,0,16));
    gui_main.add(outChannels.set("outChannels",2,0,16));
    gui_main.add(sampleRate.set("sampleRate",48000,44100,96000));
    gui_main.loadFromFile("gui_main.xml");
    
    old_inDeviceIndex = -1;
    old_outDeviceIndex = -1;

	
    ofxSoundUtils::printInputSoundDevices();
    ofxSoundUtils::printOutputSoundDevices();

#ifdef USE_LOAD_DIALOG
//	this will open a dialog to select a folder in which you should have audio files.
	// Each audio file will be connected to a new input of your matrix mixer.
	// by default the file dialog should open in the soundplayer example's folder that has some audio files so you can simply pres open if you want to try those 
	ofFileDialogResult r = ofSystemLoadDialog("Select folder with audio files(wav, aiff, mp3)", true, "../../../../../examples/sound/soundPlayerExample/bin/data/sounds");
	if(r.bSuccess){
		loadPath = r.getPath();
#else
		// change this path if you want to use another one and not use the system load dialog
//        loadPath = ofToDataPath("../../../../../examples/sound/soundPlayerExample/bin/data/sounds");
//        loadPath = "/Users/roy/openFrameworks/examples/sound/soundPlayerExample/bin/data/sounds";
//        loadPath = ofToDataPath("/Users/stephanschulz/Desktop/recordings_all/recordings_wav48k");
         loadPath = ofToDataPath("recordings_wav48k");
#endif
		
        ofLog()<<"loadPath "<<loadPath;
		loadFolder(loadPath);
#ifdef USE_LOAD_DIALOG
	}
#endif
	//Here we also connect the audio input to the mixer
	//
	input.connectTo(mixer);
	
	 inDevices = ofxSoundUtils::getInputSoundDevices();
	 outDevices = ofxSoundUtils::getOutputSoundDevices();
	
		// IMPORTANT!!!
	// The following two lines of code is where you set which audio interface to use.
	// the index is the number printed in the console inside [ ] before the interface name 
	// You can use a different input and output device.
	
//    inDeviceIndex = 4; //3; //5;
//    outDeviceIndex = 4; //2; //5;
    inChannels = inDevices[inDeviceIndex].inputChannels;
    outChannels = outDevices[outDeviceIndex].outputChannels;

	// Setup the sound stream.
	ofSoundStreamSettings settings;
	settings.bufferSize = 256;
	settings.numBuffers = 1;
    ofLog()<<"inDevices[inDeviceIndex].inputChannels "<<inDevices[inDeviceIndex].inputChannels;
	settings.numInputChannels =  inDevices[inDeviceIndex].inputChannels;
	settings.numOutputChannels = outDevices[outDeviceIndex].outputChannels;
	
	if(players.size()){
		// we setup the samplerate of the sound stream according to the one of the first player
		if(players[0])
            ofLog()<<"players[0]->getSoundFile().getSampleRate() "<<players[0]->getSoundFile().getSampleRate();
			settings.sampleRate = players[0]->getSoundFile().getSampleRate();
	}else{
		ofLogWarning("ofApp::setup", "could not set sample rate for soundstream") ;
	}
	
	
	settings.setInDevice(inDevices[inDeviceIndex]);
	settings.setOutDevice(outDevices[outDeviceIndex]);
	
	
	stream.setup(settings);
	
	input.setInputStream(stream);
	mixer.setOutputStream(stream);
	
	mixer.setOutputVolumeForAllChannels(1);
	
//		mixerRenderer.setObject(&mixer);
	mixerRenderer.obj = &mixer;
	mixerRenderer.enableSliders();
	mixerSettingsXmlPath = "mixerSettings.xml";
	
    input.connectTo(mixer);
    
//    testFBO.allocate(900,900,GL_RGBA);
//    
//    testFBO.begin();
//    ofClear(255,255,255,0);
//    
//    ofFill();
//    ofSetColor(255, 0, 0);
//    ofDrawCircle(0, 0, 300);
//    testFBO.end();
}
//--------------------------------------------------------------
void ofApp::loadFolder(const string& path){
	ofFile f(path);
	if(f.isDirectory()){
		ofDirectory dir(path);
		dir.allowExt("wav");
		dir.allowExt("aiff");
		dir.allowExt("mp3");
		dir.listDir();
		auto startIndex = players.size();
		players.resize( startIndex + dir.size());
		for (int i = 0; i < dir.size(); i++) {
			players[startIndex + i] = make_shared<ofxSoundPlayerObject>();
			if(players[startIndex + i]->load(dir.getPath(i))){
				players[startIndex + i]->connectTo(mixer);
				players[startIndex + i]->play();
				players[startIndex + i]->setLoop(true);
			}
		}
	}	
}
//--------------------------------------------------------------
void ofApp::update(){
	
    if(old_inDeviceIndex != inDeviceIndex){
        inDeviceIndex = MIN(inDeviceIndex.get(),inDevices.size()-1);
        old_inDeviceIndex = inDeviceIndex;
        vector<string> splitName = ofSplitString(inDevices[inDeviceIndex].name, ":");
        inDeviceName = splitName[1].substr(0,10); //str.substr (3,5);
        //        ofLog()<<inDeviceIndex<<" inDevices[inDeviceIndex].name "<<inDevices[inDeviceIndex].name<<" inDeviceName "<<inDeviceName;
    }
    if(old_outDeviceIndex != outDeviceIndex){
        outDeviceIndex = MIN(outDeviceIndex.get(),outDevices.size()-1);
        vector<string> splitName = ofSplitString(outDevices[outDeviceIndex].name, ":");
        
        old_outDeviceIndex = outDeviceIndex;
        outDeviceName = splitName[1];
        //          ofLog()<<outDeviceIndex<<" outDevices[outDeviceIndex].name "<<outDevices[outDeviceIndex].name<<" outDeviceName "<<outDeviceName;
    }

    
//	float x = ofMap(ofGetMouseX(), 0, ofGetWidth(), 0, 1, true);
//	float y = ofMap(ofGetMouseY(), 0, ofGetHeight(), 0, 1, true);
	//the following sets the volume for the second connection (sound player) for its first input channel and its second output channels based on the mouse x position 
//	mixer.setVolumeForConnectionChannel(x, 1, 0, 0);
	
	// the following sets the volume of the matrix channels. In this case it would be the channel number 7 for which you can find out to which sound player it belongs by calling 
	//mixer.getConnectionIndexAtInputChannel(7);
//	mixer.setVolumeForChannel(y, 7, 1);
	
    if(mixer.getNumOutputChannels() > 0){
//        ofLog()<<"mixer.getNumInputChannels() "<<mixer.getNumInputChannels();
        int chan = int(ofGetElapsedTimef() / 1) % mixer.getNumInputChannels();
        float x = ofMap(ofGetMouseX(), 0, ofGetWidth(), 0, 1, true);
        //    float y = ofMap(ofGetMouseY(), 0, ofGetHeight(), 0, 1, true);
        
        float y  = 1 - fmod(ofGetElapsedTimef(),1);
        //the following sets the volume for the second connection (sound player) for its first input channel and its second output channels based on the mouse x position 
        //    mixer.setVolumeForConnectionChannel(x, 1, 0, 0);
        
        // the following sets the volume of the matrix channels. In this case it would be the channel number 7 for which you can find out to which sound player it belongs by calling 
        //mixer.getConnectionIndexAtInputChannel(7);
        mixer.setVolumeForChannel(y, chan, 1);
        
        mixer.setVolumeForChannel(y, (mixer.getNumInputChannels()-1) - chan, 0);
    }
}
//--------------------------------------------------------------
void ofApp::exit(){
	stream.stop();
	stream.close();
}
//--------------------------------------------------------------
void ofApp::draw(){
	
    
    if(ofGetElapsedTimef() > 2){
        mixerRenderer.draw(10,100,500,700);
    }
    stringstream ss;
    ss << "Press l key to load mixer settings." << endl;
    ss << "Press s key to save mixer settings." << endl;
    ss << "Press e key to toggle slider's mouse interaction." << endl;
	ss << "Press n key to toggle non-slider mode.";
#ifdef OFX_SOUND_ENABLE_MULTITHREADING 
	ss << endl << "Press the space bar to load more audio file players";
#endif
	ofBitmapFont bf;
	
//    testFBO.draw(0,0,200,200);
	
	
	stringstream ss2;
	ss2 << "Sliders enabled: " << (mixerRenderer.isSlidersEnabled()?"YES":"NO") << endl;
	ss2 << "Non Slider Mode: " << (mixerRenderer.isNonSliderMode()?"YES":"NO") << endl;
	
	
	
	auto r2 = bf.getBoundingBox(ss2.str(), 0, 0);
	
	r2.x = ofGetWidth() - 20 - r2.width;
	
	ofDrawBitmapStringHighlight(ss2.str(), r2.x, 20);
	
	
	auto r = bf.getBoundingBox(ss.str(), 0, 0);
	
	r.x = r2.x - 20 - r.width;
	
	ofDrawBitmapStringHighlight(ss.str(), r.x, 20);
	
//    float temp_vol = mixer.getVolumeForChannel(1,1);
//    ofDrawBitmapString("temp_vol chan "+ofToString(temp_vol), mouseX, mouseY);
//    
//    temp_vol = mixer.getVolumeForConnectionChannel(0,1,1);
//    ofDrawBitmapString("temp_vol conn "+ofToString(temp_vol), mouseX, mouseY+20);

      if(bShowGui) gui_main.draw();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
	
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
	if(key == 's'){
		mixer.save(mixerSettingsXmlPath);
	}else if(key == 'l'){
		mixer.load(mixerSettingsXmlPath);
	}else if(key == 'e'){
		mixerRenderer.toggleSliders();
	}else if(key == 'n'){
		mixerRenderer.setNonSliderMode(!mixerRenderer.isNonSliderMode());
	}else if(key == ' '){
		loadFolder(loadPath);
	}
	
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

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){
	
}
