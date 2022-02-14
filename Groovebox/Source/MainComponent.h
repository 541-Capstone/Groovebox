#pragma once

#include <JuceHeader.h>
#include <vector>

//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainComponent  : public juce::AudioAppComponent, public juce::Timer
{
public:
    //==============================================================================
    MainComponent();
    ~MainComponent() override;

    //==============================================================================
    void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;

    //==============================================================================
    void paint (juce::Graphics& g) override;
    void resized() override;

    void timerCallback() override;

    enum class PlayState {
        Play,
        Stop,
        Record
    };

private:
    //==============================================================================
    // Your private member variables go here...
    tracktion_engine::Engine engine { ProjectInfo::projectName };
    int numTracks;
    int numAudioTracks;
    
    //tracktion_engine::Track* currentTrack;
    std::vector<tracktion_engine::Track*> audioTrackList;
    int currentTrack;

    tracktion_engine::Edit edit{ engine,
                                 tracktion_engine::createEmptyEdit(engine),
                                 tracktion_engine::Edit::EditRole::forEditing,
                   nullptr,
                   0 };

    juce::TextButton playBtn{ "Play" };
    juce::TextButton stopBtn{ "Stop" };
    juce::TextButton addTrackBtn{ "Add Track" };
    juce::TextButton leftBtn{ "<<<" };
    juce::TextButton rightBtn{ ">>>" };


    juce::Label statusLbl{ "statusLbl", "Status" };
    juce::Label trackCountLbl{ "trackCountLbl", "Track count" };

    PlayState playState = PlayState::Stop;

    void play();
    void stop();
    void record();
    void addTrack();
    void nextTrack();
    void prevTrack();


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
