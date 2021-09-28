#pragma once // so it will only be copied in once, also known as include guard

// Note to self: Back up code. LRAs resonance frequency at 175
#include <syntacts>
#include <iostream>
#include <stdlib.h>
#include <stdio.h>

// Best practice is to not explicitly call namespaces
// using namespace tact;

// Declare function first
tact::Sequence combineSignals(int numberOfSignals, tact::Signal oscillatorsDesired[], tact::Signal envelopesDesired[], double amplitudesDesired[]);

// std::vector<tact::Signal> a_notes = {a_note1, a_note2}
// a_notes[0]
// a_notes[1]
// if I put into a vector, can create buttons for each easily
// for (i, a_notes.size()){
//     ImGui::Button(0, names[i]){
//       s1.play(a_notes[i])
//       }
// }

// You usually include hpp, cpp typically implements functions you have in hpp file
// The definitions in general are in hpp files (functions, variables, yada)
tact::Signal a_note1 = tact::Sine(55.00);
tact::Signal a_snote1 = tact::Sine(58.27); // a sharp and b flat
tact::Signal b_note1 = tact::Sine(61.74);

tact::Signal c_note2 = tact::Sine(65.41);
tact::Signal c_snote2 = tact::Sine(69.30); // c sharp and d flat
tact::Signal d_note2 = tact::Sine(73.42); 
tact::Signal d_snote2 = tact::Sine(77.78); // d sharp and e flat
tact::Signal e_note2 = tact::Sine(82.41);
tact::Signal f_note2 = tact::Sine(87.31);
tact::Signal f_snote2 = tact::Sine(92.50); // f sharp and g flat
tact::Signal g_note2 = tact::Sine(98.00);
tact::Signal g_snote2 = tact::Sine(103.83); // g sharp and a flat
tact::Signal a_note2 = tact::Sine(110.00);
tact::Signal a_snote2 = tact::Sine(116.54); // a sharp and b flat
tact::Signal b_note2 = tact::Sine(123.47);

tact::Signal c_note3 = tact::Sine(130.81);
tact::Signal c_snote3 = tact::Sine(138.59); // c sharp and d flat
tact::Signal d_note3 = tact::Sine(146.83);

// Define tact::signal lengths
tact::Signal sharp = tact::ASR(0, 0.3, 0); // total time 0.3
tact::Signal normall = tact::ASR(0.1, 0.5, 0.1); // total time 0.7s
tact::Signal hold = tact::ASR(0.1, 0.8, 0.2); // total time 1.1 s

// Define vectors for chords;
std::vector<tact::Signal> a_minor_n1 = {a_note1, c_note2, e_note2};
std::vector<tact::Signal> a_major_n1 = {a_note1, c_snote2, e_note2};
std::vector<tact::Signal> b_minor_n1 = {b_note1, d_snote2, f_snote2};
std::vector<tact::Signal> b_major_n1 = {b_note1, d_note2, f_snote2};

std::vector<tact::Signal> c_minor_n2 = {c_note2, d_snote2, g_note2};
std::vector<tact::Signal> c_major_n2 = {c_note2, e_note2, g_note2};
std::vector<tact::Signal> d_minor_n2 = {d_note2, g_note2, a_note2};
std::vector<tact::Signal> d_major_n2 = {d_note2, f_snote2, a_note2};
std::vector<tact::Signal> e_minor_n2 = {e_note2, g_note2, b_note2};
std::vector<tact::Signal> e_major_n2 = {e_note2, g_snote2, b_note2};
std::vector<tact::Signal> f_minor_n2 = {f_note2, g_snote2, c_note3};
std::vector<tact::Signal> f_major_n2 = {f_note2, a_note2, c_note3};
std::vector<tact::Signal> g_minor_n2 = {g_note2, a_snote2, d_note3};
std::vector<tact::Signal> g_major_n2 = {g_note2, b_note2, d_note3};

// Define the vector list for chords
std::vector<std::vector<tact::Signal>> signal_list = {a_minor_n1, a_major_n1, b_minor_n1, b_major_n1, c_minor_n2, c_major_n2,
                                                      d_minor_n2, d_major_n2, e_minor_n2, e_major_n2, f_minor_n2, f_major_n2,
                                                      g_minor_n2, g_major_n2};

