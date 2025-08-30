/*
  ==============================================================================

    EffectsDeck.h
    Created: 19 Aug 2025 3:31:42pm
    Author:  Lena

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
// signal processing
#include <juce_dsp/juce_dsp.h>


class EffectsDeck  : public juce::Component
{
public:
    EffectsDeck() = default;

    // Reverb
    // https://docs.juce.com/master/namespacedsp.html
    // https://docs.juce.com/master/classReverb.html
    
    // call from player
    void prepare(double sampleRate, int maxBlockSize, int numChannels);
    void reset();
    void process(juce::AudioBuffer<float>& buffer);

    // 0..1 wet amount (0 = dry/bypass, 1 = very wet)
    void setReverbAmount(float wet01);

    // chorus
    void setChorusAmount(float amt01); 

    // compression
    void setCompressionAmount(float amt01);

    // delay
    void setDelayAmount(float amt01);

private:
    // effects settings

    // reverb
    juce::dsp::Reverb reverb;
    juce::dsp::Reverb::Parameters params{};

    // chorus
    juce::dsp::Chorus<float> chorus;
    float chorusMix{ 0.0f };

    // compression
    juce::dsp::Compressor<float> compressor;

    // delay
    using DL = juce::dsp::DelayLine<float, juce::dsp::DelayLineInterpolationTypes::Linear>;
    std::unique_ptr<DL> delay;   // mono line
    double fs{ 44100.0 };
    float  delayTimeMs{ 0.0f };  
    float  delayFeedback{ 0.0f };  
    float  delayMix{ 0.0f };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (EffectsDeck)
};
