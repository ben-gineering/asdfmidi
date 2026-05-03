#include "app.hpp"

#include <iostream>

App::App(const AppConfig &config) : config_(config) {}

bool App::init() {
    // Initialize MIDI output
    midi_ = std::make_unique<MidiOutput>();
    if (!midi_->openVirtualPort("asdfmidi")) {
        std::cerr << "Failed to open MIDI virtual port. Is ALSA available?\n";
        return false;
    }

    // Initialize keyboard input
    keyboard_ = std::make_unique<KeyboardX11>();
    if (!keyboard_->init(nullptr)) {
        std::cerr << "Failed to initialize X11 keyboard. Is X11 running?\n";
        return false;
    }

    // Initialize key mapping
    mapping_ = std::make_unique<KeyMapping>(config_.baseNote);
    mapping_->setOctaveOffset(config_.octaveOffset);

    return true;
}

int App::run() {
    if (!midi_ || !keyboard_ || !mapping_) {
        std::cerr << "Application not properly initialized\n";
        return 1;
    }

    KeyEvent ev;
    while (keyboard_->nextEvent(ev)) {
        bool isPress = (ev.type == KeyEventType::Press);
        auto action = mapping_->handleKeyEvent(ev.keySym, isPress);

        switch (action.type) {
            case NoteActionType::NoteOn:
                midi_->noteOn(static_cast<std::uint8_t>(config_.midiChannel), action.note, config_.velocity);
                break;
            case NoteActionType::NoteOff:
                midi_->noteOff(static_cast<std::uint8_t>(config_.midiChannel), action.note);
                break;
            case NoteActionType::AllNotesOff:
                midi_->allNotesOff(static_cast<std::uint8_t>(config_.midiChannel));
                break;
            case NoteActionType::None:
                break;
        }
    }

    // Graceful shutdown: send all notes off
    midi_->allNotesOff(static_cast<std::uint8_t>(config_.midiChannel));

    return 0;
}
