#include "utils.h"


Utils::Utils()
{

}

void Utils::statistical(QString text, int *count_arr)  //统计字符
{
    /* 26 26 10 5 = 67
     * A-Z   a-z    0-9    ,   .   space  '   "
     * 65-90 97-122 48-57  44  46    32   39  34
     * 数组123个，下标0-122，在自己的ASCII码处+1表示这个值的出现次数+1；
     * 数个数时直接遍历这个数组，数组大小固定为宏CODENUM
     *
     * 传入传出参数count_arr数组
     * 统计字符出现次数
     */

    QByteArray bstr = text.toLocal8Bit();
    const char *str = bstr;

    /*    init arr    */
    for(int i = 0; i < CODENUM; i++){
        count_arr[i] = 0;
    }

    /*    save value    */
    for( ; *str != '\0'; str++){
        count_arr[int(*str)]++;
    }
}

void Utils::source_shannon_code(int *count_arr, s_dat *s_code)
{
    /*
     * 指针接收数组，作传入传出参数
     *
     * 香农编码
     */

    int total_count = 0;
    double sum_pro = 0;     //累计概率
    double left;    //乘二余数

    /*    赋初值，统计总个数    */
    for(int i = 0; i < CODENUM; i++){
        if(count_arr[i] == 0){    //跳过，value就为默认的-1
            s_code[i].code.value = -1;
            continue;
        }
        s_code[i].count = count_arr[i];
        s_code[i].code.value = i;
        total_count += count_arr[i];
    }
    /*    计算概率    */
    for(int i = 0; i < CODENUM; i++){
        if(s_code[i].code.value == -1)
            continue;
        //保留五位小数，四舍五入
        s_code[i].probability = (double)int((double(s_code[i].count)/total_count + 0.000005) * 100000)/100000;
    }

    /*    排序，降序    */
    s_dat temp[CODENUM];
    s_MergeSort(s_code, temp, 0, CODENUM -1);

    /*    计算数据，编码    */
    for(int i = 0; i < CODENUM; i++){
        if(s_code[i].code.value == -1)
            continue;
        if(i == 0){
            s_code[i].s_probability = 0;
        }else{
            s_code[i].s_probability = sum_pro;
        }
        sum_pro += s_code[i].probability;
        //log值和Ki值
        s_code[i].log_value = -log2(s_code[i].probability);
        s_code[i].Ki = int(s_code[i].log_value) + 1;   //取整舍去小数，再加1
        //编码(对累加概率取二进制，前Ki位)
        left = s_code[i].s_probability;
        for(int j = 0; j < s_code[i].Ki; j++){
            s_code[i].code.codes.append(int(left * 2) +  48);  //转换成对应的char
            //update left
            if(qAbs(left*2 - 1) < 0.000001){ //乘尽了
                /*  补充剩下的0  */
                for(int fill = j + 1; fill < s_code[i].Ki - j; fill++){
                    s_code[i].code.codes.append("0");
                }
                break;
            }
            if(int(left * 2) == 1)  //取1
                left = left * 2 - 1;
            else //取0
                left = left * 2;
        }
    }
}

void Utils::source_feno_code(int *count_arr, f_dat *f_code)
{
    /*
     * 指针接收数组，作传入传出参数
     *
     * 费诺编码
     */

    int total_count = 0;

    /*    赋初值，统计总个数    */
    for(int i = 0; i < CODENUM; i++){
        if(count_arr[i] == 0){    //跳过，value就为默认的-1
            f_code[i].code.value = -1;
            continue;
        }
        f_code[i].count = count_arr[i];
        f_code[i].code.value = i;
        total_count += count_arr[i];
    }

    /*    计算概率    */
    for(int i = 0; i < CODENUM; i++){
        if(f_code[i].code.value == -1)
            continue;
        //保留五位小数，四舍五入
        f_code[i].probability = (double)int((double(f_code[i].count)/total_count + 0.000005) * 100000)/100000;
    }

    /*    排序，降序    */
    f_dat temp[CODENUM];
    f_MergeSort(f_code, temp, 0, CODENUM -1);

    /*  编码  */
    int start = 0;
    int end = -1;
    for(int i = 0; i < CODENUM; i++){
        if(f_code[i].code.value != -1)
        end++;
    }

    Utils::feno_rev(f_code, start, end);
}

