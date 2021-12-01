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
        // declare the constructor for the chord
        Chord(std::string name = "a_minor_n1", int duration = 0, int amplitude = 0, bool isSimultaneous = false);
        // defined a default chord because need for when I generically declare it
        
        // declare public variables
        std::string name_; // name of the note
        int duration_;
        int amplitude_;
        bool isSimultaneous_; // holds info of which type of sequence
        // variables needed for internal functions
        std::vector<Note> notes_;
        std::vector<Note> notesMod_;
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
        std::vector<int> a_minor_n1 = {1, 5, 9, 0};
        std::vector<int> a_major_n1 = {1, 6, 9, 1};
        std::vector<int> b_minor_n1 = {3, 8, 11, 0};
        std::vector<int> b_major_n1 = {3, 7, 11, 1};

        std::vector<int> c_minor_n2 = {5, 8, 12, 0};
        std::vector<int> c_major_n2 = {5, 9, 12, 1};
        std::vector<int> d_minor_n2 = {7, 12, 14, 0};
        std::vector<int> d_major_n2 = {7, 11, 14, 1};
        std::vector<int> e_minor_n2 = {9, 12, 16, 0};
        std::vector<int> e_major_n2 = {9, 13, 16, 1};
        std::vector<int> f_minor_n2 = {10, 13, 18, 0};
        std::vector<int> f_major_n2 = {10, 14, 18, 1};
        std::vector<int> g_minor_n2 = {12, 15, 20, 0};
        std::vector<int> g_major_n2 = {12, 16, 20, 1};

        std::vector<int> d_minor_n3 = {20, 23, 27, 0};
        std::vector<int> d_major_n3 = {20, 24, 27, 1};
        std::vector<int> e_minor_n3 = {22, 25, 29, 0};
        std::vector<int> e_major_n3 = {22, 26, 29, 1};

        // predefine the chords list
        // what's their names

};
