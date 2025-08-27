/*
  ==============================================================================

    CustomLookAndFeel.cpp
    Created: 17 Aug 2025 4:20:43pm
    Author:  User

  ==============================================================================
*/

#include <JuceHeader.h>
#include "CustomLookAndFeel.h"
#include "Theme.h"

//==============================================================================
CustomLookAndFeel::CustomLookAndFeel()
{
    // backgrounds
    setColour(juce::ResizableWindow::backgroundColourId, Theme::windowBg);
    setColour(juce::PopupMenu::backgroundColourId, Theme::panelBg);

    // text
    setColour(juce::Label::textColourId, Theme::textOnDarkMain);

    // buttons
    setColour(juce::TextButton::buttonColourId, Theme::controlFill);
    setColour(juce::TextButton::textColourOnId, Theme::textOnLight);
    setColour(juce::TextButton::textColourOffId, Theme::textOnLight);

    // sliders
    setColour(juce::Slider::trackColourId, Theme::trackBase);
    setColour(juce::Slider::thumbColourId, Theme::accent);

    // comno inputs
    setColour(juce::ComboBox::backgroundColourId, Theme::controlFill);
    setColour(juce::ComboBox::textColourId, Theme::textOnLight);
    setColour(juce::ComboBox::outlineColourId, Theme::panelOutline());
    setColour(juce::TextEditor::backgroundColourId, Theme::controlFill);
    setColour(juce::TextEditor::textColourId, Theme::textOnLight);
    setColour(juce::CaretComponent::caretColourId, Theme::accent);

    // tbale / list
    setColour(juce::TableHeaderComponent::backgroundColourId, Theme::panelBg);
    setColour(juce::TableHeaderComponent::textColourId, Theme::textOnDarkMain);
    setColour(juce::ListBox::backgroundColourId, Theme::panelBg);
    setColour(juce::ListBox::outlineColourId, Theme::panelOutline());

    // scrollbar track background
    setColour(juce::ScrollBar::backgroundColourId, Theme::trackBase);
    // scrollbar thumb (draggable handle)
    setColour(juce::ScrollBar::thumbColourId, Theme::accent);
    // scrollbar track outline
    setColour(juce::ScrollBar::trackColourId, Theme::controlFill);



}

CustomLookAndFeel::~CustomLookAndFeel()
{
}

void CustomLookAndFeel::drawButtonBackground(juce::Graphics& g, juce::Button& b,
    const juce::Colour&, bool over, bool down) 
{
    auto r = b.getLocalBounds().toFloat();
    auto c = down ? Theme::controlFillDown
        : (over ? Theme::controlFillOver : Theme::controlFill);

    g.setColour(c);
    g.fillRoundedRectangle(r, 8.0f);

    // subtle border to separate from dark panels
    g.setColour(Theme::panelOutline());
    g.drawRoundedRectangle(r, 8.0f, 1.0f);
}

void CustomLookAndFeel::drawTableHeaderBackground(juce::Graphics& g, juce::TableHeaderComponent& th) 
{
    g.fillAll(Theme::panelBg);
    g.setColour(Theme::panelOutline());
    g.drawLine(0.f, (float)th.getHeight() - 0.5f, (float)th.getWidth(), (float)th.getHeight() - 0.5f);
}

