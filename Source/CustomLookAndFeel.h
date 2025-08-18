/*
  ==============================================================================

    CustomLookAndFeel.h
    Created: 17 Aug 2025 4:20:43pm
    Author:  User

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/*
*/
class CustomLookAndFeel  : public juce::LookAndFeel_V4
{
public:
    CustomLookAndFeel();
    ~CustomLookAndFeel() override;

    void drawButtonBackground(juce::Graphics&, juce::Button&,
        const juce::Colour& backgroundColour,
        bool isMouseOverButton, bool isButtonDown) override;

    void drawTableHeaderBackground(juce::Graphics&, juce::TableHeaderComponent&) override;

    void drawLinearSlider(juce::Graphics&, int x, int y, int width, int height,
        float sliderPos, float minSliderPos, float maxSliderPos,
        const juce::Slider::SliderStyle, juce::Slider&) override;

    void drawComboBox(juce::Graphics&, int width, int height, bool isButtonDown,
        int buttonX, int buttonY, int buttonW, int buttonH,
        juce::ComboBox&) override;

    // set images
    void setSliderThumbImages(juce::Image defaultImg,
        juce::Image hoverImg = juce::Image(),
        juce::Image pressedImg = juce::Image(),
        int desiredPixels = 28);

    // get from base name
    bool setSliderThumbImagesFromBasename(const juce::String& base,
                                          const juce::String& defaultSuffix = "_default.png",
                                          const juce::String& hoverSuffix = "_hover.png",
                                          const juce::String& pressedSuffix = "_pressed.png",
                                          int desiredPixels = 28);

    // custom png slider thumb
    void setSliderThumbPixels(int px) { sliderThumbPixels = px; }

    // custom title bar
    void CustomLookAndFeel::drawDocumentWindowTitleBar(juce::DocumentWindow& window,
                                                       juce::Graphics& g,
                                                       int w, int h,
                                                       int titleSpaceX, int titleSpaceW,
                                                       const juce::Image* icon,
                                                       bool drawTitleTextOnLeft);
    // custom title bar buttons
    juce::Button* createDocumentWindowButton(int buttonType) override;
   
private:
    static void drawCrispImageAt(juce::Graphics&, const juce::Image&, int cx, int cy, int targetPx);
    static juce::File findButtonsFolder();
    static juce::Image loadFromButtons(const juce::String& filename);

    // 3-state assets
    juce::Image thumbDefault, thumbHover, thumbPressed;
    int sliderThumbPixels{ 28 };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CustomLookAndFeel)
};

