#!/bin/sh
# Set Session Name

SESSION="laby"

SESSIONEXISTS=$(tmux list-sessions | grep $SESSION)

  

# Only create tmux session if it doesn't already exist

if [ "$SESSIONEXISTS" = "" ]
then
    # Start New Session with our name
    tmux new-session -d -s $SESSION  
    # Name first Pane and start zsh
    tmux rename-window -t 0 'RTSP'
    # split window vertically to panels 0 and 1
    tmux split-window -h
    # split panel 1 horizontally to panels 1 and 2 (left to right)
    tmux split-window -v
    # split panel 1 horizontally to panels 2 and 3 (left to right)
    tmux split-window -v

    # execute rtsp
    tmux send-keys -t 0 "bash rtsp.sh" C-m

    # run datetime script
    tmux send-keys -t 1 "sleep 1 && bash streamVideo.sh video4.mp4" C-m
    tmux send-keys -t 2 "    sleep 1 && ffmpeg -re -stream_loop -1 -i saigon.mp4 -vcodec libx264 -bf 0  -c copy -f rtsp rtsp://localhost:8554/in1" C-m
    tmux send-keys -t 3 "sleep 1 &&  ffmpeg -re -stream_loop -1 -i vietnam.mp4 -vcodec libx264 -bf 0  -c copy -f rtsp rtsp://localhost:8554/in2" C-m

    window=2
    tmux new-window -t $SESSION:$window -n 'Main'
    # split window vertically to panels 0 and 1
    tmux split-window -h
    # split panel 1 horizontally to panels 1 and 2 (left to right)
    tmux split-window -v

    # execute rtsp
    tmux send-keys -t 0 "sleep 2 && ./build/bin/main" C-m

    # run datetime script
    tmux send-keys -t 1 "sleep 2 && ./build/bin/main -f ../otherconfigs/config1.yaml" C-m
    tmux send-keys -t 2 "sleep 2 && ./build/bin/main -f ../otherconfigs/config2.yaml" C-m

    tmux attach-session -t ses-0
fi

# Attach Session, on the Main window
tmux attach-session -t $SESSION:0