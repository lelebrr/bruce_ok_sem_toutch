import datetime
import os


def generate_build_header():
    year = datetime.datetime.now().year
    header_content = f'#pragma once\n#define CURRENT_YEAR {year}\n'

    output_path = os.path.join("include", "current_year.h")
    os.makedirs("include", exist_ok=True)

    # Check if the file already exists and has the same content
    if os.path.exists(output_path):
        with open(output_path, "r") as f:
            existing_content = f.read()
        if existing_content == header_content:
            return  # No need to rewrite

    with open(output_path, "w") as f:
        f.write(header_content)


def before_build(source, target, env):
    generate_build_header()


# Hook for PlatformIO
def on_pre_build(env):
    before_build(None, None, env)


# Register the pre-build hook
try:
    from SCons.Script import Import
    env = Import("env")
    on_pre_build(env)
except ImportError:
    # Fallback for when SCons is not available (e.g., during linting)
    pass
