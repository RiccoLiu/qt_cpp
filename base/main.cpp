#include <QCoreApplication>
#include <QDebug>
#include <iostream>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    /* QString 基本用法 */

    // 1.创建 QString
    // 1.1.使用字符串创建
    QString str1 = "hello";
    QString str2("world");

    // 1.2.格式化创建
    QString str_format = QString("  %1   +   %2   +   %3  ").arg(str1,str2,"format   ");

    qDebug() << "str1:" << str1;
    qDebug() << "str2:" << str2;
    qDebug() << "str_format:" << str_format;

    // 2.QString 转换
    // 2.1.QString 与 数字转换
    int i = 123;
    float f = 3.14;

    QString str_i = QString::number(i);
    QString str_f = QString::number(f);

    int num_i = str_i.toInt(); // toInt(&ret): ret判断转换是否成功
    float num_f = str_f.toFloat();

    qDebug() << "i = " << i << ", str_i = " << str_i << ", num_i = " << num_i;
    qDebug() << "f = " << f << ", str_f = " << str_f << ", num_f = " << num_f;

    // 2.2.QString 转 std::string
    std::string std_str_format = str_format.toStdString();
    QString str_format2 = QString::fromStdString(std_str_format);

    std::cout << "std_str_format:" << std_str_format << std::endl;
    qDebug() << "str_format2:" << str_format2;

    // 2.3.QString 转 char*
    QByteArray str1_utf8 = str1.toUtf8();
    const char* str1_data = str1_utf8.constData();

    // 未定义的行为：toUtf8()返回一个临时的对象，返回即被销毁，str1_data变成一个野指针
    // char* str1_data = str1.toUtf8().data();

    QString str_from_char = QString::fromUtf8("hello");

    std::cout << "str1_data:" << str1_data << std::endl;
    qDebug() << "str_from_char:" << str_from_char;

    // 3.QString处理
    // 3.1.去除空格
    QString str_format_tri = str_format.trimmed(); // 去掉开始和结尾的空格
    QString str_format_sim  = str_format.simplified();

    qDebug() << "str_format:" << str_format;
    qDebug() << "str_format_tri:" << str_format_tri;
    qDebug() << "str_format_sim:" << str_format_sim;

    // 3.2.大小写转换
    QString str_format_upper = str_format.toUpper();
    QString str_format_lower = str_format.toLower();

    qDebug() << "str_format:" << str_format;
    qDebug() << "str_format_upper:" << str_format_upper;
    qDebug() << "str_format_lower:" << str_format_lower;

    // 3.3.字符删除
    str_format_upper.remove("  "); // 删除字符串中所有两个空格
    qDebug() << "str_format_upper.remove(\"  \"): " << str_format_upper;

    str_format_upper.remove(2, 3);  // 删除字符串下标2的位置，长度是3个字符
    qDebug() << "str_format_upper.remove(2, 3)" << str_format_upper;

    // 3.4.字符串分割
    QStringList str_list = str_format_upper.split("+");
    for (auto& str : str_list) {
        qDebug() << str ;
    }

    // 4.查找字符串
    // 4.1.是否为空
    QString str_null;
    QString str_empty = "";

    qDebug() << "str_null.isNull():" << str_null.isNull() << ", str_null.isEmpty()" << str_null.isEmpty();
    qDebug() << "str_empty.isNull():" << str_empty.isNull() << ", str_empty.isEmpty()" << str_null.isEmpty();

    // 4.2.是否包含字符串
    qDebug() << "contains(\"format\"):" << str_format.contains("format");
    qDebug() << "contains(\"Format\"):" << str_format.contains("Format", Qt::CaseInsensitive);

    // 4.3.查找字符串的位置
    // indexOf(): 从前往后找指定字符串
    // lastIndexOf(): 从后往前找指定字符串
    qDebug() << "indexOf(\"o\"):" << str_format.indexOf("o");
    qDebug() << "lastIndexOf(\"o\"):" << str_format.lastIndexOf("o");

    // 4.4.字符串比较
    if (str1 == "HELLO") { // 直接比较
        qDebug() << "str1 == \"HELLO\"";
    }
    else {
        qDebug() << "str1 != \"HELLO\"";
        if (str1.compare("HELLO", Qt::CaseInsensitive) == 0) { // 忽略大小写比较
            qDebug() << "str1.compare(\"HELLO\", Qt::CaseInsensitive) == 0";
        }
        else {
            qDebug() << "str1.compare(\"HELLO\", Qt::CaseInsensitive) != 0";
        }
    }

    // 4.5.访问单个字符
    str1[0] = 'H';
    qDebug() << "str1:" << str1;

    // 5.字符串拼接
    // 5.1.连接符"+"进行拼接
    QString str3 = str1 + " " + str2 + " QT";

    // 5.2.append进行拼接
    QString str4;
    str4.append(str1).append(" ").append(str2).append(" QT");

    // 5.3.连接符"+="进行拼接
    QString str5;
    str5 += str1;
    str5 += " ";
    str5 += str2;
    str5 += " QT";

    // 5.4.大量字符串拼接使用：QStringBuilder 与 "+" 连接符相比性能更好
    QString str6;
    str6 = str1 % " " % str2 % " QT";

    qDebug() << "str3:" << str3;
    qDebug() << "str4:" << str4;
    qDebug() << "str5:" << str5;
    qDebug() << "str6:" << str6;

    // 5.字符串截取
    // 5.1.指定位置截取
    QString head5 = str6.left(5);   //
    QString mid5 = str6.mid(6, 5);
    QString tail2 = str6.right(2);

    qDebug() << "head5:" << head5;
    qDebug() << "mid5:" << mid5;
    qDebug() << "tail2:" << tail2;

    // 5.2.替换
    str6.replace("Hello", "HELLO");
    str6.replace("world", "WORLD");
    str6.replace(" ", "+");
    qDebug() << "str6:" << str6;

    return a.exec();
}
