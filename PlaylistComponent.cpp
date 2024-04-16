/*
  ==============================================================================

    PlaylistComponent.cpp
    Created: 6 Feb 2023 2:35:01pm
    Author:  cheng

  ==============================================================================
*/

#include "PlaylistComponent.h"
#include <iostream>
#include <fstream>

PlaylistComponent::PlaylistComponent(DeckGUI* _deckGUI1, 
                                     DeckGUI* _deckGUI2) :
                                     deckGUI1(_deckGUI1),
                                     deckGUI2(_deckGUI2)
{
    // add and make visible buttons, search bar and table component
    addAndMakeVisible(loadAButton);
    addAndMakeVisible(clearAButton);
    addAndMakeVisible(searchBar);
    addAndMakeVisible(loadBButton);
    addAndMakeVisible(clearBButton);
    addAndMakeVisible(tableComponent);
    addAndMakeVisible(importButton);
    addAndMakeVisible(deleteButton);

    // add listeners to buttons and search bar
    loadAButton.addListener(this);
    clearAButton.addListener(this);
    searchBar.addListener(this);
    loadBButton.addListener(this);
    clearBButton.addListener(this);
    importButton.addListener(this);
    deleteButton.addListener(this);

    // set model used for table component
    tableComponent.setModel(this);

    tableComponent.setMultipleSelectionEnabled(true);

    // add columns to table component
    tableComponent.getHeader().addColumn("TITLE", 1, 450);
    tableComponent.getHeader().addColumn("LENGTH", 2, 158);
    tableComponent.getHeader().addColumn("LOCATION", 3, 750);

    // table is sorted by track title when app is run
    tableComponent.getHeader().setSortColumnId(1, true);

    // register basic formats for the audio files
    formatManager.registerBasicFormats();

    // read from files to retrieve previously loaded tracks
    readFromDeckFile();
    readFromPlaylistFile();

    // set style of search bar
    searchBar.setText("Search for tracks...", juce::NotificationType::dontSendNotification);
    searchBar.setSelectAllWhenFocused(true);
    searchBar.setIndents(searchBar.getLeftIndent(), 0);
    searchBar.setJustification(juce::Justification::centredLeft);
    searchBar.applyFontToAllText(juce::Font("Avenir LT Std", 18.0, 0));
}

PlaylistComponent::~PlaylistComponent()
{
    // write most recent versions of decks and playlist to their respective files
    writeToDeckFile();
    writeToPlaylistFile();
}

void PlaylistComponent::paint(juce::Graphics& g)
{
    // set background colour
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));

    // draw border around playlist
    g.setColour(juce::Colours::grey);
    g.drawRect(getLocalBounds(), 1);
}

void PlaylistComponent::resized()
{
    double rowH = getHeight() / 10;
    loadAButton.setBounds(0, 0, getWidth() * 1/7, rowH);
    clearAButton.setBounds(getWidth() * 1/7, 0, getWidth() * 1/7, rowH);
    searchBar.setBounds(getWidth() * 2/7, 0, getWidth() * 3/7, rowH);
    loadBButton.setBounds(getWidth() * 5/7, 0, getWidth() * 1/7, rowH);
    clearBButton.setBounds(getWidth() * 6/7, 0, getWidth() * 1/7, rowH);
    tableComponent.setBounds(0, rowH, getWidth(), rowH * 8);
    importButton.setBounds(0, rowH * 9, getWidth() / 2, rowH);
    deleteButton.setBounds(getWidth() / 2, rowH * 9, getWidth() / 2, rowH);
}

// get number of rows in table component by getting number of tracks in playlist
int PlaylistComponent::getNumRows()
{
    return existingFiles.size();
}

