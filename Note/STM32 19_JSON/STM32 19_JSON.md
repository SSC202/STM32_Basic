# STM32 19_JSON

## 1. JSON 

JSON，全称是 JavaScript Object Notation，即 JavaScript 对象标记法。<br>
JSON是一种轻量级(Light-Meight)、基于文本的(Text-Based)、可读的(Human-Readable)格式。
>JSON 的名称中虽然带有JavaScript，但这是指其语法规则是参考JavaScript对象的，而不是指只能用于JavaScript 语言。
>
>JSON无论对于人，还是对于机器来说，都是十分便于阅读和书写的，而且相比 XML(另一种常见的数据交换格式)，文件更小，因此迅速成为网络上十分流行的交换格式。
>
>近年来 JavaScript 已经成为浏览器上事实上的标准语言，JavaScript 的风靡，与JSON 的流行也有密切的关系。
>
>因为JSON本身就是参考 JavaScript 对象的规则定义的，其语法与JavaScript定义对象的语法几乎完全相同。

- JSON 语法定义

1. 数组（`Array`）用方括号(“`[]`”)表示。
2. 对象（`Object`）用大括号(“`{}`”)表示。
3. 名称/值对(`name/value`)组合成数组和对象。
4. 名称(`name`)置于双引号中，值（`value`）有字符串、数值、布尔值、`null`、对象和数组。
5. 并列的数据之间用逗号(“`,`”)分隔

```json
{
    "name": "Hello World",
    "age": 13
}
```

- JSON 与 XML

相比 XML，JSON的优势如下:

没有结束标签,长度更短,读写更快<br>
能够直接被JavaScript解释器解析
可以使用数组<br>