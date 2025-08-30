/*
  ==============================================================================

    MixerStrip.h
    Created: 27 Aug 2025 4:26:10pm
    Author:  Lena

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PixelButton.h"

class MixerStrip : public juce::Component
{
public:
    MixerStrip();
    ~MixerStrip() override = default;

    // crossfader 0 - 1
    void setCrossfade(float x);

    // event handler
    std::function<void(float)> onCrossfadeChanged; 

    // snaps playback to deck A or B
    std::function<void(bool)>  onSnapToDeck;       

    void resized() override;

private:
    juce::Label  title;
    juce::Slider crossfader;
    PixelButton btnA, btnB;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MixerStrip)
};
