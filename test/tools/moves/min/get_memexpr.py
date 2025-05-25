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
            for x in finditer(r"std::unique_ptr<[_a-zA-Z]\w*\b> ", line.rstrip()):
                print(x.group())
                #id_set.add(x.group())

#for id in id_set:
#    if len(id) > MAX_LEN:
#        print(id)
