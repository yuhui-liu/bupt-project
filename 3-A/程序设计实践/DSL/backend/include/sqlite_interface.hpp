/**
 * @file sqlite_interface.hpp
 * @brief SQLiteDB 类的声明。
 */
#ifndef SQLITE_INTERFACE_HPP
#define SQLITE_INTERFACE_HPP

#include <spdlog/spdlog.h>
#include <sqlite3.h>

#include <string>

/// @brief 用于 query 和 write 的函数参数
struct DB {
  std::string tbl_name, col_name;
};

/**
 * @class   SQLiteDB
 * @brief   SQLite 数据库接口
 * @details SQLite 数据库接口，用于查询和写入数据库。
 */
class SQLiteDB {
 public:
  /**
   * @brief 构造函数
   * @param dbName 数据库名称
   */
  explicit SQLiteDB(const std::string& dbName) {
    if (sqlite3_open(dbName.c_str(), &db) != SQLITE_OK) {
      spdlog::error("无法打开数据库：{}", sqlite3_errmsg(db));
      db = nullptr;
    }
  }

  /// @brief 析构函数，关闭数据库连接
  ~SQLiteDB() {
    if (db != nullptr) {
      sqlite3_close(db);
    }
  }

  [[nodiscard]] auto query(int id, const DB& dbInfo) const -> std::string;
  [[nodiscard]] auto write(int id, const DB& dbInfo, const std::string& content) const -> bool;

  // 禁用默认拷贝构造函数和赋值运算符，确保对象不会被复制，保证数据库连接的唯一性
  SQLiteDB(const SQLiteDB&) = delete;
  auto operator=(const SQLiteDB&) -> SQLiteDB& = delete;

 private:
  /// @brief SQLite 数据库指针
  sqlite3* db{nullptr};
};

#endif  // SQLITE_INTERFACE_HPP
