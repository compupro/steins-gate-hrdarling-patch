#This program turns \r\r\n into \r\n in atlas-script scripts
#I use this because my build of topographer keeps outputting \r\r\n
#which, of course, confuses Atlas

from os import listdir, getcwd
from os.path import isfile, join
path = getcwd() + '\\atlas-scripts'
pathfiles = [path + '\\' + f for f in listdir(path) if isfile(join(path, f))]
for filename in pathfiles:
    with open(filename, 'rb') as f:
        newText = f.read().replace(b'\r\r\n', b'\r\n')
    with open(filename, 'wb') as f:
        f.write(newText)