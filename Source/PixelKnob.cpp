/*
  ==============================================================================

    PixelKnob.cpp
    Created: 19 Aug 2025 3:48:53pm
    Author:  Lena

  ==============================================================================
*/

#include "PixelKnob.h"
// knob rendered from a sequence of png frames
// supports dragging and mouse wheel
PixelKnob::PixelKnob(const juce::String& filePrefix)
    : prefix(filePrefix)
{
    setInterceptsMouseClicks(true, true);
    reloadFrames();
}

PixelKnob::~PixelKnob() = default;

void PixelKnob::reloadFrames()
{
    // reset frames
    frames.clearQuick();

    // looks for assets
    auto folder = findKnobsFolder();
    if (folder.isDirectory())
    {
        // loads by prefix + suffix
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

    // refresh UI
    repaint();
}

void PixelKnob::loadFramesFromFolder(const juce::File& folder)
{
    frames.clearQuick();

    if (!folder.isDirectory())
        return;

    // filenames are knob_0 ... knob_6 
    for (int i = 0; i <= 6; ++i)
    {
        auto fname = prefix + "_" + juce::String(i) + ".png";
        auto f = folder.getChildFile(fname);

        if (!f.existsAsFile())
            continue; // skip missing frames

        if (auto in = f.createInputStream())
        {
            juce::Image img = juce::ImageFileFormat::loadFrom(*in);
            if (img.isValid())
                frames.add(img);
        }
    }

    // if nothing found frames stays empty
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
    // clamp to range
    currentPosition = juce::jlimit(0, frames.size() - 1, newPosition);
    if (old != currentPosition)
    {
        // redraw and notify listeners
        repaint();
        notifyIfChanged(old, currentPosition);
    }
}

// callback to owner
void PixelKnob::notifyIfChanged(int oldPos, int newPos)
{
    if (onValueChange)
        onValueChange(newPos);
}

void PixelKnob::paint(juce::Graphics& g)
{
    auto dest = getLocalBounds();

    // if frames are missing
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

    // nearest/low-quality resampling for crisp pixel art
    g.setImageResamplingQuality(juce::Graphics::lowResamplingQuality);

    // centered and aspect fitted
    g.drawImageWithin(img,
        dest.getX(), dest.getY(),
        dest.getWidth(), dest.getHeight(),
        juce::RectanglePlacement::centred,
        false);
}


void PixelKnob::mouseDown(const juce::MouseEvent& e)
{
    isDragging = true;
    dragStartY = e.position.getY(); // starting Y for vertical data
    startPosition = currentPosition;

    // can cycle to next by left click
    if (clickSteps && e.mods.isLeftButtonDown() && e.mods.isAnyModifierKeyDown() == false)
    {
        setPosition(currentPosition + 1);

        step = (step + 1) % (int)frames.size();
        repaint();
    }
}

void PixelKnob::mouseDrag(const juce::MouseEvent& e)
{
    if (!isDragging || frames.size() == 0)
        return;

    float dy = static_cast<float>(dragStartY - e.position.getY()); // drag up = positive
    if (!dragIncreases)
        dy = -dy;

    // convert pixels to steps
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

    // up -> increases
    // down -> decreases
    if (wheel.deltaY > 0)
    {
        setPosition(currentPosition + 1);
    }
    else if (wheel.deltaY < 0)
    {
        setPosition(currentPosition - 1);
    }
}

// finds folder where assets are located
juce::File PixelKnob::findKnobsFolder()
{
    // search up from .exe
    juce::File dir = juce::File::getSpecialLocation(juce::File::currentExecutableFile).getParentDirectory();

    for (int i = 0; i < 10; ++i)
    {
        auto candidate = dir.getChildFile("Assets").getChildFile("Knobs");
        if (candidate.isDirectory())
            return candidate;
        dir = dir.getParentDirectory();
    }

    // fallback: search from source folder
    auto cwd = juce::File::getCurrentWorkingDirectory().getChildFile("Assets").getChildFile("Knobs");
    if (cwd.isDirectory())
        return cwd;

    return {};
}

// getters/setters
int PixelKnob::getStep() const 
{
    return step;
}

void PixelKnob::setStep(int newStep)
{
    step = juce::jlimit(0, (int)frames.size() - 1, newStep);
    repaint();
}