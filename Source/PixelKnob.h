/*
  ==============================================================================

    PixelKnob.h
    Created: 19 Aug 2025 3:48:53pm
    Author:  Lena

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

/*
  PixelKnob
  - loads a sequence of PNG frames from Assets/Knobs whose filename begins with the given prefix
  - shows one of the frames depending on position [0..frames-1]
  - user can click/drag vertically to change the position (drag up = increase)
  - mouse wheel also steps the position
  - onValueChange when the position changes
*/

class PixelKnob : public juce::Component
{
public:
    PixelKnob(const juce::String& filePrefix = "knob");
    ~PixelKnob() override;

    // load frames 
    void reloadFrames();

    // current position (0 .. numFrames()-1)
    int getPosition() const noexcept { return currentPosition; }
    void setPosition(int newPosition);                   

    // gets number of frames
    int numFrames() const noexcept { return (int)frames.size(); }

    // callback when position changes
    std::function<void(int)> onValueChange;

    // set whether dragging is inverted (drag up increases when true)
    void setDragIncreases(bool shouldIncrease) noexcept { dragIncreases = shouldIncrease; }

    // allow click-to-cycle behavior
    void setClickSteps(bool allow) noexcept { clickSteps = allow; }

    // overrides
    void paint(juce::Graphics& g) override;
    void resized() override {}

    // mouse event handlers
    void mouseDown(const juce::MouseEvent& e) override;
    void mouseDrag(const juce::MouseEvent& e) override;
    void mouseUp(const juce::MouseEvent& e) override;
    void mouseWheelMove(const juce::MouseEvent&, const juce::MouseWheelDetails& wheel) override;

    // getters / setters
    int getStep() const; // (0 - 6)         
    void setStep(int newStep);



private:
    juce::Array<juce::Image> frames;
    juce::String prefix;

    int currentPosition{ 0 };
    bool dragIncreases{ true };
    bool clickSteps{ true };

    // dragging state
    bool isDragging{ false };
    int dragStartY{ 0 };
    int startPosition{ 0 };
    float pixelsPerStep{ 12.0f }; // how many vertical pixels = one step

    // find knob pngs
    static juce::File findKnobsFolder();

    // loads into frames
    void loadFramesFromFolder(const juce::File& folder);

    void notifyIfChanged(int oldPos, int newPos);

    int step = 0; // active frame


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PixelKnob)
};
