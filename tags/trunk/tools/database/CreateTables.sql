-- $Id: CreateTables.sql,v 1.8 2000-07-18 07:15:57 bird Exp $
--
-- Create all tables.
--

CREATE DATABASE Odin32;

USE Odin32;


--
-- This table holds the known states.
--
CREATE TABLE state (
    refcode TINYINT NOT NULL PRIMARY KEY,
    color   CHAR(7) NOT NULL,
    name    VARCHAR(32) NOT NULL,
    description VARCHAR(128),
    UNIQUE u1(refcode),
    UNIQUE u2(name),
    UNIQUE u3(color)
);


--
-- This table holds the dll names.
--
CREATE TABLE dll (
    refcode     TINYINT NOT NULL AUTO_INCREMENT PRIMARY KEY,
    name        VARCHAR(32) NOT NULL,
    description VARCHAR(255),
    UNIQUE u1(refcode),
    UNIQUE u2(name)
);


--
-- This table holds fileinformation (per dll).
--
CREATE TABLE file (
    refcode         INTEGER NOT NULL AUTO_INCREMENT PRIMARY KEY,
    dll             TINYINT NOT NULL,
    name            VARCHAR(128) NOT NULL,
    lastdatetime    DATETIME NOT NULL,
    lastauthor      SMALLINT NOT NULL,
    revision        CHAR(10) NOT NULL,
    description     TEXT,
    UNIQUE u1(refcode),
    UNIQUE u2(dll, name),
    INDEX  i1(name)
);

--
-- This table holds design notes (per dll).
--
CREATE TABLE designnote (
    refcode     INTEGER NOT NULL AUTO_INCREMENT PRIMARY KEY,
    dll         TINYINT NOT NULL,
    file        INTEGER NOT NULL,
    seqnbrfile  SMALLINT NOT NULL,
    seqnbr      INTEGER NOT NULL,
    title       TEXT,
    note        TEXT    NOT NULL,
    UNIQUE      u1(refcode),
    INDEX       u2(file, seqnbrfile, seqnbr, dll)
);


--
-- This table holds API information (per dll / file).
--
CREATE TABLE function (
    refcode  INTEGER  NOT NULL AUTO_INCREMENT PRIMARY KEY,
    dll      TINYINT  NOT NULL,
    aliasfn  INTEGER  NOT NULL DEFAULT -1,
    file     INTEGER  NOT NULL DEFAULT -1,
    name     VARCHAR(100) NOT NULL,
    intname  VARCHAR(100) NOT NULL,
    state    TINYINT  NOT NULL DEFAULT 0,
    ordinal  INTEGER  NOT NULL,
    apigroup SMALLINT,
    return   VARCHAR(64),
    updated  TINYINT  NOT NULL DEFAULT 0,
    description TEXT,
    remark      TEXT,
    returndesc  TEXT,
    sketch      TEXT,
    equiv       TEXT,
    time        TEXT,
    UNIQUE i1(refcode, aliasfn),
    UNIQUE i1a(dll, aliasfn, refcode),
    UNIQUE i1b(aliasfn, name, dll),
    UNIQUE i1c(aliasfn, intname, dll, refcode),
    UNIQUE i2(name, dll, refcode),
    UNIQUE i3(intname, dll, refcode),
    INDEX  i4(dll, file),
    INDEX  i5(file),
    UNIQUE u1(refcode),
    UNIQUE u2(name, dll)
);


--
-- This table holds parameters for APIs.
--
CREATE TABLE parameter (
    function INTEGER NOT NULL,
    sequencenbr TINYINT NOT NULL,
    name     VARCHAR(64) NOT NULL,
    type     VARCHAR(64) NOT NULL,
    description TEXT,
    INDEX  i1(function, name),
    UNIQUE u1(function, name)
);



--
-- Manually created Groups of APIs
--
CREATE TABLE apigroup (
    refcode SMALLINT NOT NULL AUTO_INCREMENT PRIMARY KEY,
    dll     TINYINT NOT NULL,
    name    VARCHAR(64) NOT NULL,
    description VARCHAR(128),
    UNIQUE u1(refcode),
    UNIQUE u2(name)
);


--
-- Manually create author table.
--
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


--
-- Many to many relation between functions and authors.
--
CREATE TABLE fnauthor (
    author   SMALLINT NOT NULL,
    function INTEGER NOT NULL,
    UNIQUE u1(function, author)
);


--
-- Status history for dlls.
--
CREATE TABLE historydll (
    dll TINYINT NOT NULL,
    state SMALLINT NOT NULL,
    date  DATE NOT NULL,
    count SMALLINT NOT NULL,
    UNIQUE u1(dll, state, date)
);


--
-- Status history for API groups.
--
CREATE TABLE historyapigroup (
    apigroup SMALLINT NOT NULL,
    state SMALLINT NOT NULL,
    date  DATE NOT NULL,
    count SMALLINT NOT NULL,
    UNIQUE u1(apigroup, state, date)
);


--
-- Dll API count history.
--
CREATE TABLE historydlltotal (
    dll SMALLINT NOT NULL,
    date DATE NOT NULL,
    totalcount SMALLINT NOT NULL,
    UNIQUE u1(dll, DATE)
);


--
-- API Group API count history.
--
CREATE TABLE historyapigrouptotal (
    apigroup SMALLINT NOT NULL,
    date DATE NOT NULL,
    totalcount SMALLINT NOT NULL,
    UNIQUE u1(apigroup, date)
);