void CustomLookAndFeel::drawLinearSlider(juce::Graphics& g, int x, int y, int w, int h,
    float pos, float /*min*/, float /*max*/,
    const juce::Slider::SliderStyle style, juce::Slider& s)
{
    const bool isVertical = (style == juce::Slider::LinearVertical
        || style == juce::Slider::LinearBarVertical);

    const bool isBar = (style == juce::Slider::LinearBar
        || style == juce::Slider::LinearBarVertical);

    const float t = 5.0f;

    g.setColour(Theme::trackBase);

    if (!isVertical)
    {
        // HORIZONTAL 
        auto track = juce::Rectangle<float>((float)x, (float)y + (h - t) * 0.5f, (float)w, t);
        g.fillRoundedRectangle(track, t * 0.5f);

        g.setColour(Theme::trackStrong.withAlpha(0.7f));
        const float filledW = juce::jlimit(0.0f, track.getWidth(), pos - track.getX());
        g.fillRoundedRectangle(track.withWidth(filledW), t * 0.5f);

        // draw thumb 
        const int cx = (int)std::round(pos);
        const int cy = (int)std::round(track.getCentreY());

        const bool over = s.isMouseOver(true);
        const bool down = s.isMouseButtonDown();
        const juce::Image& thumb = down ? thumbPressed : (over ? thumbHover : thumbDefault);

        if (thumb.isValid()) drawCrispImageAt(g, thumb, cx, cy, sliderThumbPixels);
        else { g.setColour(Theme::accent); g.fillEllipse((float)cx - 7.f, (float)cy - 7.f, 14.f, 14.f); }
    }
    else
    {
        // VERTICAL 
        auto track = juce::Rectangle<float>((float)x + (w - t) * 0.5f, (float)y, t, (float)h);
        g.fillRoundedRectangle(track, t * 0.5f);

        g.setColour(Theme::trackStrong.withAlpha(0.7f));
        const float fillTop = juce::jlimit(track.getY(), track.getBottom(), pos);
        const float fillH = track.getBottom() - fillTop;
        if (fillH > 0.0f)
            g.fillRoundedRectangle({ track.getX(), fillTop, track.getWidth(), fillH }, t * 0.5f);

        // draw thumb
        const int cx = (int)std::round(track.getCentreX());
        const int cy = (int)std::round(pos);

        const bool over = s.isMouseOver(true);
        const bool down = s.isMouseButtonDown();
        const juce::Image& thumb = down ? thumbPressed : (over ? thumbHover : thumbDefault);

        if (thumb.isValid()) drawCrispImageAt(g, thumb, cx, cy, sliderThumbPixels);
        else { g.setColour(Theme::accent); g.fillEllipse((float)cx - 7.f, (float)cy - 7.f, 14.f, 14.f); }
    }
}



void CustomLookAndFeel::drawComboBox(juce::Graphics& g, int w, int h, bool, int, int, int, int, juce::ComboBox& box) 
{
    auto r = juce::Rectangle<float>(0, 0, (float)w, (float)h);
    g.setColour(Theme::controlFill);
    g.fillRoundedRectangle(r, 6.f);

    g.setColour(Theme::panelOutline());
    g.drawRoundedRectangle(r, 6.f, 1.f);

    // arrow
    g.setColour(Theme::textOnLight.withAlpha(0.7f));
    juce::Path p; const float aw = 8.f, ah = 5.f;
    p.addTriangle(w - 14.f - aw, h * 0.5f - ah * 0.5f, w - 14.f + aw, h * 0.5f - ah * 0.5f, w - 14.f, h * 0.5f + ah * 0.5f);
    g.fillPath(p);

    box.setColour(juce::ComboBox::textColourId, Theme::textOnLight);
}

// helpers for assets
juce::File CustomLookAndFeel::findButtonsFolder()
{
    auto dir = juce::File::getSpecialLocation(juce::File::currentExecutableFile).getParentDirectory();
    for (int i = 0; i < 10; ++i)
    {
        auto candidate = dir.getChildFile("Assets").getChildFile("Buttons");
        if (candidate.isDirectory()) return candidate;
        dir = dir.getParentDirectory();
    }
    auto cwd = juce::File::getCurrentWorkingDirectory().getChildFile("Assets").getChildFile("Buttons");
    return cwd.isDirectory() ? cwd : juce::File{};
}

juce::Image CustomLookAndFeel::loadFromButtons(const juce::String& name)
{
    auto folder = findButtonsFolder();
    if (!folder.isDirectory()) return {};
    auto f = folder.getChildFile(name);
    if (!f.existsAsFile())     return {};
    if (auto in = f.createInputStream()) return juce::ImageFileFormat::loadFrom(*in);
    return {};
}

void CustomLookAndFeel::setSliderThumbImages(juce::Image defaultImg, juce::Image hoverImg, juce::Image pressedImg, int px)
{
    thumbDefault = defaultImg;
    thumbHover = hoverImg.isValid() ? hoverImg : defaultImg;
    thumbPressed = pressedImg.isValid() ? pressedImg : thumbHover;
    sliderThumbPixels = juce::jmax(8, px);
}

bool CustomLookAndFeel::setSliderThumbImagesFromBasename(const juce::String& base,
    const juce::String& defaultSuffix,
    const juce::String& hoverSuffix,
    const juce::String& pressedSuffix,
    int px)
{
    auto d = loadFromButtons(base + defaultSuffix);
    auto h = loadFromButtons(base + hoverSuffix);
    auto p = loadFromButtons(base + pressedSuffix);

    if (!d.isValid()) return false;
    setSliderThumbImages(d, h, p, px);
    return true;
}


