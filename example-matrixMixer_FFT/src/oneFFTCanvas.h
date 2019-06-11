//
//  oneFFTCanvas.h
//  example-matrixMixer_simpleFFT
//
//  Created by Stephan Schulz on 2019-05-24.
//

#ifndef oneFFTCanvas_h
#define oneFFTCanvas_h

class oneFFTcanvas{
private:
    
public:
    
    int index;
    
     ofParameter<int> fboPick;
    
   ofParameterGroup parameters_canvas;
        ofParameter<bool> bShowRowsFrames;
    
    int fboDimensions;
    ofFbo debugFbo; // manually draw in to fbo, and as a result effect lights
    ofFbo barFbo; //FFT bins as bars
    ofFbo timeSliceFbo; //multi time instances of the last x FFT bins
    ofFbo timeSliceFbo2;//multi time instances of the last x FFT bins, stretched 
    ofFbo volumeFbo;
    ofFbo resultFbo;
    
    ofPixels resultPix; //eventually an FBO will load it's pixels in here so we can check/read them
    
    ofColor soundColor;
    ofParameter<int> fboBGcolor;
 
    float barWidth_big;
    
    ofParameter<int> numFFTbins;
    int old_numFFTbins;
    
    vector<vector<float>> customSpectrumBins;
    vector<float> customSpectrumBinsAvg;

    //
    ofParameter<float> levelCurve_fft;
    ofParameter<float> levelCurve_level;
    ofParameter<int> minBrightness;
    ofParameter<int> maxBrightness;
    
    //---FFT time slices
    ofParameterGroup parameters_timeSlice;
    
    
    //collection of the last n FFT bins buffers
    //--a slice is one FFT snap shot 
    //--we will play multiple snapshots per location, before it moves to next location
    deque<vector<float>> spectrumTimeslice; 
    
    int frameCnt;
    ofParameter<int> maxFrameCnt; 
    int old_maxFrameCnt;
    ofParameter<int> rowAmount;
    int old_rowAmount;
    ofParameter<int>frameCollectInterval;
    int frameTimer;
    
    void setup(int _index){
        
        index = _index;
        
        parameters_canvas.setName("canvas"+ofToString(index));
        parameters_canvas.add(bShowRowsFrames.set("bShowRowsFrames",false));
        parameters_canvas.add(numFFTbins.set("numFFTbins",20,1,256));
        parameters_canvas.add(fboPick.set("fboPick",0,-1,3));
        parameters_canvas.add(fboBGcolor.set("fboBGcolor",10,0,255));
        
        parameters_canvas.add(minBrightness.set("minBrightness",0,0,255));
        parameters_canvas.add(maxBrightness.set("maxBrightness",186,0,255));
        //        parameters_canvas.add(levelCurve_level.set("levelCurve_level",1.15,0.1,2));
        parameters_canvas.add(levelCurve_fft.set("levelCurve_fft",1.15,0.1,2));

        
        parameters_timeSlice.setName("timeSlice"+ofToString(index));
        parameters_timeSlice.add(maxFrameCnt.set("maxFrameCnt",8,0,100));
        parameters_timeSlice.add(rowAmount.set("rowAmount",11,0,200));
        parameters_timeSlice.add(frameCollectInterval.set("collectInterval",0,0,1000));

   
    }
    
    void init(){
        fboDimensions = 900;
        soundColor = ofColor(255,255,255,255);
        
        old_rowAmount = rowAmount;
        old_maxFrameCnt = maxFrameCnt;
        spectrumTimeslice.resize(rowAmount*maxFrameCnt);
        for(int i=0; i<spectrumTimeslice.size();i++){
            spectrumTimeslice[i].resize(50);
        }
    }
    
