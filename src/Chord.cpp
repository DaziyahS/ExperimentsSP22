// local includes
#include <Chord.hpp>

// chord struct declaration
Chord::Chord(std::string name, int duration, int amplitude, bool isSimultaneous)
{
    // this list always exists
    signal_list = {"d_minor_n3", "d_major_n3", "e_minor_n3", "e_major_n3", "f_minor_n3", "f_major_n3", "g_minor_n3", "g_major_n3"};

    // variables based on given
    name_ = name;
    duration_ = duration;
    amplitude_ = amplitude;
    isSimultaneous_ = isSimultaneous;
    createNotes();
    getMajor();
}

// determine the note values using the local variables in Chord::Chord
void Chord::createNotes(){
        // if Simultaneous
    if(isSimultaneous_)
    {
        // determine the envelope for duration
        switch(duration_)
        {
            case 0:
                envelope = tact::ASR(0, 0.9, 0); // total time 0.9 s
                break;
            case 1:
                envelope = tact::ASR(0.3, 1.5, 0.3); // total time 2.1 s
                break;
            case 2:
                envelope = tact::ASR(0.3, 2.4, 0.6); // total time 3.3 s
                break;
            default: // should never occur
                envelope = tact::ASR(0, 0.9, 0); // total time 0.9 s
                break;

        }
        // determine the magnitude of the amplitude
        switch(amplitude_)
        {
            case 1: // medium-high amplitude
                sigAmp = 0.75;
                break;
            case 2: // medium amplitude
                sigAmp = 0.5;
                break;
            case 3: // low amplitude
                sigAmp = 0.25;
                break;

            default: // full amplitude
                sigAmp = 1;
                break;
        }
    }
    // if Sequential
    else
    {
        switch(duration_)
        {
            case 0:
                envelope = tact::ASR(0, 0.3, 0); // total time 0.3 s
                break;
            case 1:
                envelope = tact::ASR(0.1, 0.5, 0.1); // total time 0.7 s
                break;
            case 2:
                envelope = tact::ASR(0.1, 0.8, 0.2); // total time 1.1 s
                break;
            default: // should never occur
                envelope = tact::ASR(0, 0.9, 0); // total time 0.9 s
                break;
        }
         // determine the magnitude of the amplitude
        switch(amplitude_)
        {
            case 1: // medium-high amplitude
                sigAmp = 0.75;
                break;
            case 2: // medium amplitude
                sigAmp = 0.5;
                break;
            case 3: // low amplitude
                sigAmp = 0.25;
                break;

            default: // full amplitude
                sigAmp = 1;
                break;
        }
    }
    // determine the notes themselves
    if (name_.compare("d_minor_n3") == 0) // if name given is d_minor_n2
    {
        Note note1(d_minor_n3[0], sigAmp, envelope);
        Note note2(d_minor_n3[1], sigAmp, envelope);
        Note note3(d_minor_n3[2], sigAmp, envelope);
        isMajor_ = d_minor_n3[3]; // determine if it is a major chord
        notes_ = {note1, note2, note3}; 
    }  
    else if (name_.compare("d_major_n3") == 0) // if name given is d_major_n2
    {
        Note note1(d_major_n3[0], sigAmp, envelope);
        Note note2(d_major_n3[1], sigAmp, envelope);
        Note note3(d_major_n3[2], sigAmp, envelope);
        isMajor_ = d_major_n3[3]; // determine if it is a major chord
        notes_ = {note1, note2, note3}; 
    }  
    else if (name_.compare("e_minor_n3") == 0) // if name given is e_minor_n2
    {
        Note note1(e_minor_n3[0], sigAmp, envelope);
        Note note2(e_minor_n3[1], sigAmp, envelope);
        Note note3(e_minor_n3[2], sigAmp, envelope);
        isMajor_ = e_minor_n3[3]; // determine if it is a major chord
        notes_ = {note1, note2, note3}; 
    }  
    else if (name_.compare("e_major_n3") == 0) // if name given is e_major_n2
    {
        Note note1(e_major_n3[0], sigAmp, envelope);
        Note note2(e_major_n3[1], sigAmp, envelope);
        Note note3(e_major_n3[2], sigAmp, envelope);
        isMajor_ = e_major_n3[3]; // determine if it is a major chord
        notes_ = {note1, note2, note3}; 
    }   
    else if (name_.compare("f_minor_n3") == 0) // if name given is d_minor_n2
    {
        Note note1(f_minor_n3[0], sigAmp, envelope);
        Note note2(f_minor_n3[1], sigAmp, envelope);
        Note note3(f_minor_n3[2], sigAmp, envelope);
        isMajor_ = f_minor_n3[3]; // determine if it is a major chord
        notes_ = {note1, note2, note3}; 
    }  
    else if (name_.compare("f_major_n3") == 0) // if name given is d_major_n2
    {
        Note note1(f_major_n3[0], sigAmp, envelope);
        Note note2(f_major_n3[1], sigAmp, envelope);
        Note note2Mod(f_major_n3[1], sigAmp*0.5, envelope);
        Note note3(f_major_n3[2], sigAmp, envelope);
        isMajor_ = f_major_n3[3]; // determine if it is a major chord
        notes_ = {note1, note2, note3}; 
    }  
    else if (name_.compare("g_minor_n3") == 0) // if name given is e_minor_n2
    {
        Note note1(g_minor_n3[0], sigAmp, envelope);
        Note note2(g_minor_n3[1], sigAmp, envelope);
        Note note3(g_minor_n3[2], sigAmp, envelope);
        isMajor_ = g_minor_n3[3]; // determine if it is a major chord
        notes_ = {note1, note2, note3}; 
    }  
    else if (name_.compare("e_major_n3") == 0) // if name given is e_major_n2
    {
        Note note1(g_major_n3[0], sigAmp, envelope);
        Note note2(g_major_n3[1], sigAmp, envelope);
        Note note3(g_major_n3[2], sigAmp, envelope);
        isMajor_ = g_major_n3[3]; // determine if it is a major chord
        notes_ = {note1, note2, note3}; 
    }   
    else
    {
        // do nothing?
        Note note1(0, 1, tact::Envelope(0.1)); // .1 seconds of nothing
        Note note2(0, 1, tact::Envelope(0.1)); // .1 seconds of nothing
        Note note3(0, 1, tact::Envelope(0.1)); // .1 seconds of nothing
        notes_ = {note1, note2, note3};

    }
}

