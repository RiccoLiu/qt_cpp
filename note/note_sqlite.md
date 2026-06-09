
# SQL 基本概念

| 概念 | 说明|
|------|------|
| 表（Table） | 数据以行（记录）和列（字段）形式存储。 |
| 主键（Primary Key） | 唯一标识一条记录。|
| 外键（Foreign Key） | 关联其他表的主键 |

# 安装 SQLite3

SQLite 下载地址: https://www.sqlite.org/download.html, 下载文件: sqlite-tools-win-x64-3510300.zip

下载解压到指定路径后，设置环境变量, PowerShell 中执行 sqlite3.exe --version 显示版本号表示安装成功。

```
PS E:\LC\work\tmp\sqlite_test> sqlite3.exe --version
3.51.3 2026-03-13 10:38:09 737ae4a34738ffa0c3ff7f9bb18df914dd1cad163f28fd6b6e114a344fe6d618 (64-bit)
PS E:\LC\work\tmp\sqlite_test>
PS E:\LC\work\tmp\sqlite_test> sqlite3.exe tmp.db
SQLite version 3.51.3 2026-03-13 10:38:09
Enter ".help" for usage hints.
sqlite>
sqlite> CREATE TABLE users (id INTEGER, name TEXT); // 创建表:users, 第一列:id(INTEGER), 第二列: name(TEXT)
sqlite>
sqlite> INSERT INTO users VALUES (1, 'Alice');   // users 表中插入一条数据(1, 'Alice')
sqlite>
sqlite> INSERT INTO users VALUES (2, 'Bob');     // users 表中插入一条数据(2, 'Bob')
sqlite>
sqlite> SELECT * FROM users;                     //  显示 users 表所有数据

```
##  SQLite Shell 指令

| 命令 | 说明 |
|------|------|
| .headers on | 查询结果中显示列名 |
| .mode column | 让查询结果对齐成表格形式 |
| .tables | 列出当前数据库中的所有表 |
| .schema [table] | 显示建表语句（不加表名则显示所有） |
| .database | 显示当前打开的数据库文件路径 |
| .read file.sql | 执行一个 SQL 脚本文件 |
| .exit 或 .quit | 退出 SQLite 回到 PowerShell |

# 数据类型

## 基本类型

| 类型 | 说明 | 使用场景 |
|---------|------|---------|
| INT / INTEGER | 整型（通常为 4 字节，范围约 ±21 亿）还有 TINYINT（1字节）、SMALLINT（2字节）、BIGINT（8字节）等变体 | 用户 ID、计数器、状态码、自增主键等 |
| DECIMAL(p,s) / NUMERIC(p,s) | 精确小数，p=总位数，s=小数位数(如 DECIMAL(10,2) 表示最多 8 位整数 + 2 位小数) | 金额、财务数据、科学计量(避免浮点误差) |
| FLOAT / REAL | 单精度浮点数（近似值，约 7 位有效数字） | 科学计算、传感器读数（允许微小误差） |
| DOUBLE / DOUBLE PRECISION | 双精度浮点数（近似值，约 15 位有效数字） | 地理坐标（经纬度）、物理模拟、AI 特征值 |
| CHAR(n) | 定长字符串，固定占用 n 字符空间（不足用空格填充） | 存储长度固定的代码，如国家代码（CHAR(2)）、性别（CHAR(1)）、MD5 的 hex 字符串（CHAR(32)） |
| VARCHAR(n) | 可变长度字符串，实际存储 = 内容长度 + 长度前缀 | 用户名、邮箱、标题、地址等长度不固定的文本 |
| TEXT | 大文本类型（MySQL 中最大 64KB；PostgreSQL 无硬限制） 变体：TINYTEXT, MEDIUMTEXT, LONGTEXT | 文章正文、评论、日志、JSON 字符串、配置内容 |
| BOOLEAN / BOOL | 布尔值（TRUE/FALSE）⚠️ MySQL 不支持原生 BOOLEAN，常用 TINYINT(1) 模拟（0=false, 1=true | 开关状态（是否激活、是否删除、是否已读） |

