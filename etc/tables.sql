/* SQL LUNG DATABASE CREATION */

DROP TABLE IF EXISTS data;
CREATE TABLE data
(
    id INTEGER PRIMARY KEY,
    patient_name CHAR NOT NULL ,
    patient_id CHAR NOT NULL,
    study_uid CHAR NOT NULL,
    study_date DATE NOT NULL DEFAULT '00000000',
    study_time TIME NOT NULL DEFAULT '000000',
    series_uid CHAR NOT NULL,
    receive_date DATE NOT NULL DEFAULT '00000000',
    receive_time TIME NOT NULL DEFAULT '000000',
    receive_status INTEGER NOT NULL DEFAULT 0,
    process_date DATE NOT NULL DEFAULT '00000000',
    process_time TIME NOT NULL DEFAULT '000000',
    process_status INTEGER NOT NULL DEFAULT 0,
    send_date DATE NOT NULL DEFAULT '00000000',
    send_time TIME NOT NULL DEFAULT '000000',
    send_status INTEGER NOT NULL DEFAULT 0,
    storage CHAR NOT NULL,
    results CHAR,
    slices INTEGER NOT NULL,
    size_stack INTEGER NOT NULL,
    plane_res FLOAT NOT NULL,
    z_res FLOAT NOT NULL,
    thickness FLOAT NOT NULL
);

DROP TABLE IF EXISTS status;
CREATE TABLE status
(
    id INTEGER ,
    desc CHAR NOT NULL
);

INSERT INTO status VALUES ( 0 , 'WAITING' );
INSERT INTO status VALUES ( 1 , 'READY' );
INSERT INTO status VALUES ( 2 , 'RUNNING' );
INSERT INTO status VALUES ( 3 , 'DONE' );
INSERT INTO status VALUES ( 4 , 'ERROR' );
