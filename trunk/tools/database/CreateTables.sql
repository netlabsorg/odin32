-- $Id: CreateTables.sql,v 1.19 2001-09-07 10:24:06 bird Exp $
--
-- Create all tables.
--

CREATE DATABASE Odin32;

USE Odin32;


--
-- Manually create author table.
--
CREATE TABLE author (
    refcode     SMALLINT NOT NULL AUTO_INCREMENT PRIMARY KEY,
    name        VARCHAR(64) NOT NULL,
    first       VARCHAR(45) NOT NULL,
    last        VARCHAR(45) NOT NULL,
    initials    CHAR(4) NOT NULL,
    alias       VARCHAR(32),
    email       VARCHAR(64),
    country     VARCHAR(64),
    location    VARCHAR(64),
    description TEXT,
    UNIQUE u1(refcode),
    UNIQUE u2(name),
    UNIQUE u3(initials),
    INDEX  i1(first),
    INDEX  i2(last)
--   ,UNIQUE i4(alias), UNIQUE columns have to be defined NOT NULL in mySql.
--    UNIQUE i5(email)  UNIQUE columns have to be defined NOT NULL in mySql.
);



--
-- Support table for Codes
--
CREATE TABLE code (
    codegroup   CHAR(4) NOT NULL,
    code        CHAR(4) NOT NULL,
    description VARCHAR(128) NOT NULL,
    UNIQUE  u1(codegroup, code)
);


--
--
-- Documentation
--
--

--
-- This table holds the known states.
--
CREATE TABLE state (
    refcode     TINYINT NOT NULL PRIMARY KEY,
    color       CHAR(7) NOT NULL,
    weight      TINYINT NOT NULL,
    name        VARCHAR(32) NOT NULL,
    description TEXT NOT NULL,
    UNIQUE u1(refcode),
    UNIQUE u2(name),
    UNIQUE u3(color)
);


--
-- This table holds the module names.
--    Type has these known types:
--       'A' for Odin32 API dll
--       'I' for Internal Odin32 (API) dll
--       'S' for support stuff (ie. pe.exe and win32k.sys).
--       'T' for tools (executables and dlls)
--
CREATE TABLE module (
    refcode     SMALLINT      NOT NULL AUTO_INCREMENT PRIMARY KEY,
    name        VARCHAR(32)   NOT NULL,
    type        CHAR          NOT NULL DEFAULT 'A',
    description VARCHAR(255),
    UNIQUE u1(refcode),
    UNIQUE u2(name)
);


--
-- This table holds fileinformation (per module).
--
CREATE TABLE file (
    refcode         INTEGER NOT NULL AUTO_INCREMENT PRIMARY KEY,
    module          SMALLINT NOT NULL,
    name            VARCHAR(128) NOT NULL,
    lastdatetime    DATETIME NOT NULL,
    lastauthor      SMALLINT NOT NULL,
    revision        CHAR(10) NOT NULL,
    updated         TINYINT  NOT NULL DEFAULT 0,
    description     TEXT,
    UNIQUE u1(refcode),
    UNIQUE u2(module, name),
    INDEX  i1(name)
);


--
-- This table holds design notes (per module).
--
-- seqnbrnote is a unique number used to order the
--            sections within a design note.
-- level      is the nesting level of the section.
--            0 is the top section in the note.
--
CREATE TABLE designnote (
    refcode     INTEGER NOT NULL AUTO_INCREMENT,
    module      SMALLINT NOT NULL,
    file        INTEGER NOT NULL,
    line        INTEGER  NOT NULL DEFAULT -1,
    seqnbrnote  SMALLINT NOT NULL,
    level       TINYINT NOT NULL,
    seqnbr      INTEGER NOT NULL,
    name        TEXT,
    note        TEXT    NOT NULL,
    PRIMARY KEY(refcode, seqnbrnote),
    UNIQUE      u1(refcode, seqnbrnote),
    UNIQUE      u2(refcode, seqnbrnote, level),
    UNIQUE      u3(module, seqnbr, level, seqnbrnote, refcode),
    INDEX       i1(file, refcode)
);


