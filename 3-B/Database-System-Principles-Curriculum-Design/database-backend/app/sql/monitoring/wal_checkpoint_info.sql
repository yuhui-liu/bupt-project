-- 查询WAL（预写日志）和检查点信息，包括当前WAL位置、WAL字节数、检查点统计和缓冲区状态
SELECT 
    pg_current_wal_lsn() as current_wal_lsn,
    pg_wal_lsn_diff(pg_current_wal_lsn(), '0/0') as wal_bytes,
    checkpoints_timed,
    checkpoints_req,
    checkpoint_write_time,
    checkpoint_sync_time,
    buffers_checkpoint,
    buffers_clean,
    buffers_backend
FROM pg_stat_bgwriter