from sys import argv
from os import listdir
from os.path import isfile, join
from re import finditer

MAX_LEN=31

filenames = []
def listfiles(dir):
    for file in listdir(dir):
        file = "/".join((dir, file))
        if isfile(file):
            filenames.append(file)
        else:
            listfiles(file)

if len(argv) > 1:
    filenames = [argv[1]]
else:
    listfiles("../../include")
    listfiles("../../src")

id_set = set({})
for filename in filenames:
    with open(filename) as file:
        for line in file:
            for x in finditer(r"[a-zA-Z_]\w*\b", line.rstrip()):
                id_set.add(x.group())

lt_max_id_set = set({})
signif_id_set = set({})
n_uniq_id_set = set({})
for id in id_set:
    if len(id) > MAX_LEN:
        lt_max_id_set.add(id)
        signif_id = id[:MAX_LEN]
        if signif_id in signif_id_set:
            n_uniq_id_set.add(id)
        signif_id_set.add(signif_id)

print(f"Larger than maxlen ids: {len(lt_max_id_set)}")
print(f"Unique significant ids: {len(signif_id_set)}")
print(f"Ids with conflicting significant section:\n")
for id in sorted(n_uniq_id_set):
    print(id)
