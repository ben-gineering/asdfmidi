#include "midi_output.hpp"

#include "RtMidi.h"

#include <vector>

struct MidiOutput::Impl {
    RtMidiOut midiOut;
};

MidiOutput::MidiOutput() : impl_(new Impl) {}

MidiOutput::~MidiOutput() = default;

bool MidiOutput::openVirtualPort(const std::string &name) {
    try {
        impl_->midiOut.openVirtualPort(name);
        return true;
    } catch (const RtMidiError &e) {
        std::cerr << "MIDI Error: " << e.what() << std::endl;
        return false;
    }
}

bool MidiOutput::isOpen() const {
    // If we successfully constructed and opened without throwing, we're open
    return impl_ != nullptr;
}

void MidiOutput::noteOn(std::uint8_t channel, std::uint8_t note, std::uint8_t velocity) {
    std::vector<unsigned char> msg;
    msg.push_back(static_cast<unsigned char>(0x90 | (channel & 0x0F)));
    msg.push_back(note);
    msg.push_back(velocity);
    impl_->midiOut.sendMessage(&msg);
}

void MidiOutput::noteOff(std::uint8_t channel, std::uint8_t note) {
    std::vector<unsigned char> msg;
    msg.push_back(static_cast<unsigned char>(0x80 | (channel & 0x0F)));
    msg.push_back(note);
    msg.push_back(0);
    impl_->midiOut.sendMessage(&msg);
}

void MidiOutput::allNotesOff(std::uint8_t channel) {
    for (int n = 0; n < 128; ++n) {
        noteOff(channel, static_cast<std::uint8_t>(n));
    }
}
