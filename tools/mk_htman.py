#! /usr/bin/env python3
# -*- coding: utf-8 -*-

import sys, os

if __name__ == "__main__":
    if len(sys.argv) != 2:
        print("INFO: mk_htman.py <manual_path>")
        sys.exit(os.EX_CONFIG)

    manual_path = sys.argv[1]
    if not os.path.exists(manual_path):
        print(f"ERROR: Manual path '{manual_path}' does not exist.")
        sys.exit(os.EX_NOINPUT)

    if os.path.isdir(manual_path):
        print(f"ERROR: Manual path '{manual_path}' is a directory.")
        sys.exit(os.EX_NOTDIR)

    if not manual_path.endswith('.man'):
        print(f"ERROR: Manual path '{manual_path}' must end with '.man'")
        sys.exit(os.EX_DATAERR)

    try:
        with open(manual_path, 'r') as file:
            content = file.read()
            if not content.strip():
                print(f"ERROR: Manual file '{manual_path}' is empty.")
                sys.exit(os.EX_DATAERR)
        html_content = f"<html><head><title>NeKernel Manual: {manual_path}</title></head><body><pre>{content}</pre></body></html>"

        html_path = manual_path.replace('.man', '.html')
        
        with open(html_path, 'w') as html_file:
            html_file.write(html_content)
    except IOError as e:
        print(f"ERROR: Could not read manual file '{manual_path}': {e}")
        sys.exit(os.EX_IOERR)

    print(f"INFO: Wrote manual '{manual_path}' to HTML.")
    sys.exit(os.EX_OK)
