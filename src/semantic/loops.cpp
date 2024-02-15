#include "semantic/loops.hpp"
#include "util/error.hpp"
#include "util/names.hpp"
#include "ast/ast.hpp"
#include "ast/c_ast.hpp"

#include <vector>

static std::vector<TIdentifier> loop_labels;

void annotate_while_loop(CWhile* node) {
    node->target = represent_label_identifier("while");
    loop_labels.push_back(node->target);
}

void annotate_do_while_loop(CDoWhile* node) {
    node->target = represent_label_identifier("do_while");
    loop_labels.push_back(node->target);
}

void annotate_for_loop(CFor* node) {
    node->target = represent_label_identifier("for");
    loop_labels.push_back(node->target);
}

void annotate_break_loop(CBreak* node) {
    if(loop_labels.empty()) {
        raise_runtime_error("An error occurred in loop annotation, " + em("break") +
                            "is outside of loop");
    }
    node->target = loop_labels.back();
}

void annotate_continue_loop(CContinue* node) {
    if(loop_labels.empty()) {
        raise_runtime_error("An error occurred in loop annotation, " + em("continue") +
                            "is outside of loop");
    }
    node->target = loop_labels.back();
}

void deannotate_loop() {
    loop_labels.pop_back();
}

void clear_annotate_loops() {
    loop_labels.clear();
}
