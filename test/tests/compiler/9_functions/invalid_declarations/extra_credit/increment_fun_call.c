int x(void);

int main(void) {
    // a function call is not an lvalue, so we can't increment it
    ++x();
}
