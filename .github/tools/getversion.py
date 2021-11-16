import os
import re

makefile_dir = os.path.join(os.getcwd(), "CMakeLists.txt")

file_contents = ""

with open(makefile_dir) as f:
    file_contents = f.read()

match = re.search(r'\"SnowMeltArcade\.ProjectBirdRacing\" VERSION (\d+\.\d+\.\d+)', file_contents)

version = match.group(1)

print(version)
