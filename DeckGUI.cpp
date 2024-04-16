/*
  ==============================================================================

    DeckGUI.cpp
    Created: 31 Jan 2023 4:32:33pm
    Author:  cheng

  ==============================================================================
*/

#include "DeckGUI.h"
#include <iostream>
#include <fstream>

DeckGUI::DeckGUI(DJAudioPlayer* _player,
                 juce::AudioFormatManager& formatManagerToUse,
                 juce::AudioThumbnailCache& cacheToUse) :
                 player(_player),
                 waveformDisplay(formatManagerToUse, cacheToUse)
{
    // add and make visible labels, waveform display, sliders and buttons
    addAndMakeVisible(trackTitle);
    addAndMakeVisible(trackPosition);
    addAndMakeVisible(trackLength);
    addAndMakeVisible(waveformDisplay);
    addAndMakeVisible(posSlider);
    addAndMakeVisible(rewindButton);
    addAndMakeVisible(playPauseButton);
    addAndMakeVisible(stopButton);
    addAndMakeVisible(forwardButton);
    addAndMakeVisible(loopButton);
    addAndMakeVisible(volSlider);
    addAndMakeVisible(volSliderLabel);
    addAndMakeVisible(speedSlider);
    addAndMakeVisible(speedSliderLabel);

    // add listeners to sliders and buttons
    posSlider.addListener(this);
    rewindButton.addListener(this);
    playPauseButton.addListener(this);
    stopButton.addListener(this);
    forwardButton.addListener(this);
    loopButton.addListener(this);
    volSlider.addListener(this);
    speedSlider.addListener(this);

    // set style for trackTitle
    trackTitle.setFont(18.0);

    // set style of trackPosition
    trackPosition.setFont(17.0);
    trackPosition.setJustificationType(juce::Justification::centredRight);
    trackPosition.setText("--:--:--", juce::NotificationType::dontSendNotification);

    // set style of trackLength
    trackLength.setFont(17.0);
    trackLength.setJustificationType(juce::Justification::centredRight);
    trackLength.setText("--:--:--", juce::NotificationType::dontSendNotification);

    // set range of posSlider
    posSlider.setRange(0.0, 1.0);

    // set posSlider as transparent and overlay above the waveform display
    // this allows the user to click on the waveform display directly to adjust the track position
    posSlider.setSliderStyle(juce::Slider::LinearBar);
    posSlider.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
    posSlider.setColour(juce::Slider::trackColourId, juce::Colours::transparentWhite);

    // set images of buttons
    rewindButton.setImages(true, true, true, rewindImage, 0.5f, juce::Colours::transparentBlack, rewindImage, 1.0f, juce::Colours::transparentBlack, rewindImage, 0.5f, juce::Colours::transparentBlack);
    playPauseButton.setImages(true, true, true, playImage, 0.5f, juce::Colours::transparentBlack, playImage, 1.0f, juce::Colours::transparentBlack, playImage, 0.5f, juce::Colours::transparentBlack);
    stopButton.setImages(true, true, true, stopImage, 0.5f, juce::Colours::transparentBlack, stopImage, 1.0f, juce::Colours::transparentBlack, stopImage, 0.5f, juce::Colours::transparentBlack);
    forwardButton.setImages(true, true, true, forwardImage, 0.5f, juce::Colours::transparentBlack, forwardImage, 1.0f, juce::Colours::transparentBlack, forwardImage, 0.5f, juce::Colours::transparentBlack);
    loopButton.setImages(true, true, true, loopImage, 0.5f, juce::Colours::transparentBlack, loopImage, 1.0f, juce::Colours::transparentBlack, loopImage, 0.5f, juce::Colours::transparentBlack);

    // set range and values of volSlider
    volSlider.setRange(0.0, 2.0);
    volSlider.setValue(1.0);
    volSlider.setDoubleClickReturnValue(true, 1.0);

    // set style of volSlider
    volSlider.setSliderStyle(juce::Slider::Rotary);
    volSlider.setColour(juce::Slider::rotarySliderFillColourId, juce::Colour(11, 174, 244));
    volSlider.setColour(juce::Slider::rotarySliderOutlineColourId, juce::Colour(93, 96, 107));
    volSlider.setColour(juce::Slider::thumbColourId, juce::Colours::white);
    volSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 70, 20);
    volSlider.setColour(juce::Slider::textBoxOutlineColourId, juce::Colour(35, 47, 52));

    // attach label to volSlider
    volSliderLabel.setText("VOLUME", juce::NotificationType::dontSendNotification);
    volSliderLabel.attachToComponent(&volSlider, false);
    volSliderLabel.setJustificationType(juce::Justification::centred);

    // set range and values of speedSlider
    speedSlider.setRange(0.0, 2.0);
    speedSlider.setValue(1.0);
    speedSlider.setDoubleClickReturnValue(true, 1.0);

    // set style of speedSlider
    speedSlider.setSliderStyle(juce::Slider::Rotary);
    speedSlider.setColour(juce::Slider::rotarySliderFillColourId, juce::Colour(8, 227, 169));
    speedSlider.setColour(juce::Slider::rotarySliderOutlineColourId, juce::Colour(93, 96, 107));
    speedSlider.setColour(juce::Slider::thumbColourId, juce::Colours::white);
    speedSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 70, 20);
    speedSlider.setColour(juce::Slider::textBoxOutlineColourId, juce::Colour(35, 47, 52));

    // attach label to speedSlider
    speedSliderLabel.setText("SPEED", juce::NotificationType::dontSendNotification);
    speedSliderLabel.attachToComponent(&speedSlider, false);
    speedSliderLabel.setJustificationType(juce::Justification::centred);

    // timer loops every 10 milliseconds
    startTimer(10);
}

