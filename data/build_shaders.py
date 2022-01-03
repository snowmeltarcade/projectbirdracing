import os
import shutil
import subprocess

glslc_path = shutil.which("glslc")

def run_cmd(cmd):
    print(f"Running command: {cmd}")

    try:
        subprocess.run(cmd)
    except Exception as error:
        print(f"Error running command: {error}")


def build():
    compile_shader("vertex.vert")
    compile_shader("fragment.frag")


def compile_shader(file_name):
    shader_path = os.path.join(os.getcwd(), file_name)
    output_path = shader_path + ".spv"

    cmd = [glslc_path, shader_path, "-o", output_path]
    run_cmd(cmd)


print("Building...")

build()

print("Built.")
