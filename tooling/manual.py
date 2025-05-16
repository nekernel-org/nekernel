#! /usr/bin/env python3
# -*- coding: utf-8 -*-

import sys, os

if __name__ == "__main__":
    if len(sys.argv) != 2:
        print("Usage: manual.py <manual_path>")
        sys.exit(os.EX_CONFIG)
