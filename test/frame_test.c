#include "frame.h"
#include "log.h"
#include "mem.h"

void test__frame_var_headers() {
    bpsp__frame* frame = frame__new();

    if (!frame) {
        log__error("NULL frame.");
        exit(1);
    }

    char no_pair[] = "";

    char one_pair[] = "\"x-username\"\"haidao\";";

    char one_escape_pair[] = "\"x-usern\\\"ame\"\"haidao\";";

    char two_pair[] = "\"x-username\"\"haidao\";\"x-password\"\"12345\";";

    char two_pair_dup[] = "\"x-username\"\"haidao\";\"x-username\"\"12345\";";

    char more_pair[] = "\"x-username\"\"haidao\";\"x-password\"\"12345\";\"content-type\"\"json\";";

    /* log__info("%s, %u", one_pair, strlen(one_pair)); */
    /* log__info("%u", frame->_is_completed); */

    frame__parse_var_header(frame, (bpsp__byte*)no_pair, strlen(no_pair));
    bpsp__byte* data = mem__malloc(sizeof(bpsp__byte) * (frame->vars_size + 1));
    var_header__to_bytes(frame->var_headers, data);
    *(data + frame->vars_size) = 0;
    log__info("%s - %u", data, frame->vars_size);
    mem__free(data);
    frame__empty(frame);

    frame__parse_var_header(frame, (bpsp__byte*)one_pair, strlen(one_pair));
    data = mem__malloc(sizeof(bpsp__byte) * (frame->vars_size + 1));
    var_header__to_bytes(frame->var_headers, data);
    *(data + frame->vars_size) = 0;
    log__info("%s - %u", data, frame->vars_size);
    mem__free(data);
    frame__empty(frame);

    frame__parse_var_header(frame, (bpsp__byte*)one_escape_pair, strlen(one_escape_pair));
    data = mem__malloc(sizeof(bpsp__byte) * (frame->vars_size + 1));
    var_header__to_bytes(frame->var_headers, data);
    *(data + frame->vars_size) = 0;
    log__info("%s - %u", data, frame->vars_size);
    mem__free(data);
    frame__empty(frame);

    frame__parse_var_header(frame, (bpsp__byte*)two_pair, strlen(two_pair));
    data = mem__malloc(sizeof(bpsp__byte) * (frame->vars_size + 1));
    var_header__to_bytes(frame->var_headers, data);
    *(data + frame->vars_size) = 0;
    log__info("%s - %u", data, frame->vars_size);
    mem__free(data);
    frame__empty(frame);

    frame__parse_var_header(frame, (bpsp__byte*)more_pair, strlen(more_pair));
    data = mem__malloc(sizeof(bpsp__byte) * (frame->vars_size + 1));
    var_header__to_bytes(frame->var_headers, data);
    *(data + frame->vars_size) = 0;
    log__info("%s - %u", data, frame->vars_size);
    mem__free(data);
    frame__empty(frame);

    frame__parse_var_header(frame, (bpsp__byte*)two_pair_dup, strlen(two_pair_dup));
    data = mem__malloc(sizeof(bpsp__byte) * (frame->vars_size + 1));
    var_header__to_bytes(frame->var_headers, data);
    *(data + frame->vars_size) = 0;
    log__info("%s - %u", data, frame->vars_size);
    mem__free(data);
    frame__empty(frame);

    frame__free(frame);
}

int main() {
    // test cases
    test__frame_var_headers();

    return 0;
}
