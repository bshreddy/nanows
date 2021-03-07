#include "request.h"

int main() {
    request *req = _initialize_request();
    // request *req = parse_request(NULL);
    // print_request(req);
    _free_request(req);
    
    return 0;
}