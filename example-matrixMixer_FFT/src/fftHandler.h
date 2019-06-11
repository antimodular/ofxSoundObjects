#pragma once

#include "ofMain.h"
#include "ofxFft.h"

#include "ofxSoundObjects.h"
#include "oneFFTCanvas.h"

class fftHandler : public ofxSoundObject {
    
private:
    
public:
    
    vector<oneFFTcanvas> canvas_objects;
    
    ofxPanel gui_fft;
    ofParameter<bool> bDebug;
    
    unsigned int bufferSize;
    
      ofParameterGroup parameters_fft;
    
    //    shared_ptr<ProcessFFT> fft;
    //    ProcessFFT process_fft;
//    ProcessFFT fft;
    shared_ptr<ofxFft> fft;
//    ofxEasyFft fft;
    ofMutex soundMutex;

    
//    ofParameter<int> volumeRange;
//    int old_volumeRange;
    
        ofParameter<int> amplifyFactor;
    ofParameter<float> amplifyFactorNormalized;
    
    ofParameter<bool> bVolumeNormalize;
    bool old_bVolumeNormalize;
    
    ofParameter<bool> bLogScale;
    
    int numChannels = 0;
    bool bIsProcessed = false;
    
//    vector<float> current_fftSpectrum;
//    vector<float> averaged_fftSpectrum;
    ofParameter<float> spectrumAverageFactor;
    vector<vector<float> > audioBins, processBins ,drawBins; // middleBins, 
    vector<vector<float> > last_processBinsAvg; 
    
    //----visual canavas stuff
     ofParameterGroup parameters_canvas;
    ofParameter<bool> bShowCanvas;

       
    
   
    
    ofParameter <int> spectrumBegin;
    ofParameter <int> spectrumEnd;
//    ofParameter <int> startBin;
    

//     vector<float> rowHeights_percentage; //was used in voice Theatre because each arc has different spacing
    
