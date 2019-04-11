/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
Evans_a2AudioProcessor::Evans_a2AudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
    currentSampleRate = 0.0f;
    currentAngle = 0.0f;
    angleData = 0.0f;
    sinFreq = 60.0f;
}

Evans_a2AudioProcessor::~Evans_a2AudioProcessor()
{
}

//==============================================================================
const String Evans_a2AudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool Evans_a2AudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool Evans_a2AudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool Evans_a2AudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double Evans_a2AudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int Evans_a2AudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int Evans_a2AudioProcessor::getCurrentProgram()
{
    return 0;
}

void Evans_a2AudioProcessor::setCurrentProgram (int index)
{
}

const String Evans_a2AudioProcessor::getProgramName (int index)
{
    return {};
}

void Evans_a2AudioProcessor::changeProgramName (int index, const String& newName)
{
}

//==============================================================================
void Evans_a2AudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{

    const int numInputChannels = getTotalNumInputChannels();
    const int delayBufferSize = 2 * (sampleRate + samplesPerBlock);
    mSampleRate = sampleRate;
    
    mDelayBuffer.setSize(numInputChannels, delayBufferSize);
    
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    
    currentSampleRate = sampleRate;
    sinFreq = 60.0f;
    updateAngleDelta();
    mixLevel.reset(sampleRate, 0.1f);
    mixLevel.setTargetValue(0.15f);
    mixLevel = 0.15f;
    //==========================================
    delayLevel.reset(sampleRate, 0.1f);
    delayLevel.setTargetValue(0.15f);
    delayLevel = 0.15f;
    //==========================================
    gain.setGainDecibels(12.0f);
 
}

void Evans_a2AudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool Evans_a2AudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    if (layouts.getMainOutputChannelSet() != AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != AudioChannelSet::stereo())
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

void Evans_a2AudioProcessor::processBlock (AudioBuffer<float>& buffer, MidiBuffer& midiMessages)
{
    ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());
    
    //set up another copy of input buffer so we can keep our original
    AudioBuffer<float> wetBuffer(totalNumInputChannels,
    buffer.getNumSamples());
    wetBuffer.makeCopyOf(buffer);
    
    auto* channelData = buffer.getWritePointer (channel);
    auto* wetData = wetBuffer.getWritePointer (channel);
    
    // ..do something to the each audio sample
    for (int sample = 0; sample < 512; ++sample)
    {
        // generating a set of random values to modulate input amplitude
        float modulator = random.nextFloat() * 0.25f - 0.125f;
        wetData[sample] = wetData[sample] * modulator;
        // calculating value to put into buffer
        auto currentSinSample = (float) std::sin(currentAngle);
        currentAngle += angleData; // currentAngle = currentAngle + angleDelta
        // let's make our float wiggle
        //float modulator = random.nextFloat() * 0.1f - 0.05f;
        wetData[sample] = wetData[sample] * currentSinSample;
        
        
        channelData[sample] = channelData[sample] * mixLevel.getNextValue();
        
        }
        
        {
        dsp::AudioBlock<float> output(buffer);
        gain.process(dsp::ProcessContextReplacing<float> (output));
        }
    
    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    // Make sure to reset the state if your inner loop is processing
    // the samples and the outer loop is handling the channels.
    // Alternatively, you can process the samples with the channels
    // interleaved by keeping the same state.
    
    const int bufferLength = buffer.getNumSamples();
    const int delayBufferLength = mDelayBuffer.getNumSamples();
    
    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        const float* bufferData = buffer.getReadPointer(channel);
        const float* delayBufferData = mDelayBuffer.getReadPointer(channel);
        float* dryBuffer = buffer.getWritePointer(channel);
        
        fillDelayBuffer(channel, bufferLength, delayBufferLength, bufferData, delayBufferData);
        getFromDelayBuffer(buffer, channel, bufferLength, delayBufferLength, bufferData, delayBufferData);
        feedbackDelay(channel, bufferLength, delayBufferLength, dryBuffer);
    }
    
    mWritePosition += bufferLength;
    mWritePosition %= delayBufferLength;
    
}
        
void Evans_a2AudioProcessor::fillDelayBuffer (int channel, const int bufferLength, const int delayBufferLength, const float* bufferData, const float* delayBufferData)
{
    const float gain = 0.3;
    
    //copy the data from main buffer to delay buffer
    if (delayBufferLength > bufferLength + mWritePosition)
    {
        mDelayBuffer.copyFromWithRamp(channel, mWritePosition, bufferData, bufferLength, gain, gain);
    }
    else {
        const int bufferRemaining = delayBufferLength - mWritePosition;
                
        mDelayBuffer.copyFromWithRamp(channel, mWritePosition, bufferData, bufferRemaining, gain, gain);
        mDelayBuffer.copyFromWithRamp(channel, 0, bufferData, bufferLength - bufferRemaining, gain, gain);
    }
}

void Evans_a2AudioProcessor::getFromDelayBuffer (AudioBuffer<float>& buffer, int channel, const int bufferLength, const int delayBufferLength, const float* bufferData, const float* delayBufferData)
{
    int delayTime = 75;
    const int readPosition = static_cast<int> (delayBufferLength + mWritePosition - (mSampleRate * delayTime /1000)) % delayBufferLength;
    
    if (delayBufferLength > bufferLength + readPosition)
    {
        buffer.copyFrom(channel, 0, delayBufferData + readPosition, bufferLength);
    }
    else {
        const int bufferRemaining = delayBufferLength - readPosition;
        buffer.copyFrom(channel, 0, delayBufferData + readPosition, bufferRemaining);
        buffer.copyFrom(channel, bufferRemaining, delayBufferData, bufferLength - bufferRemaining);
}
}
                                            
void Evans_a2AudioProcessor::feedbackDelay (int channel, const int bufferLength, const int delayBufferLength, float*dryBuffer)
{
    if (delayBufferLength > bufferLength + mWritePosition)
    {
        mDelayBuffer.addFromWithRamp(channel, mWritePosition, dryBuffer, bufferLength, 0.8, 0.8);
    }
    else {
        const int bufferRemaining = delayBufferLength - mWritePosition;
        
        mDelayBuffer.addFromWithRamp(channel, bufferRemaining, dryBuffer, bufferRemaining, 0.8, 0.8);
        mDelayBuffer.addFromWithRamp(channel, 0, dryBuffer, bufferLength - bufferRemaining, 0.8, 0.8);
    }
    
    
}



//==============================================================================
bool Evans_a2AudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* Evans_a2AudioProcessor::createEditor()
{
    return new Evans_a2AudioProcessorEditor (*this);
}

//==============================================================================
void Evans_a2AudioProcessor::getStateInformation (MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void Evans_a2AudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// User functions

void Evans_a2AudioProcessor::updateAngleDelta()
{
    // calculate no. of cycles that will need to complete for each output sample
    auto cyclesPerSample = sinFreq / currentSampleRate;
    // mul
    angleData = cyclesPerSample * MathConstants<float>::twoPi;
}
//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new Evans_a2AudioProcessor();
}
