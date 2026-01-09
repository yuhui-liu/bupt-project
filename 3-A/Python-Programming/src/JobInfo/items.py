import scrapy


class JobinfoItem(scrapy.Item):
    """
    招聘信息的item。

    :ivar topic: 招聘主题。
    :ivar date: 发布日期。
    :ivar views: 浏览次数。
    """
    topic = scrapy.Field()
    date = scrapy.Field()
    views = scrapy.Field()
