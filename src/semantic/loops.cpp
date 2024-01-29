#include "semantic/loops.hpp"

/** TODO
cdef list[str] loop_labels = []
*/

/** TODO
cdef void annotate_while_loop(CWhile node):
    node.target = represent_label_identifier("while")
    loop_labels.append(node.target.str_t)
*/

/** TODO
cdef void annotate_do_while_loop(CDoWhile node):
    node.target = represent_label_identifier("do_while")
    loop_labels.append(node.target.str_t)
*/

/** TODO
cdef void annotate_for_loop(CFor node):
    node.target = represent_label_identifier("for")
    loop_labels.append(node.target.str_t)
*/

/** TODO
cdef void annotate_break_loop(CBreak node):
    if not loop_labels:

        raise RuntimeError(
            "An error occurred in loop annotation, break is outside of loop")

    node.target = TIdentifier(loop_labels[-1])
*/

/** TODO
cdef void annotate_continue_loop(CContinue node):
    if not loop_labels:

        raise RuntimeError(
            "An error occurred in loop annotation, continue is outside of loop")

    node.target = TIdentifier(loop_labels[-1])
*/

/** TODO
cdef void deannotate_loop():
    del loop_labels[-1]
*/

/** TODO
cdef void init_annotate_loops():
    loop_labels.clear()
*/