    void update(int _id, vector<float> data, float _fftRange){
        checkGui();
        
        //---reduce incoming spectrum data to fewer bins. put the bins in larger bins
        customSpectrumBins.clear();
        
        if(numFFTbins >= data.size() ){
            
        
            //            customSpectrum = data;
            ofLog()<<_id<<" numFFTbins >= data.size() "<<numFFTbins<<" data size "<<data.size();
            //make a 2d vector. Each bin in a new vector bin
//            for(int i=0; i<data.size(); i++){ 
//                vector<float> temp_v;
//                temp_v.push_back(data[i]);
//                customSpectrumBins.push_back(temp_v);
//            }
        } else {
            // make a 2d vectro bin 
            // for example the 600 bins will now be placed in 4 bigger bins, each having 150 bins inside
            //            int spectrumSteps = data.size() / numFFTbins;
            //            ofLog()<<"data.size() "<<data.size()<<" spectrumSteps "<<spectrumSteps;
            
            //            float temp_binValue = 0;
            //            int temp_cnt = 0;
            //            for(int i=0; i<data.size(); i++){
            //             
            //                    
            //                    temp_binValue += data[i];
            //                    temp_cnt++;
            //            
            //                if(temp_cnt == spectrumSteps || i == data.size()-1){
            //                    temp_binValue = temp_binValue / float(temp_cnt);
            //                    customSpectrum.push_back(temp_binValue);
            //                    temp_cnt = 0;
            //                    temp_binValue = 0;
            //                }
            //                
            //            }
            
            
            //binning the bins
            customSpectrumBins = SplitVector(data,numFFTbins);
            
            //make a 1D vector of the avergage of the binned bins
            customSpectrumBinsAvg.clear();
            for(int i=0; i<customSpectrumBins.size(); i++){ 
                
                float temp_avg = 0;
                for(int n=0; n<customSpectrumBins[i].size(); n++){ 
                    temp_avg+= customSpectrumBins[i][n];
                }
                //                ofLog()<<i <<" temp_avg "<<temp_avg<<" size[i] "<<customSpectrumBins[i].size();
                temp_avg = temp_avg / float(customSpectrumBins[i].size());
                customSpectrumBinsAvg.push_back(temp_avg);
                
                ofLog()<<i<<" customSpectrumBins[i].size() "<<customSpectrumBins[i].size()<<" customSpectrumBinsAvg "<<customSpectrumBinsAvg.size();
                
                
            }
            
            //            ofLog()<<"customSpectrum "<<customSpectrum.size();
            
            if(customSpectrumBins.size() > 0 && customSpectrumBinsAvg.size() > 0){
                updateCanvas(_fftRange);
            }else{
                ofLog()<<_id<<" "<<ofGetTimestampString()<<" error customSpectrumBins.size() "<<customSpectrumBins.size()<<" customSpectrumBinsAvg "<<customSpectrumBinsAvg.size(); 
            }
            
        }//end else if(numFFTbins >= data.size() 
        
        
    }
    
