/* Test out simple cases involving constant indices and one-dimensional arrays */

#if defined SUPPRESS_WARNINGS && !defined __clang__
#pragma GCC diagnostic ignored "-Wtautological-compare"
#endif

//int nothing(void) {
//}

int integer_types(unsigned *arr /*, unsigned expected*/) {
//    unsigned val1 = arr[5];
//    nothing();
//    unsigned val2 = arr[5u];
//    nothing();
    unsigned val3 = arr[5l];
//    nothing();
//    unsigned val4 = arr[5ul];
//    return 0;
}

int main(void) {
}













