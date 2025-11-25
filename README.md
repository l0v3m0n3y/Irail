# Irail
api for irail.be site provides access to Belgium's railway data, including train schedules, stations, liveboards, connections, vehicles, and disturbances
# main
```cpp
#include "Irail.h"
#include <iostream>

int main() {
   Irail api;
    auto stations = api.get_stations().then([](json::value result) {
        std::cout << result<< std::endl;
    });
    stations.wait();
    
    return 0;
}
```

# Launch (your script)
```
g++ -std=c++11 -o main main.cpp -lcpprest -lssl -lcrypto -lpthread -lboost_system -lboost_chrono -lboost_thread
./main
```
