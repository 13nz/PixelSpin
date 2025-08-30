/*
  ==============================================================================

    MixerStrip.cpp
    Created: 27 Aug 2025 4:26:10pm
    Author:  Lena

  ==============================================================================
*/

#include <JuceHeader.h>
#include "MixerStrip.h"

// set title, configure slider, wire callbacks, and add child components
MixerStrip::MixerStrip()
{
    // title
    title.setText("Mix", juce::dontSendNotification);
    title.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(title);

    // crossfade slider setup
    crossfader.setSliderStyle(juce::Slider::LinearHorizontal);
    crossfader.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    crossfader.setRange(0.0, 1.0, 0.0);
    crossfader.setValue(0.5);

    // notify when it changes
    crossfader.onValueChange = [this]()
        {
            if (onCrossfadeChanged)
                onCrossfadeChanged((float)crossfader.getValue());
        };

    addAndMakeVisible(crossfader);

    // set pixel art images
    btnA.setImagesFromBaseName("A");
    btnB.setImagesFromBaseName("B");

    // fallback text
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

// setter for crossfade
// synchronous notification
void MixerStrip::setCrossfade(float x)
{
    crossfader.setValue(juce::jlimit(0.0, 1.0, (double)x), juce::sendNotificationSync);
}

// center controls with component
void MixerStrip::resized()
{
    auto area = getLocalBounds().reduced(8);

    // fixed heights
    const int titleH = 22;
    const int btnH = 32;
    const int crossH = 24;
    const int gap = 6;

    // total block height
    const int blockH = titleH + gap + btnH + gap + crossH;

    // vertically centered block
    auto block = area.withHeight(blockH).withCentre(area.getCentre());

    // title row
    title.setBounds(block.removeFromTop(titleH));

    block.removeFromTop(gap);

    // centered buttons
    auto btnRow = block.removeFromTop(btnH);
    auto btnW = 32;
    auto btnTotalW = btnW * 2 + gap;
    auto btnArea = btnRow.withWidth(btnTotalW).withCentre(btnRow.getCentre());
    btnA.setBounds(btnArea.removeFromLeft(btnW));
    btnArea.removeFromLeft(gap);
    btnB.setBounds(btnArea.removeFromLeft(btnW));

    block.removeFromTop(gap);

    // crossfade row
    auto crossRow = block.removeFromTop(crossH);
    auto crossW = crossRow.getWidth(); // leave margin
    crossfader.setBounds(crossRow.withWidth(crossW).withCentre(crossRow.getCentre()));
}

