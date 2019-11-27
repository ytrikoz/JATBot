#include "JATBotUtils.h"

int i64toa(char *buf, const int64_t value) {
    return sprintf(buf, "%0lld", value);
}

void urlencode(char *buf, const char *in, const size_t size) {
    memset(buf, '\x00', size);
    for (size_t i = 0; (i < size - 1) && (in[i]); i++) {
        if (isalnum(in[i])) {
            *buf ++ = in[i];
            continue;
        }        
        if (in[i] == ' ') {
            *buf ++ = '+';
            continue;
        } 
        char c = in[i];
        char hi_byte = (c & 0xF) + '0';
        if ((c & 0xF) > 9)
            hi_byte = (c & 0xF) - 10 + 'A';
        c = (c >> 4) & 0xF;
        char low_byte = c + '0';
        if (c > 9)
            low_byte = c - 10 + 'A';
        *buf ++ = '%';
        *buf ++ = low_byte;
        *buf ++ = hi_byte;
    }
}