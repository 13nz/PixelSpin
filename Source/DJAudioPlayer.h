#pragma once

#include <JuceHeader.h>
#include "EffectsDeck.h"
#include "MixerStrip.h"


class DJAudioPlayer : public juce::AudioSource, public juce::ChangeBroadcaster
{
public:
    DJAudioPlayer(juce::AudioFormatManager& _formatManager);
    ~DJAudioPlayer();

    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;

    // loads track
    void loadURL(juce::URL audioURL);

    // setters
    void setGain(double gain);
    void setSpeed(double ratio);
    void setPosition(double posInSecs);
    void setPositionRelative(double pos);

    // playback control
    void start();
    void stop();

    // get the relative position of the playhead
    double getPositionRelative();

    // exposed lates URL
    juce::URL getCurrentURL() const { return currentURL; }

    // check if music is playing
    bool isPlaying() const;

    // -- EFFECTS --
    
    // reverb setter
    void setReverbAmount(float wet01) { effects.setReverbAmount(wet01); }

    // chorus setter
    void setChorusAmount(float amt01) { effects.setChorusAmount(amt01); }

    // compression setter
    void setCompressionAmount(float amt01) { effects.setCompressionAmount(amt01); }

    // delay setter
    void setDelayAmount(float amt01) { effects.setDelayAmount(amt01); }

    // exporting methods
    double getPositionSeconds() const;
    double getTrackLengthSeconds() const;


private:
    // audio & playback
    juce::AudioFormatManager& formatManager;
    std::unique_ptr<juce::AudioFormatReaderSource> readerSource;
    juce::AudioTransportSource transportSource;

    juce::ResamplingAudioSource resampleSource{ &transportSource, false, 2 };

    juce::URL currentURL;

    // effect deck
    // handles all effects and don't have to processs each individually
    EffectsDeck effects;
};