// Creating a function to make signals, assumes that all inputs are vectors and that all parameters
// in a vector will be utilized
tact::Sequence combineSignals(std::vector<tact::Signal> oscillatorsDesired,
std::vector<tact::Signal> envelopesDesired, std::vector<double> amplitudesDesired = {})
{
    // Define default value for amplitudes
    if (amplitudesDesired.empty()){
        amplitudesDesired = std::vector<double>(oscillatorsDesired.size(),1.0);
    }

    // set up necessary parameters
    std::vector<tact::Signal> signalDesired;

    // initialize the value of the newSignal
    tact::Sequence newSignal = 0 << oscillatorsDesired[0]*envelopesDesired[0]*amplitudesDesired[0];

    for(int i = 1; i < oscillatorsDesired.size(); i++){ 
        signalDesired.push_back(oscillatorsDesired[i]*envelopesDesired[i]*amplitudesDesired[i]);
        newSignal = newSignal << signalDesired[i];
    }


    return newSignal;
} // Use it until it doesn't work :) Then add more functionality to it

// Remember to not start with the hardest case you will have, can start with the easiest case if you want

// There can only be one main !
/*
int main(int argc, char const *argv[])
{ 
    tact::Session s1;
    // open default device
    s1.open("Speakers (USB Sound Device)",API::MME); 
 
    // Define the notes within the range we can use (assume 50 - 150)
    

    // Define signal lengths
    Signal sharp = ASR(0, 0.3, 0); // total time 0.3
    Signal normall = ASR(0.1, 0.5, 0.1); // total time 0.7s
    Signal hold = ASR(0.1, 0.8, 0.2); // total time 1.1 s
    std::cout << "hey" << std::endl;
//    s1.play(0,c_note3);
//     sleep(3);
//     s1.stop(0); 

    // Define individual signals
    // Normals
    Signal a_note1_n = a_note1*normall;
    Signal b_note1_n = b_note1*normall;
    
    Signal c_note2_n = c_note2*normall;
    Signal d_note2_n = d_note2*normall;
    Signal e_note2_n = e_note2*normall;
    Signal f_note2_n = f_note2*normall;
    Signal g_note2_n = g_note2*normall;
    Signal a_note2_n = a_note2*normall;
    Signal b_note2_n = b_note2*normall;

    Signal c_note3_n = c_note3*normall;
    Signal d_note3_n = d_note3*normall;
   
    // Sharps
    Signal a_note1_s = a_note1*sharp;
    Signal b_note1_s = b_note1*sharp;

    Signal c_note2_s = c_note2*sharp;
    Signal d_note2_s = d_note2*sharp;
    Signal e_note2_s = e_note2*sharp;
    Signal f_note2_s = f_note2*sharp;
    Signal g_note2_s = g_note2*sharp;
    Signal a_note2_s = a_note2*sharp;
    Signal b_note2_s = b_note2*sharp;

    Signal c_note3_s = c_note3*sharp;
    Signal d_note3_s = d_note3*sharp;
    // Holds
    Signal a_note1_h = a_note1*hold;
    Signal b_note1_h = b_note1*hold;

    Signal c_note2_h = c_note2*hold; 
    Signal d_note2_h = d_note2*hold;
    Signal e_note2_h = e_note2*hold;
    Signal f_note2_h = f_note2*hold;
    Signal g_note2_h = g_note2*hold;
    Signal a_note2_h = a_note2*hold;
    Signal b_note2_h = b_note2*hold;

    Signal c_note3_h = c_note3*hold;
    Signal d_note3_h = d_note3*hold;

    // Create Chords
    Sequence a_minor_n1 = a_note1_n << c_note2_n << e_note2_n;
    Sequence c_major_n2 = c_note2_n << e_note2_n << g_note2_n;
    Sequence d_minor_n2 = d_note2_n << f_note2_n << a_note2_n;
    Sequence e_minor_n2 = e_note2_n << g_note2_n << b_note2_n;
    Sequence f_major_n2 = f_note2_n << a_note2_n << c_note3_n;
    Sequence g_major_n2 = g_note2_n << b_note2_n << d_note3_n;

    // Play Signals
    s1.play(0,a_minor_n1); // play a minor chord on channel 0
    s1.play(1,c_major_n2); // play c major chord on channel 1
    sleep(a_minor_n1.length()); // plays for length of chord
    // s1.stop(0); //stop the signal only if it was infinite
    //s.playAll(seq2) would play on all tactors sequence 2
    
    s1.close();

    return 0;
    /*
} 
*/