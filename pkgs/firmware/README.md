# firmware

## Setup

1. `cp ./include/Secrets.h.example ./include/Secrets.h` and fill in the needed values

## misc

the `index.html`, `main.js` and other static files are stored in the `/data` folder, this is used by PlatformIO to upload small files to the esp
-> click `upload filesystem image` 
-> uses the built-in flash memory of the esp
-> you need to upload it separately, because when clicking on the regular upload, only the compiled code gets uploaded
