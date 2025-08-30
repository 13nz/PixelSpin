/*
  ==============================================================================

    PixelButton.h
    Created: 17 Aug 2025 4:56:24pm
    Author:  Lena

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

/*
* pixel art button
* can set png as images (for default, hover, pressed)
* scales to fit inside bounds
* no blur
*/
class PixelButton  : public juce::Button
{
public:
    PixelButton(const juce::String& name = {});
    ~PixelButton() override;

    // sets button image
    void setImages(juce::Image defaultImg, juce::Image hoverImg = juce::Image(), juce::Image pressedImg = juce::Image());

    // sets using basename
    bool setImagesFromBaseName(const juce::String& baseName,
                               const juce::String& defaultSuffix = "_default.png",
                               const juce::String& hoverSuffix = "_hover.png",
                               const juce::String& pressedSuffix = "_pressed.png");

    // draws button
    void paintButton(juce::Graphics& g, bool isHover, bool isPressed) override;

    // sets default folder
    static void setDefaultButtonsFolder(const juce::File& folder);

    // gets default folder
    static juce::File getDefaultButtonsFolder();

private:
    // helper to draw scaled without blurring
    static void drawCrisp(juce::Graphics& g, const juce::Image& img, int compW, int compH, double angle = 0.0);

    // image variables
    juce::Image imgDefault;
    juce::Image imgHover;
    juce::Image imgPressed;

    // file finding helpers
    static juce::File findButtonsFolder(); // walk up from exe to Assets/Buttons
    static juce::Image loadImageFromButtons(const juce::String& filename);
    static juce::Image tryLoad(const juce::File& f);

    static juce::File defaultButtonsFolder; // defined once in PixelButton.cpp


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PixelButton)
};
