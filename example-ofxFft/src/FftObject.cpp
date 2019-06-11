#include "FftObject.h"
//--------------------------------------------------------------
void FftObject::setup(unsigned int bufferSize) {
    this->bufferSize = bufferSize;
    
	
    fft = shared_ptr<ofxFft>(ofxFft::create(bufferSize, OF_FFT_WINDOW_HAMMING));
    
    numChannels = 0;
    bIsProcessed = false;
    setBins(2);
}
//--------------------------------------------------------------

void FftObject::setBins(int numChans){
    if (numChans < numChannels) {
        drawBins.resize(numChans);
        middleBins.resize(numChans);
        audioBins.resize(numChans);
    }else if (numChans > numChannels){
        for (int i = numChannels; i < numChans; i++) {
            vector<float> db, mb, ab;
            db.resize(fft->getBinSize());
            mb.resize(fft->getBinSize());
            ab.resize(fft->getBinSize());
            drawBins.push_back(db);
            middleBins.push_back(mb);
            audioBins.push_back(ab);
        }
    }
    numChannels = numChans;
}
//--------------------------------------------------------------
void FftObject::draw(const ofRectangle & r, bool bDrawLogScale) {
    if (bIsProcessed) {
        
        soundMutex.lock();
        drawBins = middleBins;
        soundMutex.unlock();
        
        ofSetColor(255);
        ofRectangle rs;
        float h = r.height/numChannels;
        
        for (int i =0; i < numChannels; i++) {
            rs.set(0,h*i+r.y,r.width, h);
//            rs.scaleFromCenter(0.95);
            plot(drawBins[i],rs, bDrawLogScale);
        }
    }
}
//--------------------------------------------------------------
void FftObject::plot(vector<float>& buffer, const ofRectangle &r, bool bDrawLogScale) {
    ofMesh m;
    m.clear();
    m.setMode(OF_PRIMITIVE_LINE_STRIP);

    ofPolyline pLine;
    vector<float> pVec;
    vector<ofVec3f> points;
    
    int n = buffer.size();
    float offset = r.getMaxY();
    float xinc = r.width/n;
    
    float start , end;
    for (int i = 0; i < n; i++) {
        if (bDrawLogScale) {
            ofVec3f v;
           
            v.x = ofMap(log10(i+1), 0, log10(n), r.x, r.getMaxX());
             if(i == 0) start = v.x;
            if(i == n-1) end = v.x;
            v.y = ofMap(log10(buffer[i]+1), 0, log10(2), offset, r.y);
            m.addVertex(v);
//            points.push_back(v);
            pLine.addVertex(v);
        }else{
            m.addVertex(ofVec3f(i*xinc+r.x, offset - buffer[i]*r.height ));
        }
        m.addColor(ofFloatColor::white);
    }
    ofPushStyle();
    m.draw();
    ofNoFill();
    ofDrawRectangle(r);
    ofPopStyle();
    
 
    
    float dist = end - start;
    ofLog()<<" dist "<<dist;
    ofLog()<<"A pLine "<<pLine.getVertices().size();
//    pLine.getResampledByCount(n);
//     pLine.getResampledBySpacing(dist/float(n));
    ofLog()<<"B pLine "<<pLine.getVertices().size();

    ofSetColor(255,0,0);
    ofPushMatrix();
    ofTranslate(0,-20);
//    pLine.draw();
    ofPopMatrix();
    
    for(int i=0; i<buffer.size(); i++){
        ofVec3f p = pLine.getPointAtPercent(ofMap(i, 0, buffer.size(), 0, 1));
         pVec.push_back(p.y);
         points.push_back(p);
    }
//    for(auto &p: pLine.getVertices()){
//        pVec.push_back(p.y);
//        points.push_back(p);
//    }
    ofPushMatrix();
    ofTranslate(40,20);

     ofSetColor(0,25,250);
    for(auto &p: points){
        ofDrawCircle(p, 1);
    }
     ofPopMatrix();
    ofLog()<<"C pVec "<<pVec.size();

    
    ofPushMatrix();
    ofTranslate(0,20);
     ofSetColor(255,255,0);
    for(int i=1; i<pVec.size(); i++){
//        ofDrawLine(i-1, pVec[i-1], i, pVec[i]);
     }
     ofPopMatrix();
}
//--------------------------------------------------------------
void FftObject::process(ofSoundBuffer &input, ofSoundBuffer &output) {
    
    input.copyTo(output);//this sound object does not process sound so it will passthrough.
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
            }
        }
    }else{
        cout << "input.getNumFrames() != bufferSize" << endl;
    }
    
    soundMutex.lock();
    middleBins = audioBins;
    soundMutex.unlock();
    bIsProcessed = true;
}
