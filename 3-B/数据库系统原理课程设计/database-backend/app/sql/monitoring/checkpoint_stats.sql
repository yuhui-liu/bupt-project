-- 查询数据库检查点统计信息，包括定时和请求检查点数量、写入时间、同步时间和缓冲区状态
SELECT 
    checkpoints_timed,
    checkpoints_req,
    checkpoint_write_time,
    checkpoint_sync_time,
    buffers_checkpoint,
    buffers_clean,
    buffers_backend
FROM pg_stat_bgwriter
