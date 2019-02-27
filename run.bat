cd Debug
start ServerPrototype.exe
timeout /t 4
cd "../WpfAppClient/bin/x86/Debug"
start WpfAppClient.exe
timeout /t 60
start WpfAppClient.exe -port 8083