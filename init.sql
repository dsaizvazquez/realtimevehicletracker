DROP DATABASE RTVT_err;
CREATE DATABASE RTVT_err;
USE RTVT_err;
CREATE TABLE targets (
  id INTEGER NOT NULL AUTO_INCREMENT,
  height FLOAT,
  roll FLOAT,
  pitch FLOAT,
  yaw FLOAT,
  car_lane INT,
  PRIMARY KEY (id)
);

CREATE TABLE positions (
  id INTEGER NOT NULL AUTO_INCREMENT,
  target_id INTEGER,
  xpx FLOAT,
  ypx FLOAT,
  x FLOAT,
  y FLOAT,
  vx FLOAT,
  vy FLOAT,
  num INTEGER,
  class_id INT,

  PRIMARY KEY (id),
  FOREIGN KEY (target_id) REFERENCES targets(id)
)