    void setup(unsigned int bufferSize = 2048){
        this->bufferSize = bufferSize;
        
        
        canvas_objects.resize(2);
        for(int i=0; i<canvas_objects.size(); i++){
            canvas_objects[i].setup(i);
        }
        //    fft = shared_ptr<ofxProcessFFT>(ofxProcessFFT::create(bufferSize, OF_FFT_WINDOW_HAMMING));
        
        //    fft = shared_ptr<ProcessFFT>(ofxFft::create(bufferSize, OF_FFT_WINDOW_HAMMING));
        //    fft = shared_ptr<ofxFft>(ofxFft::create(bufferSize, OF_FFT_WINDOW_HAMMING));
        
        parameters_canvas.setName("canvas");
        parameters_canvas.add(bShowCanvas.set("bShowCanvas",true));
        //        parameters_canvas.add(bShowGrid.set("bShowGrid",false));
        for(int i=0; i<canvas_objects.size(); i++){
            parameters_canvas.add(canvas_objects[i].parameters_canvas);
            parameters_canvas.add(canvas_objects[i].parameters_timeSlice);
        }
        
        
        parameters_fft.setName("fft");
        parameters_fft.add(bLogScale.set("bLogScale",true));
             parameters_fft.add(spectrumAverageFactor.set("avgerageFactor",0.8,0,0.99));
        
        parameters_fft.add(amplifyFactor.set("amplifyFactor",1,1,3000));
        
//        parameters_fft.add(volumeRange.set("volumeRange",50,1,400));
        parameters_fft.add(bVolumeNormalize.set("normalize",false));
          parameters_fft.add(amplifyFactorNormalized.set("amplifyFactorNorm",1,0.1,2));
        parameters_fft.add(spectrumBegin.set("spectrumEraseBegin",1,0,100));
        parameters_fft.add(spectrumEnd.set("spectrumEraseEnd",11,0,100));
//        parameters_fft.add(startBin.set("startBin",0,0,100));
        
        
        
        
        gui_fft.setup();
        gui_fft.setHeaderBackgroundColor(ofColor(255,0,0));
        gui_fft.setName("fft");        
        gui_fft.setPosition(ofGetWidth() - 300,0);
        gui_fft.add(parameters_canvas);
        gui_fft.add(parameters_fft);

        gui_fft.loadFromFile("gui_fft.xml");
        
        //by using ofxFFT the binSize will be half the buffe size
        fft = shared_ptr<ofxFft>(ofxFft::create(bufferSize, OF_FFT_WINDOW_HAMMING));

//        bufferSize = 10024; //when set to be very high FFT gets slower and looks more smooth 
//        fft.setupWithoutMic(bufferSize);
//        fft.setNumFFTBins(numFFTbins); //bUseMic = false;numFFTbins); //100);
//        fft.setNormalize(bVolumeNormalize);
//        if(bVolumeNormalize == false) fft.setVolumeRange(volumeRange);
        
        bIsProcessed = false;
        numChannels = 0;
        bIsProcessed = false;
        
        for(int i=0; i<canvas_objects.size(); i++){
            canvas_objects[i].init();
        }
        

        /*
         fft.volumeRange
         fft.setVolumeRange(volumeRange);
         fft.getSpectrum()
         fft.setNumFFTBins(numFFTbins);
         */
    }

	
  
    
    void update(){
        
        checkGui();
        
        if(bIsProcessed == true){
            bIsProcessed = false;
            
          if(bDebug)  cout<<endl;
            
            if(bDebug) ofLog()<<"org processBins[0][10] "<<processBins[0][10]<<" | "<<processBins[1][10];
            
                     
            if(spectrumBegin != 0 || spectrumEnd != 0)frequencyWindow(processBins,spectrumBegin,spectrumEnd);
            
            if(bVolumeNormalize) normalize(processBins);

            
            if(bLogScale) resampleToLog(processBins);

    
                     
           if(bDebug)   ofLog()<<"post window processBins[0][10] "<<processBins[0][10]<<" | "<<processBins[1][10];

            
//            if(amplifyFactor != 1){
                if(bVolumeNormalize == true) amplify(processBins,amplifyFactorNormalized);
                else amplify(processBins,float(amplifyFactor));
//            }
           if(bDebug)   ofLog()<<"post amp processBins[0][10] "<<processBins[0][10]<<" | "<<processBins[1][10];

            
            if(spectrumAverageFactor > 0) runningAverage(processBins,spectrumAverageFactor);


     
            //               updateCanvas(fft.getSpectrum(),temp_range);
            
//            drawBins = processBins;
            
            soundMutex.lock();
            
            if(bDebug)   ofLog()<<"post Avg processBins[0][10] "<<processBins[0][10]<<"| "<<processBins[1][10];

            drawBins = processBins;
            
             if(bDebug)   ofLog()<<"drawBins processBins[0][10] "<<drawBins[0][10]<<" | "<<drawBins[1][10];
            
            soundMutex.unlock();
            
            for(int i=0; i<canvas_objects.size(); i++){
                canvas_objects[i].update(i,drawBins[i],1);    
            }
            
        }//end if(bProcced)
    }

    
    void draw(int _x, int _y){
        
        //    if (bIsProcessed) {
        
        //        fft.drawBars();
        //        fft.drawDebug();
        ofPushMatrix();
        ofTranslate(_x,_y);
        
        float plotHeight = 100;
        ofRectangle rs;
        for (int chan =0; chan < drawBins.size(); chan++) {
            rs.set(0,plotHeight*chan+3,300,plotHeight);
            plot(drawBins[chan], rs , 1,  chan);
        }
        
        for (int chan =0; chan < audioBins.size(); chan++) {
            rs.set(350,plotHeight*chan+3,300,plotHeight);
            plot(audioBins[chan], rs , 1,  chan);
        }
        
        for(int i=0; i<canvas_objects.size(); i++){
            canvas_objects[i].draw(10,ofGetHeight()/2 + (220*i));
        }
        
        
        ofPopMatrix();
        //        ofNoFill();
        //        ofDrawRectangle(824, 0, 200, 200);
        //        ofDrawRectangle(824, 200, 200, 200);
        //        ofDrawRectangle(824, 400, 200, 200);
        //        ofDrawRectangle(824, 600, 200, 200);
        
        //        fft.drawHistoryGraph(ofPoint(824,0), LOW);
        //        fft.drawHistoryGraph(ofPoint(824,200),MID );
        //        fft.drawHistoryGraph(ofPoint(824,400),HIGH );
        //        fft.drawHistoryGraph(ofPoint(824,600),MAXSOUND );
        
        //        ofDrawBitmapString("LOW",850,20);
        //        ofDrawBitmapString("HIGH",850,420);
        //        ofDrawBitmapString("MID",850,220);
        //        ofDrawBitmapString("MAX VOLUME",850,620);
        
        //        ofSetColor(0);
        //        ofDrawBitmapString("Press 'r' or 'q' to toggle normalization of values", 20,320);
        
        //    }
    }

    
    void plot(vector<float>& buffer, const ofRectangle &rect, float _scaler, int _chan) {
        ofMesh m;
        m.clear();
        m.setMode(OF_PRIMITIVE_LINE_STRIP);
       
        int temp_size = buffer.size();
        float offset = rect.getMaxY();
        float xinc = rect.width/float(temp_size);
        
        if(bDebug)   ofLog()<<_chan<<" plot buffer[10] "<<buffer[10]<<" temp_size "<<temp_size<<" offset "<<offset<<" xinc "<<xinc; 
        for (int i = 0; i < temp_size; i++) {
            
            
            //            if (bDrawLogScale) {
            //                ofVec3f v;
            //                v.x = ofMap(log10(i+1), 0, log10(temp_size), rect.x, rect.getMaxX());
            //                v.y = ofMap(log10(buffer[i]+1), 0, log10(2), offset, rect.y);
            //                m.addVertex(v);
            //            }else{
            m.addVertex(ofVec3f(i*xinc+rect.x, offset - buffer[i]*rect.height*_scaler ));
            
            if(bDebug && i == 10) ofLog()<<"m.addVertex "<<ofVec3f(i*xinc+rect.x, offset - buffer[i]*rect.height*_scaler );
            //            }
            m.addColor(ofFloatColor::white);
        }
        ofPushStyle();
        ofFill();
        ofSetColor(0,255);
        ofDrawRectangle(rect);
        ofSetColor(255,255);
        m.draw();
        ofNoFill();
        ofDrawRectangle(rect);
        ofPopStyle();
    }
   
    
    void process(ofSoundBuffer &input, ofSoundBuffer &output){
        
        input.copyTo(output);//this sound object does not process sound so it will passthrough.
        
       /*
        
        if (input.getNumFrames() == bufferSize) {
            
            float * outData_float;
            outData_float = &input.getBuffer()[0]; //convert vector to pointer array;
            fft.fft.audioReceived(outData_float, bufferSize, 1);
           
        fft.update();    
        }
        */
        
        
        if (numChannels != input.getNumChannels() ) {
            cout << "numChannels != input.getNumChannels()" << numChannels << " " << input.getNumChannels() << endl;
            setBins( input.getNumChannels());
        }
        
        
        if (input.getNumFrames() == bufferSize) {
            if (input.getNumChannels() == 1) {
                fft->setSignal(input.getBuffer());
          
            }else{
                for (int i =0; i < numChannels; i++) {
                    ofSoundBuffer b;
                    input.getChannel(b, i);
                    fft->setSignal(b.getBuffer());
               
                    float* curFft = fft->getAmplitude();
                    memcpy(&audioBins[i][0], curFft, sizeof(float) * fft->getBinSize());
                    
                   
                    if(i == 1){
                        for(int n=0; n< 10; n++){
                            audioBins[i][n] = 0;
                        }
                    } else {
//                        for(int n=0; n< 10; n++){
//                            audioBins[i][n] = 1;
//                        }
                    }
                }
            }

        }else{
            cout << "input.getNumFrames() != bufferSize" << endl;
        }
        
        
        
          soundMutex.lock();
            processBins = audioBins;
    
           soundMutex.unlock();
            bIsProcessed = true;
    }
    
