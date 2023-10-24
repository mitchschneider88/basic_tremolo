/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class BasicTremoloAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    BasicTremoloAudioProcessorEditor (BasicTremoloAudioProcessor&);
    ~BasicTremoloAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    BasicTremoloAudioProcessor& audioProcessor;
    
    juce::Slider rate;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> rateAttachment;
        
    juce::Slider depth;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> depthAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BasicTremoloAudioProcessorEditor)
};
