import sys
import re
from os import walk
import os
import fnmatch

def find_acronyms(source_path: str) -> [str]:
    result = []
    with open(source_path) as f:
        while line := f.readline():
            s = re.search(r'\{([A-Z]*)\}', line)
            if s != None:
                result.append(s.group(1))
    return result

def find_in_file(tokens: [str], file_path: str) -> None :
    with open(file_path) as f:
        line_num = 1
        print(file_path + ":")
        while line := f.readline():
            hit = False
            for token in tokens:
                s = re.search(token, line)
                if s != None:
                    line = re.sub(token, '\033[96m' + token + '\033[0m', line)
                    hit = True

            if hit == True:
                print("{}: {}".format(line_num, line.rstrip()))
            line_num += 1

def find_in_directory(tokens: [str], path: str) -> None:
    exclude_list = ["03_glossary.tex", "04_acronyms.tex"]
    matches = []
    for root, dirnames, filenames in os.walk(path):
        for filename in fnmatch.filter(filenames, '*.tex'):
            if filename not in exclude_list:
                fn = (os.path.join(root, filename))
                find_in_file(tokens, fn)

def main() -> None:
    print("Arguments: <path to acronym file> <path to file/directory of file to check>")
    find_in_directory(find_acronyms(sys.argv[1]), sys.argv[2])

if __name__ == '__main__':
    main()
