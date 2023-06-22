#include "SqlConn.h"


SqlConn::SqlConn(const sql::SQLString& hostName, const sql::SQLString& userName, const sql::SQLString& password,const sql::SQLString& schema ){
      sql::ConnectOptionsMap connection_properties;
      connection_properties["hostName"] = hostName;
      connection_properties["userName"] = userName;
      connection_properties["password"] = password;
      connection_properties["schema"] = schema;
      connection_properties["OPT_CONNECT_TIMEOUT"] = 10;
      driver = get_driver_instance();
      con = driver->connect(connection_properties);
      stmt = con->createStatement();

}

int SqlConn::insertPosition(int targetId, PositionDatabase position){
      std::string query = fmt::format(insertPositionQuery,targetId,position.xpx,position.ypx,position.x,position.y,position.vx, position.vy,position.num,position.class_id);
      stmt->execute(query);
      sql::ResultSet *res = stmt->executeQuery(selectLastId);
      res->next();
      return res->getInt("id");
}

int SqlConn::insertTarget(TargetDatabase target){
      std::string query = fmt::format(insertTargetQuery, target.height,target.roll, target.pitch, target.yaw, target.carLane);
      stmt->execute(query);
      sql::ResultSet *res = stmt->executeQuery(selectLastId);
      res->next();
      int targetId =res->getInt("id");
      for (auto pos : target.positions){
        insertPosition(targetId,pos);
      }
      return targetId;
}