// determine the signals for each channel
std::vector<tact::Signal> Chord::playValues()
{
    createNotes(); // recreate the current note
    std::vector<tact::Signal> channel_sigs;
    if(isSimultaneous_)
    {
        channel1_sig = notes_[0].getSignal();
        channel2_sig = notes_[1].getSignal();
        channel3_sig = notes_[2].getSignal();
    }
    else
    {
        finalSignal = notes_[0].getSignal() << notes_[1].getSignal() << notes_[2].getSignal();
        channel1_sig = finalSignal;
        channel2_sig = finalSignal;
        channel3_sig = finalSignal;
    }
    // what's the final vector
    channel_sigs = {channel1_sig, channel2_sig, channel3_sig};
    return channel_sigs;
}
std::vector<tact::Signal> Chord::playValuesMod()
{
    createNotes(); // recreate the current note
    std::vector<tact::Signal> channel_sigs;
    if(isSimultaneous_)
    {
        channel1_sig = notesMod_[0].getSignal();
        channel2_sig = notesMod_[1].getSignal();
        channel3_sig = notesMod_[2].getSignal();
    }
    else
    {
        finalSignal = notesMod_[0].getSignal() << notesMod_[1].getSignal() << notesMod_[2].getSignal();
        channel1_sig = finalSignal;
        channel2_sig = finalSignal;
        channel3_sig = finalSignal;
    }
    // what's the final vector
    channel_sigs = {channel1_sig, channel2_sig, channel3_sig};
    return channel_sigs;
}

// find out if it is a major chord
bool Chord::getMajor()
{ 
    // determine the notes themselves
    if (name_.compare("d_minor_n3") == 0) // if name given is d_minor_n2
    {
        isMajor_ = d_minor_n3[3]; // determine if it is a major chord
    }   
    else if (name_.compare("d_major_n3") == 0) // if name given is d_major_n2
    {
        isMajor_ = d_major_n3[3]; // determine if it is a major chord
    }   
    else if (name_.compare("e_minor_n3") == 0) // if name given is e_minor_n2
    {
        isMajor_ = e_minor_n3[3]; // determine if it is a major chord
    }   
    else if (name_.compare("e_major_n3") == 0) // if name given is e_major_n2
    {
        isMajor_ = e_major_n3[3]; // determine if it is a major chord
    }
    else if (name_.compare("f_minor_n3") == 0) // if name given is d_minor_n2
    {
        isMajor_ = f_minor_n3[3]; // determine if it is a major chord
    }   
    else if (name_.compare("f_major_n3") == 0) // if name given is d_major_n2
    {
        isMajor_ = f_major_n3[3]; // determine if it is a major chord
    }   
    else if (name_.compare("g_minor_n3") == 0) // if name given is e_minor_n2
    {
        isMajor_ = g_minor_n3[3]; // determine if it is a major chord
    }   
    else if (name_.compare("g_major_n3") == 0) // if name given is e_major_n2
    {
        isMajor_ = g_major_n3[3]; // determine if it is a major chord
    }
    else
    {
        // do nothing
    }
    return isMajor_;
}