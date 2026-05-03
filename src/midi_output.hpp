#pragma once

#include <cstdint>
#include <memory>
#include <string>

class MidiOutput {
public:
    MidiOutput();
    ~MidiOutput();

    bool openVirtualPort(const std::string &name);
    void noteOn(std::uint8_t channel, std::uint8_t note, std::uint8_t velocity);
    void noteOff(std::uint8_t channel, std::uint8_t note);
    void allNotesOff(std::uint8_t channel);

    bool isOpen() const;

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};