## 二进制类型

| 类型 | 说明 | 使用场景 |
|------|------|---------|
| BINARY(n) | 定长二进制类型(最大255字节) | MD5 哈希（16字节）、UUID（16字节） | 
| VARBINARY(n) | 可变长二进制类型(最多65535字节)，通常小于64KB | Session ID、加密 token、签名|
| BLOB | 二进制大对象(大文本，图片，文件等) | 图片、PDF、加密文件、固件包 |

| MySQL BLOB 变种 | 说明 |
|------|------|
| TINYBLOB | 最大255字节 |
| BLOB | 最大 65,535 字节（≈64KB） |
| MEDIUMBLOB | 最大 16MB |
| LONGBLOB | 最大 4GB |


## 日期类型

| 类型 | 说明 |
|------|------|
| DATE | 日期 |
| TIME | 时间 |
| DATATIME | 日期时间 |
| TIMESTAMP | 时间戳 |
| YEAR |  年 |

## 其他特殊类型

| 类型 | 说明 |
|------|------|
| ENUM | 枚举类型(MySQL)|
| SET | 集合类型(MySQL)|
| JSON | 原生JSON类型(MySQL)|
| UUID / GUID | 通用唯一标识符（部分数据库原生支持） |

# 标准指令

## 语法

查询语句
```
SELECT name, email FROM users 
[WHERE]
[GROUP BY]
[HAVING]
[ORDER BY]
[LIMIT [OFFSET]]
```


## DDL(Data Definition Language) - 定义结构


| 命令 | 说明 | 使用方法 | 示例 |
|------|-----|------|--|
| CREATE | 创建表结构 | CREATE TABLE <表单> <(字段1, 字段2, 字段3)>  | CREATE TABLE users (id INT PRIMARY KEY, name VARCHAR(50)); |
| ALTER | 修改表结构 |  ALTER TABLE <表单> ADD COLUMN <字段，数据类型> // RENAME TO 重命名表单 | ALTER TABLE users ADD COLUMN email VARCHAR(100); |
| DROP | 删除表或数据库 | DROP TABLE <表单> | DROP TABLE users; |
| TRUNCATE | 清空数据(保留结构) | TRUNCATE TABLE <表单> | TRUNCATE TABLE users; |


**DELETE FROM table 也能清空数据，但 TRUNCATE 更快且不记录单条日志。**

## DML(Data Manipulation Language) - 操作数据

| 命令 | 说明 | 使用方法 | 示例 |
|------|-----|----------|-----|
| SELECT | 查询数据 | SELECT [字段1，字段2] FROM <表单> [WHERE 条件语句] | SELECT name, email FROM users WHERE id > 10 |
| INSERT | 插入数据 | INSERT INTO <表单> [(字段1，字段2)] VALUS <(数据1, 数据2)> | INSERT INTO users (name, email)  VALUES ('Alice', 'alice@example.com') |
| UPDATE | 更新数据 | UPDATE <表单> SET <字段=数据> [WHERE 条件语句] | UPDATE users SET email = 'new@example.com' WHERE id = 1;  |
| DELETE | 删除数据 | DELETE FROM <表单> [WHERE 条件语句] | DELETE FROM users WHERE id = 1; | 


## DQL(Data Query Language) - 查询数据

- 查询
```
SELECT * FROM table_name;                   -- 查询所有字段
SELECT id, username FROM table_name;        -- 查询指定字段
SELECT name FROM table_name WHERE id > 10;  -- 查询指定字段中，id 大于10的字段
SELECT * FROM products ORDER BY price DESC; -- 查询所有字段，根据 price 降序排列, (ASC升序, DESC降序)
```

- 分页

