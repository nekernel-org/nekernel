# !/bin/zsh

mkdir -p html

XSDocgen --source ./dev/ --output ./html --undocumented --project-name "ZKA CoreFoundation" --enable-c++ --project-copyright "EL Mahrouss Logic &copy; %Y - All rights Reserved" --project-version "930.2024.1" --company-name "EL Mahrouss Logic"
