SHOW DATABASES;

use wateringsystemdb;

select MD_id, data, created from MoistureData;

INSERT INTO MoistureData (data)
VALUES (1);