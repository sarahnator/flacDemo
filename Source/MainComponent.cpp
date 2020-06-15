/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#include "MainComponent.h"
#include <iostream>
#include <fstream>

//==============================================================================
MainComponent::MainComponent() : state(Stopped)
{
    // Make sure you set the size of the component after
    // you add any child components.
    addAndMakeVisible (&openButton);
    openButton.setButtonText ("Open...");
    openButton.onClick = [this] { openButtonClicked(); };

    addAndMakeVisible (&playButton);
    playButton.setButtonText ("Play");
    playButton.onClick = [this] { playButtonClicked(); };
    playButton.setColour (TextButton::buttonColourId, Colours::green);
    playButton.setEnabled (false);

    addAndMakeVisible (&stopButton);
    stopButton.setButtonText ("Stop");
    stopButton.onClick = [this] { stopButtonClicked(); };
    stopButton.setColour (TextButton::buttonColourId, Colours::red);
    stopButton.setEnabled (false);
    
    addAndMakeVisible (&flacButton);
    flacButton.setButtonText ("Play flac format");
    flacButton.onClick = [this] { flacButtonClicked(); };
    flacButton.setColour (TextButton::buttonColourId, Colours::lightblue);
    flacButton.setEnabled (false);

    
    setSize (800, 600);

    formatManager.registerBasicFormats();
    transportSource.addChangeListener (this);

    // Some platforms require permissions to open input channels so request that here
    if (RuntimePermissions::isRequired (RuntimePermissions::recordAudio)
        && ! RuntimePermissions::isGranted (RuntimePermissions::recordAudio))
    {
        RuntimePermissions::request (RuntimePermissions::recordAudio,
                                     [&] (bool granted) { if (granted)  setAudioChannels (2, 2); });
    }
    else
    {
        // Specify the number of input and output channels that we want to open
        setAudioChannels (2, 2);
    }
}

MainComponent::~MainComponent()
{
    // This shuts down the audio device and clears the audio source.
    shutdownAudio();
}

//==============================================================================
void MainComponent::prepareToPlay (int samplesPerBlockExpected, double sampleRate)
{
    transportSource.prepareToPlay (samplesPerBlockExpected, sampleRate);

    
    // This function will be called when the audio device is started, or when
    // its settings (i.e. sample rate, block size, etc) are changed.

    // You can use this function to initialise any resources you might need,
    // but be careful - it will be called on the audio thread, not the GUI thread.

    // For more details, see the help for AudioProcessor::prepareToPlay()
}

void MainComponent::getNextAudioBlock (const AudioSourceChannelInfo& bufferToFill)
{
    if (readerSource.get() == nullptr)
    {
        bufferToFill.clearActiveBufferRegion();
        return;
    }

    transportSource.getNextAudioBlock (bufferToFill);
    // Your audio-processing code goes here!

}

void MainComponent::releaseResources()
{
        // This will be called when the audio device stops, or when it is being
    // restarted due to a setting change.

    // For more details, see the help for AudioProcessor::releaseResources()
}

//==============================================================================
void MainComponent::paint (Graphics& g)
{
 
    g.fillAll(Colours::lightslategrey);
    g.setColour(Colours::white);
    g.fillRect(getWidth()/2 - 100, getHeight()/2 - 30, 200, 65);
    g.setColour(Colours::black);
    g.drawRect(getWidth()/2 - 100, getHeight()/2 - 30, 200, 65, 2);
    g.setFont(40.0f);
    g.drawText ("Flac demo", getLocalBounds(), Justification::centred, true);


}

void MainComponent::resized()
{
    openButton.setBounds (10, 10, getWidth() - 20, 30);
    playButton.setBounds (10, 50, getWidth() - 20, 30);
    stopButton.setBounds (10, 90, getWidth() - 20, 30);
    flacButton.setBounds (10, 130, getWidth() - 20, 30);
    // This is called when the MainContentComponent is resized.
    // If you add any child components, this is where you should
    // update their positions.
}
void MainComponent::changeListenerCallback(ChangeBroadcaster* source)
{
    if (source == &transportSource)
    {
        if (transportSource.isPlaying())
            changeState (Playing);
        else if ((state == Stopping) || (state == Playing))
            changeState (Stopped);
        else if (Pausing == state)
            changeState (Paused);
    }
}