    void updateCanvas(float _fftRange){
        
        
       
        
        if(resultFbo.isAllocated() == false){
//            old_numFFTbins = numFFTbins;
            //            ofLog()<<"fboDimensions "<<fboDimensions;
            int temp_s = fboDimensions; 
            debugFbo.allocate(temp_s,temp_s, GL_RGBA, false);
            barFbo.allocate(temp_s,temp_s, GL_RGBA, false);
            volumeFbo.allocate(temp_s,temp_s, GL_RGBA, false);
            timeSliceFbo.allocate(temp_s,temp_s, GL_RGBA, false);
            timeSliceFbo2.allocate(temp_s,temp_s, GL_RGBA, false);
            resultFbo.allocate(temp_s,temp_s, GL_RGBA, false);  
            
            resultFbo.begin();
            ofClear(255,0);
            resultFbo.end();
        }
        
  
        barWidth_big = fboDimensions / float(customSpectrumBinsAvg.size());
        barWidth_big = MAX(barWidth_big,1);

        //--------debug-------
        //manually draw in to FBO and in turn trigger related lights
        debugFbo.begin();
        ofClear(255,255,255,0);
        
        ofFill();
        ofSetColor(fboBGcolor,fboBGcolor,fboBGcolor,255);
        ofDrawRectangle(0,0,fboDimensions,fboDimensions); //barFbo.getWidth(),barFbo.getHeight());
        
        ofSetColor(255,255);
        float temp_x = ofGetMouseX();
        float temp_y = ofGetMouseY();
        
        ofDrawCircle(temp_x,temp_y,50);
        
#ifdef USE_VIDEO
        ofSetHexColor(0xFFFFFF);
        fingerMovie.draw(temp_x,temp_y,debugFbo.getWidth(),debugFbo.getHeight());
#endif
        
        debugFbo.end();
        
        //--------------bar---------------
        //MARK:make fbo of FFT bins as bars
        barFbo.begin();
        {
            ofClear(255,255,255,0);
            
            ofFill();
            ofSetColor(fboBGcolor,fboBGcolor,fboBGcolor,255);
            ofDrawRectangle(0,0,fboDimensions,fboDimensions); //barFbo.getWidth(),barFbo.getHeight());
            //    ofSetColor(255, 0, 0,255);
            //    ofDrawCircle(mouseX, mouseY, 20);
            ofSetColor(soundColor);
            ofFill();
            //    ofLog()<<"fftSpectrum.size() "<<fftSpectrum.size();
//            int ii = 0; //startBin;
            
            //            ofLog()<<"fftSpectrum.size() "<<customSpectrum.size();
            for(int i=0; i<customSpectrumBins.size(); i++){
                
                ofPushMatrix();
                {
                    ofTranslate(barWidth_big * i, fboDimensions);
                    //draw bigger bins rect
                    ofSetColor(soundColor);
                    ofFill();
                    ofRectangle temp_rect(0,0, barWidth_big, -ofMap(customSpectrumBinsAvg[i], 0, _fftRange, 0, fboDimensions,true));
                    ofDrawRectangle(temp_rect);
                    
                    float barWidth_small = barWidth_big / float(customSpectrumBins[i].size());
                    
                    //draw smaller internal bins of
                    //                    ofLog()<<"customSpectrumBins[i].size() "<<customSpectrumBins[i].size();
                    for(int n=0; n<customSpectrumBins[i].size(); n++){ //for the number of columns
                        
                        
                        //            ofDrawRectangle(temp_w*((float)i/numFFTbins), temp_h, temp_w/float(numFFTbins), -ofMap(fftSpectrum[ii], 0, _fftRange, 0, temp_h));
                        
                        ofSetColor(0,255,0,150);
                        ofFill();
                        ofRectangle temp_rect(barWidth_small * n, 0, barWidth_small, -ofMap(customSpectrumBins[i][n], 0, 1, 0, fboDimensions,true));
                        ofDrawRectangle(temp_rect);
                 
                    }//end   for(int n=0; n<customSpectrumBins[i].size(); n++)
                    
                }
                ofPopMatrix();
            }//end   for(int i=0; i<customSpectrumBins.size(); i++)
        }
        barFbo.end();
        
        
        //--------------make time based slices of the FFT readings--------------
        //--a slice is one FFT snap shot 
        //--we will play multiple snapshots per location, before it moves to next location
        if(ofGetElapsedTimeMillis() - frameTimer > frameCollectInterval){
            
            spectrumTimeslice.push_back(customSpectrumBinsAvg);
            ////            if(spectrumTimeslice.size() > rowAmount*maxFrameCnt) spectrumTimeslice.erase(spectrumTimeslice.begin());
            if(spectrumTimeslice.size() > rowAmount*maxFrameCnt) spectrumTimeslice.pop_front();
            
            frameTimer = ofGetElapsedTimeMillis();
        }
        
        //--------------timeSliceFbo---------------
        //MARK: timeSliceFbo - displays frequencies as vertical bars with varying height
        timeSliceFbo.begin();
        {
            ofClear(255,255,255,0);
            ofFill();
            ofSetColor(fboBGcolor);
            ofDrawRectangle(0,0,fboDimensions,fboDimensions); //barFbo.getWidth(),barFbo.getHeight());
            //cycle through all rows
            float accumY = 0;
            int ii = 0; //startBin;
            for(int j= 0; j< rowAmount; j++){
                //            break;
                //TODO: rename maxFrameCnt to framesPerSlice
                //pick the right position in the recorded spectrum history.
                int temp_timeIndex = spectrumTimeslice.size() -1 - (maxFrameCnt*j);
                
                float row_height= fboDimensions / float(rowAmount);
                //was used in voiceTheatre when rows has varying spacing. data was in text file
                //            float row_height = rowHeights_percentage[j] * fboDimensions;
                
                if(temp_timeIndex >= 0 && temp_timeIndex < spectrumTimeslice.size()){
                    
                    //                    ofSetColor(ofRandom(255),ofRandom(255),ofRandom(255),255); //temp_alpha);
                    ofSetColor(soundColor.r,soundColor.g,soundColor.b,255);
                    ii = 0; //startBin;
                    ofFill();
                    for(int i= 0; i< spectrumTimeslice[temp_timeIndex].size(); i++){
                        
                        ofRectangle temp_rect(barWidth_big * i, fboDimensions - accumY, barWidth_big, -ofMap(spectrumTimeslice[temp_timeIndex][ii], 0, _fftRange, 0, row_height,true));
                        ofDrawRectangle(temp_rect);
                        
                        ii++;
                        ii %= spectrumTimeslice[temp_timeIndex].size();
                    }
                    
                    if(bShowRowsFrames){
                        ofNoFill();
                        ofSetColor(255);
                        ofDrawRectangle(0,fboDimensions - accumY,fboDimensions,row_height);
                    }
                    
                    accumY += row_height;
                    
                }//end   if(temp_timeIndex >= 0 && 
            }//end  for(int j= 0; j< rowAmount; j++){
        }
        timeSliceFbo.end();
        
        //--------------timeSliceFbo 2 ---------------
        //MARK: timeSliceFbo2 - displays frequencies as rectangles with varying alpha
        timeSliceFbo2.begin();
        {
            
            
            ofClear(255,255,255,0);
            ofFill();
            ofSetColor(fboBGcolor);
            ofDrawRectangle(0,0,fboDimensions,fboDimensions); //barFbo.getWidth(),barFbo.getHeight());
            //    //cycle through all rows
            
            float temp_c;
            ofFill();
            int temp_cnt = 0;
            float accumY = 0;
            int ii = 0; //startBin;
            for(int j = 0; j< rowAmount; j++){
                //            break;
                int temp_timeIndex = spectrumTimeslice.size() -1 - (maxFrameCnt*j);
                float temp_hh = fboDimensions / float(rowAmount);
                
                if(temp_timeIndex >= 0 && temp_timeIndex < spectrumTimeslice.size()){
                    
                    ii = 0; //startBin;
                    int temp_size = spectrumTimeslice[temp_timeIndex].size();
                    
                    float row_height= fboDimensions / float(rowAmount);
                    //was used in voiceTheatre when rows has varying spacing. data was in text file
                    //            float row_height = rowHeights_percentage[j] * fboDimensions;
                    
                    
                    ofFill();
                    
                    float temp_avg = 0;
                    for(int i= 0; i< temp_size; i++){
                        
                        //                    temp_c = ofMap(spectrumTimeslice[temp_timeIndex][ii], 0, _fftRange, 0, 255);
                        temp_c = ofMap(spectrumTimeslice[temp_timeIndex][ii], 0, _fftRange, 0, 1);                    
                        float temp_alpha = adjustBrightness(temp_c,1,levelCurve_fft);
                        
                        temp_avg += temp_alpha;
                        
                        //                        ofSetColor(ofRandom(255),ofRandom(255),ofRandom(255),temp_alpha);
                        //                        ofSetColor(soundColor.r,soundColor.g,soundColor.b, temp_alpha);
                        ofSetColor(temp_alpha,255);
                        ofRectangle temp_rect(barWidth_big*i, fboDimensions - accumY,barWidth_big, -row_height);
                        ofDrawRectangle(temp_rect);
                        
                        //                    ofDrawLine(temp_wStep*i, temp_h - (j*temp_hh),temp_wStep*i + temp_wStep,  temp_h - (j*temp_hh));
                        temp_cnt++;
                        ii++;
                        ii %= temp_size;
                    }
                    
                    //                    averageRowBrightness[j] = temp_avg / 255.0  / float(temp_size);
                    
                    if(bShowRowsFrames){
                        ofNoFill();
                        ofSetColor(255);
                        ofDrawRectangle(0,fboDimensions - accumY,fboDimensions,-row_height);
                    }
                    
                    accumY += row_height;
                    
                }//end if(temp_timeIndex >= 0 && 
                
            }//end for(int j = 0; j< rowAmount; j++){
            
            //        ofLog()<<"temp_cnt "<<temp_cnt;
            
        }
        timeSliceFbo2.end();
    }
    
