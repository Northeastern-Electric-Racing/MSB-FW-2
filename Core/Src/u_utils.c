#include <stdint.h>

int16_t float_to_int16(float value) {
    if (value > 32767.0f) {
        return 32767;
    }

    if (value < -32768.0f) {
        return -32768;
    }

    return (int16_t) value;
}