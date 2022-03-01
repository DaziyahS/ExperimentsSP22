#pragma once

// include needed libraries
#include <syntacts>
#include <iostream>

// include local libraries
#include <Note.hpp>

// declare the structure for the cue
struct Chord
{
    public:
        // declare the constructor for the chord (and default values)
        Chord(std::string name = "d_minor_n3", int duration = 0, int amplitude = 0, bool isSimultaneous = false);
        
        // declare public variables
        std::string name_; // name of the note
        int duration_;
        int amplitude_;
        bool isSimultaneous_; // holds info of which type of sequence
        // variables needed for internal functions
        std::vector<Note> notes_;
        tact::Signal envelope;
        double sigAmp;
        bool isMajor_; // if 1 then it is major
        // std::vector<Chord> chordList;
        // Note note1, note2, note3; // may be unnecessary
        tact::Sequence finalSignal;
        tact::Signal channel1_sig, channel2_sig, channel3_sig;
        // std::vector<tact::Signal> channel_sigs;

        std::vector<std::string> signal_list;

        // create a function to play the values
        std::vector<tact::Signal> playValues();
        std::vector<tact::Signal> playValuesMod();

        void createNotes();
        bool Chord::getMajor();

    private:
        // define the chords based on numbers and whether it is major
        std::vector<int> d_minor_n3 = {1, 4, 8, 0};
        std::vector<int> d_major_n3 = {1, 5, 8, 1};
        std::vector<int> e_minor_n3 = {3, 6, 10, 0};
        std::vector<int> e_major_n3 = {3, 7, 10, 1};
        std::vector<int> f_minor_n3 = {4, 7, 11, 0};
        std::vector<int> f_major_n3 = {4, 8, 11, 1};
        std::vector<int> g_minor_n3 = {6, 9, 13, 0};
        std::vector<int> g_major_n3 = {6, 10, 13, 1};

};
