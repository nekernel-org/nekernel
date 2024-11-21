# !/bin/zsh

mkdir -p html

XSDocgen --source ./dev/ --output ./html --undocumented --project-name "ZKA MicroKernel" --enable-c++ --project-copyright "Amlal EL Mahrouss &copy; %Y - All rights Reserved" --project-version "930.2024.1" --company-name "Amlal EL Mahrouss"
