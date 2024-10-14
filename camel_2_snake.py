#!/usr/bin/env python3

# This script changes variables names in C++ files from
# camelCase style to snake_case style, with corresponding
# naming conventions observed (e.g. "bIsFoo" => "is_foo")
#
#
# Limitations caused by lack of syntax awareness:
#  a. despite best efforts (see besteffort_* below), in corner cases it leaves some
#     variables initialized by ()-style expression untouched if the variable does
#     not end with an underscore, as it thinks the variable is a function:
#       what it does: variableName(initArgument) => variableName(init_argument)
#       it should do: variableName(initArgument) => variable_name(init_argument)
#  b. it replaces function name if it is used as a pointer, as it thinks the function
#     is a variable:
#       what it does: int (*funcPtr)(int, int) = &addInts; => int (*func_ptr)(int, int) = &add_ints;
#       it should do: int (*funcPtr)(int, int) = &addInts; => int (*func_ptr)(int, int) = &addInts;
#  c. it does not check name collision:
#       what it does: bool isGreek, bIsGreek; => bool is_greek, is_greek;
#       it should do: bool isGreek, bIsGreek; => bool is_greek, is_greek_2;

import os, sys
import re
import argparse

REGEX_PIECE_1 = r"(\A|(?<=\W))([a-jl-z]|[a-z]{2,})([A-Z][a-z]*|[0-9]+)+_?(?=[^\w\(]|$)"
REGEX_PIECE_2 = r"(\A|(?<=\W))([a-jl-z]|[a-z]{2,})([A-Z][a-z]*|[0-9]+)+_(?=\()"
REGEX_PIECE_IN_CTOR_INIT = r"(\A|(?<=\)\s:\s|\S\),\s)|(?<=\A:\s|\s\s))[a-z]+([A-Z][a-z]*|[0-9]+)+(?=\()"
DROMEDARY_CAMEL_CASE_VAR = re.compile(REGEX_PIECE_1 + "|" + REGEX_PIECE_2)
DROMEDARY_CAMEL_CASE_VAR_IN_CTOR_INIT = re.compile(
    REGEX_PIECE_1 + "|" + REGEX_PIECE_2 + "|" + REGEX_PIECE_IN_CTOR_INIT)

BOOLEAN_PREFIXES = [
    "is", "are", "was", "were",
    "has", "have", "had",
    "does", "do", "did", "done",
    "find", "found", "get", "got"
]
COMMON_ABBREVIATIONS = {
    # NOTE no "obj", "num", "it", "iter", "var", "src", "dest",
    #         "ret", "init", "ptr", "op", "db"
    "res" : "result",   "buf" : "buffer", "vec" : "vector",  "msg"  : "message",  
    "seq" : "sequence", "cnt" : "count",  "mem" : "memory",  "val"  : "value",  
    "loc" : "location", "ans" : "answer", "ctx" : "context", "elem" : "element",
    "ty"  : "type",
}

CAMEL_CASE_PIECE_REGEX = re.compile(r"[A-Z]?[a-z]+|[A-Z]+(?=[A-Z]|[0-9]|_|$)|[0-9]+|\_")
def compute_snake_case(camel_case, testing=False):
    splitted_words = CAMEL_CASE_PIECE_REGEX.findall(camel_case)
    if testing:
        print("%-15s => %s" % (camel_case, splitted_words))
    splitted_words = list(map(lambda w : w.lower(), splitted_words))
    ends_with_underscore = False
    if splitted_words[-1] == '_':
        splitted_words = splitted_words[:-1]
        ends_with_underscore = True
    assert len(splitted_words) >= 2
    # special rules in conversion: observe naming conventions for snake_case 
    had_hungarian_prefix = False
    if ((splitted_words[0] == "p" or splitted_words[0] == "m"
        or splitted_words[0] == "n" or splitted_words[0] == "f")
        and splitted_words[1].isalpha()):
        had_hungarian_prefix = True
        splitted_words = splitted_words[1:]
    if splitted_words[0] == "b" and splitted_words[1].isalpha():
        if splitted_words[1] in BOOLEAN_PREFIXES:
            splitted_words = splitted_words[1:]
        else:
            splitted_words = [ "is" ] + splitted_words[1:]
    if splitted_words[0] == "it":
        splitted_words = [ "iter" ] + splitted_words[1:]
    if splitted_words[-1] == "num":
        splitted_words = splitted_words[:-1] + [ "number" ]
    splitted_words = [ COMMON_ABBREVIATIONS.get(w, w) for w in splitted_words ]
    # make snake_case
    if not ((len(splitted_words) >= 1 if had_hungarian_prefix else len(splitted_words) >= 2)
            and splitted_words[0].isalpha()):
        raise RuntimeError("'%s' => %s" % (camel_case, splitted_words))
    snake_case = '_'.join(splitted_words) + ('_' if ends_with_underscore else '')
    return snake_case

MAX_LOOP_STEPS = 16 # unlikely to have more than this number of camelCase variables in one line
def process_one_line(old_line, in_ctor_init_list=False, testing=False):
    step_count, instance_count = 0, 0
    line = old_line
    while True:
        step_count += 1
        if step_count > MAX_LOOP_STEPS:
            raise RuntimeError("maximum loop steps (%d) exceeded, line:\n%s" % (
                MAX_LOOP_STEPS, old_line))
        regex_obj = DROMEDARY_CAMEL_CASE_VAR
        if in_ctor_init_list:
            regex_obj = DROMEDARY_CAMEL_CASE_VAR_IN_CTOR_INIT
        matchObj = re.search(regex_obj, line)
        if not matchObj:
            return line, instance_count
        camel_case_var = matchObj.group(0)
        camel_case_var_start, camel_case_var_end = matchObj.start(), matchObj.end()
        instance_count += 1
        snake_case_var = compute_snake_case(camel_case_var, testing)
        line = line[:camel_case_var_start] + snake_case_var + line[camel_case_var_end:]

