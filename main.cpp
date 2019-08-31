#include <iostream>
#include "CEM133Collector.h"

using namespace std;

int main(int argc, char *argv[]){
    cout << "This is EM133 collector program." << endl;
    CEM133Collector dev1;
    dev1.SetUpTCPServer("169.254.142.221", 502);
    return 0;
}
