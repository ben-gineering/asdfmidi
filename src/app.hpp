#pragma once

#include <cstdint>

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
};