void PlaylistComponent::paintRowBackground(juce::Graphics& g,
    int rowNumber,
    int width,
    int height,
    bool rowIsSelected)
{
    // if playlist is in search mode, paint rows containing search text
    if (searchBar.getText() != "Search for tracks...")
    {
        if (std::find(searchedRows.begin(), searchedRows.end(), rowNumber) != searchedRows.end())
        {
            g.fillAll(juce::Colour(58, 185, 248));
        }

        else
        {
            g.fillAll(juce::Colour(92, 100, 102));
        }

        // deselect all rows when in search mode
        tableComponent.deselectAllRows();
    }
    
    // if playlist is in selection mode, paint selected row
    else
    {
        if (importedTracks.size() > 0) // if the user has imported tracks
        {
            // deselect the current selected rows
            tableComponent.deselectAllRows();

            // select imported rows
            for (juce::String track : importedTracks)
            {
                tableComponent.selectRow(std::find(existingFiles.begin(), existingFiles.end(), track) - existingFiles.begin(), true, false);
            }

            // clear importedTracks vector
            importedTracks.clear();
        }

        if (rowIsSelected)
        {
            g.fillAll(juce::Colour(58, 248, 197));
        }

        else
        {
            g.fillAll(juce::Colour(92, 100, 102));
        }
    }
}

void PlaylistComponent::paintCell(juce::Graphics& g,
    int rowNumber,
    int columnId,
    int width,
    int height,
    bool rowIsSelected)
{
    // set font size
    g.setFont(16);

    // populate column 1 with track titles
    if (columnId == 1)
    {
        g.drawText(trackTitles[rowNumber],
            2,
            0,
            width - 4,
            height,
            juce::Justification::centredLeft,
            true);
    }
    
    // populate column 2 with track lengths
    else if (columnId == 2)
    {
        g.drawText(trackLengths[rowNumber],
            2,
            0,
            width - 4,
            height,
            juce::Justification::centredLeft,
            true);
    }

    // populate column 3 with track locations
    else
    {
        g.drawText(existingFiles[rowNumber],
            2,
            0,
            width - 4,
            height,
            juce::Justification::centredLeft,
            true);
    }
}

juce::Component* PlaylistComponent::refreshComponentForCell(int rowNumber,
    int columnId,
    bool rowIsSelected,
    juce::Component* existingComponentToUpdate)
{
    return existingComponentToUpdate;
}

void PlaylistComponent::buttonClicked(juce::Button* button)
{
    if (button == &loadAButton)
    {
        DBG("Load A button was clicked");

        // if only 1 row is selected, load deck 1
        if (tableComponent.getSelectedRows().size() == 1)
        {
            deckGUI1->loadTrack(existingFiles[tableComponent.getSelectedRows()[0]]);
            existingDecks["1"] = existingFiles[tableComponent.getSelectedRows()[0]];
        }

        // if multiple rows are selected, display an alert window
        else if (tableComponent.getSelectedRows().size() > 1)
        {
            juce::AlertWindow loadError("Unable to load files", "Cannot load multiple files!", juce::MessageBoxIconType::InfoIcon);
            loadError.addButton("OK", true);
            loadError.runModalLoop();
        }
    }

    // if clear A button is clicked, clear deck 1
    if (button == &clearAButton)
    {
        DBG("Clear A button was clicked");
        deckGUI1->clearDeck();
        existingDecks.erase("1");
    }

    if (button == &loadBButton)
    {
        DBG("Load B button was clicked");

        // if only 1 row is selected, load deck 2
        if (tableComponent.getSelectedRows().size() == 1)
        {
            deckGUI2->loadTrack(existingFiles[tableComponent.getSelectedRows()[0]]);
            existingDecks["2"] = existingFiles[tableComponent.getSelectedRows()[0]];
        }

        // if multiple rows are selected, display an alert window
        else if (tableComponent.getSelectedRows().size() > 1)
        {
            juce::AlertWindow loadError("Unable to load files", "Cannot load multiple files!", juce::MessageBoxIconType::InfoIcon);
            loadError.addButton("OK", true);
            loadError.runModalLoop();
        }
    }

    // if clear B button is clicked, clear deck 2
    if (button == &clearBButton)
    {
        DBG("Clear B button was clicked");
        deckGUI2->clearDeck();
        existingDecks.erase("2");
    }

    if (button == &importButton)
    {
        DBG("Import button was clicked");
        juce::FileChooser chooser{ "Select files..." };

        if (chooser.browseForMultipleFilesToOpen())
        {
            for (juce::File file : chooser.getResults())
            {
                // if file already exists, display an alert window
                if (std::count(existingFiles.begin(), existingFiles.end(), file.getFullPathName()))
                {
                    DBG("Element found");
                    juce::AlertWindow alertDuplicate("Duplicate file", file.getFileName() + " already exists!", juce::MessageBoxIconType::InfoIcon);
                    alertDuplicate.addButton("OK", true);
                    alertDuplicate.runModalLoop();
                }

                // if file does not exist, add to playlist and highlight imported rows
                else
                {
                    DBG("Element not found");
                    existingFiles.push_back(file.getFullPathName());
                    importTrack(file);
                    importedTracks.push_back(file.getFullPathName());
                }
            }
        }
    }

    if (button == &deleteButton)
    {
        DBG("Delete button was clicked");
        deleteTrack();
    }
}

