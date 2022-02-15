#pragma once

// include important libraries
#include <syntacts>
#include <iostream>

// Note structure declaration

struct Note
{
    public:
        // declare the constructor for the note
        Note(int name, double amplitude, tact::Signal envelope);

        // function to generate the wanted signal
        tact::Signal getSignal();

        // declare public variables
        int name_;
        double amplitude_;
        tact::Signal signal_;
        tact::Signal envelope_; 

    private:
        double d_note3 = 146.83; 
        double d_snote3 = 155.56; // d sharp and e flat
        double e_note3 = 164.81;
        double f_note3 = 174.61;
        double f_snote3 = 185; // f sharp and g flat
        double g_note3 = 196;
        double g_snote3 = 207.65; // g sharp and a flat
        double a_note3 = 220;
        double a_snote3 = 233.08; // a sharp and b flat
        double b_note3 = 246.95;

        double c_note4 = 261.63; 
        double c_snote4 = 277.18; // d sharp and e flat
        double d_note4 = 293.66;
};
