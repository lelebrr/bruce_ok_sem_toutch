import os
import re

# Directory containing the menu item files
menu_dir = "src/core/menu_items"

# Files that have already been fixed
fixed_files = [
    "BleMenu.cpp",
    "ClockMenu.cpp",
    "FMMenu.cpp",
    "FileMenu.cpp",
    "RFIDMenu.cpp",
    "ConnectMenu.cpp"
]

# Pattern to find drawImg calls that need to be fixed
pattern1 = r'drawImg\(\s*\*bruceConfig\.themeFS\(\)\s*,'
replacement1 = 'drawImgFromFS(\n        *bruceConfig.themeFS(),'

# Pattern to find drawImg calls with SD or LittleFS
pattern2 = r'drawImg\(\s*(?:SD|LittleFS)\s*,'
replacement2 = 'drawImgFromFS(\n        '

# Process remaining files
for filename in os.listdir(menu_dir):
    if filename.endswith(".cpp") and filename not in fixed_files:
        filepath = os.path.join(menu_dir, filename)
        print(f"Processing {filename}...")

        with open(filepath, 'r', encoding='utf-8') as f:
            content = f.read()

        # Fix themeFS calls
        content = re.sub(pattern1, replacement1, content)

        # Fix SD/LittleFS calls
        content = re.sub(pattern2, replacement2, content)

        # Replace drawImg with drawImgFromFS
        content = content.replace('drawImg(', 'drawImgFromFS(')

        with open(filepath, 'w', encoding='utf-8') as f:
            f.write(content)

print("Fixed all remaining files!")
