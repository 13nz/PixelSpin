/*
  ==============================================================================

    PlaylistComponent.cpp
    Created: 1 May 2025 1:51:39pm
    Author:  Lena

  ==============================================================================
*/

#include <JuceHeader.h>
#include "PlaylistComponent.h"
#include "DJAudioPlayer.h"
#include "DeckGUI.h"
#include "Theme.h"


//==============================================================================
PlaylistComponent::PlaylistComponent(DJAudioPlayer& targetPlayer, DeckGUI& targetDeckGUI, juce::AudioFormatManager& fmt, juce::String playId) : player(targetPlayer), deckGUI(targetDeckGUI), formatManager(fmt), playlistId(playId)
{
    tableComponent.getHeader().addColumn("Track title", 1, 400);
    tableComponent.getHeader().addColumn("Length", 2, 90);
    tableComponent.getHeader().addColumn("", 3, 100); // Load



    // set model on table component
    tableComponent.setModel(this);

    addAndMakeVisible(tableComponent);
}

PlaylistComponent::~PlaylistComponent()
{
}

void PlaylistComponent::paint (juce::Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));   // clear the background

    g.setColour (juce::Colours::grey);
    g.drawRect (getLocalBounds(), 1);   // draw an outline around the component

    g.setColour (juce::Colours::white);
    g.setFont (juce::FontOptions (14.0f));
    g.drawText ("PlaylistComponent", getLocalBounds(),
                juce::Justification::centred, true);   // draw some placeholder text
}

void PlaylistComponent::resized()
{
    auto r = getLocalBounds();

    tableComponent.setBounds(r);
}


int PlaylistComponent::getNumRows()
{
    return tracks.size();
}

void PlaylistComponent::paintRowBackground(
    juce::Graphics& g, 
    int rowNumber, 
    int width, 
    int height, 
    bool rowIsSelected
)
{
    g.fillAll(rowIsSelected ? Theme::accent.withAlpha(0.22f)
        : Theme::panelBg);
    g.setColour(Theme::panelOutline());
    g.drawLine(0.f, (float)height - 0.5f, (float)width, (float)height - 0.5f);
}

void PlaylistComponent::paintCell(
    juce::Graphics& g, 
    int rowNumber, 
    int columnId, 
    int width, 
    int height, 
    bool rowIsSelected
)
{
    if (rowNumber < 0 || rowNumber >= (int)tracks.size()) return;

    const auto& t = tracks[(size_t)rowNumber];
    juce::String text;

    if (columnId == 1)
    {
        text = t.title;
    }
    else if (columnId == 2)
    {
        text = formatSeconds(t.lengthSeconds);
    }

    if (text.isNotEmpty())
    {
        g.drawText(text, 2, 0, width - 4, height, juce::Justification::centredLeft, true);
    }

    g.setColour(Theme::textOnDarkMain);
    g.drawText(text, 6, 0, width - 12, height, juce::Justification::centredLeft, true);

}

juce::Component* PlaylistComponent::refreshComponentForCell(int rowNumber, int columnId, bool isRowSelected, juce::Component* existingComponentToUpdate)
{
    if (columnId == 2)
    {
        if (existingComponentToUpdate == nullptr)
        {
            juce::TextButton* btn = new juce::TextButton{ "play" };
            juce::String id{ std::to_string(rowNumber) };
            btn->setComponentID(id);
            btn->addListener(this);
            existingComponentToUpdate = btn;
        }
    }
    return existingComponentToUpdate;
}

void PlaylistComponent::buttonClicked(juce::Button* button)
{
    const int row = button->getComponentID().getIntValue();
    playRow(row);

}


void PlaylistComponent::addTrackFromFile(const juce::File& file)
{
    if (!file.existsAsFile())
    {
        return;
    }

    Track t;
    t.file = file;
    t.title = file.getFileNameWithoutExtension();

    // create file reader
    std::unique_ptr<juce::AudioFormatReader> reader(formatManager.createReaderFor(file));

    if (reader != nullptr)
    {
        t.lengthSeconds = (double)reader->lengthInSamples / reader->sampleRate;
    }

    tracks.push_back(std::move(t));

}

juce::String PlaylistComponent::formatSeconds(double total) const
{
    if (total <= 0.0)
    {
        return "--:--";
    }

    int secs = (int)std::round(total);
    int mins = secs / 60;
    secs %= 60;

    return juce::String(mins) + ":" + juce::String(secs).paddedLeft('0', 2);
}

void PlaylistComponent::addFiles(const juce::Array<juce::File>& files)
{
    for (auto f : files)
    {
        addTrackFromFile(f);
    }

    tableComponent.updateContent();
    repaint();
}

void PlaylistComponent::cellDoubleClicked(int rowNumber, int columnId, const juce::MouseEvent&)
{
    playRow(rowNumber);
}

void PlaylistComponent::playRow(int row)
{
    if (row < 0 || row >= (int)tracks.size()) return;

    juce::URL url{ tracks[(size_t)row].file };
    player.loadURL(url);
    deckGUI.showWaveForm(url);
    player.start();
}

// save user library
void PlaylistComponent::saveLibrary() const
{
    juce::Array<juce::var> items;

    // loop through tracks, create object, store in array
    for (const auto& t : tracks)
    {
        auto* obj = new juce::DynamicObject();
        obj->setProperty("path", t.file.getFullPathName());
        obj->setProperty("title", t.title);
        obj->setProperty("length", t.lengthSeconds);

        items.add(juce::var(obj));
    }

    // save as JSON string in one line
    juce::String json = juce::JSON::toString(items, true);
    auto file = getLibraryFile();
    file.replaceWithText(json);
}

// load library from JSON
void PlaylistComponent::loadLibrary()
{
    // clear tracks
    tracks.clear();

    auto file = getLibraryFile();

    // if empty reload UI
    if (!file.existsAsFile())
    {
        tableComponent.updateContent();
        repaint();
        return;
    }

    juce::var parsed = juce::JSON::parse(file);
    if (!parsed.isArray()) return;

    auto* arr = parsed.getArray();

    if (arr == nullptr)
    {
        tableComponent.updateContent();
        repaint();
        return;
    }

    // loop through array and create tracks
    for (int i = 0; i < arr->size(); i++)
    {
        const juce::var& v = (*arr)[i];

        if (!v.isObject()) continue;

        auto* obj = v.getDynamicObject();

        if (obj)
        {
            juce::File f = obj->getProperty("path").toString();
            juce::String title = obj->getProperty("title").toString();
            double len = 0.0;

            if (obj->hasProperty("length"))
            {
                len = static_cast<double>(obj->getProperty("length"));
            }

            // skip missing files
            if (!f.existsAsFile()) continue;

            Track t;
            t.file = f;
            t.title = title.isNotEmpty() ? title : f.getFileNameWithoutExtension();
            t.lengthSeconds = len;

            tracks.push_back(std::move(t));
        }
    }

    tableComponent.updateContent();
    repaint();
}

// get library file
juce::File PlaylistComponent::getLibraryFile() const
{
    auto dir = juce::File::getSpecialLocation(juce::File::userApplicationDataDirectory).getChildFile(juce::String(ProjectInfo::projectName));

    dir.createDirectory();
    const juce::String filename = juce::String("library_") + playlistId + ".json";

    return dir.getChildFile(filename);
}

// clear playlist (called by DeckGUI)
void PlaylistComponent::clearAll()   
{
    tracks.clear();
    tableComponent.updateContent();
    repaint();
    saveLibrary();  
}