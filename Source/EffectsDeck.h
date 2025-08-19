/*
  ==============================================================================

    EffectsDeck.h
    Created: 19 Aug 2025 3:31:42pm
    Author:  User

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/*
*/
class EffectsDeck  : public juce::Component
{
public:
    EffectsDeck();
    ~EffectsDeck() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (EffectsDeck)
};
