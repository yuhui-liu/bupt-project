"use client";

import {
  Row,
  Col,
  Card,
  Statistic,
  Table,
  Button,
  Space,
  Alert,
  Tabs,
  Progress,
  Badge,
  Tag,
} from "antd";
import {
  DatabaseOutlined,
  DesktopOutlined,
  LineChartOutlined,
  SettingOutlined,
  ClockCircleOutlined,
  TeamOutlined,
  HddOutlined,
  ThunderboltOutlined,
  WarningOutlined,
  FileTextOutlined,
} from "@ant-design/icons";
import { useEffect, useState } from "react";
import { getSystemInfo } from "@/lib/fetchData";

export default function Page() {
  // All system info data with type definitions
  const [systemData, setSystemData] = useState<any>({
    active_connections: [],
    cache_hit_stats: [],
    checkpoint_stats: [],
    connection_stats: [],
    database_activity: [],
    database_info: [],
    database_size: [],
    database_stats: [],
    deadlock_stats: [],
    important_settings: [],
    index_usage: [],
    locks_info: [],
    long_running_queries: [],
    most_accessed_tables: [],
    replication_status: [],
    server_info: [],
    table_stats: [],
    tablespace_usage: [],
    vacuum_stats: [],
    wal_checkpoint_info: [],
  });

  const [loading, setLoading] = useState(true);

  // Table columns definitions
  const activeConnectionsColumns = [
    { title: "PID", dataIndex: "pid", key: "pid" },
    { title: "用户", dataIndex: "usename", key: "usename" },
    {
      title: "应用名称",
      dataIndex: "application_name",
      key: "application_name",
    },
    { title: "客户端地址", dataIndex: "client_addr", key: "client_addr" },
    {
      title: "状态",
      dataIndex: "state",
      key: "state",
      render: (state: string) => (
        <Badge
          status={state === "active" ? "processing" : "default"}
          text={state}
        />
      ),
    },
    { title: "查询开始时间", dataIndex: "query_start", key: "query_start" },
    { title: "运行时长(秒)", dataIndex: "duration", key: "duration" },
  ];

  const databaseSizeColumns = [
    { title: "数据库名称", dataIndex: "database_name", key: "database_name" },
    { title: "大小", dataIndex: "size", key: "size" },
    { title: "字节大小", dataIndex: "size_bytes", key: "size_bytes" },
  ];

  const importantSettingsColumns = [
    { title: "参数名", dataIndex: "name", key: "name" },
    { title: "值", dataIndex: "setting", key: "setting" },
    { title: "单位", dataIndex: "unit", key: "unit" },
    { title: "描述", dataIndex: "short_desc", key: "short_desc" },
    {
      title: "上下文",
      dataIndex: "context",
      key: "context",
      render: (context: string) => (
        <Tag
          color={
            context === "postmaster"
              ? "red"
              : context === "sighup"
                ? "orange"
                : "blue"
          }
        >
          {context}
        </Tag>
      ),
    },
  ];

  const indexUsageColumns = [
    { title: "Schema", dataIndex: "schemaname", key: "schemaname" },
    { title: "表名", dataIndex: "relname", key: "relname" },
    { title: "索引名", dataIndex: "indexrelname", key: "indexrelname" },
    { title: "索引大小", dataIndex: "index_size", key: "index_size" },
    { title: "读取次数", dataIndex: "idx_tup_read", key: "idx_tup_read" },
    { title: "获取次数", dataIndex: "idx_tup_fetch", key: "idx_tup_fetch" },
    {
      title: "使用状态",
      dataIndex: "usage_status",
      key: "usage_status",
      render: (status: string) => (
        <Tag color={status === "Used" ? "green" : "red"}>{status}</Tag>
      ),
    },
  ];

  const tableStatsColumns = [
    { title: "Schema", dataIndex: "schemaname", key: "schemaname" },
    { title: "表名", dataIndex: "tablename", key: "tablename" },
    { title: "表大小", dataIndex: "table_size", key: "table_size" },
    { title: "索引大小", dataIndex: "index_size", key: "index_size" },
    { title: "总大小", dataIndex: "total_size", key: "total_size" },
    { title: "插入数", dataIndex: "inserts", key: "inserts" },
    { title: "更新数", dataIndex: "updates", key: "updates" },
    { title: "删除数", dataIndex: "deletes", key: "deletes" },
    { title: "活跃元组", dataIndex: "live_tuples", key: "live_tuples" },
    { title: "死亡元组", dataIndex: "dead_tuples", key: "dead_tuples" },
  ];

  const mostAccessedTablesColumns = [
    { title: "Schema", dataIndex: "schemaname", key: "schemaname" },
    { title: "表名", dataIndex: "relname", key: "relname" },
    { title: "顺序扫描", dataIndex: "seq_scan", key: "seq_scan" },
    { title: "索引扫描", dataIndex: "idx_scan", key: "idx_scan" },
    { title: "总扫描次数", dataIndex: "total_scans", key: "total_scans" },
    {
      title: "总修改次数",
      dataIndex: "total_modifications",
      key: "total_modifications",
    },
    { title: "最后清理", dataIndex: "last_vacuum", key: "last_vacuum" },
    { title: "最后分析", dataIndex: "last_analyze", key: "last_analyze" },
  ];

  const tablespaceUsageColumns = [
    {
      title: "表空间名称",
      dataIndex: "tablespace_name",
      key: "tablespace_name",
    },
    { title: "位置", dataIndex: "location", key: "location" },
    { title: "大小", dataIndex: "size", key: "size" },
  ];

  const vacuumStatsColumns = [
    { title: "Schema", dataIndex: "schemaname", key: "schemaname" },
    { title: "表名", dataIndex: "tablename", key: "tablename" },
    { title: "活跃元组", dataIndex: "n_live_tup", key: "n_live_tup" },
    { title: "死亡元组", dataIndex: "n_dead_tup", key: "n_dead_tup" },
    {
      title: "死亡元组比例",
      dataIndex: "dead_tuple_ratio",
      key: "dead_tuple_ratio",
      render: (ratio: string) => (
        <Progress
          percent={parseFloat(ratio)}
          size="small"
          status={parseFloat(ratio) > 50 ? "exception" : "normal"}
        />
      ),
    },
    { title: "清理次数", dataIndex: "vacuum_count", key: "vacuum_count" },
    {
      title: "自动清理次数",
      dataIndex: "autovacuum_count",
      key: "autovacuum_count",
    },
    { title: "最后清理", dataIndex: "last_vacuum", key: "last_vacuum" },
    {
      title: "最后自动清理",
      dataIndex: "last_autovacuum",
      key: "last_autovacuum",
    },
  ];

  const longRunningQueriesColumns = [
    { title: "PID", dataIndex: "pid", key: "pid" },
    { title: "用户", dataIndex: "usename", key: "usename" },
    {
      title: "应用名称",
      dataIndex: "application_name",
      key: "application_name",
    },
    { title: "客户端地址", dataIndex: "client_addr", key: "client_addr" },
    { title: "运行时长", dataIndex: "duration", key: "duration" },
    { title: "查询语句", dataIndex: "query", key: "query", ellipsis: true },
  ];

  const locksInfoColumns = [
    { title: "锁类型", dataIndex: "locktype", key: "locktype" },
    { title: "模式", dataIndex: "mode", key: "mode" },
    { title: "用户", dataIndex: "usename", key: "usename" },
    { title: "查询开始时间", dataIndex: "query_start", key: "query_start" },
    { title: "查询片段", dataIndex: "query", key: "query", ellipsis: true },
  ];

  const getInfoAndParse = async () => {
    try {
      setLoading(true);
      const response = (await getSystemInfo()).data;
      console.log("System data:", response);
      setSystemData(response);
    } catch (error) {
      console.error("Failed to fetch system info:", error);
    } finally {
      setLoading(false);
    }
  };

  useEffect(() => {
    getInfoAndParse();
  }, []);

  // Helper functions
  const formatUptime = (seconds: number) => {
    const days = Math.floor(seconds / 86400);
    const hours = Math.floor((seconds % 86400) / 3600);
    const minutes = Math.floor((seconds % 3600) / 60);
    return `${days}天 ${hours}小时 ${minutes}分钟`;
  };

  const getConnectionStatus = () => {
    if (systemData.database_info && systemData.database_info.length > 0) {
      return "connected";
    }
    return "disconnected";
  };

  // Tab items for different categories
  const tabItems = [
    {
      key: "1",
      label: "概览",
      children: (
        <Space direction="vertical" size="large" style={{ display: "flex" }}>
          {/* Database Status Section */}
          <Row gutter={[16, 16]}>
            <Col span={24}>
              <Alert
                message={`数据库状态: ${getConnectionStatus() === "connected" ? "已连接" : "未连接"}`}
                type={
                  getConnectionStatus() === "connected" ? "success" : "error"
                }
                showIcon
              />
            </Col>
          </Row>

          {/* Basic Database Info */}
          {systemData.database_info && systemData.database_info.length > 0 && (
            <Card title="数据库基本信息">
              <Row gutter={[16, 16]}>
                <Col span={6}>
                  <Statistic
                    title="数据库名称"
                    value={systemData.database_info[0].database_name}
                    prefix={<DatabaseOutlined />}
                  />
                </Col>
                <Col span={6}>
                  <Statistic
                    title="PostgreSQL版本"
                    value={systemData.database_info[0].server_version}
                    prefix={<SettingOutlined />}
                  />
                </Col>
                <Col span={6}>
                  <Statistic
                    title="运行时间"
                    value={formatUptime(systemData.database_info[0].uptime)}
                    prefix={<ClockCircleOutlined />}
                  />
                </Col>
                <Col span={6}>
                  <Statistic
                    title="数据目录"
                    value={systemData.database_info[0].data_directory}
                    prefix={<HddOutlined />}
                  />
                </Col>
              </Row>
            </Card>
          )}

          {/* Connection and Activity Stats */}
          {systemData.database_stats &&
            systemData.database_stats.length > 0 && (
              <Card title="连接与活动统计">
                <Row gutter={[16, 16]}>
                  <Col span={6}>
                    <Statistic
                      title="总连接数"
                      value={systemData.database_stats[0].total_connections}
                      prefix={<TeamOutlined />}
                    />
                  </Col>
                  <Col span={6}>
                    <Statistic
                      title="活跃连接数"
                      value={systemData.database_stats[0].active_connections}
                      prefix={<DesktopOutlined />}
                    />
                  </Col>
                  <Col span={6}>
                    <Statistic
                      title="后端进程数"
                      value={systemData.database_stats[0].backend_processes}
                      prefix={<SettingOutlined />}
                    />
                  </Col>
                  <Col span={6}>
                    <Statistic
                      title="数据库大小"
                      value={systemData.database_stats[0].database_size}
                      prefix={<HddOutlined />}
                    />
                  </Col>
                </Row>
              </Card>
            )}

          {/* Cache Hit Stats */}
          {systemData.cache_hit_stats &&
            systemData.cache_hit_stats.length > 0 && (
              <Card title="缓存命中率">
                <Row gutter={[16, 16]}>
                  {systemData.cache_hit_stats.map(
                    (stat: any, index: number) => (
                      <Col span={12} key={index}>
                        <Card>
                          <Statistic
                            title={`${stat.type} 缓存命中率`}
                            value={parseFloat(stat.hit_ratio)}
                            suffix="%"
                            precision={2}
                            valueStyle={{
                              color:
                                parseFloat(stat.hit_ratio) > 95
                                  ? "#3f8600"
                                  : parseFloat(stat.hit_ratio) > 90
                                    ? "#faad14"
                                    : "#cf1322",
                            }}
                          />
                          <Progress
                            percent={parseFloat(stat.hit_ratio)}
                            showInfo={false}
                            strokeColor={
                              parseFloat(stat.hit_ratio) > 95
                                ? "#52c41a"
                                : parseFloat(stat.hit_ratio) > 90
                                  ? "#faad14"
                                  : "#ff4d4f"
                            }
                          />
                        </Card>
                      </Col>
                    ),
                  )}
                </Row>
              </Card>
            )}

          {/* Database Activity Details */}
          {systemData.database_activity &&
            systemData.database_activity.length > 0 && (
              <Card title="数据库活动详情">
                <Row gutter={[16, 16]}>
                  <Col span={8}>
                    <Statistic
                      title="已提交事务"
                      value={
                        systemData.database_activity[0].transactions_committed
                      }
                      prefix={<LineChartOutlined />}
                    />
                  </Col>
                  <Col span={8}>
                    <Statistic
                      title="回滚事务"
                      value={
                        systemData.database_activity[0].transactions_rolled_back
                      }
                      prefix={<WarningOutlined />}
                      valueStyle={{ color: "#cf1322" }}
                    />
                  </Col>
                  <Col span={8}>
                    <Statistic
                      title="缓冲区命中"
                      value={systemData.database_activity[0].buffer_blocks_hit}
                      prefix={<ThunderboltOutlined />}
                    />
                  </Col>
                  <Col span={8}>
                    <Statistic
                      title="磁盘读取"
                      value={systemData.database_activity[0].disk_blocks_read}
                      prefix={<HddOutlined />}
                    />
                  </Col>
                  <Col span={8}>
                    <Statistic
                      title="元组插入"
                      value={systemData.database_activity[0].tuples_inserted}
                      prefix={<DatabaseOutlined />}
                    />
                  </Col>
                  <Col span={8}>
                    <Statistic
                      title="元组返回"
                      value={systemData.database_activity[0].tuples_returned}
                      prefix={<DatabaseOutlined />}
                    />
                  </Col>
                </Row>
              </Card>
            )}
        </Space>
      ),
    },
    {
      key: "2",
      label: "连接管理",
      children: (
        <Space direction="vertical" size="large" style={{ display: "flex" }}>
          {/* Active Connections */}
          <Card title="活跃连接">
            <Table
              columns={activeConnectionsColumns}
              dataSource={systemData.active_connections.map(
                (conn: any, index: number) => ({ ...conn, key: index }),
              )}
              size="small"
              scroll={{ x: 1000 }}
            />
          </Card>

          {/* Connection Stats */}
          {systemData.connection_stats &&
            systemData.connection_stats.length > 0 && (
              <Card title="连接状态统计">
                <Row gutter={[16, 16]}>
                  {systemData.connection_stats.map(
                    (stat: any, index: number) => (
                      <Col span={8} key={index}>
                        <Card>
                          <Statistic
                            title={stat.state || "空闲连接"}
                            value={stat.count}
                            suffix={`连接 (${parseFloat(stat.percentage).toFixed(1)}%)`}
                          />
                          <Progress
                            percent={parseFloat(stat.percentage)}
                            showInfo={false}
                          />
                        </Card>
                      </Col>
                    ),
                  )}
                </Row>
              </Card>
            )}

          {/* Long Running Queries */}
          <Card title="长时间运行查询">
            <Table
              columns={longRunningQueriesColumns}
              dataSource={systemData.long_running_queries.map(
                (query: any, index: number) => ({ ...query, key: index }),
              )}
              size="small"
              scroll={{ x: 1200 }}
            />
          </Card>

          {/* Locks Info */}
          <Card title="锁信息">
            <Table
              columns={locksInfoColumns}
              dataSource={systemData.locks_info.map(
                (lock: any, index: number) => ({ ...lock, key: index }),
              )}
              size="small"
              scroll={{ x: 1000 }}
            />
          </Card>
        </Space>
      ),
    },
    {
      key: "3",
      label: "存储与性能",
      children: (
        <Space direction="vertical" size="large" style={{ display: "flex" }}>
          {/* Database Sizes */}
          <Card title="数据库大小">
            <Table
              columns={databaseSizeColumns}
              dataSource={systemData.database_size.map(
                (db: any, index: number) => ({ ...db, key: index }),
              )}
              size="small"
            />
          </Card>

          {/* Table Statistics */}
          <Card title="表统计信息">
            <Table
              columns={tableStatsColumns}
              dataSource={systemData.table_stats.map(
                (table: any, index: number) => ({ ...table, key: index }),
              )}
              size="small"
              scroll={{ x: 1400 }}
            />
          </Card>

          {/* Most Accessed Tables */}
          <Card title="最常访问的表">
            <Table
              columns={mostAccessedTablesColumns}
              dataSource={systemData.most_accessed_tables.map(
                (table: any, index: number) => ({ ...table, key: index }),
              )}
              size="small"
              scroll={{ x: 1200 }}
            />
          </Card>

          {/* Index Usage */}
          <Card title="索引使用情况">
            <Table
              columns={indexUsageColumns}
              dataSource={systemData.index_usage.map(
                (index: any, idx: number) => ({ ...index, key: idx }),
              )}
              size="small"
              scroll={{ x: 1200 }}
            />
          </Card>

          {/* Tablespace Usage */}
          <Card title="表空间使用情况">
            <Table
              columns={tablespaceUsageColumns}
              dataSource={systemData.tablespace_usage.map(
                (space: any, index: number) => ({ ...space, key: index }),
              )}
              size="small"
            />
          </Card>
        </Space>
      ),
    },
    {
      key: "4",
      label: "系统配置",
      children: (
        <Space direction="vertical" size="large" style={{ display: "flex" }}>
          {/* Important Settings */}
          <Card title="重要配置参数">
            <Table
              columns={importantSettingsColumns}
              dataSource={systemData.important_settings.map(
                (setting: any, index: number) => ({ ...setting, key: index }),
              )}
              size="small"
              scroll={{ x: 1000 }}
            />
          </Card>

          {/* Server Info */}
          {systemData.server_info && systemData.server_info.length > 0 && (
            <Card title="服务器详细信息">
              <Row gutter={[16, 16]}>
                <Col span={8}>
                  <Statistic
                    title="最大连接数"
                    value={systemData.server_info[0].max_connections}
                    prefix={<TeamOutlined />}
                  />
                </Col>
                <Col span={8}>
                  <Statistic
                    title="共享缓冲区"
                    value={systemData.server_info[0].shared_buffers}
                    prefix={<ThunderboltOutlined />}
                  />
                </Col>
                <Col span={8}>
                  <Statistic
                    title="工作内存"
                    value={systemData.server_info[0].work_mem}
                    prefix={<SettingOutlined />}
                  />
                </Col>
                <Col span={8}>
                  <Statistic
                    title="有效缓存大小"
                    value={systemData.server_info[0].effective_cache_size}
                    prefix={<HddOutlined />}
                  />
                </Col>
                <Col span={8}>
                  <Statistic
                    title="PostgreSQL版本"
                    value={systemData.server_info[0].server_version}
                    prefix={<DatabaseOutlined />}
                  />
                </Col>
                <Col span={8}>
                  <Statistic
                    title="数据目录"
                    value={systemData.server_info[0].data_directory}
                    prefix={<HddOutlined />}
                  />
                </Col>
              </Row>
            </Card>
          )}
        </Space>
      ),
    },
    {
      key: "5",
      label: "维护与监控",
      children: (
        <Space direction="vertical" size="large" style={{ display: "flex" }}>
          {/* Vacuum Statistics */}
          <Card title="清理统计信息">
            <Table
              columns={vacuumStatsColumns}
              dataSource={systemData.vacuum_stats.map(
                (stat: any, index: number) => ({ ...stat, key: index }),
              )}
              size="small"
              scroll={{ x: 1200 }}
            />
          </Card>

          {/* Deadlock Stats */}
          {systemData.deadlock_stats &&
            systemData.deadlock_stats.length > 0 && (
              <Card title="死锁统计">
                <Row gutter={[16, 16]}>
                  {systemData.deadlock_stats.map((stat: any, index: number) => (
                    <Col span={8} key={index}>
                      <Card title={stat.datname}>
                        <Statistic
                          title="死锁次数"
                          value={stat.deadlocks}
                          valueStyle={{
                            color: stat.deadlocks > 0 ? "#cf1322" : "#3f8600",
                          }}
                        />
                        <Statistic title="临时文件数" value={stat.temp_files} />
                        <Statistic
                          title="临时文件大小"
                          value={stat.temp_bytes}
                          suffix="bytes"
                        />
                      </Card>
                    </Col>
                  ))}
                </Row>
              </Card>
            )}

          {/* Checkpoint Stats */}
          {systemData.checkpoint_stats &&
            systemData.checkpoint_stats.length > 0 && (
              <Card title="检查点统计">
                <Row gutter={[16, 16]}>
                  <Col span={8}>
                    <Statistic
                      title="定时检查点"
                      value={systemData.checkpoint_stats[0].checkpoints_timed}
                      prefix={<ClockCircleOutlined />}
                    />
                  </Col>
                  <Col span={8}>
                    <Statistic
                      title="请求检查点"
                      value={systemData.checkpoint_stats[0].checkpoints_req}
                      prefix={<WarningOutlined />}
                    />
                  </Col>
                  <Col span={8}>
                    <Statistic
                      title="检查点写入时间"
                      value={
                        systemData.checkpoint_stats[0].checkpoint_write_time
                      }
                      suffix="ms"
                      prefix={<FileTextOutlined />}
                    />
                  </Col>
                  <Col span={8}>
                    <Statistic
                      title="检查点同步时间"
                      value={
                        systemData.checkpoint_stats[0].checkpoint_sync_time
                      }
                      suffix="ms"
                      prefix={<FileTextOutlined />}
                    />
                  </Col>
                  <Col span={8}>
                    <Statistic
                      title="缓冲区检查点"
                      value={systemData.checkpoint_stats[0].buffers_checkpoint}
                      prefix={<ThunderboltOutlined />}
                    />
                  </Col>
                  <Col span={8}>
                    <Statistic
                      title="后端缓冲区"
                      value={systemData.checkpoint_stats[0].buffers_backend}
                      prefix={<ThunderboltOutlined />}
                    />
                  </Col>
                </Row>
              </Card>
            )}

          {/* WAL Checkpoint Info */}
          {systemData.wal_checkpoint_info &&
            systemData.wal_checkpoint_info.length > 0 && (
              <Card title="WAL和检查点信息">
                <Row gutter={[16, 16]}>
                  <Col span={8}>
                    <Statistic
                      title="当前WAL位置"
                      value={systemData.wal_checkpoint_info[0].current_wal_lsn}
                      prefix={<FileTextOutlined />}
                    />
                  </Col>
                  <Col span={8}>
                    <Statistic
                      title="WAL字节数"
                      value={systemData.wal_checkpoint_info[0].wal_bytes}
                      prefix={<HddOutlined />}
                    />
                  </Col>
                  <Col span={8}>
                    <Statistic
                      title="定时检查点"
                      value={
                        systemData.wal_checkpoint_info[0].checkpoints_timed
                      }
                      prefix={<ClockCircleOutlined />}
                    />
                  </Col>
                </Row>
              </Card>
            )}

          {/* Replication Status */}
          <Card title="复制状态">
            {systemData.replication_status &&
            systemData.replication_status.length > 0 ? (
              <Table
                dataSource={systemData.replication_status.map(
                  (rep: any, index: number) => ({ ...rep, key: index }),
                )}
                size="small"
              />
            ) : (
              <Alert message="当前没有复制配置" type="info" />
            )}
          </Card>
        </Space>
      ),
    },
  ];

  return (
    <div style={{ padding: "24px" }}>
      <Space direction="vertical" size="large" style={{ display: "flex" }}>
        <div
          style={{
            display: "flex",
            justifyContent: "space-between",
            alignItems: "center",
          }}
        >
          <h1>系统监控</h1>
          <Button type="primary" onClick={getInfoAndParse} loading={loading}>
            刷新数据
          </Button>
        </div>

        <Tabs items={tabItems} size="large" type="card" />
      </Space>
    </div>
  );
}
