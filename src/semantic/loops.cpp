#include "semantic/loops.hpp"
#include "util/error.hpp"
#include "util/names.hpp"
#include "ast/ast.hpp"
#include "ast/c_ast.hpp"

#include <vector>

/**
cdef list[str] loop_labels = []
*/
static std::vector<TIdentifier> loop_labels;

/**
cdef void annotate_while_loop(CWhile node):
    node.target = represent_label_identifier("while")
    loop_labels.append(node.target.str_t)
*/
void annotate_while_loop(CWhile* node) {
    node->target = represent_label_identifier("while");
    loop_labels.push_back(node->target);
}

/**
cdef void annotate_do_while_loop(CDoWhile node):
    node.target = represent_label_identifier("do_while")
    loop_labels.append(node.target.str_t)
*/
void annotate_do_while_loop(CDoWhile* node) {
    node->target = represent_label_identifier("do_while");
    loop_labels.push_back(node->target);
}

/**
cdef void annotate_for_loop(CFor node):
    node.target = represent_label_identifier("for")
    loop_labels.append(node.target.str_t)
*/
void annotate_for_loop(CFor* node) {
    node->target = represent_label_identifier("for");
    loop_labels.push_back(node->target);
}

/**
cdef void annotate_break_loop(CBreak node):
    if not loop_labels:

        raise RuntimeError(
            "An error occurred in loop annotation, break is outside of loop")

    node.target = TIdentifier(loop_labels[-1])
*/
void annotate_break_loop(CBreak* node) {
    if(loop_labels.empty()) {
        raise_runtime_error("An error occurred in loop annotation, " + em("break") +
                            "is outside of loop");
    }
    node->target = loop_labels.back();
}

/**
cdef void annotate_continue_loop(CContinue node):
    if not loop_labels:

        raise RuntimeError(
            "An error occurred in loop annotation, continue is outside of loop")

    node.target = TIdentifier(loop_labels[-1])
*/
void annotate_continue_loop(CContinue* node) {
    if(loop_labels.empty()) {
        raise_runtime_error("An error occurred in loop annotation, " + em("continue") +
                            "is outside of loop");
    }
    node->target = loop_labels.back();
}

/**
cdef void deannotate_loop():
    del loop_labels[-1]
*/
void deannotate_loop() {
    loop_labels.pop_back();
}

/**
cdef void init_annotate_loops():
    loop_labels.clear()
*/
void clear_annotate_loops() {
    loop_labels.clear();
}
