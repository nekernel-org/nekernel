#! /usr/bin/env python3
# -*- coding: utf-8 -*-

import os
import json
import sys

"""
    Create directory structure for the framework.
"""
def create_directory_structure(base_path_fwrk, project_file_name, project_name):
    # Define the directory structure
    structure = {
        project_name: {
            "headers": {
                ".keep": None
            },
            "dist": {
                ".keep": None
            },
            "src": {
                ".keep": None,
                "DylibMain.cc": None,
            },
            "xml": {
                ".keep": None
            },
            ".keep": None,
            f"{project_file_name}.json": {}
        }
    }

    def create_structure(path, structure_in):
        for name, content in structure_in.items():
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
    os.makedirs(base_path_fwrk, exist_ok=True)
    create_structure(base_path_fwrk, structure)

    # Create the JSON file
    proj_json_path = os.path.join(base_path_fwrk, project_name, f"{project_file_name}.json")

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

    proj_cpp_path = os.path.join(base_path_fwrk, project_name, f"src/DylibMain.cc")

    cpp_file = "#include <libSystem/System.h>\n\nSInt32 _DylibAttach(SInt32 argc, Char* argv[]) {\n\treturn EXIT_FAILURE;\n}"

    with open(proj_cpp_path, 'w') as cpp_file_io:
        cpp_file_io.write(cpp_file)

    xml_blob = f"<PropertyList>\n<PLEntry Type=\"CFString\" Name=\"LibraryName\" Len=\"{len(project_name)}\" Value=\"{project_name}\" /></PropertyList>"
    proj_xml_path = os.path.join(base_path_fwrk, project_name, f"xml/app.xml")

    with open(proj_xml_path, 'w') as cpp_file_io:
        cpp_file_io.write(xml_blob)

if __name__ == "__main__":
    if len(sys.argv) != 2:
        print("HELP: mk_fwrk.py <project_name>")
        sys.exit(os.EX_CONFIG)

    base_path = os.getcwd()  # Use the current working directory as the base path
    create_directory_structure(base_path, sys.argv[1], sys.argv[1] + '.fwrk')

    print("INFO: Framework created successfully.")