DeckGUI::~DeckGUI()
{
    stopTimer();
}

void DeckGUI::paint(juce::Graphics& g)
{
    // set background colour
    g.fillAll(juce::Colour(35, 47, 52));

    // draw border around deck
    g.setColour(juce::Colours::grey);
    g.drawRect(getLocalBounds(), 1);
}

void DeckGUI::resized()
{
    double rowH = getHeight() / 10;
    trackTitle.setBounds(0, 0, getWidth() * 0.75, rowH * 2);
    trackPosition.setBounds(getWidth() * 0.75, 0, getWidth() * 0.25, rowH);
    trackLength.setBounds(getWidth() * 0.75, rowH, getWidth() * 0.25, rowH);
    waveformDisplay.setBounds(0, rowH * 2, getWidth(), rowH * 3);
    posSlider.setBounds(0, rowH * 2, getWidth(), rowH * 3);
    stopButton.setBounds(0, rowH * 5.25, getWidth() * 0.2, rowH / 2);
    rewindButton.setBounds(getWidth() * 0.2, rowH * 5.25, getWidth() * 0.2, rowH / 2);
    playPauseButton.setBounds(getWidth() * 0.4, rowH * 5.25, getWidth() * 0.2, rowH / 2);
    forwardButton.setBounds(getWidth() * 0.6, rowH * 5.25, getWidth() * 0.2, rowH / 2);
    loopButton.setBounds(getWidth() * 0.8, rowH * 5.25, getWidth() * 0.2, rowH / 2);
    volSlider.setBounds(0, rowH * 6.75, getWidth() / 2, rowH * 3);
    speedSlider.setBounds(getWidth() / 2, rowH * 6.75, getWidth() / 2, rowH * 3);
}

void DeckGUI::buttonClicked(juce::Button* button)
{
    // if rewind button is clicked, -1 second to track position
    if (button == &rewindButton)
    {
        DBG("Rewind button was clicked");
        player->setPosition(player->getPosition() - 1);
    }

    // if play/pause button is clicked, start/stop the audio file accordingly
    if (button == &playPauseButton)
    {
        DBG("Play/pause button was clicked");

        if (player->isPlaying() && trackTitle.getText() != "") // pause track, change to play button
        {
            player->stop();
            playPauseButton.setImages(false, true, true, playImage, 0.5f, juce::Colours::transparentBlack, playImage, 1.0f, juce::Colours::transparentBlack, playImage, 0.5f, juce::Colours::transparentBlack);
        }

        else if (player->isPlaying() == false && trackTitle.getText() != "") // play track, change to pause button
        {
            player->start();
            playPauseButton.setImages(false, true, true, pauseImage, 0.5f, juce::Colours::transparentBlack, pauseImage, 1.0f, juce::Colours::transparentBlack, pauseImage, 0.5f, juce::Colours::transparentBlack);
        }
    }

    // if stop button is clicked, stop the audio file and set position relative to 0
    if (button == &stopButton)
    {
        DBG("Stop button was clicked");
        player->stop();
        player->setPositionRelative(0);
        playPauseButton.setImages(false, true, true, playImage, 0.5f, juce::Colours::transparentBlack, playImage, 1.0f, juce::Colours::transparentBlack, playImage, 0.5f, juce::Colours::transparentBlack);
    }

    // if forward button is clicked, +1 second to track position
    if (button == &forwardButton)
    {
        DBG("Forward button was clicked");
        player->setPosition(player->getPosition() + 1);
    }

    // if loop button is clicked, set isLooping to true/false accordingly
    if (button == &loopButton)
    {
        DBG("Loop button was clicked");

        if (isLooping && trackTitle.getText() != "")
        {
            isLooping = false;
            loopButton.setImages(false, true, true, loopImage, 0.5f, juce::Colours::transparentBlack, loopImage, 1.0f, juce::Colours::transparentBlack, loopImage, 0.5f, juce::Colours::transparentBlack);
        }

        else if (isLooping == false && trackTitle.getText() != "")
        {
            isLooping = true;
            loopButton.setImages(false, true, true, loopImage, 0.5f, juce::Colour(8, 227, 169), loopImage, 1.0f, juce::Colour(8, 227, 169), loopImage, 0.5f, juce::Colour(8, 227, 169));
        }
    }
}

