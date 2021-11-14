import shutil
import subprocess

doxygen_path = shutil.which("doxygen")

def run_cmd(cmd):
    print(f"Running command: {cmd}")
    subprocess.run(cmd)


def build_documentation():
    cmd = [doxygen_path]
    run_cmd(cmd)


print("Building documentation...")

build_documentation()

print("Built documentation.")
