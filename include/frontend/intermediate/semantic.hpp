#ifndef _FRONTEND_INTERMEDIATE_SEMANTIC_HPP
#define _FRONTEND_INTERMEDIATE_SEMANTIC_HPP

#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "ast/ast.hpp"
#include "ast/front_ast.hpp"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Semantic analysis

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Type checking
// Loop labeling
// Identifier resolution

struct SemanticContext {
    // Type checking
    std::unordered_map<TIdentifier, size_t> external_linkage_scope_map;
    std::vector<std::unordered_map<TIdentifier, TIdentifier>> scoped_identifier_maps;
    std::vector<std::unordered_map<TIdentifier, TIdentifier>> scoped_structure_tag_maps;
    std::unordered_map<TIdentifier, TIdentifier> goto_map;
    std::unordered_set<TIdentifier> label_set;
    // Loop labeling
    std::vector<TIdentifier> break_loop_labels;
    std::vector<TIdentifier> continue_loop_labels;
    // Identifier resolution
    TIdentifier function_definition_name;
    CSwitch* p_switch_statement;
    std::unordered_set<TIdentifier> function_definition_set;
    std::unordered_set<TIdentifier> struct_definition_set;
    std::unordered_set<TIdentifier> union_definition_set;
    std::vector<std::shared_ptr<StaticInit>>* p_static_inits;
};

void analyze_semantic(CProgram* node);

#endif