void DeckGUI::sliderValueChanged(juce::Slider* slider)
{
    // if volume slider is moved, set volume to slider value
    if (slider == &volSlider)
    {
        DBG("Vol slider moved " << slider->getValue());
        player->setGain(slider->getValue());
    }

    // if speed slider is moved, set speed to slider value
    if (slider == &speedSlider)
    {
        DBG("Speed slider moved " << slider->getValue());
        player->setSpeed(slider->getValue());
    }

    // if position slider is moved, set position relative to slider value
    if (slider == &posSlider)
    {
        DBG("Pos slider moved " << slider->getValue());
        player->setPositionRelative(slider->getValue());
    }
}

void DeckGUI::timerCallback()
{
    // update position on trackPosition and waveform display
    if (trackTitle.getText() != "")
    {
        trackPosition.setText(formatTime(player->getPosition()), juce::NotificationType::dontSendNotification);
        waveformDisplay.setPositionRelative(player->getPositionRelative());
    }

    // when track reaches the end, check if loop is enabled
    if (player->getPositionRelative() >= 1)
    {
        if (isLooping)
        {
            player->setPositionRelative(0);
            player->start();
        }

        else if (isLooping == false)
        {
            player->setPositionRelative(0);
            player->stop();
            playPauseButton.setImages(false, true, true, playImage, 0.5f, juce::Colours::transparentBlack, playImage, 1.0f, juce::Colours::transparentBlack, playImage, 0.5f, juce::Colours::transparentBlack);
        }
    }
}

// load track onto deck
void DeckGUI::loadTrack(juce::String filePath)
{
    juce::File file = juce::File(filePath);
    player->loadURL(juce::URL(file));
    waveformDisplay.loadURL(juce::URL(file));
    trackTitle.setText(file.getFileName(), juce::NotificationType::dontSendNotification);
    trackPosition.setText(formatTime(player->getPosition()), juce::NotificationType::dontSendNotification);
    trackLength.setText(formatTime(player->getLength()), juce::NotificationType::dontSendNotification);
    playPauseButton.setImages(false, true, true, playImage, 0.5f, juce::Colours::transparentBlack, playImage, 1.0f, juce::Colours::transparentBlack, playImage, 0.5f, juce::Colours::transparentBlack);
    isLooping = false;
    loopButton.setImages(false, true, true, loopImage, 0.5f, juce::Colours::transparentBlack, loopImage, 1.0f, juce::Colours::transparentBlack, loopImage, 0.5f, juce::Colours::transparentBlack);
    volSlider.setValue(1.0);
    speedSlider.setValue(1.0);
}

// format time from seconds to hh:mm:ss
juce::String DeckGUI::formatTime(double time)
{
    juce::String seconds, minutes, hours;
    if (int(time) % 60 < 10) seconds = "0" + juce::String(int(time) % 60);
    else seconds = juce::String(int(time) % 60);
    if (int(time) / 60 % 60 < 10) minutes = "0" + juce::String(int(time) / 60 % 60);
    else minutes = juce::String(int(time) / 60 % 60);
    if (int(time) / 3600 < 10) hours = "0" + juce::String(int(time) / 3600);
    else hours = juce::String(int(time) / 3600);

    return hours + ":" + minutes + ":" + seconds;
}

// clear deck when track is deleted from playlist
void DeckGUI::clearDeck()
{
    trackTitle.setText("", juce::NotificationType::dontSendNotification);
    trackPosition.setText("--:--:--", juce::NotificationType::dontSendNotification);
    trackLength.setText("--:--:--", juce::NotificationType::dontSendNotification);
    waveformDisplay.fileLoaded = false;
    waveformDisplay.repaint();
    player->stop();
    playPauseButton.setImages(false, true, true, playImage, 0.5f, juce::Colours::transparentBlack, playImage, 1.0f, juce::Colours::transparentBlack, playImage, 0.5f, juce::Colours::transparentBlack);
    isLooping = false;
    loopButton.setImages(false, true, true, loopImage, 0.5f, juce::Colours::transparentBlack, loopImage, 1.0f, juce::Colours::transparentBlack, loopImage, 0.5f, juce::Colours::transparentBlack);
    volSlider.setValue(1.0);
    speedSlider.setValue(1.0);
}

std::vector<double> DeckGUI::getSliderValues()
{
    std::vector<double> sliderValues;

    sliderValues.push_back(posSlider.getValue());
    sliderValues.push_back(volSlider.getValue());
    sliderValues.push_back(speedSlider.getValue());

    return sliderValues;
}

void DeckGUI::setSliderValues(double position, double volume, double speed)
{
    posSlider.setValue(position);
    volSlider.setValue(volume);
    speedSlider.setValue(speed);
}