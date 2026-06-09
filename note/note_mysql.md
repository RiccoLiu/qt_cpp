
# 安装 MySQL

下载MySQL安装包: https://dev.mysql.com/downloads/installer/

1. 安装MySQL: MySQL Server、 MySQL Workbench、 MySQL Shell

2. 设置环境变量: C:\mysql\mysql-8.0.28-winx64\bin

3. 创建 my.ini配置文件，使用powershell 管理员权限登录，Mysql初始化
```
.\mysqld.exe --initialize --console   # 记下临时密码！
```
4. 安装 MySQL 服务

```
PS E:\LC\work> Get-Service mysql*                   # 查看已经安装的MySQL服务
Status   Name               DisplayName
------   ----               -----------
Stopped  MySQL_TEST         MySQL_TEST
Stopped  mysql3310          mysql3310
PS E:\LC\work>
PS E:\LC\work>
PS E:\LC\work> tasklist | findstr mysql             # 查看mysql 相关所有进程
mysql.exe                    61060 Console                    1     10,772 K
mysqld.exe                    8328 Services                   0     26,564 K
mysqld.exe                   47776 Services                   0    390,172 K
PS E:\LC\work>
PS E:\LC\work> 
PS E:\LC\work> mysqld.exe --install MySQL_TEST --defaults-file="C:\mysql\mysql-8.0.28-winx64\my.ini" # 安装 MySQL_TEST 服务
Service successfully installed.
PS E:\LC\work>
PS E:\LC\work>
PS E:\LC\work> Get-Service mysql*                   # 查看已经安装的MySQL服务
Status   Name               DisplayName
------   ----               -----------
Stopped  MySQL_TEST         MySQL_TEST
Stopped  mysql3310          mysql3310
Running  MySQL80            MySQL80
PS E:\LC\work>
PS E:\LC\work> sc.exe delete MySQL_TEST             # 删除 MySQL_TEST 服务
[SC] DeleteService 成功
PS E:\LC\work>
PS E:\LC\work>

// # 对已有的 MySQL 服务, 查看其启动命令
PS C:\> 
PS C:\> Get-WmiObject Win32_Service | Where-Object { $_.Name -like "mysql*" } | Select-Object Name, DisplayName, PathName
Name      DisplayName PathName
----      ----------- --------
mysql3310 mysql3310   C:\mysql\mysql-8.0.28-winx64\bin\mysqld mysql3310
MySQL80   MySQL80     "C:\Program Files\MySQL\MySQL Server 8.0\bin\mysqld.exe" --defaults-file="C:\ProgramData\MySQL\MySQL Server 8.0\my.ini" MySQL80
PS C:\> 
```

5. 测试

```
PS E:\LC\work> mysql.exe --version                              # 查看版本号
C:\mysql\mysql-8.0.28-winx64\bin\mysql.exe  Ver 8.0.28 for Win64 on x86_64 (MySQL Community Server - GPL)
PS E:\LC\work>
PS E:\LC\work> Get-Service mysql*                               # 查看已经安装的MySQL服务
Status   Name               DisplayName 
------   ----               -----------
Stopped  mysql3310          mysql3310
Stopped  MySQL80            MySQL80
PS E:\LC\work>
PS E:\LC\work> net start MySQL80                                # 启动 MySQL80服务
MySQL80 服务正在启动 .
MySQL80 服务已经启动成功。
PS E:\LC\work>
PS E:\LC\work> mysql.exe -h localhost -P 3306 -u root -p123456lc  # 登录 MySQL80 服务
mysql>
mysql>
mysql> SHOW DATABASES;                                          # 显示所有数据库
+--------------------+
| Database           |
+--------------------+
| information_schema |
| lc_test            |
| lc_test2           |
| mysql              |
| performance_schema |
| runoob             |
| sakila             |
| sys                |
| world              |
+--------------------+
9 rows in set (0.00 sec)
mysql>
mysql>

```

# MySQL 常用命令

## 连接与退出

