import csv


class JobinfoPipeline:
    """
    将获取到的招聘信息预处理后存入csv文件。

    :ivar file: csv文件。
    :ivar writer: csv.writer。
    """
    def __init__(self):
        self.file = None
        self.writer = None

    def process_item(self, item, spider):
        """
        处理爬虫返回的item。

        :param item: 爬虫返回的item。
        :param spider: 爬虫。
        :return: None
        """
        # 将item转化为字典
        dict_item = dict(item)
        # 去除招聘主题首尾空白
        topic = dict_item['topic'].strip()
        # 获得日期的年月日
        date = dict_item['date'].split(' ')[0]
        views = dict_item['views']
        # views为空则是0
        if views == '':
            views = '0'
        # 转换为int
        views = int(views)
        # 写入文件中
        self.writer.writerow([topic, date, views])
        return item

    def open_spider(self, spider):
        """
        打开爬虫时打开 `{爬虫名}.csv` 文件，并用csv writer打开。
        """
        self.file = open(f'{spider.name}.csv', 'w', encoding='utf-8', newline='')
        self.writer = csv.writer(self.file)

    def close_spider(self, spider):
        """
        关闭爬虫时关闭文件。
        """
        self.file.close()
