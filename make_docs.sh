# !/bin/zsh

mkdir -p html

XSDocgen --source ./dev/ --output ./html --undocumented --project-name "ZKA MicroKernel" --enable-c++ --project-copyright "TQ B.V &copy; %Y - All rights Reserved" --project-version "930.2024.1" --company-name "TQ B.V"
