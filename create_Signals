#include <Mahi/Gui.hpp>
#include <Mahi/Util.hpp>
#include <Mahi/Util/Logging/Log.hpp>
#include <notes_info.hpp>
#include <syntacts>
#include <random>
#include <iostream> 
#include <fstream> // need to include inorder to save to csv
#include <chrono> 
#include <string> // for manipulating file name

// local includes
#include <Chord.hpp>
#include <Note.hpp>

// open the namespaces that are relevant for this code
using namespace mahi::gui;
using namespace mahi::util;
using tact::Signal;
using tact::sleep;
using tact::Sequence;

// actually open GUI
int main() {
    Chord chordNow;
    bool isSim = false;

    for (int i = 0; i < 4; ++i)
    {
        amp = i;
        for (int j = 0; j < 3; ++j)
        {
            sus = j;
            chordNow = Chord(14, sus, amp, isSim);
            Library::saveSignal(chordNow, "DMajor_Amp" + amp + "_Sus" + sus);
        }
    }

    return 0;
} 
