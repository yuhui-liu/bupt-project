import os
from typing import Dict, Optional


class SQLLoader:
    """SQL查询文件加载器"""

    def __init__(self, sql_dir: str = None):
        if sql_dir is None:
            # 获取当前文件的目录，然后构建sql目录的路径
            current_dir = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
            self.sql_dir = os.path.join(current_dir, "sql")
        else:
            self.sql_dir = sql_dir

        self._queries = {}
        self._load_queries()

    def _load_queries(self):
        """加载所有SQL查询文件"""
        if not os.path.exists(self.sql_dir):
            return

        for root, dirs, files in os.walk(self.sql_dir):
            for file in files:
                if file.endswith(".sql"):
                    file_path = os.path.join(root, file)
                    # 生成查询名称：目录名_文件名（不含扩展名）
                    rel_path = os.path.relpath(file_path, self.sql_dir)
                    query_name = rel_path.replace(os.sep, "_").replace(".sql", "")

                    try:
                        with open(file_path, "r", encoding="utf-8") as f:
                            self._queries[query_name] = f.read().strip()
                    except Exception as e:
                        print(f"加载SQL文件失败 {file_path}: {e}")

    def get_query(self, name: str) -> Optional[str]:
        """获取指定名称的SQL查询"""
        return self._queries.get(name)

    def get_all_queries(self) -> Dict[str, str]:
        """获取所有SQL查询"""
        return self._queries.copy()

    def reload(self):
        """重新加载所有SQL查询"""
        self._queries.clear()
        self._load_queries()


# 创建全局实例
sql_loader = SQLLoader()