    void resampleToLog(vector<vector<float>>& data){
        
        for(int chan = 0; chan < data.size(); chan++){
            ofPolyline logLine;
            ofVec3f v;
            int temp_size = data[chan].size();
            
            for(int i=0; i<temp_size; i++){
                v.x = log10(i+1); //ofMap(log10(i+1), 0, log10(n), 0, temp_size);
                v.y = log10(data[chan][i]+1); //ofMap(log10(data[chan][i]+1), 0, log10(2), offset, r.y);
                logLine.addVertex(v);
            }
            
            for(int i=0; i<temp_size; i++){
                
                v = logLine.getPointAtPercent(ofMap(i, 0, temp_size, 0, 1));
                data[chan][i] = v.y;
            }
        }   
        
    }
    
    void frequencyWindow(vector<vector<float>>& data, int _begin, int _end){
        for(int chan = 0; chan < data.size(); chan++){
            //remove fequency bins from begining of bin array
         if(bDebug)   ofLog()<<"data[chan].size() "<<data[chan].size();
            if(_begin > 0 && data[chan].size() > _begin){
                data[chan].erase(data[chan].begin(),data[chan].begin()+_begin);
            }
            //remove fequency bins from end of bin array
            if(_end > 0 && data[chan].size() > _end){
                data[chan].erase(data[chan].end()-spectrumEnd,data[chan].end());
            }
        }
        
    }
    void runningAverage(vector<vector<float>>& data, float _alpha){
        
        //create running average to slow down the change of the bins
        //                //i could just increase bufferSize which also slows down change, but does more
        
        if(last_processBinsAvg.size() != data.size()){
            last_processBinsAvg.resize(data.size()); 
            ofLog()<<"set last_processBinsAvg to size "<<last_processBinsAvg.size();
        }
        
        for(int chan = 0; chan < data.size(); chan++){
            
            int temp_size = data[chan].size();
            
            if(last_processBinsAvg[chan].size() != temp_size){
                last_processBinsAvg[chan].resize(temp_size);
                 ofLog()<<"set last_processBinsAvg[chan]. to size "<<last_processBinsAvg[chan].size();
                last_processBinsAvg[chan] = data[chan];
            } 
            
            for(int i=0; i<temp_size; i++){
                //                averaged_fftSpectrum[i] = spectrumAverageFactor * averaged_fftSpectrum[chan][i] + (1 - spectrumAverageFactor) * resampled_processBins[chan][i];
                data[chan][i] = _alpha * last_processBinsAvg[chan][i] + (1 - _alpha) * data[chan][i];
                if(bDebug && i == 10) ofLog()<<chan<<" , "<<i<<" , last "<<last_processBinsAvg[chan][i]<<" new "<<data[chan][i] ;
                
                last_processBinsAvg[chan][i] = data[chan][i];
            }
            
        }//end  for(int chan = 0; chan < numChannels; chan++)
        
        /*
         resampled_processBins.clear();
         resampled_processBins.resize(numChannels);
         
         //apply volumeRange multiplier 
         if(volumeRange != 1){
         for(int chan = 0; chan < numChannels; chan++){
         
         int temp_size = processBins[chan].size();
         resampled_processBins[chan].resize(temp_size);
         
         for (int i = 0; i < temp_size; i++) {
         float new_x = ofMap(log10(i+1), 0, log10(temp_size), 0, temp_size);
         float new_y = ofMap(log10(processBins[chan][i]+1), 0, log10(2), 0, 0.5);
         //                        resampled_processBins[chan].push_back(log10(processBins[chan][i]+1)); 
         resampled_processBins[chan][i] = processBins[chan][i] * volumeRange; 
         //                        ofLog()<<(log10(processBins[chan][i]+1))<<" , "<<(log10(i+1));
         //                        ofLog()<<"resampled_processBins "<<resampled_processBins[chan][i];
         }
         }//end  for(int chan = 0; chan < numChannels; chan++)
         }else{
         resampled_processBins =  processBins;
         }
         */
    }
    void amplify(vector<vector<float>>& data, float _factor){
        for(int chan = 0; chan < data.size(); chan++){
            for(int i = 0; i < data[chan].size(); i++) {
                data[chan][i] = data[chan][i] * _factor;
            }
        }
    }
    void normalize(vector<vector<float>>& data) {
        
          for(int chan = 0; chan < data.size(); chan++){
              
              float maxValue = 0;
              for(int i = 0; i < data[chan].size(); i++) {
                  if(abs(data[chan][i]) > maxValue) {
                      maxValue = abs(data[chan][i]);
                  }
              }
              for(int i = 0; i < data[chan].size(); i++) {
                  data[chan][i] /= maxValue;
              }
          }
    }
    
    void setBins(int numChans){
        if (numChans < numChannels) {
            drawBins.resize(numChans);
            audioBins.resize(numChans);
            processBins.resize(numChans);
        }else if (numChans > numChannels){
            for (int i = numChannels; i < numChans; i++) {
                vector<float> ab, db, mb;
                db.resize(fft->getBinSize());
                mb.resize(fft->getBinSize());
                ab.resize(fft->getBinSize());
                drawBins.push_back(db);
                audioBins.push_back(mb);
                processBins.push_back(ab);
            }
        }
        numChannels = numChans;
    }
    
    void checkGui(){
 
    }
};
