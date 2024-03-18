int main(void) {
    char c = -10;
    return (1 ? c : 1u) != 4294967286l;
}