// when search text changes, update search results
void PlaylistComponent::textEditorTextChanged(juce::TextEditor& editor)
{
    searchedRows.clear();

    for (int i = 0; i < trackTitles.size(); i++)
    {
        if (trackTitles[i].containsIgnoreCase(searchBar.getText()) && searchBar.getText() != "")
        {
            searchedRows.push_back(i);
        }
    }

    repaint();
}

// when user clicks out of search bar, set placeholder text
void PlaylistComponent::textEditorFocusLost(juce::TextEditor& editor)
{
    searchBar.setText("Search for tracks...");
}

// function is called when user clicks on a column header or when setSortColumnId() is called
void PlaylistComponent::sortOrderChanged(int newSortColumnId, bool isForwards)
{
    if (existingFiles.size() > 1)
    {
        if (newSortColumnId == 1 && isForwards) // sort forwards by track title
        {
            for (int i = 0; i < trackTitles.size() - 1; i++)
            {
                for (int j = 0; j < trackTitles.size() - 1 - i; j++)
                {
                    if (trackTitles[j] > trackTitles[j + 1])
                    {
                        std::swap(trackTitles[j], trackTitles[j + 1]);
                        std::swap(trackLengths[j], trackLengths[j + 1]);
                        std::swap(existingFiles[j], existingFiles[j + 1]);
                    }
                }
            }
        }

        else if (newSortColumnId == 1 && isForwards == false) // sort backwards by track title
        {
            for (int i = 0; i < trackTitles.size() - 1; i++)
            {
                for (int j = 0; j < trackTitles.size() - 1 - i; j++)
                {
                    if (trackTitles[j] < trackTitles[j + 1])
                    {
                        std::swap(trackTitles[j], trackTitles[j + 1]);
                        std::swap(trackLengths[j], trackLengths[j + 1]);
                        std::swap(existingFiles[j], existingFiles[j + 1]);
                    }
                }
            }
        }

        else if (newSortColumnId == 2 && isForwards) // sort forwards by track length
        {
            for (int i = 0; i < trackLengths.size() - 1; i++)
            {
                for (int j = 0; j < trackLengths.size() - 1 - i; j++)
                {
                    if (trackLengths[j] > trackLengths[j + 1])
                    {
                        std::swap(trackTitles[j], trackTitles[j + 1]);
                        std::swap(trackLengths[j], trackLengths[j + 1]);
                        std::swap(existingFiles[j], existingFiles[j + 1]);
                    }
                }
            }
        }

        else if (newSortColumnId == 2 && isForwards == false) // sort backwards by track length
        {
            for (int i = 0; i < trackLengths.size() - 1; i++)
            {
                for (int j = 0; j < trackLengths.size() - 1 - i; j++)
                {
                    if (trackLengths[j] < trackLengths[j + 1])
                    {
                        std::swap(trackTitles[j], trackTitles[j + 1]);
                        std::swap(trackLengths[j], trackLengths[j + 1]);
                        std::swap(existingFiles[j], existingFiles[j + 1]);
                    }
                }
            }
        }

        else if (newSortColumnId == 3 && isForwards) // sort forwards by track location
        {
            for (int i = 0; i < existingFiles.size() - 1; i++)
            {
                for (int j = 0; j < existingFiles.size() - 1 - i; j++)
                {
                    if (existingFiles[j] > existingFiles[j + 1])
                    {
                        std::swap(trackTitles[j], trackTitles[j + 1]);
                        std::swap(trackLengths[j], trackLengths[j + 1]);
                        std::swap(existingFiles[j], existingFiles[j + 1]);
                    }
                }
            }
        }

        else if (newSortColumnId == 3 && isForwards == false) // sort backwards by track location
        {
            for (int i = 0; i < existingFiles.size() - 1; i++)
            {
                for (int j = 0; j < existingFiles.size() - 1 - i; j++)
                {
                    if (existingFiles[j] < existingFiles[j + 1])
                    {
                        std::swap(trackTitles[j], trackTitles[j + 1]);
                        std::swap(trackLengths[j], trackLengths[j + 1]);
                        std::swap(existingFiles[j], existingFiles[j + 1]);
                    }
                }
            }
        }
    }

    // deselect all rows when sort order is changed
    tableComponent.deselectAllRows();

    // refresh table component
    tableComponent.updateContent();
}

