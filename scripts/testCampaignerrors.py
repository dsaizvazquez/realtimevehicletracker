import os
import subprocess
import time

# Config
folder_path = "videos/videos4"
study_lane = "5"



# Get a list of all the video files in the folder
video_files = [f for f in os.listdir(folder_path) if os.path.isfile(os.path.join(folder_path, f))]
f = open("used_h.txt", "r")
content = f.read()
f.close()
f = open("used_h.txt", "a")
# Iterate through the video files
for video_file in video_files:
    video_path = os.path.join(folder_path, video_file)
    print(video_path)
    output_url = "rtsp://localhost:8554/in1"
    if video_file not in content:
        video_splice = video_file.split("_")
        print(video_splice)
        pitch =video_splice[1]
        yaw=video_splice[2]
        altitude=video_splice[3]
        lane =video_splice[4].split(".")[0]
        print(lane)
        #if lane ==  study_lane:
            # execute testCampaign cpp to send the message
        cpp_command = ['./build/bin/testCampaign', '-p', pitch, '-y', yaw, "-a", altitude,"-l",lane, "-o", "8999"]
        cpp_process = subprocess.Popen(cpp_command)
        time.sleep(5)

        print(f"Sending data: {pitch},{yaw},{altitude},{lane}")
        if cpp_process.wait()!= 1:
            # Start ffmpeg subprocess to stream the video
            ffmpeg_command = ['ffmpeg', '-re',"-stream_loop", "-1", '-i', video_path, '-vcodec', 'libx264', "-c", "copy", "-f","rtsp", output_url]
            ffmpeg_process = subprocess.Popen(ffmpeg_command)
            
            print(f"Streaming {video_file}")
            
            # Wait for 20 seconds
            time.sleep(80)
            
            # Terminate the ffmpeg subprocess after 20 seconds
            ffmpeg_process.terminate()
            ffmpeg_process.wait()
            
            print(f"Streaming of {video_file} finished.")
            f.write(video_file+"\n")
f.close()
        