def process_one_file(filepath, handling_dir, echo, rewrite):
    with open(filepath, 'r') as f:
        raw_lines = f.readlines()
    instance_count, new_lines = 0, []
    if echo:
        print() # newline
    # best effort of determing whether "foo(a)" is a ()-style initialization or a function call
    besteffort_in_ctor_init_list = False
    for i, raw_line in enumerate(raw_lines):
        old_line = raw_line.rstrip()
        ### check if in ctor initializer list (1)
        besteffort_index_of_colon = old_line.find(": ") # -1 if absent
        if besteffort_index_of_colon >= 0:
            if ((besteffort_index_of_colon >= 2
                 and old_line[besteffort_index_of_colon - 2:besteffort_index_of_colon] == ") "
                 and " ?" not in old_line[:besteffort_index_of_colon])
                or (i >= 1 and len(old_line[:besteffort_index_of_colon].strip()) == 0
                 and raw_lines[i - 1].rstrip().endswith(")")
                 and (" ?" not in raw_lines[i - 1]))):
                besteffort_in_ctor_init_list = True
        ### essential works
        new_line, instance_count_in_line = process_one_line(
            old_line, besteffort_in_ctor_init_list)
        if echo:
            ctor_init_list_mark = "\x1b[48;5;237m" if besteffort_in_ctor_init_list else "\x1b[0m"
            if instance_count_in_line > 0:
                print("-|\x1b[0m" + ctor_init_list_mark
                    + "\x1b[35m" + old_line + "\x1b[0m")
                print("+|\x1b[0m" + ctor_init_list_mark
                    + "\x1b[32m" + new_line + "\x1b[0m")
            else:
                print(" |\x1b[0m" + ctor_init_list_mark + old_line + "\x1b[0m")
        new_lines.append(new_line)
        instance_count += instance_count_in_line
        ### check if in ctor initializer list (2)
        if besteffort_in_ctor_init_list:
            besteffort_index_of_open_brace = old_line.find(" {") # -1 if absent
            if besteffort_index_of_open_brace >= 0:
                besteffort_in_ctor_init_list = False
    if rewrite and instance_count:
        with open(filepath, 'w') as f:
            f.write('\n'.join(new_lines) + "\n\n")
    if not rewrite and not echo and not handling_dir:
        sys.stderr.write("--- begin : %s ---\n" % filepath)
        sys.stdout.write('\n'.join(new_lines) + "\n\n")
        sys.stderr.write("--- end : %s ---\n" % filepath)
    return instance_count

def is_c_cxx(filename):
    if (filename.endswith(".h") or filename.endswith(".cc")
        or filename.endswith(".cpp") or filename.endswith(".c")):
       return True
    return False

def work(args):
    if args.test != None:
        new_line, _ = process_one_line(args.test, testing=True)
        print("\x1b[32;m" + new_line + "\x1b[0m")
        return
    files_to_read, handling_dir = [], False
    if os.path.isfile(args.path):
        files_to_read = [ args.path ]
    else:
        assert os.path.isdir(args.path)
        handling_dir = True
        for (dirpath, dirnames, filenames) in os.walk(args.path, followlinks=True):
            for filename in filter(lambda f : is_c_cxx(f), filenames):
                if (((os.sep + "test-inputs") in dirpath)
                    or ((os.sep + "third-party") in dirpath)
                    or ((os.sep + "linters") in dirpath)):
                    continue
                files_to_read.append(os.path.join(dirpath, filename))
    processed_instances_sum = 0
    for filepath in files_to_read:
        sys.stderr.write("%s .." % (filepath))
        sys.stderr.flush()
        processed_instances = process_one_file(
            filepath, handling_dir=handling_dir, echo=args.echo, rewrite=args.rewrite)
        processed_instances_sum += processed_instances
        sys.stderr.write("\r%s count: %d\n" % (filepath, processed_instances))
        sys.stderr.flush()
    if not args.rewrite:
        if handling_dir:
            sys.stderr.write("file count: %d, instance count: %d\n" % (
                len(files_to_read), processed_instances_sum))
        sys.stderr.write("\nTo rewrite files, use '--rewrite'; to echo lines, use '--echo'\n")
    return 0

def main():
    parser = argparse.ArgumentParser(description="C/C++ vairable name camelCase => snake_case")
    parser.add_argument("path", nargs='?', default=None,
                        help="file or directory")
    parser.add_argument("--rewrite", action="store_true",
                        help="rewrite visited C/C++ files [[caution advised]]")
    parser.add_argument("-e", "--echo", action='store_true',
                        help="echo each line, before and after")
    parser.add_argument("-t", "--test", metavar="\"..\"", type=str, default=None,
                        help="(dev) test one line in \"..\"")
    args = parser.parse_args()
    has_error = False
    if args.test == None and args.path == None:
        has_error = True
        sys.stderr.write("[Error] you need to give argument 'path' or use option '--test'\n")
    if args.test != None and (args.path != None or args.rewrite or args.echo):
        has_error = True
        sys.stderr.write("[Error] '--test \"..\"' can only be used alone, but you gave something else too\n")
    if args.test == None and (args.rewrite or args.echo) and args.path == None:
        has_error = True
        sys.stderr.write("[Error] you need to give the path argument while using '--rewrite' or '--echo'\n")
    if args.path != None and not os.path.exists(args.path):
        has_error = True
        sys.stderr.write("[Error] not found: %s\n" % args.path)
    return 1 if has_error else work(args)

if __name__ == "__main__":
    sys.exit(main())
