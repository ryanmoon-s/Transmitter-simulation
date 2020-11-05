#include "codehdb3.h"

CodeHDB3::CodeHDB3(QLineSeries *series, QString source_code):CodeBase(series)
{
    this->source_code = source_code;
    QByteArray bstr = this->source_code.toLocal8Bit();
    str = bstr;
}

void CodeHDB3::Coding(bool now)
{
    /*
     * 半即时编码 HDB3
     * last_v_pos.level只有v_jump里面设置了，其它地方需要手动设置
     */
do{
    if(now){
        /*
         * 高电平，直接译码
         * 先打出积累了几个0的平电平
         * 要么出现1直接转、要么四个零转，最后如果是四个零以内，就得不到输出。设置flush标志位。如果传true进来，说明是最后一位，且是0
         *      应该刷新出积累的0
         */
        for(int i = 0; i < HDB3_zero_num; i++){
            normal_jump(range);
        }
        /*  再打1  */
        HDB3_normal_jump(range);
        HDB3_zero_num = 0;
    }else{
        /*  积累0  */
        HDB3_zero_num++;
        /*  是否是最后一个0，是就刷新出积累的0  */
        if(flush){
            for(int i = 0; i < HDB3_zero_num; i++){
                normal_jump(range);
            }
        }
        /*  不到四个，退出  */
        if(HDB3_zero_num != 4){
            count++;
            break;
        }

        HDB3_zero_num = 0;
        if(HDB3_is_first_v){
            /*  全场出现的第一个V  */
            HDB3_is_first_v = false;
            last_v_pos = count;
            /*  展示前3个0，固定为平，没有B  */
            for (int i = 0; i < 3; i++) {
                normal_jump(range);
            }
            /*  反向检索前面是否有1，若有则和前面1的电平相同  */
            for(int i = count - 1; i >= 0; i--){
                if(source_code.at(i) == '1'){
                    HDB3_v_jump(range);
                    count++;
                    break;
                }else if(i == 0){
                    /*  若没有1，则它为下翻(如果不是四连零，第一个1将上翻。这样区别开来，为解码作铺垫)  */
                    buttom_jump(range);
                    HDB3_ch_flag = true;  //下一个应该上翻，继续交替
                }
            }
        }else{
            /*
             * 不是第一个v
             * 查看与前一个v之间（不包含上一个v和本v）有奇数还是偶数个1
             */

            for (int i = count - 1; i > last_v_pos; i--) {
                if(*(str + i) == '1'){
                    HDB3_one_num++;
                }
            }
            last_v_pos = count;
            //奇偶分开译
            if(HDB3_one_num % 2 == 0){
                /*  偶数个1,第一个零变为与前面一个1相反  */
                HDB3_normal_jump(range);
                /* 中间两个0  */
                for (int i = 0; i < 2; i++) {
                    normal_jump(range);
                }
                /* 最后一个零变为与前面一个零相同  */
                HDB3_v_jump(range);
            }else{
                /*  奇数个1,展示前3个0，固定为平，没有B  */
                for (int i = 0; i < 3; i++) {
                    normal_jump(range);
                }
                /*  最后一个v变为与前面一个1电平相同  */
                HDB3_v_jump(range);
            }
            /*  两个V中间1数量清零  */
            HDB3_one_num = 0;
        }
    }
    count++;

}while(0);    //用于跳出上个代码块，直接执行这一步

    last = now; //上一个电平赋值为当前电平
    /*  超出x边界  */
    if(UpX >= 23){
        /*  清空图表  */
        /* 牺牲一格，避免换到初始坐标后没有前导，出现空格的情况
         * 到了0坐标后，下一个从1开始，那么0-1之间没有连线
         * 只受视觉影响，不影响上面实际点的添加
         */
        UpX = -1;
        QVector<QPointF> point;
        point.clear();
        series->replace(point);
    }
}

void CodeHDB3::HDB3_v_jump(qreal range)
{
    if(HDB3_ch_flag){  //应该上翻，上一个是下翻，此处和前一个相同，所以下翻
        buttom_jump(range);
        HDB3_ch_flag = true;  //下一个应该上翻，继续交替
    }else{  //下翻
        top_jump(range);
        HDB3_ch_flag = false;   //下一个应该下翻，继续交替
    }
}

void CodeHDB3::HDB3_normal_jump(qreal range)
{
    if(HDB3_ch_flag){  //上翻
        top_jump(range);
        HDB3_ch_flag = false;
    }else{  //下翻
        buttom_jump(range);
        HDB3_ch_flag = true;
    }
}