void Utils::feno_rev(f_dat *f_code, int start, int end)
{
    /*
     * 费诺递归生成编码
     */

    /*  终止条件  */
    if(start >= end){
        return;
    }

    int start1 = -1, end1 = -1, start2 = -1, end2 = -1;
    double rate_count = 0;
    double rate_total = 0;
    double rate_half = 0;

    /*  算总概率的一半  */
    for(int i = start; i < (end - start + 1); i++){
        rate_total += f_code[i].probability;
    }
    rate_half = rate_total/2;

    /*  赋值界值  */
    for(int i = start; i < end; i++){
        rate_count += f_code[i].probability;
        /*  当本次累计合到一半的距离，比下次累计合到一半的距离小：找到  */
        if(qAbs(rate_count - rate_half) < qAbs(rate_count + f_code[i + 1].probability - rate_half)){
            /*  分成两半  */
            start1 = start;
            end1 = i;
            start2 = i + 1;
            end2 = end;
            break;
        }
    }

    /*  增加编码  */
    for(int j = start1; j <= end1; j++){
        f_code[j].code.codes.append("0");
    }
    for(int j = start2; j <= end2; j++){
        f_code[j].code.codes.append("1");
    }

    /*  两边分别递归  */
    feno_rev(f_code, start1, end1);
    feno_rev(f_code, start2, end2);
}

h_dat *Utils::source_hauffman_code(int *count_arr)
{
    /*
     * 返回root节点
     *
     * 哈夫曼编码
     */

    /*  创建节点，因为要排序，将所有节点指针-存在一个数组中  */
    h_dat *h_arr[CODENUM];
    int count = 0;

    /*    赋初值，统计总个数    */
    int total_count = 0;
    for(int i = 0; i < CODENUM; i++){
        if(count_arr[i] != 0){
            /*  分配内存  */
            h_arr[count] = new h_dat();
            /*  赋值  */
            h_arr[count]->count = count_arr[i];
            h_arr[count]->code.value = i;
            total_count += count_arr[i];
            count++;
        }
    }

    /*    计算概率    */
    for(int i = 0; i < count; i++){
        //保留五位小数，四舍五入
        h_arr[i]->probability = (double)int((double(h_arr[i]->count)/total_count + 0.000005) * 100000)/100000;
    }

    /*    生成树   */
    h_dat *temp[count];
    int index = count - 1;   //最后一个元素的下标
    h_dat *root;
    while(index >= 1){
        /*  排序  */
        h_MergeSort(h_arr, temp, 0, index);
        /*  新建根节点  */
        root = new h_dat();
        root->probability = h_arr[index - 1]->probability + h_arr[index]->probability;
        /*  结子节点  */
        h_arr[index - 1]->c_code = '0';  //第二小
        root->left = h_arr[index - 1];
        h_arr[index]->c_code = '1';   //最小
        root->right = h_arr[index];
        /*  加入根，同等竞争  */
        h_arr[index - 1] = root;
        /*  更新index  */
        index--;
    }
    return root;
}


