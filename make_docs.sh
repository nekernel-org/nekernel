# !/bin/zsh

mkdir -p html

XSDocgen --source ./dev/ --output ./html --undocumented --project-name "ZKA MicroKernel" --enable-c++ --project-copyright "ELMH Group &copy; %Y - All rights Reserved" --project-version "930.2024.1" --company-name "ELMH Group"
