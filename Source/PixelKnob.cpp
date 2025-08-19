/*
  ==============================================================================

    PixelKnob.cpp
    Created: 19 Aug 2025 3:48:53pm
    Author:  User

  ==============================================================================
*/

#include "PixelKnob.h"

PixelKnob::PixelKnob(const juce::String& filePrefix)
    : prefix(filePrefix)
{
    setInterceptsMouseClicks(true, true);
    reloadFrames();
}

PixelKnob::~PixelKnob() = default;

void PixelKnob::reloadFrames()
{
    frames.clearQuick();

    auto folder = findKnobsFolder();
    if (folder.isDirectory())
    {
        loadFramesFromFolder(folder);
    }
    else
    {
        // fallback: try current working dir / Assets/Knobs
        auto cwd = juce::File::getCurrentWorkingDirectory().getChildFile("Assets").getChildFile("Knobs");
        if (cwd.isDirectory())
            loadFramesFromFolder(cwd);
    }

    // ensure currentPosition is valid
    if (frames.size() > 0)
    {
        if (currentPosition >= frames.size())
            currentPosition = frames.size() - 1;
    }
    else
    {
        currentPosition = 0;
    }

    repaint();
}

void PixelKnob::loadFramesFromFolder(const juce::File& folder)
{
    frames.clearQuick();

    if (!folder.isDirectory())
        return;

    // We know the filenames are prefix_0 ... prefix_6 (e.g. knob_0.png .. knob_6.png)
    // Try them in order and add any valid images we find.
    for (int i = 0; i <= 6; ++i)
    {
        auto fname = prefix + "_" + juce::String(i) + ".png";
        auto f = folder.getChildFile(fname);

        if (!f.existsAsFile())
            continue; // skip missing frames (still fine)

        if (auto in = f.createInputStream())
        {
            juce::Image img = juce::ImageFileFormat::loadFrom(*in);
            if (img.isValid())
                frames.add(img);
        }
    }

    // if nothing found, frames stays empty (paint() draws a fallback)
    if (frames.size() == 0)
    {
        currentPosition = 0;
    }
    else if (currentPosition >= frames.size())
    {
        currentPosition = frames.size() - 1;
    }

    repaint();
}


void PixelKnob::setPosition(int newPosition)
{
    if (frames.size() == 0)
    {
        currentPosition = 0;
        repaint();
        return;
    }

    const int old = currentPosition;
    currentPosition = juce::jlimit(0, frames.size() - 1, newPosition);
    if (old != currentPosition)
    {
        repaint();
        notifyIfChanged(old, currentPosition);
    }
}

void PixelKnob::notifyIfChanged(int oldPos, int newPos)
{
    if (onValueChange)
        onValueChange(newPos);
}

void PixelKnob::paint(juce::Graphics& g)
{
    auto dest = getLocalBounds();

    if (frames.size() == 0)
    {
        // placeholder: draw a simple knob circle
        g.setColour(juce::Colours::darkgrey);
        g.fillEllipse(dest.toFloat());
        g.setColour(juce::Colours::lightgrey);
        g.drawEllipse(dest.toFloat(), 2.0f);
        return;
    }

    const juce::Image& img = frames.getReference(currentPosition);

    // Use nearest/low-quality resampling for crisp pixel art
    g.setImageResamplingQuality(juce::Graphics::lowResamplingQuality);

    // Draw the image centered and aspect-fitted into dest
    g.drawImageWithin(img,
        dest.getX(), dest.getY(),
        dest.getWidth(), dest.getHeight(),
        juce::RectanglePlacement::centred,
        false);
}


void PixelKnob::mouseDown(const juce::MouseEvent& e)
{
    isDragging = true;
    dragStartY = e.position.getY();
    startPosition = currentPosition;

    if (clickSteps && e.mods.isLeftButtonDown() && e.mods.isAnyModifierKeyDown() == false)
    {
        // simple click: cycle forward by 1
        // (if you prefer click-to-cycle without modifiers, remove the modifier check)
        setPosition(currentPosition + 1);
    }
}

void PixelKnob::mouseDrag(const juce::MouseEvent& e)
{
    if (!isDragging || frames.size() == 0)
        return;

    float dy = static_cast<float>(dragStartY - e.position.getY()); // drag up = positive
    if (!dragIncreases)
        dy = -dy;

    int deltaSteps = (int)std::floor((dy + (pixelsPerStep * 0.5f)) / pixelsPerStep);
    setPosition(startPosition + deltaSteps);
}

void PixelKnob::mouseUp(const juce::MouseEvent&)
{
    isDragging = false;
}

void PixelKnob::mouseWheelMove(const juce::MouseEvent&, const juce::MouseWheelDetails& wheel)
{
    if (frames.size() == 0) return;

    if (wheel.deltaY > 0)      setPosition(currentPosition + 1);
    else if (wheel.deltaY < 0) setPosition(currentPosition - 1);
}


juce::File PixelKnob::findKnobsFolder()
{
    juce::File dir = juce::File::getSpecialLocation(juce::File::currentExecutableFile).getParentDirectory();

    for (int i = 0; i < 10; ++i)
    {
        auto candidate = dir.getChildFile("Assets").getChildFile("Knobs");
        if (candidate.isDirectory())
            return candidate;
        dir = dir.getParentDirectory();
    }

    auto cwd = juce::File::getCurrentWorkingDirectory().getChildFile("Assets").getChildFile("Knobs");
    if (cwd.isDirectory())
        return cwd;

    return {};
}
