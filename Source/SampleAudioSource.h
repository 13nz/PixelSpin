/*
  ==============================================================================

    SampleAudioSource.h
    Created: 27 Aug 2025 3:58:56pm
    Author:  User

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

class SampleAudioSource : public juce::AudioSource
{
public:
    SampleAudioSource() = default;
    ~SampleAudioSource() override = default;

    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
    void releaseResources() override;
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) override;

    // trigger sample by id
    void trigger(const juce::String& id, float gain = 1.0f);

private:
    struct SampleData
    {
        juce::AudioBuffer<float> buffer; // separate channels
    };

    struct Voice
    {
        const SampleData* sample = nullptr;
        // current frame
        int pos = 0;          
        float gain = 1.0f;
    };

    double fs{ 44100.0 };
    juce::CriticalSection lock;

    // id --> sample
    std::unordered_map<std::string, SampleData> samples;
    // active voices
    std::vector<Voice> voices;

    bool ensureSampleLoaded(const juce::String& id);
    static juce::File findSamplesFolder();
    static std::unique_ptr<juce::AudioFormatReader> openReader(const juce::File& f);
};

