#ifndef _SEMANTIC_LOOPS_HPP
#define _SEMANTIC_LOOPS_HPP

#include "ast/c_ast.hpp"

void annotate_while_loop(CWhile* node);
void annotate_do_while_loop(CDoWhile* node);
void annotate_for_loop(CFor* node);
void annotate_break_loop(CBreak* node);
void annotate_continue_loop(CContinue* node);
void deannotate_loop();

#endif
