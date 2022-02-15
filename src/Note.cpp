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

    // determine the sine wave with the adjusted amplitude inherent - normalized at 1
    switch(name_)
    {   case 1:
            signal_ = tact::Sine(d_note3);
            break;
            break;
        case 2:
            signal_ = tact::Sine(d_snote3);
            break;
        case 3:
            signal_ = tact::Sine(e_note3) * 2/3;
            break;
        case 4:
            signal_ = tact::Sine(f_note3) * 1/2;
            break;
        case 5:
            signal_ = tact::Sine(f_snote3) * 2/3;
            break;
        case 6:
            signal_ = tact::Sine(g_note3)*8/9;
            break;
        case 7:
            signal_ = tact::Sine(g_snote3);
            break;
        case 8:
            signal_ = tact::Sine(a_note3);
            break;
        case 9:
            signal_ = tact::Sine(a_snote3);
            break;
        case 10:
            signal_ = tact::Sine(b_note3);
            break;
        case 11:
            signal_ = tact::Sine(c_note4);
            break;
        case 12:
            signal_ = tact::Sine(c_snote4);
            break;
        case 13:
            signal_ = tact::Sine(d_note4);
            break;
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