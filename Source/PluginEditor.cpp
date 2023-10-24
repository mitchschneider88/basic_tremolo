/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
BasicTremoloAudioProcessorEditor::BasicTremoloAudioProcessorEditor (BasicTremoloAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (400, 300);
    
    addAndMakeVisible(rate);
    std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.treeState, "rateID", rate);
    
    addAndMakeVisible(depth);
    std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.treeState, "depthID", depth);
}

BasicTremoloAudioProcessorEditor::~BasicTremoloAudioProcessorEditor()
{
}

//==============================================================================
void BasicTremoloAudioProcessorEditor::paint (juce::Graphics& g)
{
  
}

void BasicTremoloAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
}
