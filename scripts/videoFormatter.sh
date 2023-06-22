#!/bin/bash

# Path to the folder containing the files
folder_path="videos/videos3"

# Character to add in front of each file name
prefix_character="b_"

# Iterate through the files in the folder
for file_path in "$folder_path"/*
do
    if [ -f "$file_path" ]; then
        # Get the file name without the path
        file_name=$(basename "$file_path")
        
        # Add the prefix character in front of the file name
        new_file_name="${prefix_character}${file_name}"
        
        # Rename the file with the new name
        ffmpeg -i "$file_path" -vcodec libx264 -y "$new_file_name"
        mv "$new_file_name" "$folder_path/$file_name"
        
    fi
done