| 特性 | 偏移分页 | 游标分页 |
|-----|----------|---------|
| 语法 | LIMIT <page_size> OFFSET <offset> | WHERE id > position ORDER BY id LIMIT <page_size> |
| 性能 | 深分页性能差 | 稳定较快 |
| 优缺点 | 支持跳转指定页 | 只能上一页 / 下一页 |
| 适用场景 | 后台管理、小数据集 | 大数据集 |

1. 偏移分页

```
SELECT <字段> FROM <表单> ORDER BY id LIMIT <page_size> OFFSET <offset>;    -- 按照page_size分页，跳过offset 显示，跳过1页时 offset = page_size
````

2. 游标分页

根据游标位置(id)直接跳转到指定起点，游标字段需要有唯一性、有序性(如：时间、ID等)

```
SELECT <字段> FROM <表单> <WHERE id > 100> ORDER BY id LIMIT <page_size>;
```

- 聚合功能

```
SELECT COUNT(*) FROM <表单>;        -- 查看表单有多少行数据
SELECT AVG(age) FROM <表单>;        -- 查看表单中的平均年龄
SELECT SUM(age) FROM <表单>;        -- 查看表单中的求和年龄
SELECT MAX/MIN(age) FROM <表单>;    -- 查看表单中的最大/最小年龄
```

- 分组

操作顺序: 先分组，再聚合，最后用 HAVING 筛选组

```
SELECT <字段>, COUNT(*)  FROM <表单> GROUP BY <字段> HAVING COUNT(*) > 5;
```

## DCL(Data Control Language) - 权限控制(MySQL & SQL Server)

```
GRANT SELECT ON users TO 'user1'@'host';           -- 授予 user1 查询 users 数据库操作权限
REVOKE INSERT ON users FROM 'user1'@'host';        -- 撤销 user1 插入 users 数据库操作权限
```

```
CREATE USER 'reporter'@'localhost' IDENTIFIED BY 'secure_password';         -- 创建用户
GRANT SELECT ON myapp.users TO 'reporter'@'localhost';                      -- 授予查询权限
REVOKE INSERT, UPDATE, DELETE ON myapp.users FROM 'reporter'@'localhost';   -- 禁止插入、更新、删除操作
```

## TCL(Transaction Control Language) - 事务控制

```
BEGIN;          -- 开始事务（部分数据库自动开启）
UPDATE accounts SET balance = balance - 100 WHERE user = 'A';
UPDATE accounts SET balance = balance + 100 WHERE user = 'B';
COMMIT;         -- 提交
-- ROLLBACK;    -- 回滚（取消更改）
```

## 多表查询

### 多表连接(JOIN)

同时获取多个表的内容，多个表之间通过id进行关联，

| 连接类型 | 语法 | 功能说明 |
|-------|------|---------|
| 内连接 | SELECT <别名1.字段>, <别名2.字段> FROM <表单1> <别名1> INNER JOIN <表单2> <别名2> ON <条件语句> | 显示匹配成功的行 |
| 左连接 | SELECT <别名1.字段>, <别名2.字段> FROM <表单1> <别名1> LEFT JOIN <表单2> <别名2> ON <条件语句> | 显示左表所有记录，如果右表没有匹配将会填充NULL |
| 右连接 | SELECT <别名1.字段>, <别名2.字段> FROM <表单1> <别名1> RIGHT JOIN <表单2> <别名2> ON <条件语句> | 显示右表所有记录，如果左表没有匹配将会填充NULL |

```
-- 列出所有订单及其用户信息
SELECT o.id AS order_id, u.name, u.email, o.amount
FROM orders o
INNER JOIN users u ON o.user_id = u.id;

-- 在左表中筛选出右表没有匹配的行
SELECT u.name
FROM users u
LEFT JOIN orders o ON u.id = o.user_id
WHERE o.user_id IS NULL;  -- 关键！筛选出右表无匹配的行

-- 显示所有用户及其订单数量
SELECT u.name, COUNT(o.id) AS order_count
FROM users u
LEFT JOIN orders o ON u.id = o.user_id
GROUP BY u.id, u.name;

// name	    order_count
// Alice	2
// Bob	    1
// Carol	0

