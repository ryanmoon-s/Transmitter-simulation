#ifndef UTILS_H
#define UTILS_H
#define CODENUM 128    //码元数量
#define DEBUG

#include <QString>
#include <QDebug>
struct code_t{
    int value = -1;   //ASCII码对应的整数
    char codes[16];  //编码
};

struct s_dat{    //香农编码数据结构
    code_t code;
    int count;
    double probability = -1;    //概率，要以这个排序，所以初始化为-1，不会出错
    double s_probability = 0;   //累计概率
    double log_value;
    int Ki;
};


class Utils
{
public:
    Utils();
    static void statistical(QString text, int *count_arr);  //统计字符出现次数
    static void source_shannon_code(int *count_arr, s_dat *s_code);  //香农编码
    static void MergeSort(s_dat *arr, s_dat *temp,  int start, int end);   //归并排序

    static int arr_length(char *str);  //字符串数组装了几个元素

};

#endif // UTILS_H
