#! /usr/bin/env python3
# -*- coding: utf-8 -*-

import os
import json
import sys

def create_directory_structure(base_path, project_name):
    # Define the directory structure
    structure = {
        project_name: {
            "dist": {
                ".keep": None
            },
            "src": {
                ".keep": None,
                "CommandLine.cc": None,
            },
            "vendor": {
                ".keep": None
            },
            ".keep": None,
            f"{project_name}.json": {}
        }
    }

    def create_structure(path, structure):
        for name, content in structure.items():
            current_path = os.path.join(path, name)
            # Create directories or files based on the content type
            if isinstance(content, dict) and current_path.endswith(".json") == False:
                os.makedirs(current_path, exist_ok=True)
                create_structure(current_path, content)
            elif content is None:
                # Create an empty file
                with open(current_path, 'w') as f:
                    pass

    # Create the base directory
    os.makedirs(base_path, exist_ok=True)
    create_structure(base_path, structure)

    # Create the JSON file
    proj_json_path = os.path.join(base_path, project_name, f"{project_name}.json")

    manifest = {
        "compiler_path": "clang++",
        "compiler_std": "c++20",
        "headers_path": ["./", "../../../dev/kernel", "../../../public/frameworks/", "../../../dev/", "./"],
        "sources_path": [
            
        ],
        "output_name": f"./dist/{project_name}",
        "cpp_macros": [
            "kSampleFWVersion=0x0100",
            "kSampleFWVersionHighest=0x0100",
            "kSampleFWVersionLowest=0x0100",
            "__NE_SDK__"
        ]
    }
    
    with open(proj_json_path, 'w') as json_file:
        json.dump(manifest, json_file, indent=4)

    proj_cpp_path = os.path.join(base_path, project_name, f"src/CommandLine.cc")

    cpp_file = "#include <user/SystemCalls.h>\n\nSInt32 _NeMain(SInt32 argc, Char* argv[]) {\n\treturn EXIT_FAILURE;\n}"

    with open(proj_cpp_path, 'w') as cpp_file_io:
        cpp_file_io.write(cpp_file)

if __name__ == "__main__":
    if len(sys.argv) != 2:
        print("Usage: mk_app.py <project_name>")
        sys.exit(os.EX_CONFIG)

    base_path = os.getcwd()  # Use the current working directory as the base path
    create_directory_structure(base_path, sys.argv[1])
    
    print("NeKernel Application created successfully.")