```

### 子查询

1. IN /  NOT IN
```
SELECT name FROM users WHERE id IN (
    SELECT user_id FROM orders WHERE amount > 1000
);    -- 从orders表中找到 mount > 1000的 user_id, 这个user_id 和 users表中的id相关联，根据这个id取出users表中对应的name

```
2. EXISTS / NOT EXISTS

EXITS 只关心子查询是否返回至少一行，不关心具体数据，找到匹配数据即停，通常性能更好。

```
SELECT name FROM users u    // users表取别名 u 
WHERE EXISTS (
    SELECT 1                // 表示只关心是否存在，不关心具体数据
    FROM orders o           // orders 表取别名 o
    WHERE o.user_id = u.id AND o.amount > 1000  // 满足条件
);  -- 遍历 users 表数据，每一行数据 快速定位o.user_id = u.id的数据，任意一个amount > 1000就返回，不再继续查询
```

3. NULL 值问题

| 表达式 | 遇到 NULL 时的行为 |
|-------|-------------------|
| value IN (..., NULL, ...) | 有匹配值返回TRUE，无匹配值返回 UNKNOW 等效于 FALSE |
| value NOT IN (..., NULL, ...) | 有NULL 直接返回 UNKNOW 等效于 FALSE |
| value EXIT | 不受 NULL 影响 |
| value NOT EXIT | 不受 NULL 影响 |

### 联合查询

-- 自动去重；用 UNION ALL 可保留重复
| 操作符 | 是否去重 | 性能 | 使用场景 |
|-------|---------|------|----------|
| UNION | 是 | 较慢(哈希表) | 需要唯一结果 |
| UNION ALL | 否 | 更快(直接拼接) | 允许行重复 |

```
1. 基本用法
SELECT full_name FROM students UNION SELECT teacher_name FROM teachers;  -- 结果默认使用第一个full_name字段

2. 联合查询结果重命名列字段为role, 联合结果排序
(SELECT name, 'student' AS role FROM students) UNION (SELECT name, 'teacher' AS role FROM teachers) ORDER BY name; 
```

## 插入唯一选项

```
-- 建表时创建唯一约束
CREATE TABLE IF NOT EXISTS users (
    id INTEGER PRIMARY KEY,
    name TEXT,
    gender INTEGER,
    age INTEGER,
    UNIQUE(name, gender, age)
);

-- 如果表已存在，添加唯一约束
CREATE UNIQUE INDEX IF NOT EXISTS idx_users_unique ON users(name, gender, age);

```

## 实用命令

```
PRAGMA foreign_keys = ON;   // 启用外键 
PRAGMA foreign_keys;        // 查看外键是否生效
PRAGMA table_info(表名);    // 查看表结构
.schema 表名                // 查看建表语句
.tables                     // 列出所有表


```

# 性能优化

## 索引

数据库建立时只会为主键基于B+树建立索引，其他字段基于无序堆表的结构。建立索引可以是为其他字段提供查询、排序、分组、连接等操作的性能优化。

索引的优势: 基于B+树建立的索引，可以将性能由 O(n) 提升到 O(log n)  
索引的代价：占用存储空间，拖慢写操作   
索引适用场景：大表 + 高频查询 + 写操作少  

| 功能 | 说明 |
|------|------|
| 建立索引 | CREATE INDEX idx_表单_字段 ON <表单> <(字段1, 字段2, ...)> |
| 删除索引 | DROP INDEX idx_表单_字段 ON <表单>; |
| 查看索引 | SHOW INDEX FROM <表单>; | 

## 小TIPS

1. 慎用 SELECT COUNT(*) > 0 替代 EXISTS
2. EXISTS 进行子查询时，使用: SELECT 1 FROM <表单> 替代: SELECT * FROM <表单>
3. 多表查询时，关联字段建议加索引
4. 小表驱动大表， 过滤后数据量小的放在左表

# 测试

```
sqlite> CREATE TABLE student_test (id INT PRIMARY KEY, name VARCHAR(64), age INT, class VARCHAR(64));   -- 创建表(id不会自增)
sqlite> INSERT INTO student_test (name, age, class) VALUES ('lili', 10, 'grade_0');                     -- 插入数据
sqlite> INSERT INTO student_test (name, age, class) VALUES ('lanlan', 12, 'grade_0');                   -- 插入数据
sqlite> SELECT * FROM student_test;                                                                     -- 显示所有数据(id没有自增)
id  name    age  class
--  ------  ---  -------
    lili    10   grade_0
    lanlan  12   grade_0
