#!/bin/bash

FILE=${1}

cat ../include/ast/${FILE}.hpp \
    | grep --invert-match ")" \
    | grep --invert-match "(" \
    | grep -e "struct" -e ";" \
    | sed -e "s/\s*std::vector<TIdentifier>\s*/TIdentifier [/g" - \
    | sed -e "s/\s*std::vector<.*>\s*/\"[/g" - \
    | sed -e "s/\s*std::unique_ptr<.*>\s*/\"/g" - \
    | sed -e "s/\s*std::shared_ptr<.*>\s*/\"/g" - \
    | sed -e "s/\s*TIdentifier\s*/(TIdentifier, \"/g" - \
    | sed -e "s/\s*TInt\s*/(TInt, \"/g" - \
    | sed -e "s/\s*TLong\s*/(TLong, \"/g" - \
    | sed -e "s/\s*TDouble\s*/(TDouble, \"/g" - \
    | sed -e "s/\s*TUInt\s*/(TUInt, \"/g" - \
    | sed -e "s/\s*TULong\s*/(TULong, \"/g" - \
    | grep --invert-match "};" \
    | grep --invert-match -P "^(?=.*struct)(?=.*;)" \
    | tr ";" "\"" \
    | sed -e "s/struct\s*/[\"/g" - \
    | sed -e "s/\s*:.*{\s*/\", [/g" - > tmp

echo -n "" > tmp2
while read l ; 
do
    if [[ "${l}" == "["* ]]; then
        echo ${l} >> tmp2
    elif [[ "${l}" == "("* ]]; then
        echo ${l}")," >> tmp2
    else
        echo ${l}"," >> tmp2
    fi  
done < tmp
rm tmp

line=""
echo -n "" > tmp
while read l ; 
do
    if [[ "${l}" == "["* ]]; then
        echo "${line}]]," >> tmp 
        line="${l} "
    else
        line="${line} ${l}"
    fi  
done < tmp2

cat tmp \
    | sed s/"\[  /\[/g" \
    | sed s/"\[ \]/\[\]/g" \
    | sed s/",\]/\]/g" \
    | sed s/"), \"/)\], \[\"/g" \
    | sed s/"\", (/'\"\], \[(/g" \
    | sed s/"\")\]\],/\")\]\, \[\]\],/g" \
    | sed s/"\", \[\"/\", \[\], \[\"/g" \
    | sed s"/\", \[\]\],/\", \[\], \[\]\],/g" > parse_${FILE}.txt
rm tmp

cat parse_${FILE}.txt
