#!/bin/bash

function parse () {
    cat ../../include/ast/${1}.hpp \
        | grep --invert-match ")" \
        | grep --invert-match "(" \
        | sed "s/nstruction/nstr_uction/g" \
        | sed "s/structure/str_ucture/g" \
        | sed "s/struct_decl/str_uct_decl/g" \
        | grep -e "struct" -e ";" \
        | sed "s/\s*std::vector<TIdentifier>\s*/TIdentifier [/g" \
        | sed "s/\s*std::vector<TChar>\s*/TChar [/g" \
        | sed "s/\s*std::vector<.*>\s*/\"[/g" \
        | sed "s/\s*std::unordered_map<TIdentifier,.*>>\s*/\"(/g" \
        | sed "s/\s*std::unique_ptr<.*>\s*/\"/g" \
        | sed "s/\s*std::shared_ptr<.*>\s*/\"/g" \
        | sed "s/\s*TIdentifier\s*/(TIdentifier, \"/g" \
        | sed "s/\s*TChar\s*/(TChar, \"/g" \
        | sed "s/\s*TInt\s*/(TInt, \"/g" \
        | sed "s/\s*TLong\s*/(TLong, \"/g" \
        | sed "s/\s*TUInt\s*/(TUInt, \"/g" \
        | sed "s/\s*TULong\s*/(TULong, \"/g" \
        | sed "s/\s*TUChar\s*/(TUChar, \"/g" \
        | sed "s/\s*TDouble\s*/(TDouble, \"/g" \
        | sed "s/\s*bool\s*/(Bool, \"/g" \
        | grep --invert-match "};" \
        | grep --invert-match -P "^(?=.*struct)(?=.*;)" \
        | tr ";" "\"" \
        | sed "s/struct\s*/[\"/g" \
        | sed "s/\s*:.*{\s*/\", [/g" > ast.out.1

    echo -n "" > ast.out.2
    while read l ;
    do
        if [[ "${l}" == "["* ]]; then
            echo ${l} >> ast.out.2
        elif [[ "${l}" == "("* ]]; then
            echo ${l}")," >> ast.out.2
        else
            echo ${l}"," >> ast.out.2
        fi
    done < ast.out.1
    rm ast.out.1

    line=""
    echo -n "" > ast.out.1
    while read l;
    do
        if [[ "${l}" == "["* ]]; then
            echo "${line}]]," >> ast.out.1
            line="${l} "
        else
            line="${line} ${l}"
        fi
    done < ast.out.2

    cat ast.out.1 \
        | sed "s/\[  /\[/g" \
        | sed "s/\[ \]/\[\]/g" \
        | sed "s/,\]/\]/g" \
        | sed "s/), \"/)\], \[\"/g" \
        | sed "s/\", (/\"\], \[(/g" \
        | sed "s/\")\]\],/\")\]\, \[\]\],/g" \
        | sed "s/\", \[\"/\", \[\], \[\"/g" \
        | sed "s/\", \[\]\],/\", \[\], \[\]\],/g" \
        | sed "s/nstr_uction/nstruction/g" \
        | sed "s/str_ucture/structure/g" \
        | sed "s/str_uct_decl/struct_decl/g" \
        | grep --invert-match "Ast {  using(" \
        | grep --invert-match "\"Dummy\"" > ast.out.2
    rm ast.out.1

    echo ""
    echo "    # /include/ast/${1}.hpp"
    while read l;
    do
        echo "    ${l}"
    done < <(tail -n +2 ast.out.2)
    rm ast.out.2
}

function data () {
    echo "ast = ["
    parse ast
    parse front_symt
    parse back_symt
    parse front_ast
    parse interm_ast
    parse back_ast
    echo "]"
    echo ""
}

function head () {
    echo "#!/bin/python3"
    echo ""
    echo "class TIdentifier: name = \"TIdentifier\""
    echo "class TChar: name = \"TChar\""
    echo "class TInt: name = \"TInt\""
    echo "class TLong: name = \"TLong\""
    echo "class TUChar: name = \"TUChar\""
    echo "class TUInt: name = \"TUInt\""
    echo "class TULong: name = \"TULong\""
    echo "class TDouble: name = \"TDouble\""
    echo "class Bool: name = \"Bool\""
    echo "class List: name = \"List\""
    echo ""
    echo "\"\"\" AST \"\"\" ############################################################################################################"
    echo ""
}

if [[ "${1}" == "--assert" ]]; then
    data \
        | grep -P "(?=.*    \[\")(?=.*\", \[\], \[\")(?=.*\"\], \[\()(?=.*\)\], \[\]\])"
else
    head > ast.py
    data >> ast.py
fi

exit 0
