#include "app.hpp"

#include "keyboard_x11.hpp"
#include "mapping.hpp"
#include "midi_output.hpp"

App::App(const AppConfig &config) : config_(config) {}

bool App::init() {
    return true;
}

int App::run() {
    MidiOutput midi;
    midi.openVirtualPort("asdfmidi");

    KeyboardX11 keyboard;
    if (!keyboard.init(nullptr)) {
        return 1;
    }

    KeyMapping mapping(config_.baseNote);
    mapping.setOctaveOffset(config_.octaveOffset);

    KeyEvent ev;
    while (keyboard.nextEvent(ev)) {
        bool isPress = (ev.type == KeyEventType::Press);
        auto action = mapping.handleKeyEvent(ev.keySym, isPress);
        if (!action.send) {
            continue;
        }

        if (action.noteOn) {
            midi.noteOn(static_cast<std::uint8_t>(config_.midiChannel), action.note, config_.velocity);
        } else {
            midi.noteOff(static_cast<std::uint8_t>(config_.midiChannel), action.note);
        }
    }

    midi.allNotesOff(static_cast<std::uint8_t>(config_.midiChannel));

    return 0;
}
