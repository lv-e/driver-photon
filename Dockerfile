
FROM lvedock/lve_runtime

# compiler setup
    
RUN npm i -g particle-cli@"<3.0.0"

# copy files from source to 
COPY engine/lv-engine /lv/lv-engine
COPY src/lv-driver /lv/lv-driver