// allow file drag in playlist
bool PlaylistComponent::isInterestedInFileDrag(const juce::StringArray& files)
{
    DBG("PlaylistComponent::isInterestedInFileDrag");
    return true;
}

void PlaylistComponent::filesDropped(const juce::StringArray& files, int x, int y)
{
    DBG("PlaylistComponent::filesDropped");

    for (juce::String filePath : files)
    {
        // if file already exists, display an alert window
        if (std::count(existingFiles.begin(), existingFiles.end(), filePath))
        {
            DBG("Element found");
            juce::AlertWindow alertDuplicate("Duplicate file", juce::File(filePath).getFileName() + " already exists!", juce::MessageBoxIconType::InfoIcon);
            alertDuplicate.addButton("OK", true);
            alertDuplicate.runModalLoop();
        }

        // if file does not exist, add to playlist and highlight imported rows
        else
        {
            DBG("Element not found");
            existingFiles.push_back(filePath);
            importTrack(juce::File(filePath));
            importedTracks.push_back(filePath);
        }
    }
}

void PlaylistComponent::readFromDeckFile()
{
    // open deck.txt which contains the deck numbers and file paths which the user had loaded previously
    std::ifstream deckFile("deck.txt");
    std::string line;
    std::vector<std::string> lines;

    while (std::getline(deckFile, line))
    {
        lines.push_back(line);
    }

    for (int i = 0; i < lines.size(); i += 5)
    {
        // read previously loaded tracks from deck file to existingDecks
        existingDecks[lines[i]] = lines[i + 1];
    }

    // close the file
    deckFile.close();
}

void PlaylistComponent::writeToDeckFile()
{
    // open deck.txt which contains the deck numbers and file paths which the user had loaded previously
    std::ofstream deckFile("deck.txt");
    std::map<juce::String, juce::String>::iterator it;

    for (it = existingDecks.begin(); it != existingDecks.end(); it++)
    {
        std::vector<double> sliderValues;

        // get current slider values of the deck
        if (it->first == "1")
        {
            sliderValues = deckGUI1->getSliderValues();
        }

        else
        {
            sliderValues = deckGUI2->getSliderValues();
        }

        // write all details of the deck to deck.txt
        deckFile << it->first << std::endl << it->second << std::endl << sliderValues[0] << std::endl << sliderValues[1] << std::endl << sliderValues[2] << std::endl;
    }

    // close the file
    deckFile.close();
}

void PlaylistComponent::readFromPlaylistFile()
{
    // open playlist.txt which contains the file paths which the user had imported previously
    std::ifstream playlistFile("playlist.txt");
    std::string filePath;

    while (std::getline(playlistFile, filePath))
    {
        // read previously imported tracks from playlist file to existingFiles
        existingFiles.push_back(filePath);

        // import tracks into playlist
        importTrack(juce::File(filePath));
    }

    // close the file
    playlistFile.close();
}

