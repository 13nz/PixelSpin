/*
  ==============================================================================

    PixelPad.h
    Created: 27 Aug 2025 3:44:37pm
    Author:  Lena

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
/** assets in Assets/Pads
    pad_default.png, pad_hover.png, pad_pressed.png
    setImagesFromBaseName("pad") with suffixes
*/
class PixelPad : public juce::Button
{
public:
    PixelPad(const juce::String& name = juce::String()) : juce::Button(name) {}

    void paintButton(juce::Graphics& g, bool isHover, bool isDown) override;

    // sets images
    void setImages(juce::Image def, juce::Image hov = {}, juce::Image prs = {});

    // sets images from base name
    bool setImagesFromBaseName(const juce::String& baseName,
        const juce::String& defaultSuffix = "_default.png",
        const juce::String& hoverSuffix = "_hover.png",
        const juce::String& pressedSuffix = "_pressed.png");

    // sets and gets default assets folder
    static void setDefaultPadsFolder(const juce::File& folder);
    static juce::File getDefaultPadsFolder();

private:
    // file finding and loading helpers
    static juce::File findPadsFolder();
    static juce::Image tryLoad(const juce::File& f);

    // draws clean image
    static void drawCrisp(juce::Graphics& g, const juce::Image& img, int w, int h);

    juce::Image imgDefault, imgHover, imgPressed;
    static juce::File defaultPadsFolder;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PixelPad)
};
