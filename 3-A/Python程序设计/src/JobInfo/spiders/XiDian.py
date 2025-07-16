import re

import scrapy

from JobInfo.items import JobinfoItem


class XidianSpider(scrapy.Spider):
    """
    爬取西电招聘信息的爬虫。

    :ivar str name: 爬虫名称。
    :ivar list[str] allowed_domains: 允许的域。只会爬取该域下的url。
    """
    name = 'XiDian'
    allowed_domains = ['job.xidian.edu.cn']

    def start_requests(self):
        """
        开始爬取第一页，生成一个请求，使用playwright作为Downloader。

        :yield: 对第一页的请求。
        """
        yield scrapy.Request('https://job.xidian.edu.cn/campus/index/do1/job.xidian.edu.cn/domain/xidian/city',
                             meta={'playwright': True})

    def parse(self, response):
        """
        该生成器函数为 `scrapy.Request` 默认回调函数，用来处理获取到的html。
        由于html中只有主题和日期，还要进一步获取浏览次数。

        :param response: 请求得到的响应。
        :yield: 对浏览次数的请求，或对下一页的请求。
        """
        # 通过css选择器得到信息列表
        infos = response.css('.infoBox')[0].css('ul.infoList')
        for info in infos:
            # 通过css选择器得到主题、日期以及用于获得浏览次数的url
            topic = info.css('li:nth-child(1) > a::text').get()
            date = info.css('li:nth-child(2)::text').get()
            url = info.css('li:nth-child(1) > a').attrib['href']
            if date < '2024-09-01 00:00:00':
                continue
            yield scrapy.Request(response.urljoin(url), callback=self.parse_views,
                                 cb_kwargs={'topic': topic, 'date': date})

        # 最后一页的“下一页”链接与自身相同，以此判断是否到达最后一页。如果没有，继续爬下一页。
        next_page = response.css('.next > a:nth-child(1)').attrib['href']
        next_page_url = response.urljoin(next_page)
        if next_page_url != response.url:
            yield scrapy.Request(next_page_url, meta={'playwright': True})

    def parse_views(self, response, topic, date):
        """
        该生成器函数为 `parse` 中调用的请求浏览次数的回调函数，用于从响应中得到浏览次数。

        :param response: 请求得到的响应。
        :param topic: 招聘主题。由 `cb_kwargs` 传递。
        :param date: 发布日期。由 `cb_kwargs` 传递。
        :yield: 一个 `JobinfoItem` 对象。
        """
        # 用css选择器得到浏览次数
        views = response.css('.share > ul:nth-child(1) > li:nth-child(2)::text').get()
        # 用正则表达式得到数字
        match = re.search(r'\d+', views)
        # 没有数字则是0
        if match:
            views = int(match.group())
        else:
            views = 0

        item = JobinfoItem()
        item['topic'] = topic
        item['date'] = date
        item['views'] = views
        yield item
