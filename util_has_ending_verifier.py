#This program verifies that all the atlas-script scripts have <ENDING in them

from os import listdir, getcwd
from os.path import isfile, join
path = getcwd() + '\\atlas-scripts'
pathfiles = [path + '\\' + f for f in listdir(path) if isfile(join(path, f))]
for filename in pathfiles:
    with open(filename, 'r', encoding="utf-8") as f:
        if not "<ENDING" in f.read():
            print(filename)