```
// 连接数据库
mysql -u <用户名> -p
mysql -h <服务器ip> -P <端口> -u <用户名> -p 

// 退出数据库
exit;
quit;
```

## 数据库操作

```
// 查看所有数据库
SHOW DATABASES;

// 创建数据库
CREATE DATABASE IF NOT EXISTS mydatabase CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci;

// 使用/切换数据库
USER <数据库名>;

// 删除数据库
DROP DATABASE <数据库名>;
```

## 表操作

```
// 查看数据库中的所有表
SHOW TABLES;

// 创建表
CREATE TABLE <表名> (字段 数据类型 约束, 字段 数据类型 约束);

eg: CREATE TABLE IF NOT EXISTS users (
        id INT AUTO_INCREMENT PRIMARY KEY,              // 主键自增约束
        name VARCHAR(50) NOT NULL,                      // 非空约束
        email VARCHAR(100) UNIQUE,                      // 唯一约束，UNIQUE 列只允许一个 NULL
        age INT DEFAULT 10,                             // 默认值约束 (整型)
        created_at DATETIME DEFAULT CURRENT_TIMESTAMP   // 默认值约束 (时间戳)  
        UNIQUE (id, name, age)                          // 组合唯一约束
    );

// 对已经存在的表添加唯一约束
CREATE UNIQUE INDEX IF NOT EXISTS id_name_age ON users (id, name, age);

// 删除表
DROP TABLE <表名>;

// 查看表结构
DESCRIBE <表名>;

// 修改表结构
ALTER TABLE <表名> ADD COLUMN <新字段名> <数据类型>;
```
## 数据操作(CRUD)

```
// 插入数据
INSERT INTO <表名> (字段1, 字段2, ...) VALUES (值1, 值2, ...);

// 查询数据
SELECT <*, 字段1，字段2> FROM <表名>;

    // 条件查询
    SELECT <*, 字段1，字段2> FROM <表名> WHERE age > 10;

    // 模糊查询
    SELECT <*, 字段1，字段2> FROM <表名> WHERE name LIKE '张%;

    // 查询结果排序
    SELECT <*, 字段1，字段2> FROM <表名> ORDER BY age DESC; // ASC升序, DESC降序

    // 限制查询返回的数量
    SELECT <*, 字段1，字段2> FROM <表名> LIMIT 5;

// 更新数据
UPDATE <表名> SET <字段> = <新值> WHERE <条件>;

// 删除数据
DELETE FROM <表名> WHERE <条件>;
```

## 用户于权限管理

```
// 创建用户
CREATE USER '用户名'@'localhost' IDENTIFIED BY '密码';

// 授予权限
GRANT ALL PRIVILEGES ON 数据库名.* TO '用户名'@'localhost';

// 删除权限
REVOKE ALL PRIVILEGES ON 数据库名.* TO '用户名'@'localhost';

// 刷新权限
FLUSH PRIVILEGES;

// 查看用户权限
SHOW GRANTS FOR '用户名'@'localhost';

// 删除用户
DROP USER 'test_user'@'localhost';

```

## 实用命令


### 查看数据库/ 表单信息
```
SHOW DATABASES;                     // 显示所有数据库
USE <数据库名>;                      // 选择数据库                
SHOW TABLES;                        // 显示此数据库的所有表
DESCRIBE <表名>                     // 显示表的数据结构
SHOW CREATE TABLE <表名>;           // 查看创建的表单(可以看外键的约束名)

DROP DATABASE <数据库名>;           // 删除数据库
DROP TABLE <表名>;                  // 删除表单
```

### 查看当前信息

```
SELECT USER();                      // 查看当前登录的用户名
SELECT DATABASE();                  // 查看当前数据库名称
```

### 杂项

```
SELECT VERSION();                   // 查看 MySQL 版本呢
SHOW WARNINGS;                      // 显示警告信息(如: 插入时数据截断)
SELECT User, Host FROM mysql.user;  // 查看所有用户
```

# 其他

## 外键

外键是指表中的某一个字段是另一个表的主键(PRIMARY KEY)或者唯一键(UNIQUE)。

