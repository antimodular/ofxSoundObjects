//
//  ofxSoundObjectMatrixMixerRenderer.h
//
//  Created by Roy Macdonald on 5/4/19.
//
//

#pragma once
#include "ofxSoundObjectBaseRenderer.h"
#include "ofxSoundMatrixMixer.h"
#include "ofxGui.h"
class ofxSoundMatrixMixerRenderer: public ofxSoundObjectBaseRenderer<ofxSoundMatrixMixer>{
public:
	virtual void draw(float _x, float _y, float _windowWidth, float _windowHeight) override;
	void enableSliders();
	void disableSliders();
	void toggleSliders();
	bool isSlidersEnabled();
	
	void setNonSliderMode(bool b);
	bool isNonSliderMode();
	
	void setMinChannelHeight(const float& minHeight);
	
private:
	std::vector<std::vector<std::vector< std::unique_ptr<ofxFloatSlider>>>> sliders;
    std::vector<std::unique_ptr<ofxFloatSlider>> outputSliders;

	ofxFloatSlider masterSlider;	
	void initOrResizeNumSliders(const float & sliderWidth = 200);
	
	
	bool bSlidersEnabled = false;
	bool bSlidersWereEnabled = false;
	bool bMasterSliderSetup = false;
	
	bool bNonSliderMode = false;
	
	float minHeight = 1;
};

