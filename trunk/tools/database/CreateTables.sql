-- $Id: CreateTables.sql,v 1.3 2000-02-11 23:54:24 bird Exp $
--
-- Create all tables.
--

CREATE DATABASE Odin32;

USE Odin32;

CREATE TABLE dll (
    refcode TINYINT NOT NULL AUTO_INCREMENT PRIMARY KEY,
    name VARCHAR(32) NOT NULL,
    description VARCHAR(255),
    UNIQUE u1(refcode),
    UNIQUE u2(name)
);

CREATE TABLE state (
    refcode TINYINT NOT NULL PRIMARY KEY,
    color   CHAR(7) NOT NULL,
    name    VARCHAR(32) NOT NULL,
    description VARCHAR(128),
    UNIQUE u1(refcode),
    UNIQUE u2(name),
    UNIQUE u3(color)
);

CREATE TABLE function (
    refcode  INTEGER  NOT NULL AUTO_INCREMENT PRIMARY KEY,
    dll      TINYINT  NOT NULL,
    aliasfn  INTEGER  NOT NULL DEFAULT -1,
    name     VARCHAR(64) NOT NULL,
    intname  VARCHAR(64) NOT NULL,
    state    TINYINT  NOT NULL DEFAULT 0,
    ordinal  INTEGER  NOT NULL,
    apigroup SMALLINT,
    return   VARCHAR(64),
    updated  TINYINT  NOT NULL DEFAULT 0,
    UNIQUE i1(refcode, aliasfn),
    UNIQUE i1a(aliasfn, name, dll),
    UNIQUE i1b(aliasfn, dll, refcode),
    UNIQUE i1c(aliasfn, intname, dll, refcode),
    UNIQUE i2(name, dll, refcode),
    UNIQUE i3(intname, dll, refcode),
    UNIQUE u1(refcode),
    UNIQUE u2(name, dll)
);

CREATE TABLE apigroup (
    refcode SMALLINT NOT NULL AUTO_INCREMENT PRIMARY KEY,
    dll     TINYINT NOT NULL,
    name    VARCHAR(64) NOT NULL,
    description VARCHAR(128),
    UNIQUE u1(refcode),
    UNIQUE u2(name)
);

CREATE TABLE author (
    refcode  SMALLINT NOT NULL AUTO_INCREMENT PRIMARY KEY,
    name     VARCHAR(64) NOT NULL,
    initials CHAR(4) NOT NULL,
    alias    VARCHAR(32),
    email    VARCHAR(64),
    country  VARCHAR(64),
    location VARCHAR(64),
    UNIQUE u1(refcode),
    UNIQUE u2(name),
    UNIQUE u3(initials)
--   ,UNIQUE i4(alias), UNIQUE columns have to be defined NOT NULL in mySql.
--    UNIQUE i5(email)  UNIQUE columns have to be defined NOT NULL in mySql.
);

CREATE TABLE fnauthor (
    author   SMALLINT NOT NULL,
    function INTEGER NOT NULL,
    UNIQUE u1(function, author)
);

CREATE TABLE historydll (
    dll TINYINT NOT NULL,
    state SMALLINT NOT NULL,
    date  DATE NOT NULL,
    count SMALLINT NOT NULL,
    UNIQUE u1(dll, state, date)
);

CREATE TABLE historyapigroup (
    apigroup SMALLINT NOT NULL,
    state SMALLINT NOT NULL,
    date  DATE NOT NULL,
    count SMALLINT NOT NULL,
    UNIQUE u1(apigroup, state, date)
);

CREATE TABLE historydlltotal (
    dll SMALLINT NOT NULL,
    date DATE NOT NULL,
    totalcount SMALLINT NOT NULL,
    UNIQUE u1(dll, DATE)
);

CREATE TABLE historyapigrouptotal (
    apigroup SMALLINT NOT NULL,
    date DATE NOT NULL,
    totalcount SMALLINT NOT NULL,
    UNIQUE u1(apigroup, date)
);

CREATE TABLE parameter (
    function INTEGER NOT NULL,
    sequencenbr TINYINT NOT NULL,
    name     VARCHAR(64) NOT NULL,
    type     VARCHAR(64) NOT NULL,
    description VARCHAR(128),
    INDEX  i1(function, name(10)),
    UNIQUE u1(function, name)
);

