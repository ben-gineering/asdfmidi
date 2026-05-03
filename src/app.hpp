#pragma once

#include <cstdint>
#include <memory>

#include "keyboard_x11.hpp"
#include "mapping.hpp"
#include "midi_output.hpp"

struct AppConfig {
    int midiChannel = 0;
    int baseNote = 60;
    int octaveOffset = 0;
    std::uint8_t velocity = 100;
};

class App {
public:
    explicit App(const AppConfig &config);

    bool init();
    int run();

private:
    AppConfig config_;
    std::unique_ptr<MidiOutput> midi_;
    std::unique_ptr<KeyboardX11> keyboard_;
    std::unique_ptr<KeyMapping> mapping_;
};
