/**
 *  ofxSoundObject.cpp
 *
 *  Created by Marek Bereza on 10/08/2013.
 */

#include "ofxSoundObject.h"
#include "ofxSoundObjectsUtils.h"
//--------------------------------------------------------------
//  ofxSoundObject
//--------------------------------------------------------------
ofxSoundObject::ofxSoundObject() {
	ofLogWarning("ofxSoundObject::ofxSoundObject()", "the ofxSoundObjects' no args constructor should not be called. ");
}
ofxSoundObject::ofxSoundObject(ofxSoundObjectsType t){
	type = t;
}
//--------------------------------------------------------------
ofxSoundObject &ofxSoundObject::connectTo(ofxSoundObject &soundObject) {
    if (outputObjectRef != nullptr) {
        disconnect();
    }
    outputObjectRef = &soundObject;
	soundObject.setInput(this);

	// if we find an infinite loop, we want to disconnect and provide an error
	if(!checkForInfiniteLoops()) {
		ofLogError("ofxSoundObject") << "There's an infinite loop in your chain of ofxSoundObjects";
		disconnect();
	}
	return soundObject;
}
//--------------------------------------------------------------
void ofxSoundObject::disconnectInput(ofxSoundObject * input){
    if (inputObject != nullptr) {
    inputObject = nullptr;
    }
}
//--------------------------------------------------------------
void ofxSoundObject::disconnect(){
    outputObjectRef->disconnectInput(this);
    outputObjectRef =nullptr;
}
//--------------------------------------------------------------
void ofxSoundObject::setInput(ofxSoundObject *obj) {
	inputObject = obj;
}
//--------------------------------------------------------------
ofxSoundObject *ofxSoundObject::getInputObject() {
	return inputObject;
}
//--------------------------------------------------------------
bool ofxSoundObject::checkForInfiniteLoops() {
	ofxSoundObject *prev = inputObject;

	// move up the dsp chain until we find ourselves or the beginning of the chain (input==nullptr)
	while(prev!=this && prev!=nullptr) {
		prev = prev->getInputObject();
	}

	// if we found ourselves, return false (to indicate there's an infinite loop)
	return (prev==nullptr);
}
//--------------------------------------------------------------
ofxSoundObject* ofxSoundObject::getSignalSourceObject(){
	if(type == OFX_SOUND_OBJECT_SOURCE)return this;
	if(inputObject != nullptr){
		return inputObject->getSignalSourceObject();
	}
	ofLogWarning("ofxSoundObject::getSourceObject", "There is no source on your signal chain so most probaly you will get no sound");
	return nullptr;
}
//--------------------------------------------------------------
// this pulls the audio through from earlier links in the chain
void ofxSoundObject::audioOut(ofSoundBuffer &output) {
    ofxSoundObjects::checkBuffers(output, workingBuffer);
	if(inputObject!=nullptr) {
		inputObject->audioOut(workingBuffer);
	}
	this->process(workingBuffer, output);
}
////--------------------------------------------------------------
//void ofxSoundObject::setNumChannels(int num){
//    numChannels = num;
//}
//--------------------------------------------------------------
size_t ofxSoundObject::getNumChannels() const{
    return workingBuffer.getNumChannels();
}
//--------------------------------------------------------------
ofSoundBuffer& ofxSoundObject::getBuffer(){
return workingBuffer;
}
//--------------------------------------------------------------
const ofSoundBuffer& ofxSoundObject::getBuffer() const{
return workingBuffer;    
}

//--------------------------------------------------------------
//  ofxSoundInput
//--------------------------------------------------------------
ofxSoundInput::ofxSoundInput():ofxSoundObject(OFX_SOUND_OBJECT_SOURCE) {
}
//--------------------------------------------------------------
size_t ofxSoundInput::getNumChannels() const{
	return inputBuffer.getNumChannels();
}
//--------------------------------------------------------------
// copy audio in to internal buffer
void ofxSoundInput::audioIn(ofSoundBuffer &input) {
    ofxSoundObjects::checkBuffers(input, inputBuffer);
	input.copyTo(inputBuffer);
}
//--------------------------------------------------------------
void ofxSoundInput::audioOut(ofSoundBuffer &output) {
    ofxSoundObjects::checkBuffers(output, inputBuffer);
	inputBuffer.copyTo(output);
}
//--------------------------------------------------------------
int ofxSoundInput::getDeviceId(){
	return inputBuffer.getDeviceID();
}
ofSoundDevice getDevInfo(int id){
	auto ll = ofGetLogLevel();//Ugly hack to avoid printing when calling ofSoundStreamListDevices()
	ofSetLogLevel(OF_LOG_SILENT);
	auto devs = ofSoundStreamListDevices();
	ofSetLogLevel(ll); 
	if(id >= 0 && id < devs.size()){
		return devs[id];
	}
	ofSoundDevice d;
	return d;
}
//--------------------------------------------------------------
ofSoundDevice ofxSoundInput::getDeviceInfo(){
	return getDevInfo(getDeviceId());
}
//--------------------------------------------------------------
//  ofxSoundOutput
//--------------------------------------------------------------
ofxSoundOutput::ofxSoundOutput():ofxSoundObject(OFX_SOUND_OBJECT_DESTINATION) {
}
//--------------------------------------------------------------
int ofxSoundOutput::getDeviceId(){
	return getBuffer().getDeviceID();
}
//--------------------------------------------------------------
ofSoundDevice ofxSoundOutput::getDeviceInfo(){
	return getDevInfo(getDeviceId());
}
//--------------------------------------------------------------
