FERRET>FNet
====================

Network Object for Ferret that facilitates TCP/IP based communication. It can do the following currently:

- Connect or disconnect to Wi-Fi
- Instantiate a socket client or server
- Send Messages over TCP/IP

### Usage

Basic example for sending messages over sockets. 

configuration can be changed via `idf.py menuconfig`

```c++
#include<FNet.h>

FNet fnet;
FNet.wifiStart();

FNet::SocketClient fsock;
fsock.start();

for(int i = 0; i < 5; i++){
    fsock.send("Hello World!");
    vTaskDelay(500);
}

fsock.close();
FNet.wifiClose();
```

