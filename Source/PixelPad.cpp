/*
  ==============================================================================

    PixelPad.cpp
    Created: 27 Aug 2025 3:44:37pm
    Author:  User

  ==============================================================================
*/

#include "PixelPad.h"

juce::File PixelPad::defaultPadsFolder;

void PixelPad::paintButton(juce::Graphics& g, bool isHover, bool isDown)
{
    const juce::Image& img = isDown ? imgPressed : (isHover ? imgHover : imgDefault);
    if (img.isValid())
        drawCrisp(g, img, getWidth(), getHeight());
}

void PixelPad::setImages(juce::Image def, juce::Image hov, juce::Image prs)
{
    imgDefault = def;
    imgHover = hov.isValid() ? hov : def;
    imgPressed = prs.isValid() ? prs : imgHover;
    repaint();
}

bool PixelPad::setImagesFromBaseName(const juce::String& base,
    const juce::String& defSuf,
    const juce::String& hovSuf,
    const juce::String& prsSuf)
{
    auto root = getDefaultPadsFolder();
    if (!root.isDirectory()) return false;

    juce::Image d = tryLoad(root.getChildFile(base + defSuf));
    if (!d.isValid()) return false;

    juce::Image h = tryLoad(root.getChildFile(base + hovSuf));
    juce::Image p = tryLoad(root.getChildFile(base + prsSuf));
    setImages(d, h, p);
    return true;
}

void PixelPad::setDefaultPadsFolder(const juce::File& folder)
{
    if (folder.isDirectory()) defaultPadsFolder = folder;
}

juce::File PixelPad::getDefaultPadsFolder()
{
    if (defaultPadsFolder.isDirectory()) return defaultPadsFolder;

    // walk up from executable to Assets/Pads
    juce::File dir = juce::File::getSpecialLocation(juce::File::currentExecutableFile).getParentDirectory();
    for (int i = 0; i < 10; ++i)
    {
        auto cand = dir.getChildFile("Assets").getChildFile("Pads");
        if (cand.isDirectory()) return (defaultPadsFolder = cand);
        dir = dir.getParentDirectory();
    }
    // try current working dir
    auto cwd = juce::File::getCurrentWorkingDirectory().getChildFile("Assets").getChildFile("Pads");
    if (cwd.isDirectory()) return (defaultPadsFolder = cwd);

    return {};
}

juce::File PixelPad::findPadsFolder() { return getDefaultPadsFolder(); }

juce::Image PixelPad::tryLoad(const juce::File& f)
{
    if (!f.existsAsFile()) return {};
    if (auto in = f.createInputStream()) return juce::ImageFileFormat::loadFrom(*in);
    return {};
}

void PixelPad::drawCrisp(juce::Graphics& g, const juce::Image& img, int w, int h)
{
    if (!img.isValid() || w <= 0 || h <= 0) return;

    const int target = std::min(w, h);
    const int imgW = img.getWidth();
    const int imgH = img.getHeight();
    const int maxDim = std::max(imgW, imgH);
    const int cx = w / 2, cy = h / 2;

    // integer upscale: crisp
    if (target >= maxDim)  
    {
        const int k = std::max(1, target / maxDim);
        auto t = juce::AffineTransform::translation(-imgW * 0.5f, -imgH * 0.5f)
            .scaled((float)k)
            .translated((float)cx, (float)cy);
        g.setImageResamplingQuality(juce::Graphics::lowResamplingQuality);
        g.drawImageTransformed(img, t, false);
    }
    // downscale: HQ
    else                   
    {
        const float s = (float)target / (float)maxDim;
        auto t = juce::AffineTransform::translation(-imgW * 0.5f, -imgH * 0.5f)
            .scaled(s, s)
            .translated((float)cx, (float)cy);
        g.setImageResamplingQuality(juce::Graphics::highResamplingQuality);
        g.drawImageTransformed(img, t, false);
    }
}
