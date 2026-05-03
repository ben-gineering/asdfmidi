#pragma once

#include <cstdint>
#include <unordered_map>

enum class NoteActionType {
    None,
    NoteOn,
    NoteOff,
    AllNotesOff  // Panic
};

struct NoteAction {
    NoteActionType type = NoteActionType::None;
    std::uint8_t note = 0;
};

class KeyMapping {
public:
    explicit KeyMapping(int baseNote = 60);

    void setBaseNote(int baseNote);
    void setOctaveOffset(int offset);
    int octaveOffset() const;

    NoteAction handleKeyEvent(unsigned long keySym, bool isPress);

    // Trigger panic from external (e.g., signal handler)
    NoteAction panic();

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
