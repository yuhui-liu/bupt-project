-- 查询数据库基本信息，包括数据库名称、PostgreSQL版本、服务器版本、数据目录、启动时间和运行时长
SELECT 
    current_database() as database_name,
    version() as postgres_version,
    current_setting('server_version') as server_version,
    current_setting('data_directory') as data_directory,
    pg_postmaster_start_time() as start_time,
    now() - pg_postmaster_start_time() as uptime