void MainComponent::changeState (TransportState newState)
{
    if (state != newState)
    {
        state = newState;

        switch (state)
        {
            case Stopped:
                playButton.setButtonText ("Play");
                stopButton.setButtonText ("Stop");
                stopButton.setEnabled (false);
                transportSource.setPosition (0.0);
                break;

            case Starting:
                transportSource.start();
                break;

            case Playing:
                playButton.setButtonText ("Pause");
                stopButton.setButtonText ("Stop");
                stopButton.setEnabled (true);
                break;

            case Pausing:
                transportSource.stop();
                break;

            case Paused:
                playButton.setButtonText ("Resume");
                stopButton.setButtonText ("Return to Zero");
                break;

            case Stopping:
                transportSource.stop();
                break;
        }
    }
}

void MainComponent::openButtonClicked()
{
    FileChooser chooser ("Select a Wave file to play...",
                         {},
                         "*.wav");

    if (chooser.browseForFileToOpen())
    {
        auto file = chooser.getResult();
        auto* reader = formatManager.createReaderFor (file);
        
        
        
        if (reader != nullptr)
        {
            convertToFlac(reader);
           
            std::unique_ptr<AudioFormatReaderSource> newSource (new AudioFormatReaderSource (reader, true));
            transportSource.setSource (newSource.get(), 0, nullptr, reader->sampleRate);
            playButton.setEnabled (true);
            flacButton.setEnabled (true);
            readerSource.reset (newSource.release());
        }
    }
}
/*  convert wav to flac
 *  write flac to memory / file
*/
void MainComponent::convertToFlac(AudioFormatReader* reader)
{
        //Read wav file
        ScopedPointer<AudioSampleBuffer> buff = new AudioSampleBuffer(reader->numChannels, (int)reader->lengthInSamples);
        /* Alternatively:
                    AudioSampleBuffer* buff = new AudioSampleBuffer();
                    buff->setSize(reader->numChannels, (int)reader->lengthInSamples);
        */
        reader->read(buff, 0, (int)reader->lengthInSamples, 0, true, true);
        ScopedPointer<AudioFormatWriter> flacWriter;
        
        // write flac to file
        File myfile(File::getCurrentWorkingDirectory().getChildFile("converted.flac"));
        if(myfile.exists()){
            myfile.deleteFile();
            myfile.create();
        }
        std::cout<<"created: " << myfile.getCreationTime().getMonthName(false)<< " " << myfile.getCreationTime().getDayOfMonth() <<std::endl;
        ScopedPointer<OutputStream> output = myfile.createOutputStream();
    
//    write to file
//        bool r = myfile.create().wasOk();
//        std::cout<< "was file ok?: " << r << std::endl;
//        flacWriter = flacFormat->createWriterFor(output, reader->sampleRate, reader->numChannels, 16, {}, 0);
//        flacWriter->writeFromAudioReader(*reader, 0, reader->lengthInSamples);
//        output->flush();
//        flacWriter->flush();


        // write flac format to memory
        mem = new MemoryOutputStream((int)reader->lengthInSamples);
    flacWriter = flacFormat.createWriterFor(mem, reader->sampleRate, reader->numChannels, 16, NULL, 0); // 16 bits per sample (bit depth)
        bool success = flacWriter->writeFromAudioSampleBuffer(*buff, 0, (int)reader->lengthInSamples);
//  Write from memory to file (redundant if writing to file above not commented out
      output->write(mem->getData(), mem->getDataSize());
      output->flush();

}
void MainComponent::playButtonClicked()
{
    if ((state == Stopped) || (state == Paused))
        changeState (Starting);
    else if (state == Playing)
        changeState (Pausing);
}

void MainComponent::stopButtonClicked()
{
    if (state == Paused)
        changeState (Stopped);
    else
        changeState (Stopping);
}
void MainComponent::flacButtonClicked()
{
    //Does nothing for now, but will convert flac->wav and play flac file
//    flacButton.setEnabled (false);
//    File myfile(File::getCurrentWorkingDirectory().getChildFile("converted.flac"));
//    auto* reader = formatManager.createReaderFor (myfile);
//    std::unique_ptr<AudioFormatReaderSource> newSource (new AudioFormatReaderSource (reader, true));
//              transportSource.setSource (newSource.get(), 0, nullptr, reader->sampleRate);
//
//    readerSource.reset (newSource.release());
//
//
//
    
    
    
    
    //TODO: figure out how to read from flac
    //this stuff down here is just bs
//    MemoryInputStream* fromMem = new MemoryInputStream(mem->getData(), mem->getDataSize(), false);
//            play flac from memory
//                   
//                      AudioFormatReader* flacReader =  flacFormat->createReaderFor(fromMem, true);
//        std::unique_ptr<AudioFormatReaderSource> newSource (new AudioFormatReaderSource (flacReader, true));
//        transportSource.setSource (newSource.get(), 0, nullptr, flacReader->sampleRate);
//        readerSource.reset (newSource.release());
//    
//    flacButton.setEnabled (true);
//    delete flacReader;

}
