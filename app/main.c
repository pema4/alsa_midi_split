#include <alsa/asoundlib.h>
#include <alsa/seq_event.h>
#include <stdbool.h>
#include <stdio.h>

int main() {
    snd_seq_t *seq = NULL;
    snd_seq_open(&seq, "default", SND_SEQ_OPEN_DUPLEX, 0);
    snd_seq_set_client_name(seq, "split");
    // int input_id = snd_seq_client_id(seq);
    unsigned char input_port = snd_seq_create_simple_port(
        seq, "split:in", SND_SEQ_PORT_CAP_WRITE | SND_SEQ_PORT_CAP_SUBS_WRITE,
        SND_SEQ_PORT_TYPE_APPLICATION);

    unsigned char output_port = snd_seq_create_simple_port(
        seq, "split:out", SND_SEQ_PORT_CAP_READ | SND_SEQ_PORT_CAP_SUBS_READ,
        SND_SEQ_PORT_TYPE_APPLICATION);

    snd_seq_event_t *ev = NULL;
    int err = 0;
    const int low_channel = 0;
    const int channels = 6;
    int channel_note[channels];
    for (int i = 0; i < channels; ++i)
        channel_note[i] = 129;
    int next_note = 0;
    while ((err = snd_seq_event_input(seq, &ev)) >= 0) {
        if (ev->data.note.channel == 0) {
            switch (ev->type) {
            case SND_SEQ_EVENT_NOTEON: {
                int max_note_idx = next_note;
                for (int i = 0; i < channels; ++i)
                    if (channel_note[i] > channel_note[max_note_idx]) {
                        max_note_idx = i;
                    }
                next_note = (max_note_idx + 1) % channels;
                channel_note[max_note_idx] = ev->data.note.note;
                ev->data.note.channel = low_channel + max_note_idx;
                break;
            }
            case SND_SEQ_EVENT_NOTEOFF: {
                int needed_channel = -1;
                for (int i = 0; i < channels; ++i)
                    if (channel_note[i] == ev->data.note.note) {
                        needed_channel = i;
                        break;
                    }
                if (needed_channel == -1)
                    continue;
                ev->data.note.channel = low_channel + needed_channel;
                channel_note[needed_channel] = 129;
                break;
            }
            default:
                continue;
            }

            snd_seq_ev_set_source(ev, output_port);
            snd_seq_ev_set_subs(ev);
            snd_seq_ev_set_direct(ev);
            snd_seq_event_output(seq, ev);
            snd_seq_drain_output(seq);
        }
    }

    // snd_seq_delete_port(seq, output_port);
    snd_seq_delete_port(seq, input_port);
    snd_seq_close(seq);

    return 0;
}
