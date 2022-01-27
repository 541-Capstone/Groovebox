#include "MainComponent.h"

//==============================================================================
MainComponent::MainComponent()
{

    addAndMakeVisible(playBtn);
    addAndMakeVisible(stopBtn);
    addAndMakeVisible(addTrackBtn);

    addAndMakeVisible(statusLbl);
    addAndMakeVisible(trackCountLbl);

    startTimer(250);


    playBtn.onClick = [this]() { play(); };
    stopBtn.onClick = [this]() { stop(); };
    addTrackBtn.onClick = [this]() {addTrack(); };

    // Make sure you set the size of the component after
    // you add any child components.
    setSize (600, 300);

    // Some platforms require permissions to open input channels so request that here
    if (juce::RuntimePermissions::isRequired (juce::RuntimePermissions::recordAudio)
        && ! juce::RuntimePermissions::isGranted (juce::RuntimePermissions::recordAudio))
    {
        juce::RuntimePermissions::request (juce::RuntimePermissions::recordAudio,
                                           [&] (bool granted) { setAudioChannels (granted ? 2 : 0, 2); });
    }
    else
    {
        // Specify the number of input and output channels that we want to open
        setAudioChannels (2, 2);
    }

    //auto tracks = edit.trackCache;
    //for(int i = 0; i < tracks.)
}

MainComponent::~MainComponent()
{
    // This shuts down the audio device and clears the audio source.
    shutdownAudio();
}

void MainComponent::timerCallback() {
    repaint();
}

//==============================================================================
void MainComponent::prepareToPlay (int samplesPerBlockExpected, double sampleRate)
{
    // This function will be called when the audio device is started, or when
    // its settings (i.e. sample rate, block size, etc) are changed.

    // You can use this function to initialise any resources you might need,
    // but be careful - it will be called on the audio thread, not the GUI thread.

    // For more details, see the help for AudioProcessor::prepareToPlay()
}

void MainComponent::getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill)
{
    // Your audio-processing code goes here!

    // For more details, see the help for AudioProcessor::getNextAudioBlock()

    // Right now we are not producing any data, in which case we need to clear the buffer
    // (to prevent the output of random noise)
    bufferToFill.clearActiveBufferRegion();

    if (edit.getTransport().isPlaying()) {
        //DBG("Time: " << edit.getCurrentPlaybackContext()->getPosition());
        DBG("Time: " << edit.getTransport().getCurrentPosition());
    }

}

void MainComponent::releaseResources()
{
    // This will be called when the audio device stops, or when it is being
    // restarted due to a setting change.

    // For more details, see the help for AudioProcessor::releaseResources()
}

//==============================================================================
void MainComponent::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    // You can add your drawing code here!
    statusLbl.setText((juce::String)edit.getTransport().getCurrentPosition(), juce::NotificationType::dontSendNotification);
    trackCountLbl.setText((juce::String)edit.getTrackList().size(), juce::NotificationType::dontSendNotification);
    juce::Array<tracktion_engine::Track*> trackList = edit.getTrackList().objects;
    juce::String trackNames = "";
    for (int i = 0; i < trackList.size(); i++) {
        trackNames += trackList[i]->getName();
        trackNames += '\n';
    }
    trackCountLbl.setText(trackNames, juce::NotificationType::dontSendNotification);
}

void MainComponent::resized()
{
    // This is called when the MainContentComponent is resized.
    // If you add any child components, this is where you should
    // update their positions.
    juce::Rectangle<int> bounds = getLocalBounds();

    juce::FlexBox buttonBox{ juce::FlexBox::Direction::row,
                             juce::FlexBox::Wrap::noWrap,
                             juce::FlexBox::AlignContent::stretch,
                             juce::FlexBox::AlignItems::center,
                             juce::FlexBox::JustifyContent::spaceAround };

    juce::FlexBox textBox{ juce::FlexBox::Direction::column,
                           juce::FlexBox::Wrap::noWrap,
                           juce::FlexBox::AlignContent::flexStart,
                           juce::FlexBox::AlignItems::flexStart,
                           juce::FlexBox::JustifyContent::center
    };

    juce::FlexBox container{ juce::FlexBox::Direction::column,
                             juce::FlexBox::Wrap::noWrap,
                             juce::FlexBox::AlignContent::spaceAround,
                             juce::FlexBox::AlignItems::center,
                             juce::FlexBox::JustifyContent::center
    };

    buttonBox.items.add(juce::FlexItem(150, 150, playBtn));
    buttonBox.items.add(juce::FlexItem(150, 150, stopBtn));
    buttonBox.items.add(juce::FlexItem(150, 150, addTrackBtn));

    textBox.items.add(juce::FlexItem(150, 100, statusLbl));
    textBox.items.add(juce::FlexItem(150, 100, trackCountLbl));

    container.items.add(buttonBox);
    container.items.add(textBox);

    container.performLayout(bounds);
}

void MainComponent::play() {
    if (playState == PlayState::Stop) {
        playState = PlayState::Play;
        DBG("Play\n");
        edit.getTransport().play(false);
    }

    auto track = tracktion_engine::getAllTracks(edit);
}

void MainComponent::stop() {
    if (playState == PlayState::Play) {
        playState = PlayState::Stop;
    }
    else if (playState == PlayState::Record) {
        playState = PlayState::Stop;
    }
    DBG("Stop\n");

    edit.getTransport().stop(false,false);
    edit.getTransport().setCurrentPosition(0.0);

}

void MainComponent::addTrack() {
    //edit.insertNewTrack(tracktion_engine::TrackInsertPoint::precedingTrackID, )
}