/*  降序排列  */
void Utils::s_MergeSort(s_dat *arr, s_dat *temp, int start, int end)  //归并排序
{
    //递归终止条件，只有一个元素时
    if (start >= end)
        return;
    //分成两半，len并不是具体长度，只作为分成两半的条件  右移一位：/2
    int len = end - start, mid = (len >> 1) + start;
    int start1 = start, end1 = mid, start2 = mid + 1, end2 = end;
    //左右分别递归
    s_MergeSort(arr, temp, start1, end1);
    s_MergeSort(arr, temp, start2, end2);
    //排序
    int k = start;  //装在相同的位置
    while (start1 <= end1 && start2 <= end2) {
        //两个子数组(逻辑上的)中，挨个比较，大的进入临时数组。进入的才++
        temp[k++] = arr[start1].probability > arr[start2].probability ? arr[start1++] : arr[start2++];
    }
    while (start1 <= end1) {
        //子数组1(逻辑上的)有剩下没进入的元素
        temp[k++] = arr[start1++];
    }
    while (start2 <= end2) {
        //子数组2(逻辑上的)有剩下没进入的元素
        temp[k++] = arr[start2++];
    }
    //临时数组还原到原数组，返回供上层继续使用
    for (int i = start; i <= end; i++)
    {
        arr[i] = temp[i];
    }
}

/*  降序排列  */
void Utils::f_MergeSort(f_dat *arr, f_dat *temp, int start, int end)  //归并排序
{
    //递归终止条件，只有一个元素时
    if (start >= end)
        return;
    //分成两半，len并不是具体长度，只作为分成两半的条件  右移一位：/2
    int len = end - start, mid = (len >> 1) + start;
    int start1 = start, end1 = mid, start2 = mid + 1, end2 = end;
    //左右分别递归
    f_MergeSort(arr, temp, start1, end1);
    f_MergeSort(arr, temp, start2, end2);
    //排序
    int k = start;  //装在相同的位置
    while (start1 <= end1 && start2 <= end2) {
        //两个子数组(逻辑上的)中，挨个比较，大的进入临时数组。进入的才++
        temp[k++] = arr[start1].probability > arr[start2].probability ? arr[start1++] : arr[start2++];
    }
    while (start1 <= end1) {
        //子数组1(逻辑上的)有剩下没进入的元素
        temp[k++] = arr[start1++];
    }
    while (start2 <= end2) {
        //子数组2(逻辑上的)有剩下没进入的元素
        temp[k++] = arr[start2++];
    }
    //临时数组还原到原数组，返回供上层继续使用
    for (int i = start; i <= end; i++)
    {
        arr[i] = temp[i];
    }
}

/*  降序排列，通过对指针属性的比较，对指针进行排序  */
void Utils::h_MergeSort(h_dat **arr, h_dat **temp, int start, int end)  //归并排序
{
    //递归终止条件，只有一个元素时
    if (start >= end)
        return;
    //分成两半，len并不是具体长度，只作为分成两半的条件  右移一位：/2
    int len = end - start, mid = (len >> 1) + start;
    int start1 = start, end1 = mid, start2 = mid + 1, end2 = end;
    //左右分别递归
    h_MergeSort(arr, temp, start1, end1);
    h_MergeSort(arr, temp, start2, end2);
    //排序
    int k = start;  //装在相同的位置
    while (start1 <= end1 && start2 <= end2) {
        //两个子数组(逻辑上的)中，挨个比较，大的进入临时数组。进入的才++
        temp[k++] = arr[start1]->probability > arr[start2]->probability ? arr[start1++] : arr[start2++];
    }
    while (start1 <= end1) {
        //子数组1(逻辑上的)有剩下没进入的元素
        temp[k++] = arr[start1++];
    }
    while (start2 <= end2) {
        //子数组2(逻辑上的)有剩下没进入的元素
        temp[k++] = arr[start2++];
    }
    //临时数组还原到原数组，返回供上层继续使用
    for (int i = start; i <= end; i++)
    {
        arr[i] = temp[i];
    }
}

int Utils::arr_length(char *str)
{
    /*
     * 字符串数组装了几个编码，传入首地址
     */

    int count = 0;
    for (; *str != '\0'; str++) {
        count++;
        if(*(str + 1) == '\0')
            break;
    }
    return count;
}
