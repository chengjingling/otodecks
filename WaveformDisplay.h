/*
  ==============================================================================

    WaveformDisplay.h
    Created: 4 Feb 2023 8:29:34pm
    Author:  cheng

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

class WaveformDisplay : public juce::Component,
                        public juce::ChangeListener
{
public:
    WaveformDisplay(juce::AudioFormatManager& formatManagerToUse,
                    juce::AudioThumbnailCache& cacheToUse);
    ~WaveformDisplay() override;

    void paint(juce::Graphics&) override;
    void resized() override;

    void loadURL(juce::URL audioURL);

    void changeListenerCallback(juce::ChangeBroadcaster* source) override;

    void setPositionRelative(double pos);

    bool fileLoaded;

private:
    juce::AudioThumbnail audioThumb;
    double position;
    juce::Image markerImage{ juce::ImageFileFormat::loadFrom(BinaryData::marker2_png, BinaryData::marker2_pngSize) };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WaveformDisplay)
};