/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
BasicTremoloAudioProcessor::BasicTremoloAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ), treeState(*this, nullptr, "PARAMETERS", createParameterLayout())
#endif
{
    treeState.addParameterListener("rateID", this);
    treeState.addParameterListener("depthID", this);
}

BasicTremoloAudioProcessor::~BasicTremoloAudioProcessor()
{
    treeState.removeParameterListener("rateID", this);
    treeState.removeParameterListener("depthID", this);
}

//==============================================================================
juce::AudioProcessorValueTreeState::ParameterLayout BasicTremoloAudioProcessor::createParameterLayout()
{
    std::vector <std::unique_ptr<juce::RangedAudioParameter>> params;
    
    params.reserve(2);
    
    juce::NormalisableRange<float> rateRange (0.f, 10.f, 0.0001f, 1.f, false);
    params.push_back(std::make_unique<juce::AudioParameterFloat>("rateID", "rate", rateRange, 10.f));
    
    juce::NormalisableRange<float> depthRange (0.f, 1.f, 0.1f, 1.f, false);
    params.push_back(std::make_unique<juce::AudioParameterFloat>("depthID", "depth", depthRange, 1.f));
        
    return { params.begin(), params.end() };
}

void BasicTremoloAudioProcessor::parameterChanged(const juce::String &parameterID, float newValue)
{
    if (parameterID == "rateID")
    {
        LFO.setLfoRateInHz(newValue);
    }
    
    if (parameterID == "depthID")
    {
        LFO.setLfoDepth(newValue);
    }
}

const juce::String BasicTremoloAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool BasicTremoloAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool BasicTremoloAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool BasicTremoloAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double BasicTremoloAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int BasicTremoloAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int BasicTremoloAudioProcessor::getCurrentProgram()
{
    return 0;
}

void BasicTremoloAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String BasicTremoloAudioProcessor::getProgramName (int index)
{
    return {};
}

void BasicTremoloAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void BasicTremoloAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    spec.maximumBlockSize = samplesPerBlock;
    spec.sampleRate = sampleRate;
    spec.numChannels = getTotalNumOutputChannels();
    
    gain.prepare(spec);
    gain.reset();
    gain.setGainLinear(1.f);
    
    LFO.resetLFO(sampleRate);
    LFO.setLfoDepth(treeState.getRawParameterValue("depthID")->load());
    LFO.setLfoRateInHz(treeState.getRawParameterValue("rateID")->load());
    LFO.setLfoType(generatorWaveform::kSin);
    
}

void BasicTremoloAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool BasicTremoloAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void BasicTremoloAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();
    int numSamples = buffer.getNumSamples();
    
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());


    for (int sample = 0; sample < numSamples; ++sample)
    {
        auto* xnL = buffer.getReadPointer(0);
        auto* ynL = buffer.getWritePointer(0);
        
        auto* xnR = buffer.getReadPointer(1);
        auto* ynR = buffer.getWritePointer(1);
        
        ynL[sample] = xnL[sample] * LFO.generateUnipolarMaxDownLFO();
        ynR[sample] = xnR[sample] * LFO.generateUnipolarMaxDownQuadPhasePosLFO();
    }
}

//==============================================================================
bool BasicTremoloAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* BasicTremoloAudioProcessor::createEditor()
{
    return new juce::GenericAudioProcessorEditor (*this);
}

//==============================================================================
void BasicTremoloAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void BasicTremoloAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new BasicTremoloAudioProcessor();
}