sqlite>
sqlite> CREATE TABLE student_test_new (id INTEGER PRIMARY KEY, name VARCHAR(64), age INT, class VARCHAR(64));   -- 重建新表(id可以自增)
sqlite> INSERT INTO student_test_new (name, age, class) SELECT name, age, class FROM student_test;              -- 迁移数据
sqlite> SELECT * FROM student_test_new;                                                                         -- 显示所有数(id自增)
id  name    age  class
--  ------  ---  -------
1   lili    10   grade_0
2   lanlan  12   grade_0     
sqlite> DROP TABLE student_test;        -- 删除旧表
sqlite> INSERT INTO student_test_new (name, age, class) VALUES ('liuchong', 34, 'grade_100');   -- 插入数据
sqlite> INSERT INTO student_test_new (name, age, class) VALUES ('bry', 33, 'grade_90');         -- 插入数据
sqlite> INSERT INTO student_test_new (name, age, class) VALUES ('lsy', 3, 'grade_1');           -- 插入数据
sqlite> UPDATE student_test_new SET name='liushiyao' WHERE name='lsy';                          -- 更新数据
sqlite> SELECT * FROM student_test_new;                                                         -- 显示数据
id  name       age  class
--  ---------  ---  ---------
1   lili       10   grade_0
2   lanlan     12   grade_0
3   liuchong   34   grade_100
4   bry        33   grade_90
5   liushiyao  3    grade_1
sqlite>
sqlite> DELETE FROM student_test_new WHERE class='grade_0';     -- 删除指定数据
sqlite> SELECT * FROM student_test_new;                         -- 显示数据
id  name       age  class
--  ---------  ---  ---------
3   liuchong   34   grade_100
4   bry        33   grade_90
5   liushiyao  3    grade_1
sqlite> INSERT INTO student_test_new (name, age, class) VALUES ('wfy', 54, 'grade_60');     -- 插入数据
sqlite> SELECT * FROM student_test_new;                                                     -- 显示数据，新插入的数据id不会填充空的id
id  name       age  class
--  ---------  ---  ---------
3   liuchong   34   grade_100
4   bry        33   grade_90
5   liushiyao  3    grade_1
6   wfy        54   grade_60


