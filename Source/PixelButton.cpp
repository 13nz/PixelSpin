/*
  ==============================================================================

    PixelButton.cpp
    Created: 17 Aug 2025 4:56:24pm
    Author:  User

  ==============================================================================
*/

#include <JuceHeader.h>
#include "PixelButton.h"

juce::File PixelButton::defaultButtonsFolder; // empty until use

PixelButton::PixelButton(const juce::String& name) : juce::Button(name) {}

PixelButton::~PixelButton()
{
}

// set images
void PixelButton::setImages(juce::Image defaultImg,
                            juce::Image hoverImg,
                            juce::Image pressedImg)
{
    imgDefault = defaultImg;
    imgHover = hoverImg.isValid() ? hoverImg : defaultImg;
    imgPressed = pressedImg.isValid() ? pressedImg : defaultImg;

    repaint();
}

void PixelButton::paintButton(juce::Graphics& g, bool isHover, bool isPressed)
{
    const juce::Image& img = isPressed ? imgPressed : (isHover ? imgHover : imgDefault);

    if (img.isValid())
        drawCrisp(g, img, getWidth(), getHeight(), 0.0);
}

// draws pixel art not blurry
void PixelButton::drawCrisp(juce::Graphics& g, const juce::Image& img, int compW, int compH, double angle)
{
    const int imgW = img.getWidth();
    const int imgH = img.getHeight();

    if (imgW <= 0 || imgH <= 0 || compW <= 0 || compH <= 0) return;

    // square & centered
    const int target = std::min(compW, compH);
    const int maxDim = std::max(imgW, imgH);

    // center on whole pixels
    const int cx = compW / 2;
    const int cy = compH / 2;

    // 2 options
    // integer upscale when component >= image
    // fractional downscale when component < image
    if (target >= maxDim)
    {
        const int integerScale = std::max(1, target / maxDim); // floor integer
        // transform: center -> integer scale -> (optional roattion) -> traslate to component center
        juce::AffineTransform t = juce::AffineTransform::translation(-imgW * 0.5f, -imgH * 0.5f).scaled((float)integerScale).rotated((float)angle).translated((float)cx, (float)cy);

        // nearest neighbor
        g.setImageResamplingQuality(juce::Graphics::lowResamplingQuality);
        g.drawImageTransformed(img, t, false);
    }
    else
    {
        const float scale = (float)target / (float)maxDim;

        juce::AffineTransform t = juce::AffineTransform::translation(-imgW * 0.5f, -imgH * 0.5f).scaled(scale, scale).rotated((float)angle).translated((float)cx, (float)cy);

        // high quality downscale
        g.setImageResamplingQuality(juce::Graphics::highResamplingQuality);
        g.drawImageTransformed(img, t, false);
    }
}

bool PixelButton::setImagesFromBaseName(const juce::String& baseName,
    const juce::String& defaultSuffix,
    const juce::String& hoverSuffix,
    const juce::String& pressedSuffix)
{
    auto d = loadImageFromButtons(baseName + defaultSuffix);
    auto h = loadImageFromButtons(baseName + hoverSuffix);
    auto p = loadImageFromButtons(baseName + pressedSuffix);

    if (!d.isValid())
        return false;

    setImages(d, h.isValid() ? h : d, p.isValid() ? p : (h.isValid() ? h : d));
    return true;
}


void PixelButton::setDefaultButtonsFolder(const juce::File& folder)
{
    if (folder.isDirectory())
        defaultButtonsFolder = folder;
}

juce::File PixelButton::getDefaultButtonsFolder()
{
    if (defaultButtonsFolder.isDirectory()) return defaultButtonsFolder;


    // start at the executable directory 
    juce::File dir = juce::File::getSpecialLocation(juce::File::currentExecutableFile)
        .getParentDirectory();

    // go up to 10 levels and look for Assets/Buttons
    for (int i = 0; i < 10; ++i)
    {
        auto candidate = dir.getChildFile("Assets").getChildFile("Buttons");
        if (candidate.isDirectory())
            return candidate;
        dir = dir.getParentDirectory();
    }

    // fallback if launching from project root
    dir = juce::File::getCurrentWorkingDirectory()
        .getChildFile("Assets").getChildFile("Buttons");
    if (dir.isDirectory())
        return dir;

    defaultButtonsFolder = dir;
    return defaultButtonsFolder;
}

juce::File PixelButton::findButtonsFolder()
{
    // go up from the executable to Assets/Buttons
    auto dir = juce::File::getSpecialLocation(juce::File::currentExecutableFile).getParentDirectory();

    for (int i = 0; i < 10; ++i)
    {
        auto candidate = dir.getChildFile("Assets").getChildFile("Buttons");

        if (candidate.isDirectory()) return candidate;

        dir = dir.getParentDirectory();
    }

    // check current working dir
    auto cwd = juce::File::getCurrentWorkingDirectory().getChildFile("Assets").getChildFile("Buttons");

    if (cwd.isDirectory())
        return cwd;


    return {}; 
}

juce::Image PixelButton::loadImageFromButtons(const juce::String& filename)
{
    auto root = getDefaultButtonsFolder();

    if (!root.isDirectory()) return {};

    auto f = root.getChildFile(filename);
    return tryLoad(f);
}

juce::Image PixelButton::tryLoad(const juce::File& f)
{
    if (!f.existsAsFile()) return {};

    if (auto in = f.createInputStream()) return juce::ImageFileFormat::loadFrom(*in);

    return {};
}
