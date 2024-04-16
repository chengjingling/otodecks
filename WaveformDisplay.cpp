/*
  ==============================================================================

    WaveformDisplay.cpp
    Created: 4 Feb 2023 8:29:34pm
    Author:  cheng

  ==============================================================================
*/

#include "WaveformDisplay.h"

WaveformDisplay::WaveformDisplay(juce::AudioFormatManager & formatManagerToUse,
                                 juce::AudioThumbnailCache & cacheToUse) :
                                 audioThumb(1000, formatManagerToUse, cacheToUse),
                                 fileLoaded(false),
                                 position(0)
{
    // add change listener to audio thumbnail
    audioThumb.addChangeListener(this);
}

WaveformDisplay::~WaveformDisplay()
{

}

void WaveformDisplay::paint(juce::Graphics& g)
{
    // set background colour
    g.fillAll(juce::Colour(35, 47, 52));

    // draw border around waveform display
    g.setColour(juce::Colours::grey);
    g.drawRect(getLocalBounds(), 1);

    // set waveform colour
    g.setColour(juce::Colour(167, 172, 174));

    // if file loaded, draw waveform
    if (fileLoaded)
    {
        audioThumb.drawChannel(g, getLocalBounds(), 0, audioThumb.getTotalLength(), 0, 1.0f);
        g.drawImageWithin(markerImage, position * getWidth(), -20, 10, getHeight() + 40, juce::RectanglePlacement::fillDestination);
    }

    // else, display "NO TRACK LOADED"
    else
    {
        g.setColour(juce::Colours::white);
        g.setFont(17);
        g.drawText("NO TRACK LOADED", getLocalBounds(),
            juce::Justification::centred, true);
    }
}

void WaveformDisplay::resized()
{

}

// load waveform display
void WaveformDisplay::loadURL(juce::URL audioURL)
{
    audioThumb.clear();
    fileLoaded = audioThumb.setSource(new juce::URLInputSource(audioURL));
}

// update waveform display when new track is loaded
void WaveformDisplay::changeListenerCallback(juce::ChangeBroadcaster* source)
{
    repaint();
}

// set position relative of waveform display
void WaveformDisplay::setPositionRelative(double pos)
{
    if (pos != position)
    {
        position = pos;
        repaint();
    }
}