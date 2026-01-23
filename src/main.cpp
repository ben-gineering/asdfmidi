#include "app.hpp"

#include <cstdlib>
#include <cstring>
#include <iostream>

namespace {

void print_usage(const char *prog) {
    std::cout << "Usage: " << prog << " [options]\n";
    std::cout << "Options:\n";
    std::cout << "  --channel N       MIDI channel (0-15, default 0)\n";
    std::cout << "  --velocity N      Note velocity (1-127, default 100)\n";
    std::cout << "  --base-octave N   Base octave for C (default 4 -> C4=60)\n";
    std::cout << "  --help            Show this help\n";
}

}

int main(int argc, char **argv) {
    AppConfig config;

    for (int i = 1; i < argc; ++i) {
        const char *arg = argv[i];
        if (std::strcmp(arg, "--help") == 0) {
            print_usage(argv[0]);
            return 0;
        } else if (std::strcmp(arg, "--channel") == 0 && i + 1 < argc) {
            int ch = std::atoi(argv[++i]);
            if (ch < 0) ch = 0;
            if (ch > 15) ch = 15;
            config.midiChannel = ch;
        } else if (std::strcmp(arg, "--velocity") == 0 && i + 1 < argc) {
            int v = std::atoi(argv[++i]);
            if (v < 1) v = 1;
            if (v > 127) v = 127;
            config.velocity = static_cast<std::uint8_t>(v);
        } else if (std::strcmp(arg, "--base-octave") == 0 && i + 1 < argc) {
            int oct = std::atoi(argv[++i]);
            int baseNote = 12 * (oct + 1); // C octave mapping
            if (baseNote < 0) baseNote = 0;
            if (baseNote > 127) baseNote = 127;
            config.baseNote = baseNote;
        } else {
            std::cerr << "Unknown argument: " << arg << "\n";
            print_usage(argv[0]);
            return 1;
        }
    }

    App app(config);
    if (!app.init()) {
        std::cerr << "Failed to initialize application\n";
        return 1;
    }

    return app.run();
}
