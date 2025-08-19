/*
  ==============================================================================

    PixelKnob.h
    Created: 19 Aug 2025 3:48:53pm
    Author:  User

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

/*
  PixelKnob
  - loads a sequence of PNG frames from Assets/Knobs whose filename begins with the given prefix
    (e.g. prefix "knob" matches knob_0.png, knob_1.png, knob_2.png, ...)
  - shows one of the frames depending on discrete position [0..frames-1]
  - user can click/drag vertically to change the position (drag up = increase)
  - mouse wheel also steps the position
  - call `setPosition(int)` to set programmatically
  - assign `onValueChange` to receive callbacks when the position changes
*/

class PixelKnob : public juce::Component
{
public:
    // Construct with a prefix (no folder) that matches files in Assets/Knobs,
    // e.g. PixelKnob knob("knob");
    PixelKnob(const juce::String& filePrefix = "knob");
    ~PixelKnob() override;

    // load frames again (useful if assets change at runtime)
    void reloadFrames();

    // current discrete position (0 .. numFrames()-1)
    int getPosition() const noexcept { return currentPosition; }
    void setPosition(int newPosition);                   // clamps and repaints + notifies if changed

    int numFrames() const noexcept { return (int)frames.size(); }

    // callback invoked when position changes
    std::function<void(int)> onValueChange;

    // optional: set whether dragging is inverted (drag up increases when true)
    void setDragIncreases(bool shouldIncrease) noexcept { dragIncreases = shouldIncrease; }

    // optional: allow click-to-cycle behavior
    void setClickSteps(bool allow) noexcept { clickSteps = allow; }

    // Component overrides
    void paint(juce::Graphics& g) override;
    void resized() override {}

    void mouseDown(const juce::MouseEvent& e) override;
    void mouseDrag(const juce::MouseEvent& e) override;
    void mouseUp(const juce::MouseEvent& e) override;
    void mouseWheelMove(const juce::MouseEvent&, const juce::MouseWheelDetails& wheel) override;

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
    float pixelsPerStep{ 12.0f }; // how many vertical pixels = one step (tweakable)

    // find knob pngs
    static juce::File findKnobsFolder();

    // loads into frames
    void loadFramesFromFolder(const juce::File& folder);

    void notifyIfChanged(int oldPos, int newPos);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PixelKnob)
};
