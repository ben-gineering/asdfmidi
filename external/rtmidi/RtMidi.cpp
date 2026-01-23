#include "RtMidi.h"

#include <alsa/asoundlib.h>

struct RtMidiOut::Impl {
    snd_seq_t *seq = nullptr;
    int outPort = -1;
};

RtMidiOut::RtMidiOut(Api /*api*/, const std::string &clientName) : impl_(new Impl) {
    int err = snd_seq_open(&impl_->seq, "default", SND_SEQ_OPEN_OUTPUT, 0);
    if (err < 0) {
        delete impl_;
        impl_ = nullptr;
        throw RtMidiError("Failed to open ALSA sequencer", RtMidiError::DRIVER_ERROR);
    }

    snd_seq_set_client_name(impl_->seq, clientName.c_str());
}

RtMidiOut::~RtMidiOut() noexcept {
    if (impl_) {
        if (impl_->seq) {
            snd_seq_close(impl_->seq);
        }
        delete impl_;
    }
}

void RtMidiOut::openVirtualPort(const std::string &portName) {
    if (!impl_ || !impl_->seq) return;
    impl_->outPort = snd_seq_create_simple_port(
        impl_->seq,
        portName.c_str(),
        SND_SEQ_PORT_CAP_READ | SND_SEQ_PORT_CAP_SUBS_READ,
        SND_SEQ_PORT_TYPE_MIDI_GENERIC | SND_SEQ_PORT_TYPE_APPLICATION);
}

void RtMidiOut::openPort(unsigned int /*portNumber*/, const std::string &portName) {
    openVirtualPort(portName);
}

void RtMidiOut::closePort() {
    // Not strictly needed for simple usage; ports are destroyed on close.
}

unsigned int RtMidiOut::getPortCount() {
    // Minimal implementation: no external ports enumerated.
    return 0;
}

std::string RtMidiOut::getPortName(unsigned int /*portNumber*/) {
    return std::string();
}

void RtMidiOut::sendMessage(const std::vector<unsigned char> *message) {
    if (!impl_ || !impl_->seq || impl_->outPort < 0 || !message || message->empty()) {
        return;
    }

    snd_seq_event_t ev;
    snd_seq_ev_clear(&ev);
    snd_seq_ev_set_source(&ev, impl_->outPort);
    snd_seq_ev_set_subs(&ev);
    snd_seq_ev_set_direct(&ev);

    unsigned char status = (*message)[0] & 0xF0;
    if (status == 0x90 || status == 0x80) {
        unsigned char note = (*message).size() > 1 ? (*message)[1] : 0;
        unsigned char velocity = (*message).size() > 2 ? (*message)[2] : 0;
        if (status == 0x90 && velocity > 0) {
            snd_seq_ev_set_noteon(&ev, 0, note, velocity);
        } else {
            snd_seq_ev_set_noteoff(&ev, 0, note, velocity);
        }
    } else {
        // Unsupported message type in this minimal implementation.
        return;
    }

    snd_seq_event_output_direct(impl_->seq, &ev);
}
