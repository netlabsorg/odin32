-- $Id: CreateTables.sql,v 1.2 2000-02-11 18:35:54 bird Exp $
--
-- Create all tables.
--

CREATE DATABASE Odin32;

USE Odin32;

CREATE TABLE dll (
    refcode TINYINT NOT NULL AUTO_INCREMENT PRIMARY KEY,
    name VARCHAR(32) NOT NULL,
    description VARCHAR(255),
    UNIQUE i1(refcode),
    UNIQUE i2(name)
);

CREATE TABLE state (
    refcode TINYINT NOT NULL PRIMARY KEY,
    color   CHAR(7) NOT NULL,
    name    VARCHAR(32) NOT NULL,
    description VARCHAR(128),
    UNIQUE i1(refcode),
    UNIQUE i2(name),
    UNIQUE i3(color)
);

CREATE TABLE function (
    refcode INTEGER NOT NULL AUTO_INCREMENT PRIMARY KEY,
    dll     TINYINT NOT NULL,
    name    VARCHAR(64) NOT NULL,
    intname VARCHAR(64) NOT NULL,
    state   TINYINT NOT NULL DEFAULT 0,
    ordinal INTEGER NOT NULL,
    apigroup SMALLINT,
    return VARCHAR(64),
    updated TINYINT NOT NULL DEFAULT 0,
    UNIQUE i1(refcode),
    UNIQUE i2(name,dll),
    UNIQUE i3(intname, refcode)
);

CREATE TABLE apigroup (
    refcode SMALLINT NOT NULL AUTO_INCREMENT PRIMARY KEY,
    dll     TINYINT NOT NULL,
    name    VARCHAR(64) NOT NULL,
    description VARCHAR(128),
    UNIQUE i1(refcode),
    UNIQUE i2(name)
);

CREATE TABLE author (
    refcode  SMALLINT NOT NULL AUTO_INCREMENT PRIMARY KEY,
    name     VARCHAR(64) NOT NULL,
    initials CHAR(4) NOT NULL,
    alias    VARCHAR(32),
    email    VARCHAR(64),
    country  VARCHAR(64),
    location VARCHAR(64),
    UNIQUE i1(refcode),
    UNIQUE i2(name),
    UNIQUE i3(initials)
--   ,UNIQUE i4(alias), UNIQUE columns have to be defined NOT NULL in mySql.
--    UNIQUE i5(email)  UNIQUE columns have to be defined NOT NULL in mySql.
);

CREATE TABLE fnauthor (
    author   SMALLINT NOT NULL,
    function INTEGER NOT NULL,
    UNIQUE i1(function, author)
);

CREATE TABLE historydll (
    dll TINYINT NOT NULL,
    state SMALLINT NOT NULL,
    date  DATE NOT NULL,
    count SMALLINT NOT NULL,
    UNIQUE i1(dll, state, date)
);

CREATE TABLE historyapigroup (
    apigroup SMALLINT NOT NULL,
    state SMALLINT NOT NULL,
    date  DATE NOT NULL,
    count SMALLINT NOT NULL,
    UNIQUE i1(apigroup, state, date)
);

CREATE TABLE historydlltotal (
    dll SMALLINT NOT NULL,
    date DATE NOT NULL,
    totalcount SMALLINT NOT NULL,
    UNIQUE i1(dll, DATE)
);

CREATE TABLE historyapigrouptotal (
    apigroup SMALLINT NOT NULL,
    date DATE NOT NULL,
    totalcount SMALLINT NOT NULL,
    UNIQUE i1(apigroup, date)
);

CREATE TABLE parameter (
    function SMALLINT NOT NULL,
    sequencenbr TINYINT NOT NULL,
    type     VARCHAR(64) NOT NULL,
    name     VARCHAR(64) NOT NULL,
    description VARCHAR(128),
    UNIQUE i1(function, name)
);

