#include "FftObject.h"
//--------------------------------------------------------------
void FftObject::setup(unsigned int bufferSize) {
    this->bufferSize = bufferSize;
    
    
//    fft = shared_ptr<ofxProcessFFT>(ofxProcessFFT::create(bufferSize, OF_FFT_WINDOW_HAMMING));
    
//    fft = shared_ptr<ProcessFFT>(ofxFft::create(bufferSize, OF_FFT_WINDOW_HAMMING));
//    fft = shared_ptr<ofxFft>(ofxFft::create(bufferSize, OF_FFT_WINDOW_HAMMING));

    
    fft.setupWithoutMic(bufferSize);
    fft.setNumFFTBins(20); //bUseMic = false;numFFTbins); //100);
    fft.setNormalize(false);
    
    numChannels = 0;
    bIsProcessed = false;

    
    /*
     fft.volumeRange
         fft.setVolumeRange(volumeRange);
     fft.getSpectrum()
       fft.setNumFFTBins(numFFTbins);
     */
}
//--------------------------------------------------------------


void FftObject::draw() {

    
//    if (bIsProcessed) {
        
        fft.drawBars();
        fft.drawDebug();
        
        ofNoFill();
        ofDrawRectangle(824, 0, 200, 200);
        ofDrawRectangle(824, 200, 200, 200);
        ofDrawRectangle(824, 400, 200, 200);
        ofDrawRectangle(824, 600, 200, 200);
        
        fft.drawHistoryGraph(ofPoint(824,0), LOW);
        fft.drawHistoryGraph(ofPoint(824,200),MID );
        fft.drawHistoryGraph(ofPoint(824,400),HIGH );
        fft.drawHistoryGraph(ofPoint(824,600),MAXSOUND );
        
        ofDrawBitmapString("LOW",850,20);
        ofDrawBitmapString("HIGH",850,420);
        ofDrawBitmapString("MID",850,220);
        ofDrawBitmapString("MAX VOLUME",850,620);
        
//        ofSetColor(0);
//        ofDrawBitmapString("Press 'r' or 'q' to toggle normalization of values", 20,320);

//    }
}

//--------------------------------------------------------------
void FftObject::process(ofSoundBuffer &input, ofSoundBuffer &output) {
    
    input.copyTo(output);//this sound object does not process sound so it will passthrough.
    
    if (input.getNumFrames() == bufferSize) {
         float * outData_float;
        outData_float = &input.getBuffer()[0]; //convert vector to pointer array;
        fft.fft.audioReceived(outData_float, bufferSize, 1);
        fft.update();
    }
    
    /*
    if (numChannels != input.getNumChannels() ) {
        cout << "numChannels != input.getNumChannels()" << numChannels << " " << input.getNumChannels() << endl;
        setBins( input.getNumChannels());
    }
    
    if (input.getNumFrames() == bufferSize) {
        if (input.getNumChannels() == 1) {
            fft.setSignal(input.getBuffer());
        }else{
            for (int i =0; i < numChannels; i++) {
                ofSoundBuffer b;
                input.getChannel(b, i);
                fft.setSignal(b.getBuffer());
                float* curFft = fft.getAmplitude();
                memcpy(&audioBins[i][0], curFft, sizeof(float) * fft.getBinSize());
            }
        }
    }else{
        cout << "input.getNumFrames() != bufferSize" << endl;
    }
     */

    
//    soundMutex.lock();
//    middleBins = audioBins;
//    soundMutex.unlock();
//    bIsProcessed = true;
}

