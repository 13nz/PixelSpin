/*
  ==============================================================================

    VinylSpinner.h
    Created: 16 Aug 2025 5:38:18pm
    Author:  User

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
class DJAudioPlayer;

//==============================================================================
/*
*/
class VinylSpinner  : public juce::Component,
                      private juce::Timer
{
public:
    VinylSpinner(DJAudioPlayer* p, double rpm = 33.333);
    ~VinylSpinner() override;

    void paint (juce::Graphics&) override;
    void resized() override;

    void setImage(const juce::Image& img);

private:
    void timerCallback() override;

    DJAudioPlayer* player = nullptr;
    juce::Image vinyl;
    double rpm = 33.333;
    double currentAngle = 0.0;


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (VinylSpinner)
};
