import os
import subprocess
import shutil
import re

REGEX = r'([a-zA-Z]:[\\\/](?:[a-zA-Z0-9]+[^WINDOWS][\\\/])+.+\.dll\b)'# (.+\.dll)'

if __name__ == "__main__":
    executable_path = input("Executable Path: ")
    # Dynamically locating the required files from "ntldd"
    output = subprocess.check_output(f"ntldd {executable_path}", shell=True).decode("utf-8")

    # file_paths = output.replace('\n', '').replace('\r', ' ').replace('\t','')
    # print(file_paths)

    file_paths = re.findall(REGEX, output)
    # print(file_paths)
    
    # Generating the required_libraries folder space for DLL file
    if not os.path.exists("required_libraries"):
        os.mkdir("required_libraries")

    build_path = os.getcwd() + "\\required_libraries"
    for filepath in file_paths:
        print("Retrieving " + filepath)
        try:
            shutil.copy2(filepath, build_path)
        except PermissionError:
            print("File already exists at build path")
