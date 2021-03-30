import subprocess
import os

glsl_compiler = "glslc"
shader_dir = os.path.dirname(os.path.realpath(__file__))


def compile_shader_stage(filename):
    # Only compile files that end with glsl
    if filename.rsplit('.', 1)[-1] != "glsl":
        return
    print("Compiling shader: {}".format(filename))
    base_filename = os.path.basename(filename)
    is_vert = "vert" in base_filename
    shader_type = "vert" if is_vert else "frag"
    new_spv_filename = "{}.{}.spv".format(base_filename.split('.')[0], shader_type)
    cmd = [
        glsl_compiler,
        "-fshader-stage={}".format("vert" if is_vert else "frag"),
        filename,
        "-o",
        new_spv_filename
    ]
    print("\tCreated spv file: {}".format(new_spv_filename))
    subprocess.call(cmd)


print("Compiling Shaders!")
os.chdir(shader_dir)
for i in os.listdir("."):
    compile_shader_stage(i)

