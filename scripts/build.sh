

JSON_STR=$(echo $1 | base64 -d)
JSON=$(echo $1 | base64 -d)

USERNAME=$(echo $JSON | grep -o '"user" *: *"[^"]*' | cut -d'"' -f4)
PASSWORD=$(echo $JSON | grep -o '"password" *: *"[^"]*' | cut -d'"' -f4)
DEVICE=$(echo $JSON | grep -o '"device" *: *"[^"]*' | cut -d'"' -f4)

lv-cli log -i "cloud login for $DEVICE"
particle login -u $USERNAME -p $PASSWORD

lv-cli log -i "now compiling..."
particle flash $DEVICE /lv/game.ino /lv/

lv-cli log -i "ready"