    void draw(int _x, int _y){
        ofPushMatrix();
        {
            ofTranslate(_x,_y);
            barFbo.draw(0,0,200,200);
            ofDrawBitmapStringHighlight("barFbo",0,10);
            
            ofTranslate(202,0);
            timeSliceFbo.draw(0,0,200,200);
            ofDrawBitmapStringHighlight("timeSlice",0,10);
            
            ofTranslate(202,0);
            timeSliceFbo2.draw(0,0,200,200);
            ofDrawBitmapStringHighlight("timeSlice2",0,10);
        }
        ofPopMatrix();
    }
    
    
    //https://stackoverflow.com/questions/6861089/how-to-split-a-vector-into-n-almost-equal-parts
    template<typename T>
    std::vector<std::vector<T>> SplitVector(const std::vector<T>& vec, size_t n)
    {
        std::vector<std::vector<T>> outVec;
        
        size_t length = vec.size() / n;
        size_t remain = vec.size() % n;
        
        size_t begin = 0;
        size_t end = 0;
        
        for (size_t i = 0; i < std::min(n, vec.size()); ++i)
        {
            end += (remain > 0) ? (length + !!(remain--)) : length;
            
            outVec.push_back(std::vector<T>(vec.begin() + begin, vec.begin() + end));
            
            begin = end;
        }
        
        return outVec;
    }
    
