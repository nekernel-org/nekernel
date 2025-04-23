#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import os
import sys
import subprocess
import glob

def copy_to_fat(image_path, source_dir):
    if not os.path.isfile(image_path):
        print(f"Error: FAT32 image {image_path} does not exist.")
        sys.exit(1)

    if not os.path.isdir(source_dir):
        print(f"Error: {source_dir} is not a valid directory.")
        sys.exit(1)

    try:
        files_to_copy = glob.glob(os.path.join(source_dir, "*"))
        
        if not files_to_copy:
            print(f"Warning: No files found in {source_dir}. Nothing to copy.")
            return
        
        command = ["mcopy", "-spm", "-i", image_path] + files_to_copy + ["::"]
        subprocess.run(command, check=True)
        
        print(f"Info: Successfully copied contents of '{source_dir}' into '{image_path}'")
    except FileNotFoundError:
        print("Error: mcopy is not installed. Please install mtools.")
        sys.exit(1)
    except subprocess.CalledProcessError as e:
        print(f"Error: mcopy failed with error code {e.returncode}.")
        sys.exit(1)
    except Exception as e:
        print(f"Error: mcopy: {e}")
        sys.exit(1)

if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("Usage: mk_img.py <fat32_image> <source_directory>")
        sys.exit(1)

    image_path = sys.argv[1]
    source_dir = sys.argv[2]

    copy_to_fat(image_path, source_dir)