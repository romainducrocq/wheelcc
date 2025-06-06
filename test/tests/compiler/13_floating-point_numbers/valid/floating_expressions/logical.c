/* Test doubles in &&, ||, ! and controlling expressions */

#ifdef SUPPRESS_WARNINGS
#ifdef __clang__
#pragma clang diagnostic ignored "-Wliteral-conversion"
#pragma clang diagnostic ignored "-Wliteral-range"
#else
#pragma GCC diagnostic ignored "-Woverflow"
#endif
#endif

double zero = 0.0;
double non_zero = 1E-20;
double one = 1.0;
// this number is so small it will be rounded to zero
double rounded_to_zero = 1e-330;

int main(void) {

    /* double as controlling expression in if statement */

    if (zero) {
        return 1;
    }

    if (rounded_to_zero) {
        return 2;
    }

    if (non_zero) {
        // no-op; should take this one
    } else {
        return 3;
    }

    /* constant doubles as controlling expression */
    if (0.e10) {
        return 4;
    }

    /* ! operator */
    if (!non_zero) {
        return 4;
    }

    if (!(!zero)) {
        return 5;
    }

    if (!(!rounded_to_zero)) {
        return 6;
    }

    /* && operator - test w/ mix of floating-point and non-floating-point operands */

    if (!(non_zero && 1.0)) {
        return 8;
    }

    if (3.0 && zero) {
        return 8;
    }

    if (rounded_to_zero && 1000e10) {
        return 9;
    }


    // mix of double and integer operands
    if (18446744073709551615UL && zero) {
        return 10;
    }

    if (!(non_zero && 5l)) {
        return 11;
    }


    /* || operator */

    if (!(5.0 || zero)) {
        return 12;
    }

    if (zero || rounded_to_zero) {
        return 13;
    }

    if (!(rounded_to_zero || 0.0001)) {
        return 14;
    }

    // mix of double and integer operands
    if (!(non_zero || 0u)) {
        return 15;
    }

    // try || with two constants
    if (!(0 || 0.0000005)) {
        return 16;
    }

    return 0;

}
