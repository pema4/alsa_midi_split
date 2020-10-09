#include <alsa/asoundlib.h>
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

    int last_channel = 0;
    snd_seq_event_t *ev = NULL;
    int err = 0;
    while ((err = snd_seq_event_input(seq, &ev)) >= 0) {
        if (ev->type == SND_SEQ_EVENT_NOTEON && ev->data.note.channel == 0) {
            ev->data.note.channel = last_channel;
            printf("Note%02x\n", ev->data.note.note);
            last_channel = (last_channel + 1) % 6;
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