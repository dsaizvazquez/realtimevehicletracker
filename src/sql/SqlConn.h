#pragma once
#include <mysql_connection.h>
#include <mysql_driver.h>

#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>
#define FMT_HEADER_ONLY
#include <fmt/format.h>

#define TOP 0
#define CENTER 1
#define BOTTOM 2


struct PositionDatabase {
    int num;
    int xpx;
    int ypx;
    double x;
    double y;
    double vx;
    double vy;    
    int class_id;
};


struct TargetDatabase {
    float height;
    float roll;
    float pitch;
    float yaw;
    int carLane;
    std::vector<PositionDatabase> positions;
};





class SqlConn{
    private:
        sql::Driver *driver;
        sql::Connection *con;
        sql::Statement *stmt;

        std::string insertTargetQuery = "INSERT INTO targets (height, roll, pitch, yaw,car_lane) VALUES ({},{},{},{},{});";
        std::string insertPositionQuery = "INSERT INTO positions(target_id, xpx, ypx, x, y, vx, vy,num,class_id) VALUES ({},{},{},{},{},{},{},{},{})";
        std::string selectLastId = "SELECT LAST_INSERT_ID() AS id;";
    public:
        int insertTarget(TargetDatabase target);
        int insertPosition(int targetId, PositionDatabase position);
        SqlConn(const sql::SQLString& hostName, const sql::SQLString& userName, const sql::SQLString& password,const sql::SQLString& schema);

};