--
-- This table holds API information (per module / file).
--
-- Type has these known values:
--       'A' for API
--       'I' for Internal Odin32 API
--       'F' for All other functions.
---
--       'M' for Class method (?)
--       'O' for Class operator (?)
--       'C' for Class constructor (?)
--       'D' for Class destructor (?)
--
CREATE TABLE function (
    refcode  INTEGER  NOT NULL AUTO_INCREMENT PRIMARY KEY,
    module   SMALLINT NOT NULL,
    aliasfn  INTEGER  NOT NULL DEFAULT -1,
    file     INTEGER  NOT NULL DEFAULT -1,
    name     VARCHAR(100) NOT NULL,
    intname  VARCHAR(100) NOT NULL,
    state    TINYINT  NOT NULL DEFAULT 0,
    ordinal  INTEGER  NOT NULL,
    apigroup SMALLINT,
    return   VARCHAR(64),
    type     CHAR     NOT NULL DEFAULT 'A',
    updated  TINYINT  NOT NULL DEFAULT 0,
    line     INTEGER  NOT NULL DEFAULT -1,
    description TEXT,
    remark      TEXT,
    returndesc  TEXT,
    sketch      TEXT,
    equiv       TEXT,
    time        TEXT,
    UNIQUE i1(refcode, aliasfn),
    UNIQUE i1a(module, aliasfn, refcode),
    UNIQUE i1b(aliasfn, name, module, refcode),
    UNIQUE i1c(aliasfn, intname, module, refcode),
    UNIQUE i2(name, module, refcode),
    UNIQUE i3(intname, module, refcode),
    INDEX  i4(module, file),
    INDEX  i5(file, refcode),
    INDEX  i6(state, file),
    UNIQUE i7(state, refcode),
    UNIQUE i8(refcode, state),
    UNIQUE i9(module, state, refcode),
    UNIQUE u1(refcode),
    UNIQUE u2(name, module, refcode),
    UNIQUE u3(type, refcode)
);


--
-- This table holds parameters for APIs.
--
CREATE TABLE parameter (
    function    INTEGER NOT NULL,
    sequencenbr TINYINT NOT NULL,
    name        VARCHAR(64) NOT NULL,
    type        VARCHAR(64) NOT NULL,
    description TEXT,
    INDEX  i1(function, name),
    UNIQUE u1(function, name)
);


--
-- Many to many relation between functions and authors.
--
CREATE TABLE fnauthor (
    author      SMALLINT NOT NULL,
    function    INTEGER NOT NULL,
    UNIQUE u1(author, function),
    UNIQUE u2(function, author)
);


--
-- Manually created Groups of APIs
--
CREATE TABLE apigroup (
    refcode     SMALLINT NOT NULL AUTO_INCREMENT PRIMARY KEY,
    module      SMALLINT NOT NULL,
    name        VARCHAR(64) NOT NULL,
    description VARCHAR(128),
    UNIQUE u1(refcode),
    UNIQUE u2(name)
);



--
--
-- History information
--
--

--
-- Status history for modules.
--
CREATE TABLE historymodule (
    module      SMALLINT NOT NULL,
    state       SMALLINT NOT NULL,
    date        DATE     NOT NULL,
    count       SMALLINT NOT NULL,
    TYPE        CHAR     NOT NULL DEFAULT 'A',
    UNIQUE u1(module, state, date)
);


--
-- Status history for API groups.
--
CREATE TABLE historyapigroup (
    apigroup    SMALLINT NOT NULL,
    state       SMALLINT NOT NULL,
    date        DATE     NOT NULL,
    count       SMALLINT NOT NULL,
    UNIQUE u1(apigroup, state, date)
);


--
-- Module API count history.
--
CREATE TABLE historymoduletotal (
    module      SMALLINT NOT NULL,
    date        DATE     NOT NULL,
    totalcount  SMALLINT NOT NULL,
    type        CHAR     NOT NULL DEFAULT 'A',
    UNIQUE u1(module, DATE)
);


--
-- API Group API count history.
--
CREATE TABLE historyapigrouptotal (
    apigroup    SMALLINT NOT NULL,
    date        DATE     NOT NULL,
    totalcount  SMALLINT NOT NULL,
    UNIQUE u1(apigroup, date)
);



--
--
-- Administration
--
--

--
-- This table holds the teams (like MAD, WAI,...).
--
CREATE TABLE team (
    refcode     SMALLINT NOT NULL AUTO_INCREMENT PRIMARY KEY,
    name        VARCHAR(30) NOT NULL,
    longname    VARCHAR(128) NOT NULL,
    description TEXT NOT NULL,
    tasks       TEXT,
    notes       TEXT,
    UNIQUE u1(refcode),
    UNIQUE u2(name)
);


--
-- This table holds the groups under each team.
-- Currently only the WAI team is grouped.
--
CREATE TABLE tgroup (
    refcode     SMALLINT NOT NULL AUTO_INCREMENT PRIMARY KEY,
    team        SMALLINT NOT NULL,
    name        VARCHAR(80) NOT NULL,
    description TEXT NOT NULL,
    notes       TEXT,
    UNIQUE u1(refcode),
    UNIQUE u2(team, refcode)
);


--
-- This table relates a tgroup with authors
-- ( do a distinct select to get all members of a team )
--
CREATE TABLE tgroupmember (
    tgroup      SMALLINT NOT NULL,
    author      SMALLINT NOT NULL,
    codemaintainer CHAR(1) DEFAULT 'N',
    UNIQUE u1(tgroup, author)
);


--
-- This table relates a tgroup with a module.
--
CREATE TABLE tgroupmodule (
    tgroup      SMALLINT NOT NULL,
    module      SMALLINT NOT NULL,
    UNIQUE u1(tgroup, module)
);


--
-- This table relates a tgroup with an apigroup.
--
CREATE TABLE tgroupapigroup (
    tgroup      SMALLINT NOT NULL,
    apigroup    SMALLINT NOT NULL,
    UNIQUE u1(tgroup, apigroup)
);



