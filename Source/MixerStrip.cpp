/*
  ==============================================================================

    MixerStrip.cpp
    Created: 27 Aug 2025 4:26:10pm
    Author:  User

  ==============================================================================
*/

#include <JuceHeader.h>
#include "MixerStrip.h"

//==============================================================================
MixerStrip::MixerStrip()
{
    // title
    title.setText("Mix", juce::dontSendNotification);
    title.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(title);

    // slider
    crossfader.setSliderStyle(juce::Slider::LinearHorizontal);
    crossfader.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    crossfader.setRange(0.0, 1.0, 0.0);
    crossfader.setValue(0.5);

    // event handler
    crossfader.onValueChange = [this]()
        {
            if (onCrossfadeChanged)
                onCrossfadeChanged((float)crossfader.getValue());
        };

    addAndMakeVisible(crossfader);

    btnA.setButtonText("A");
    btnB.setButtonText("B");


    // deck buttons handler
    btnA.onClick = [this]()
        {
            crossfader.setValue(0.0, juce::sendNotificationSync);
            if (onSnapToDeck) onSnapToDeck(false); 
        };
    btnB.onClick = [this]()
        {
            crossfader.setValue(1.0, juce::sendNotificationSync);
            if (onSnapToDeck) onSnapToDeck(true); 
        };

    addAndMakeVisible(btnA);
    addAndMakeVisible(btnB);
}

void MixerStrip::setCrossfade(float x)
{
    crossfader.setValue(juce::jlimit(0.0, 1.0, (double)x), juce::sendNotificationSync);
}

void MixerStrip::resized()
{
    auto r = getLocalBounds().reduced(8);

    title.setBounds(r.removeFromTop(22));

    auto topRow = r.removeFromTop(28);
    btnA.setBounds(topRow.removeFromLeft(36));
    topRow.removeFromLeft(6);
    btnB.setBounds(topRow.removeFromRight(36));

    r.removeFromTop(6);
    crossfader.setBounds(r.removeFromTop(24));
}
