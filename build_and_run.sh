make

XEPHYR=$(whereis -b Xephyr | cut -f2 -d' ')
xinit ./steveWM -- \
    "$XEPHYR" \
        :98 \
        -screen 1280x720