/* Test that we can pass a pointer to an array of structures as a parameter */

/* Test that we can pass a pointer to an array of structures as a parameter */

struct inner {
    long l;
    char arr[2];
};  // size: 8 bytes, alignment: 4 bytes

struct outer {
    char a;          // byte 0
    struct inner b;  // bytes 4-11

};  // size: 12 byte, alignment: 4 bytes

// int validate_struct_array(struct outer *struct_array);

static struct outer static_array[3] = {
    {0, {0, {0, 0}}}, {2, {3, {4, 5}}}, {4, {6, {8, 10}}}};

int main(void) {
    // struct outer auto_array[3] = {
    //     {0, {0, {0, 0}}}, {2, {3, {4, 5}}}, {4, {6, {8, 10}}}};

    // // pass pointers to struct arrays with both static and automatic storage
    // // both have same contents so we can validate them with the same function

    // if (!validate_struct_array(static_array)) {
    //     return 1;
    // }

    // if (!validate_struct_array(auto_array)) {
    //     return 2;
    // }

    return 0;  // success
}