

JSON_STR=$(echo $1 | base64 -d)
JSON=$(echo $1 | base64 -d)

USERNAME=$(echo $JSON | grep -o '"user" *: *"[^"]*' | cut -d'"' -f4)
PASSWORD=$(echo $JSON | grep -o '"password" *: *"[^"]*' | cut -d'"' -f4)
DEVICE=$(echo $JSON | grep -o '"device" *: *"[^"]*' | cut -d'"' -f4)

particle login -u $USERNAME -p $PASSWORD
particle flash $DEVICE /lv/game.ino /lv/