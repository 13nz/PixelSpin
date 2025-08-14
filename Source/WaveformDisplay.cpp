/*
  ==============================================================================

    WaveformDisplay.cpp
    Created: 30 Apr 2025 5:21:03pm
    Author:  Lena

  ==============================================================================
*/

#include <JuceHeader.h>
#include "WaveformDisplay.h"

//==============================================================================
WaveformDisplay::WaveformDisplay(juce::AudioFormatManager& formatManagerToUse,
                                 juce::AudioThumbnailCache& cacheToUse)
                                 : audioThumb(1000, formatManagerToUse, cacheToUse),
                                   fileLoaded(false),
                                   position(0)
{
    audioThumb.addChangeListener(this);
}

WaveformDisplay::~WaveformDisplay()
{
}

void WaveformDisplay::paint (juce::Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));   // clear the background

    g.setColour (juce::Colours::grey);
    g.drawRect (getLocalBounds(), 1);   // draw an outline around the component

    g.setColour (juce::Colours::orange);

    if (fileLoaded) 
    {
        audioThumb.drawChannel(g, getLocalBounds(), 0, audioThumb.getTotalLength(), 0, 1.0);
        g.setColour(juce::Colours::lightgreen);
        g.drawRect(position * getWidth(), 0, getWidth() / 20, getHeight());
    }
    else 
    {
        g.setFont(juce::FontOptions(20.0f));
        g.drawText("File not loaded...", getLocalBounds(),
            juce::Justification::centred, true);   // draw some placeholder text
    }


}

void WaveformDisplay::resized()
{

}

void WaveformDisplay::changeListenerCallback(juce::ChangeBroadcaster* source)
{
    repaint();
}

void WaveformDisplay::loadURL(juce::URL audioURL)
{
    // clear thumbnail
    audioThumb.clear();
    // unpack url into inputSource and set the thumbnail to that
    fileLoaded = audioThumb.setSource(new juce::URLInputSource(audioURL));

}

void WaveformDisplay::setPositionRelative(double pos)
{
    if (pos != position)
    {
        position = pos;
        repaint();
    }
}