void PlaylistComponent::writeToPlaylistFile()
{
    // open playlist.txt which contains the file paths which the user had imported previously
    std::ofstream playlistFile("playlist.txt");

    for (int i = 0; i < existingFiles.size(); i++) {
        // write most recent version of playlist from existingFiles to playlist file
        playlistFile << existingFiles[i] << std::endl;
    }

    // close the file
    playlistFile.close();
}

void PlaylistComponent::importTrack(juce::File file)
{
    // add new file name to trackTitles
    trackTitles.push_back(file.getFileName());

    // get duration of new file in seconds
    juce::AudioFormatReader* reader = formatManager.createReaderFor(file);
    int duration = reader->lengthInSamples / reader->sampleRate;

    // format duration from seconds to hh:mm:ss
    juce::String seconds, minutes, hours;
    if (duration % 60 < 10) seconds = "0" + juce::String(duration % 60);
    else seconds = juce::String(duration % 60);
    if (duration / 60 % 60 < 10) minutes = "0" + juce::String(duration / 60 % 60);
    else minutes = juce::String(duration / 60 % 60);
    if (duration / 3600 < 10) hours = "0" + juce::String(duration / 3600);
    else hours = juce::String(duration / 3600);

    // add duration to trackLengths
    trackLengths.push_back(hours + ":" + minutes + ":" + seconds);

    // re-sort table so that new track appears in the correct row of the playlist
    tableComponent.getHeader().setSortColumnId(tableComponent.getHeader().getSortColumnId(), tableComponent.getHeader().isSortedForwards());
    tableComponent.getHeader().reSortTable();

    // refresh table component
    tableComponent.updateContent();
}

void PlaylistComponent::deleteTrack()
{
    if (tableComponent.getSelectedRows().size() > 0)
    {
        // display an alert window to confirm if the user wants to delete selected tracks from playlist
        juce::AlertWindow confirmDelete("Delete file", "Are you sure you want to delete these track(s)?", juce::MessageBoxIconType::QuestionIcon);
        confirmDelete.addButton("Delete", true);
        confirmDelete.addButton("Cancel", false);

        if (confirmDelete.runModalLoop()) // if user clicks delete
        {
            std::vector<juce::String> decksToClear;

            // loop through the selected rows and delete them from the back, so as to not mess up the indexes
            for (int i = tableComponent.getSelectedRows().size() - 1; i >= 0; i--)
            {
                // erase track from trackTitles and trackLengths
                trackTitles.erase(trackTitles.begin() + tableComponent.getSelectedRows()[i]);
                trackLengths.erase(trackLengths.begin() + tableComponent.getSelectedRows()[i]);

                juce::String deletedFilePath = existingFiles[tableComponent.getSelectedRows()[i]];
                std::map<juce::String, juce::String>::iterator it;

                // check existingDecks if deleted track has been loaded into the decks
                for (it = existingDecks.begin(); it != existingDecks.end(); it++)
                {
                    if (it->second == deletedFilePath)
                    {
                        decksToClear.push_back(it->first);
                    }
                }

                // erase track from existingFiles
                existingFiles.erase(existingFiles.begin() + tableComponent.getSelectedRows()[i]);
            }
            
            for (juce::String deckNumber : decksToClear)
            {
                // erase track from existingDecks
                existingDecks.erase(deckNumber);
                
                // clear decks where deleted track has been loaded
                if (deckNumber == "1")
                {
                    deckGUI1->clearDeck();
                }

                else
                {
                    deckGUI2->clearDeck();
                }
            }

            // deselect all rows after tracks have been deleted
            tableComponent.deselectAllRows();
            
            // refresh table component
            tableComponent.updateContent();
        }
        
        else // if user clicks cancel
        {
            // exit the alert window and do nothing
            confirmDelete.exitModalState(true);
        }
    }
}