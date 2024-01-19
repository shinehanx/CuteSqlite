**[中文介绍]**

**一、项目简介**

开源项目CuteSqlite，是轻量级数据库Sqlite的图形化管理客户端，使用C++/WTL实现，支持Windows操作系统。

![CuteSqlite](https://img-blog.csdnimg.cn/direct/6ebf21db2f4f48db9fda0e1463127d1e.png 'CuteSqlite')

**二、项目目标**

开发出轻量级的，性能优良的，功能完善的sqlite客户端。

**三、开发环境**

1.Windows 10+ / Win10 SDK

2.Visual Studio 2017 (v141) / C++ 11

3.Windows Template Library - WTL 10 - version 10.0.10320 Release (2020-11-15)

4.使用第三方项目：curl/rapidjson/Scintilla/spdlog/sqlite3

**四、遵循开源协议：**

[Apache2.0](http://www.apache.org/licenses/LICENSE-2.0)

**五、项目网址：**

GitHub: [https://github.com/shinehanx/CuteSqlite.git](https://github.com/shinehanx/CuteSqlite.git)
Release下载: [https://github.com/shinehanx/CuteSqlite/releases](https://github.com/shinehanx/CuteSqlite/releases)

**六、功能特点：**

1. **数据库浏览**：CuteSqlite允许用户浏览数据库中的所有对象，如表、视图、触发器等。用户可以轻松查看数据库结构，对数据库进行全面了解。
2. **查询执行**：CuteSqlite内置了一个强大的查询编辑器，用户可以在其中编写和执行SQL查询。此外，它还支持参数化查询，有助于提高安全性并减少SQL注入的风险。
3. **数据导入/导出**：CuteSqlite支持将数据从数据库导出到多种格式（如CSV、Excel等），同时也允许用户从这些格式导入数据到数据库中。这使得数据迁移和共享变得简单。
4. **索引管理**：用户可以通过CuteSqlite轻松创建、删除或修改数据库表的索引。索引是提高查询性能的关键，因此这个功能对于数据库性能优化非常有用。
5. **触发器和视图管理**：CuteSqlite允许用户创建、修改和删除数据库中的触发器和视图。触发器是自动执行特定操作的特殊类型的存储过程，而视图是基于SQL查询的结果集的可视化表。通过CuteSqlite，用户可以轻松管理这些数据库对象。
6. **数据库维护**：CuteSqlite还提供了一系列数据库维护工具，如数据备份、恢复和优化等。这些工具可以帮助用户保持数据库的健康和高效运行。
7. **日志和审计**：CuteSqlite支持详细的日志记录功能，可以记录所有对数据库进行的操作。这对于数据库的审计和安全性非常有用。
8. **性能分析**：针对SQL查询语句进行性能分析，分析索引是否命中，分析WHERE子句，ORDER字句，SELECT字段使用，覆盖索引，提供优化建议。
9. **存储分析**：分析数据库使用的页数，字节数，数据量等，列出详细的容量报告，并且可以针对每张数据表，每张表的索引进行存储容量分析，非常的方便实用。
10. **多语言支持**：CuteSqlite的用户界面支持多种语言，这意味着不同语言的用户都可以方便地使用它。

这些高级功能使得CuteSqlite成为一个功能完备的SQLite图形化管理客户端，适用于各种不同的应用场景，无论是个人使用还是企业级应用。

**七、项目预览**

![首页](https://img-blog.csdnimg.cn/direct/aada3b7672e54eef871c25ab081fa8ba.jpeg '首页')

![基本功能](https://img-blog.csdnimg.cn/direct/5ebcba761958443c97811a7e5fd2e9a4.png '基本功能')

![建表](https://img-blog.csdnimg.cn/direct/9f8263e757cd4023bdbee1da558c05db.png '建表')

![表复制，支持数据分片](https://img-blog.csdnimg.cn/direct/f828bd57429642c6bafc668726cba140.png '表复制，支持数据分片')

![性能分析](https://img-blog.csdnimg.cn/direct/7c03515e08854e2d918bb03cfacf85cf.png '性能分析')

![存储分析](https://img-blog.csdnimg.cn/direct/a57c8347739a4ab2b4972f745d50aa16.png '存储分析')

**八、英文介绍:**
[英文介绍](README.md)
