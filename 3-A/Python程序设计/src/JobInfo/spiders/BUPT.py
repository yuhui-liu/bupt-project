import json
import time
from math import floor

import scrapy

from JobInfo.items import JobinfoItem


class BuptSpider(scrapy.Spider):
    """
    爬取北邮招聘信息的爬虫。

    :ivar str name: 爬虫名称。
    :ivar list[str] allowed_domains: 允许的域。只会爬取该域下的url。
    :ivar int timestamp: 当前时间戳。
    :ivar str base_url: 爬取的基本url。
    :ivar int current_page: 当前正在爬的页数。
    :ivar list[str] start_urls: 加上参数的url。
    :ivar done bool: 是否已经爬到了2024-09-01。
    """
    name = 'BUPT'
    allowed_domains = ['job.bupt.edu.cn']
    timestamp = floor(time.time() * 1000)
    base_url = f'https://job.bupt.edu.cn/f/recruitmentinfo/ajax_frontRecruitinfo?ts{timestamp}'
    current_page = 1
    start_urls = [f'{base_url}&pageNo={current_page}&pageSize=100']
    done = False

    def parse(self, response):
        """
        该生成器函数为 `scrapy.Request` 默认回调函数，用来处理获取到的json。
        由于json中只有主题和日期，还要进一步获取浏览次数。

        :param response: 请求得到的响应。
        :yield: 对浏览次数的请求，或对下一页的请求。
        """
        # 反序列化爬取到的json
        data = json.loads(response.text)
        # 招聘信息数组
        info_list = data['object']['list']
        for info in info_list:
            # 由于招聘信息是按时间倒序排放的，只要找到一个20240901之前的，就可以停止找下去。
            if info['startTime'] < '2024-09-01 00:00:00':
                self.done = True
                break
            # 必须添加该头，否则服务器无法正确解析请求
            headers = {'Content-Type': 'application/x-www-form-urlencoded; charset=UTF-8'}
            # 请求包含浏览次数的json，回调函数为parse_views，方法为POST，通过cb_kwargs向parse_views传递主题和日期
            yield scrapy.Request(f'https://job.bupt.edu.cn/f/recruitmentinfo/ajax_show?ts={self.timestamp}',
                                 callback=self.parse_views,
                                 method='POST',
                                 cb_kwargs={'topic': info['title'], 'date': info['startTime']},
                                 body=f'recruitmentId={info["id"]}', headers=headers)

        # 没有爬到20240901则继续。
        if not self.done:
            self.current_page += 1
            yield scrapy.Request(f'{self.base_url}&pageNo={self.current_page}&pageSize=100')

    def parse_views(self, response, topic, date):
        """
        该生成器函数为 `parse` 中调用的请求浏览次数的回调函数，用于从响应中得到浏览次数。

        :param response: 请求得到的响应。
        :param topic: 招聘主题。由 `cb_kwargs` 传递。
        :param date: 发布日期。由 `cb_kwargs` 传递。
        :yield: 一个 `JobinfoItem` 对象。
        """
        item = JobinfoItem()
        item['topic'] = topic
        item['date'] = date
        item['views'] = json.loads(response.text)['object']['recruitmentinfo']['browseNumber']
        yield item
