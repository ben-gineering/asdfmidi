#pragma once

#include <cstdint>

enum class KeyEventType {
    Press,
    Release
};

struct KeyEvent {
    KeyEventType type;
    unsigned long keySym;
};

class KeyboardX11 {
public:
    KeyboardX11();
    ~KeyboardX11();

    bool init(const char *displayName = nullptr);
    bool nextEvent(KeyEvent &ev);
    void shutdown();

private:
    struct Impl;
    Impl *impl_;
};
