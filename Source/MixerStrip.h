/*
  ==============================================================================

    MixerStrip.h
    Created: 27 Aug 2025 4:26:10pm
    Author:  User

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
class MixerStrip : public juce::Component
{
public:
    MixerStrip();
    ~MixerStrip() override = default;

    // crossfader 0 - 1
    void setCrossfade(float x);

    std::function<void(float)> onCrossfadeChanged; 
    // deck A or B
    std::function<void(bool)>  onSnapToDeck;       

    void resized() override;

private:
    juce::Label  title;
    juce::Slider crossfader;
    juce::TextButton btnA, btnB;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MixerStrip)
};
