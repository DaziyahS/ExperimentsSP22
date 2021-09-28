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
        double a_note1 = 55.00;
        double a_snote1 = 58.27; // a sharp and b flat
        double b_note1 = 61.74;

        double c_note2 = 65.41;
        double c_snote2 = 69.30; // c sharp and d flat
        double d_note2 = 73.42; 
        double d_snote2 = 77.78; // d sharp and e flat
        double e_note2 = 82.41;
        double f_note2 = 87.31;
        double f_snote2 = 92.50; // f sharp and g flat
        double g_note2 = 98.00;
        double g_snote2 = 103.8; // g sharp and a flat
        double a_note2 = 110.0;
        double a_snote2 = 116.5; // a sharp and b flat
        double b_note2 = 123.4;

        double c_note3 = 130.8;
        double c_snote3 = 138.5; // c sharp and d flat
        double d_note3 = 146.8;
};