    float adjustBrightness(float _level,float _maxLevel, float _curveExpo){
        //            ofLog()<< j <<" temp_level "<<temp_level;
        //makes louder sounds have less visual effect; i.e. darker
        //except for last and 2nd last audio file / last dmx channel
        //        if(fboPick == 3){
        //             _level = pow(_level, levelCurve_level);
        //        }else{
        //             _level = pow(_level, levelCurve_fft);
        //        }
        
        //         ofLog()<<"A "<<_level;
        _level = pow(_level, _curveExpo);
        
        //make everything a bit brighter
        //        float temp_v = ofMap(_level,0,_maxLevel,0,255,true); //_level * 255 * brightnessMultiplier;
        //         ofLog()<<"B "<<_level<<" _maxLevel "<<_maxLevel;
        
        //limit the min and max brightness
        //        temp_v = ofMap(temp_v,0, 255 * brightnessMultiplier, minBrightness, maxBrightness,true);
        _level = ofMap(_level,0, _maxLevel, minBrightness, maxBrightness,true);
        
        
        //        ofLog()<<"C "<<_level<<" _maxLevel "<<_maxLevel;
        return _level;
    }
    
    void checkGui(){
        if(old_rowAmount != rowAmount || old_maxFrameCnt != maxFrameCnt){
            old_rowAmount = rowAmount;
            old_maxFrameCnt = maxFrameCnt;
            
            spectrumTimeslice.clear();
            spectrumTimeslice.resize(rowAmount*maxFrameCnt);
            for(int i=0; i<spectrumTimeslice.size();i++){
                spectrumTimeslice[i].resize(50);
            }
        }
    }
};

#endif /* oneFFTCanvas_h */