sqlite> ALTER TABLE student_test_new ADD COLUMN address VARCHAR(64);    -- 修改表的结构，增加address字段
sqlite> SELECT * FROM student_test_new;
id  name      age  class      address
--  --------  ---  ---------  -------
3   liuchong  34   grade_100
4   bry       33   grade_90
5   lsy       3    grade_1
6   wfy       54   grade_60
sqlite>
sqlite> UPDATE student_test_new SET address='default'                   -- 更新所有数据address字段
sqlite> SELECT * FROM student_test_new;
id  name      age  class      address
--  --------  ---  ---------  -------
3   liuchong  34   grade_100  default
4   bry       33   grade_90   default
5   lsy       3    grade_1    default
6   wfy       54   grade_60   default
sqlite>
sqlite> ALTER TABLE student_test_new RENAME TO student_test;                -- 修改变的结构，更改表单名字
sqlite>
sqlite> SELECT * FROM student_test WHERE id > 3 AND id < 6 AND age > 10;    -- WHERE条件筛选 AND/OR
id  name  age  class     address
--  ----  ---  --------  -------
4   bry   33   grade_90  default
sqlite>
sqlite> SELECT * FROM student_test ORDER BY age DESC;                       -- 按序排列(ASC：升序, DESC：降序)
id  name      age  class      address
--  --------  ---  ---------  -------
6   wfy       54   grade_60   default
3   liuchong  34   grade_100  default
4   bry       33   grade_90   default
5   lsy       3    grade_1    default
sqlite>
sqlite> ALTER TABLE student_test ADD COLUMN gender VARCHAR(32);             -- 添加新字段gender
sqlite> UPDATE student_test SET gender='girl';                              
sqlite> UPDATE student_test SET gender='boy' WHERE name='liuchong';          
sqlite> 
sqlite> SELECT * FROM student_test;                                         -- 显示数据
id  name      age  class      address  gender
--  --------  ---  ---------  -------  ------
3   liuchong  34   grade_100  default  boy
4   bry       33   grade_90   default  girl
5   lsy       3    grade_1    default  girl
6   wfy       54   grade_60   default  girl
sqlite> SELECT gender, AVG(age) as women_age_avg FROM student_test GROUP BY gender;     -- 分组显示每组平均年龄
gender  women_age_avg
------  -------------
boy     34.0
girl    30.0
sqlite> SELECT gender, COUNT(*) as total_count FROM student_test GROUP BY gender;       -- 分组显示每组的数量
gender  total_count
------  -----------
boy     1
girl    3
sqlite> SELECT gender, COUNT(*) as total_count FROM student_test GROUP BY gender HAVING COUNT(*) > 2; -- 分组显示每组数量(HAVING 过滤)
gender  total_count
------  -----------
girl    3
sqlite>
sqlite> CREATE TABLE student_test2 (id INTEGER PRIMARY KEY, name VARCHAR(64), age INT, address VARCHAR(128), gend
er VARCHAR(64));
sqlite> INSERT INTO student_test2 (id, name, age, address, gender) VALUES
   ...> (1, '张伟', 18, '北京市朝阳区', '男'),
   ...> (2, '李娜', 17,  '上海市浦东新区', '女'),
   ...> (3, '王强', 19,  '广州市天河区', '男'),
   ...> (4, '刘芳', 16, '深圳市南山区', '女'),
   ...> (5, '陈浩', 18, '杭州市西湖区', '男'),
   ...> (6, '杨雪', 17, '南京市鼓楼区', '女'),
   ...> (7, '赵磊', 19, '成都市武侯区', '男'),
   ...> (8, '黄婷', 16, '武汉市江汉区', '女'),
   ...> (9, '周杰', 18, '西安市雁塔区', '男'),
   ...> (10, '吴敏', 17, '重庆市渝中区', '女'),
   ...> (11, '徐洋', 19, '天津市和平区', '男'),
   ...> (12, '孙莉', 16, '苏州市工业园区', '女'),
   ...> (13, '胡军', 18, '长沙市岳麓区', '男'),
   ...> (14, '郭静', 17, '郑州市金水区', '女'),
   ...> (15, '何勇', 19, '青岛市市南区', '男'),
   ...> (16, '林燕', 16, '大连市沙河口区', '女'),
   ...> (17, '高飞', 18, '厦门市思明区', '男'),
   ...> (18, '郑洁', 17, '福州市鼓楼区', '女'),
   ...> (19, '罗斌', 19, '合肥市蜀山区', '男'),
   ...> (20, '梁婷', 16, '济南市历下区', '女');
sqlite>
sqlite> SELECT * FROM student_test2 ORDER BY id LIMIT 5 OFFSET 10;  -- 偏移分页，跳2页显示(offset = 2 * page_size)
id  name  age  address         gender
--  ----  ---  --------------  ------
11  徐洋  19   天津市和平区    男
12  孙莉  16   苏州市工业园区  女
13  胡军  18   长沙市岳麓区    男
14  郭静  17   郑州市金水区    女
15  何勇  19   青岛市市南区    男

