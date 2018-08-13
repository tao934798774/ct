#include"comm.h"

void test(){
    std::cout << "client version" << mysql_get_client_info() << std::endl;
}
