/**
 * @file  sqlite_interface.cpp
 * @brief SQLiteDB 类的实现。
 */

#include "sqlite_interface.hpp"

/**
 * @brief 执行查询操作，从指定表中获取指定列的值。
 *
 * @param id 要查询的记录的ID。
 * @param dbInfo 数据库信息。
 * @return std::string 查询结果，如果未找到数据或发生错误则返回空字符串。
 */
auto SQLiteDB::query(const int id, const DB& dbInfo) const -> std::string {
  auto [tblName, colName] = dbInfo;
  if (db == nullptr) {
    return {};
  }

  const std::string query = "SELECT " + colName + " FROM " + tblName + " WHERE ID = ?";
  sqlite3_stmt* stmt;
  std::string result;

  if (sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
    spdlog::error("准备语句失败：{}", sqlite3_errmsg(db));
    return {};
  }

  if (sqlite3_bind_int(stmt, 1, id) != SQLITE_OK) {
    spdlog::error("绑定ID失败：{}", sqlite3_errmsg(db));
    sqlite3_finalize(stmt);
    return {};
  }

  if (sqlite3_step(stmt) == SQLITE_ROW) {
    const auto* const data = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
    result = (data) != nullptr ? data : "";
  } else {
    spdlog::error("没有找到ID对应的数据：{}", id);
  }

  sqlite3_finalize(stmt);
  return result;
}

/**
 * @brief 执行写入操作，将指定内容写入指定表的指定列。
 *
 * @param id 要写入的记录的ID。
 * @param dbInfo 数据库信息。
 * @param content 要写入的内容。
 * @return bool 写入是否成功。
 */
auto SQLiteDB::write(const int id, const DB& dbInfo, const std::string& content) const -> bool {
  auto [tblName, colName] = dbInfo;
  if (db == nullptr) {
    return false;
  }

  const std::string query = "UPDATE " + tblName + " SET " + colName + " = ? WHERE ID = ?";
  sqlite3_stmt* stmt;

  if (sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
    spdlog::error("准备语句失败：{}", sqlite3_errmsg(db));
    return false;
  }

  if (sqlite3_bind_text(stmt, 1, content.c_str(), -1, SQLITE_STATIC) != SQLITE_OK) {
    spdlog::error("绑定content失败：{}", sqlite3_errmsg(db));
    sqlite3_finalize(stmt);
    return false;
  }

  if (sqlite3_bind_int(stmt, 2, id) != SQLITE_OK) {
    spdlog::error("绑定ID失败：{}", sqlite3_errmsg(db));
    sqlite3_finalize(stmt);
    return false;
  }

  const bool success = (sqlite3_step(stmt) == SQLITE_DONE);

  if (!success) {
    spdlog::error("执行更新失败：{}", sqlite3_errmsg(db));
  }

  sqlite3_finalize(stmt);
  return success;
}