/**********************************************************************/
/*! \class RtMidi
    \brief An abstract base class for realtime MIDI input/output.

    This is a vendored copy. For the full, up-to-date source and
    documentation, see the official project:
    https://github.com/thestk/rtmidi
*/
/**********************************************************************/

#ifndef RTMIDI_H
#define RTMIDI_H

#include <exception>
#include <iostream>
#include <string>
#include <vector>

// Minimal subset of the RtMidi API needed for this project.

class RtMidiError : public std::exception {
public:
    enum Type {
        WARNING,
        UNSPECIFIED,
        DRIVER_ERROR
    };

    explicit RtMidiError(const std::string &message, Type type = UNSPECIFIED) noexcept
        : message_(message), type_(type) {}

    const char *what() const noexcept override { return message_.c_str(); }

private:
    std::string message_;
    Type type_;
};

class RtMidiOut {
public:
    enum Api {
        UNSPECIFIED,
        LINUX_ALSA
    };

    explicit RtMidiOut(Api api = UNSPECIFIED, const std::string &clientName = "RtMidi Output Client");
    ~RtMidiOut() noexcept;

    void openVirtualPort(const std::string &portName = "RtMidi Output");
    void openPort(unsigned int portNumber = 0, const std::string &portName = "RtMidi Output");
    void closePort();

    unsigned int getPortCount();
    std::string getPortName(unsigned int portNumber = 0);

    void sendMessage(const std::vector<unsigned char> *message);

private:
    struct Impl;
    Impl *impl_;
};

#endif // RTMIDI_H
