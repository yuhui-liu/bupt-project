-- 查询数据库复制状态，显示从服务器地址、状态、LSN位置和复制延迟信息
SELECT 
    client_addr,
    state,
    sent_lsn,
    write_lsn,
    flush_lsn,
    replay_lsn,
    write_lag,
    flush_lag,
    replay_lag
FROM pg_stat_replication