sqlite> SELECT * FROM student_test2 ORDER BY id LIMIT 5 OFFSET 2;   -- 偏移分页，跳2行显示
id  name  age  address       gender
--  ----  ---  ------------  ------
3   王强  19   广州市天河区  男
4   刘芳  16   深圳市南山区  女
5   陈浩  18   杭州市西湖区  男
6   杨雪  17   南京市鼓楼区  女
7   赵磊  19   成都市武侯区  男
sqlite> SELECT * FROM student_test2 WHERE id > 0  ORDER BY id LIMIT 5;  -- 游标分页，游标记录位置: 0
id  name  age  address         gender
--  ----  ---  --------------  ------
1   张伟  18   北京市朝阳区    男
2   李娜  17   上海市浦东新区  女
3   王强  19   广州市天河区    男
4   刘芳  16   深圳市南山区    女
5   陈浩  18   杭州市西湖区    男
sqlite>
sqlite> SELECT * FROM student_test2 WHERE id > 5  ORDER BY id LIMIT 5;  -- 游标分页，游标记录位置: 5
id  name  age  address       gender
--  ----  ---  ------------  ------
6   杨雪  17   南京市鼓楼区  女
7   赵磊  19   成都市武侯区  男
8   黄婷  16   武汉市江汉区  女
9   周杰  18   西安市雁塔区  男
10  吴敏  17   重庆市渝中区  女
sqlite>
sqlite> SELECT * FROM student_test2 WHERE id > 10  ORDER BY id LIMIT 5; -- 游标分页，游标记录位置: 10
id  name  age  address         gender
--  ----  ---  --------------  ------
11  徐洋  19   天津市和平区    男
12  孙莉  16   苏州市工业园区  女
13  胡军  18   长沙市岳麓区    男
14  郭静  17   郑州市金水区    女
15  何勇  19   青岛市市南区    男
sqlite>
sqlite> DELETE FROM student_test;       -- 删除表内所有数据
sqlite> DROP TABLE student_test;        -- 删除表
```

1. sqlite 自增主键必须使用 INTEGER 类型绑定 rowid; 自增属性 AUTOINCREMENT 只能对主键 且是 INTEGER 类型使用。
2. ALTER 修改表结构只支持 增加一列 和 表单重命名。


# SQLITE API

| 函数 | 是否必须检查返回值 | 返回值 |
|------|-----------------|---------|
| sqlite3_exec | 是 | SQLITE_OK |
| sqlite3_prepare_v2  | 是 | SQLITE_OK |
| sqlite3_step | 是 | SQLITE_ROW / SQLITE_DONE |
| sqlite3_reset | 可选 | SQLITE_OK |
| sqlite3_finalize | 可选 | SQLITE_OK |
| sqlite3_bind_xxx | 否 | 绑定索引从1开始 |

# Q&A

1. SQL 注入问题

当直接使用字符串拼接作查询会有SQL注入风险，比如查询语句如下：
```
query = f"SELECT * FROM users WHERE username = '{username}' AND password = '{password}'"
```

正常用户输入username='admin',password='123456'登录成功。

攻击者在密码框输入: ' OR '1'='1 绕过验证，查询语句拼接如下：
```
query = f"SELECT * FROM users WHERE username = 'admin' AND password = '' OR '1'='1'"
```
这样不需要password就可以查询数据库。

为房主SQL注入问题，推荐使用参数化查询的方式。

2. 创建唯一约束失败问题
```
-- 对已有表创建唯一约束命令
CREATE UNIQUE INDEX IF NOT EXISTS idx_users_unique ON users(name, gender, age);

-- 报错日志
Runtime error: UNIQUE constraint failed: users.name, users.gender, users.age (19)

-- 报错原因： 
表中已经存在重复的 (name, gender, age) 组合

-- 修复报错
-- 1. 查看重复的 (name, gender, age) 组合
SELECT name, gender, age, COUNT(*) FROM users GROUP BY name, gender, age HAVING COUNT(*) > 1;

-- 2. 删除重复的组合后，重新创建唯一约束索引命令
```