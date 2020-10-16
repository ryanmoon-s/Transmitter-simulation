#ifndef UTILS_H
#define UTILS_H
#define CODENUM 128    //码元数量
#define DEBUG

#include <QString>
#include <QDebug>
struct s_dat{    //香农编码数据结构
    int value = -1;   //ASCII码对应的整数
    int count;
    double probability;    //概率
    double s_probability;   //累计概率
    double log_value;
    int Ki;
    char code[16];
};


class Utils
{
public:
    Utils();
    static void statistical(QString text, int *count_arr);  //统计字符出现次数
    static void source_shannon_code(int *count_arr, s_dat *s_code);  //香农编码
    static void MergeSort(s_dat *arr, s_dat *temp,  int start, int end);   //归并排序

};

#endif // UTILS_H
