#include "mapping.hpp"

#include <X11/keysym.h>

KeyMapping::KeyMapping(int baseNote)
    : baseNote_(baseNote), octaveOffset_(0) {
    initDefaultLayout();
}

void KeyMapping::setBaseNote(int baseNote) {
    baseNote_ = baseNote;
}

void KeyMapping::setOctaveOffset(int offset) {
    octaveOffset_ = offset;
}

int KeyMapping::octaveOffset() const {
    return octaveOffset_;
}

NoteAction KeyMapping::panic() {
    // Reset all key states
    for (auto &pair : keyStates_) {
        pair.second.pressed = false;
    }
    return {NoteActionType::AllNotesOff, 0};
}

void KeyMapping::initDefaultLayout() {
    keyToSemitone_[XK_z] = 0;
    keyToSemitone_[XK_s] = 1;
    keyToSemitone_[XK_x] = 2;
    keyToSemitone_[XK_d] = 3;
    keyToSemitone_[XK_c] = 4;
    keyToSemitone_[XK_v] = 5;
    keyToSemitone_[XK_g] = 6;
    keyToSemitone_[XK_b] = 7;
    keyToSemitone_[XK_h] = 8;
    keyToSemitone_[XK_n] = 9;
    keyToSemitone_[XK_j] = 10;
    keyToSemitone_[XK_m] = 11;

    keyToSemitone_[XK_q] = 12;
    keyToSemitone_[XK_2] = 13;
    keyToSemitone_[XK_w] = 14;
    keyToSemitone_[XK_3] = 15;
    keyToSemitone_[XK_e] = 16;
    keyToSemitone_[XK_r] = 17;
    keyToSemitone_[XK_5] = 18;
    keyToSemitone_[XK_t] = 19;
    keyToSemitone_[XK_6] = 20;
    keyToSemitone_[XK_y] = 21;
    keyToSemitone_[XK_7] = 22;
    keyToSemitone_[XK_u] = 23;
}

NoteAction KeyMapping::handleKeyEvent(unsigned long keySym, bool isPress) {
    if (keySym == XK_bracketleft && isPress) {
        --octaveOffset_;
        return {NoteActionType::None, 0};
    }
    if (keySym == XK_bracketright && isPress) {
        ++octaveOffset_;
        return {NoteActionType::None, 0};
    }

    // Panic key: Escape
    if (keySym == XK_Escape && isPress) {
        return panic();
    }

    auto it = keyToSemitone_.find(keySym);
    if (it == keyToSemitone_.end()) {
        return {NoteActionType::None, 0};
    }

    State &st = keyStates_[keySym];

    if (isPress) {
        if (st.pressed) {
            return {NoteActionType::None, 0};
        }

        int noteValue = baseNote_ + octaveOffset_ * 12 + it->second;
        if (noteValue < 0) {
            noteValue = 0;
        }
        if (noteValue > 127) {
            noteValue = 127;
        }

        st.pressed = true;
        st.note = static_cast<std::uint8_t>(noteValue);

        return {NoteActionType::NoteOn, st.note};
    }

    if (!st.pressed) {
        return {NoteActionType::None, 0};
    }

    st.pressed = false;

    return {NoteActionType::NoteOff, st.note};
}
