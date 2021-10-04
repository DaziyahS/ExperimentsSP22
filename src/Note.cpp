// local includes
#include "Note.hpp"

// Note struct declaration
Note::Note(int name, double amplitude, tact::Signal envelope) :
    name_(name),
    amplitude_(amplitude),
    envelope_(envelope)
{
    // assign values given to a variable
    // name_ = name;
    // amplitude_ = amplitude;
    // envelope_ = envelope;
    // define other needed variables for internal functions

    // determine the sine wave
    switch(name_)
    {
        case 1:
            signal_ = tact::Sine(a_note1);
            break;
        case 2:
            signal_ = tact::Sine(a_snote1);
            break;
        case 3:
            signal_ = tact::Sine(b_note1);
            break;
        case 5:
            signal_ = tact::Sine(c_note2);
            break;
        case 6:
            signal_ = tact::Sine(c_snote2);
            break;
        case 7:
            signal_ = tact::Sine(d_note2);
            break;
        case 8:
            signal_ = tact::Sine(d_snote2);
            break;
        case 9:
            signal_ = tact::Sine(e_note2);
            break;
        case 10:
            signal_ = tact::Sine(f_note2);
            break;
        case 11:
            signal_ = tact::Sine(f_snote2);
            break;
        case 12:
            signal_ = tact::Sine(g_note2);
            break;
        case 13:
            signal_ = tact::Sine(g_snote2);
            break;
        case 14:
            signal_ = tact::Sine(a_note2);
            break;
        case 15:
            signal_ = tact::Sine(a_snote2);
            break;
        case 16:
            signal_ = tact::Sine(b_note2);
            break;
        case 18:
            signal_ = tact::Sine(c_note3);
            break;
        case 19:
            signal_ = tact::Sine(c_snote3);
            break;
        case 20:
            signal_ = tact::Sine(d_note3);
            break;
            break;
        case 21:
            signal_ = tact::Sine(d_snote3);
            break;
        case 22:
            signal_ = tact::Sine(e_note3);
            break;
        case 23:
            signal_ = tact::Sine(f_note3);
            break;
        case 24:
            signal_ = tact::Sine(f_snote3);
            break;
        case 25:
            signal_ = tact::Sine(g_note3);
            break;
        case 26:
            signal_ = tact::Sine(g_snote3);
            break;
        case 27:
            signal_ = tact::Sine(a_note3);
            break;
        case 28:
            signal_ = tact::Sine(a_snote3);
            break;
        case 29:
            signal_ = tact::Sine(b_note3);
        default: // something is not right, play nothing
            signal_ = tact::Sine(0);
            break;
    }
}

tact::Signal Note::getSignal() // function no inputs
{
    // std::cout << "got milk?" << std::endl;
    return signal_ * amplitude_ * envelope_;
}