// crisp pixel art
void CustomLookAndFeel::drawCrispImageAt(juce::Graphics& g, const juce::Image& img,
    int cx, int cy, int target)
{
    const int iw = img.getWidth(), ih = img.getHeight();
    if (iw <= 0 || ih <= 0) return;

    const int maxDim = juce::jmax(iw, ih);
    target = juce::jmax(8, target);

    if (target >= maxDim)
    {
        const int k = juce::jmax(1, target / maxDim);                  
        
        // integer upscale --> crisp
        g.setImageResamplingQuality(juce::Graphics::lowResamplingQuality);
        auto t = juce::AffineTransform::translation(-iw * 0.5f, -ih * 0.5f)
            .scaled((float)k, (float)k)
            .translated((float)cx, (float)cy);
        g.drawImageTransformed(img, t, false);
    }
    else
    {
        const float s = (float)target / (float)maxDim;               
        
        // downscale --> HQ
        g.setImageResamplingQuality(juce::Graphics::highResamplingQuality);
        auto t = juce::AffineTransform::translation(-iw * 0.5f, -ih * 0.5f)
            .scaled(s, s)
            .translated((float)cx, (float)cy);
        g.drawImageTransformed(img, t, false);
    }
}

// custom title bar
void CustomLookAndFeel::drawDocumentWindowTitleBar(
    juce::DocumentWindow& window,
    juce::Graphics& g,
    int w, int h,
    int titleSpaceX, int titleSpaceW,
    const juce::Image* icon,
    bool drawTitleTextOnLeft)
{
    // bg colour
    g.fillAll(Theme::trackBase); 

    // title text
    g.setColour(Theme::textOnDarkMain);
    g.setFont(juce::FontOptions(18.0f, juce::Font::bold));

    const int y = h / 2 + g.getCurrentFont().getHeight() / 3;
    if (titleSpaceW > 0)
        g.drawText(window.getName(), titleSpaceX, 0, titleSpaceW, h,
            juce::Justification::centredLeft, true);

    // draw close/min buttons differently
    auto* buttons = window.getCloseButton();
    if (buttons)
        buttons->setColour(juce::TextButton::buttonColourId, Theme::controlFill);
}

static juce::File findTitleButtonFolder()
{
    // go up to assets/buttons/window
    auto dir = juce::File::getSpecialLocation(juce::File::currentExecutableFile).getParentDirectory();

    for (int i = 0; i < 10; ++i)
    {
        auto cand = dir.getChildFile("Assets").getChildFile("Buttons").getChildFile("Window");
        if (cand.isDirectory()) return cand;

        dir = dir.getParentDirectory();
    }

    auto cwd = juce::File::getCurrentWorkingDirectory().getChildFile("Assets").getChildFile("Buttons").getChildFile("Window");
    return cwd.isDirectory() ? cwd : juce::File{};
}

static juce::Image loadImg(const juce::String& base, const juce::String& state)
{
    auto f = findTitleButtonFolder().getChildFile(base + state + ".png");

    if (auto in = f.createInputStream())
    {
        return juce::ImageFileFormat::loadFrom(*in);
    }

    return {};
}

// create custom buttons
juce::Button* CustomLookAndFeel::createDocumentWindowButton(int buttonType)
{
    auto* b = new juce::ImageButton();

    auto base = juce::String();
    if (buttonType == juce::DocumentWindow::closeButton) base = "title_close";
    else if (buttonType == juce::DocumentWindow::minimiseButton) base = "title_min";
    else if (buttonType == juce::DocumentWindow::maximiseButton) base = "title_max";
    else base = "title_min";

    // expects: base_default.png, base_hover.png, base_pressed.png
    juce::Image def = loadImg(base, "_default");
    juce::Image hov = loadImg(base, "_hover");
    juce::Image prs = loadImg(base, "_pressed");

    if (!def.isValid()) {
        // fallback fro single file
        def = loadImg(base, "");
        hov = def; prs = def;
    }
    else {
        if (!hov.isValid()) hov = def;
        if (!prs.isValid()) prs = hov;
    }

    b->setImages(true, true, true,
        def, 1.0f, juce::Colour(), // default
        hov, 1.0f, juce::Colour(), // hover
        prs, 1.0f, juce::Colour()); // pressed

    b->setMouseCursor(juce::MouseCursor::PointingHandCursor);
    return b; 
}
