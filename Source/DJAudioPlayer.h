#pragma once

#include <JuceHeader.h>

class DJAudioPlayer : public juce::AudioSource, public juce::ChangeBroadcaster
{
public:
    DJAudioPlayer(juce::AudioFormatManager& _formatManager);
    ~DJAudioPlayer();
    //==============================================================================
    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;

    void loadURL(juce::URL audioURL);
    void setGain(double gain);
    void setSpeed(double ratio);
    void setPosition(double posInSecs);
    void setPositionRelative(double pos);

    void start();
    void stop();

    // get the relative position of the playhead
    double getPositionRelative();

    // exposed lates URL
    juce::URL getCurrentURL() const { return currentURL; }

    // check if music is playing
    bool isPlaying() const;

private:
    // audio & playback
    juce::AudioFormatManager& formatManager;
    std::unique_ptr<juce::AudioFormatReaderSource> readerSource;
    juce::AudioTransportSource transportSource;

    juce::ResamplingAudioSource resampleSource{ &transportSource, false, 2 };

    juce::URL currentURL;

};
