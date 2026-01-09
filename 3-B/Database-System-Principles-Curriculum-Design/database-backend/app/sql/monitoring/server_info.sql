-- 查询服务器详细信息，包括PostgreSQL版本、数据目录、启动时间、运行时长和关键内存配置
SELECT 
    version() as postgres_version,
    current_setting('server_version') as server_version,
    current_setting('data_directory') as data_directory,
    pg_postmaster_start_time() as start_time,
    now() - pg_postmaster_start_time() as uptime,
    current_setting('shared_buffers') as shared_buffers,
    current_setting('effective_cache_size') as effective_cache_size,
    current_setting('work_mem') as work_mem,
    current_setting('max_connections') as max_connections