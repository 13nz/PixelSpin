/*
  ==============================================================================

    SpectrumBars.h
    Created: 27 Aug 2025 5:33:54pm
    Author:  Lena

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

/* Docs: https://juce.com/tutorials/tutorial_spectrum_analyser/ <-- documentation used
*/
class SpectrumBars : public juce::Component, private juce::Timer
{
public:
    SpectrumBars(int fftOrder = 10, int numBars = 16);
    ~SpectrumBars() override = default;

    void prepare(double sampleRate);
    void pushAudioBlock(const juce::AudioBuffer<float>& buffer, int startSample, int numSamples);

    void setNumBars(int n);  
    void setDecay(float perSecond); 

    void paint(juce::Graphics& g) override;
    void resized() override {}

private:
    void timerCallback() override;
    void runFFTIfReady();

    // config
    const int fftOrder;
    const int fftSize;
    int bars;
    float decayPerSec = 3.0f;

    // fourier
    juce::dsp::FFT fft;
    juce::dsp::WindowingFunction<float> window;
    juce::AudioBuffer<float> monoFifo;
    std::atomic<int> fifoFill{ 0 }; 

    // work buffers
    std::vector<float> fftInput;
    std::vector<float> fftBuffer;        
    std::vector<float> mag;             
    std::vector<float> barLevels; 

    // helper to mixdown to mono and push into fifo
    void pushMono(const float* const* chans, int numChans, int num, int start);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SpectrumBars)
};
