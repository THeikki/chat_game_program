#include "./header_files/server.h"
#include "./header_files/client.h"
#include "./header_files/utility.h"
#include "./header_files/globals.h"

int main() {

    initialize_winsock();
    create_server();
   
    return 0;
}