拥有外键的表称为子表，或者从表; 被引用的表称为父表，或者主表。

```
// 新建表添加外键
CREATE TABLE <子表名> (字段 数据类型，字段 数据类型，
  FOREIGEN KEY (外键字段) REFERENCES 父表名(主键字段)
    ON DELETE 参照动作
    ON UPDATE 参照动作
)

// 对已存在的表添加外键
ALTER TABLE employees ADD CONSTRAINT fk_dept
  FOREIGN KEY (dept_id) REFERENCES departments(dept_id)
    ON DELETE CASCADE;

// 查看外键的约束名
SHOW CREATE TABLE employees;

// 删除外键
ALTER TABLE employees DROP FOREIGN KEY <外键的约束名>; 

```

| 外键参照动作 | 说明 |
|-------------|------|
| CASCADE | 级联操作: 父表删改, 子表同步进行删改 |
| SET NULL | 设为 NULL |
| RESTRICT / NO ACTION | (默认行为) 如果子表有相关记录，禁止删除/更新父表相关记录 |
| SET DEFAULT | MySQL 不支持 |


```
mysql>
mysql> CREATE TABLE departments (
    ->     id INT AUTO_INCREMENT PRIMARY KEY,
    ->     dept_name VARCHAR(50) NOT NULL
    -> ) ENGINE=InnoDB;                                                     // 创建父表
Query OK, 0 rows affected (0.05 sec)
mysql>
mysql> INSERT INTO departments (dept_name) VALUES ('设备部'), ('技术部');     // 插入2条数据
Query OK, 2 rows affected (0.01 sec)
Records: 2  Duplicates: 0  Warnings: 0
mysql>
mysql> SELECT * FROM departments;                                           // 显示表中的数据
+----+-----------+
| id | dept_name |
+----+-----------+
|  1 | 设备部    |
|  2 | 技术部    |
+----+-----------+
2 rows in set (0.00 sec)
mysql>
mysql> CREATE TABLE employees (                                             // 创建子表
    ->     id INT AUTO_INCREMENT PRIMARY KEY,
    ->     name VARCHAR(50) NOT NULL,
    ->     dep_id INTEGER,
    ->       FOREIGN KEY (dep_id) REFERENCES departments (id)
    ->         ON DELETE SET NULL
    ->         ON UPDATE CASCADE
    -> ) ENGINE=InnoDB;
Query OK, 0 rows affected (0.06 sec)
mysql>
mysql> INSERT INTO employees (name, dep_id) VALUES ('张三', 3);             // 插入子表失败，因为父表中没有id 3
ERROR 1452 (23000): Cannot add or update a child row: a foreign key constraint fails (`lc_test`.`employees`, CONSTRAINT `employees_ibfk_1` FOREIGN KEY (`dep_id`) REFERENCES `departments` (`id`) ON DELETE SET NULL ON UPDATE CASCADE)
mysql>
mysql> INSERT INTO employees (name, dep_id) VALUES ('张三', 2), ('李四', 1);
mysql>
mysql> SHOW CREATE TABLE employees;                                     // 查看外键的约束名

| Table     | Create Table  |
| employees | CREATE TABLE `employees` (
  `id` int NOT NULL AUTO_INCREMENT,
  `name` varchar(50) NOT NULL,
  `dep_id` int DEFAULT NULL,
  PRIMARY KEY (`id`),
  KEY `dep_id` (`dep_id`),
  CONSTRAINT `employees_ibfk_1` FOREIGN KEY (`dep_id`) REFERENCES `departments` (`id`) ON DELETE SET NULL ON UPDATE CASCADE
) ENGINE=InnoDB AUTO_INCREMENT=4 DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci |
1 row in set (0.00 sec)

mysql>
mysql> ALTER TABLE employees DROP FOREIGN KEY employees_ibfk_1;       // 根据约束名 employees_ibfk_1 删除外键 
Query OK, 0 rows affected (0.03 sec)
Records: 0  Duplicates: 0  Warnings: 0
mysql>

```


