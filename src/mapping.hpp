#pragma once

#include <cstdint>
#include <unordered_map>

struct NoteAction {
    bool send = false;
    bool noteOn = false;
    std::uint8_t note = 0;
};

class KeyMapping {
public:
    explicit KeyMapping(int baseNote = 60);

    void setBaseNote(int baseNote);
    void setOctaveOffset(int offset);
    int octaveOffset() const;

    NoteAction handleKeyEvent(unsigned long keySym, bool isPress);

private:
    int baseNote_;
    int octaveOffset_;

    struct State {
        bool pressed = false;
        std::uint8_t note = 0;
    };

    std::unordered_map<unsigned long, State> keyStates_;
    std::unordered_map<unsigned long, int> keyToSemitone_;

    void initDefaultLayout();
};
