#include "../../src/sql/SqlConn.h"


using namespace std;


int main() {
    // Connect to the database
    
    SqlConn conn("tcp://127.0.0.1:3306", "root", "XXXXXXXXX","RTVT");
    TargetDatabase target = {5.0, 0.0, 0.0, 0.0,-1, {{1, 100, 200, 3000.0, 0.0, 0.0, 0.0,0}, {2, 150, 250, 0.0, 0.0, 0.0, 0.0,1}}};
    std::cout << conn.insertTarget(target)<<std::endl;
}