#pragma once

#include "ofMain.h"
#include "ofxFft.h"
#include "ofxProcessFFT.h"

#include "ofxSoundObjects.h"

class FftObject : public ofxSoundObject {
public:
	void setup(unsigned int bufferSize = 2048);
	
    void process(ofSoundBuffer &input, ofSoundBuffer &output);
    void draw();
    
    unsigned int bufferSize;


    
//    shared_ptr<ProcessFFT> fft;
//    ProcessFFT process_fft;
    ProcessFFT fft;

    int numChannels = 0;
    bool bIsProcessed = false;
};
