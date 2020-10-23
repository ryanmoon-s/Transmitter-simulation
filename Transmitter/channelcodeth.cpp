#include "channelcodeth.h"

ChannelCodeTh::ChannelCodeTh()
{

}

void ChannelCodeTh::set_attribute(QLineSeries *series, QChart *chart, QValueAxis *axisX, QValueAxis *axisY, QString source_code, int way)
{
    /*
     * 传入操作的对象，顺便添加初始坐标原点
     */

    this->series = series;
    this->chart = chart;
    this->axisX = axisX;
    this->axisY = axisY;
    this->source_code = source_code;
    this->way = way;

    /*  添加初始坐标原点  */
    points = series->pointsVector();
    points.append(QPointF(0, 0));
    series->replace(points);
}

void ChannelCodeTh::CMI_zero_jump()
{
    qreal range = 0;
    qreal h = 1 + range, n = 0 + range;

    UpX += 0.5;
    add_point(UpX, n);
    add_point(UpX, h);
    UpX += 0.5;
    add_point(UpX, h);
}

void ChannelCodeTh::CMI_hi_jump(qreal range, qreal y)
{
    /*  分两次加点，突出最小时钟周期，方便电平识别  */

    UpX += 0.5;
    add_point(UpX, y + range);
    UpX += 0.5;
    add_point(UpX, y + range);
}

void ChannelCodeTh::top_jump(qreal range)
{
    qreal y_h = 1 + range, y_n = 0 + range;

    add_point(UpX, y_h);
    UpX += 0.5;
    add_point(UpX, y_h);
    add_point(UpX, y_n);
    UpX += 0.5;
    add_point(UpX, y_n);
}

void ChannelCodeTh::normal_jump(qreal range)
{
    qreal y_n = 0 + range;

    UpX += 0.5;
    add_point(UpX, y_n);
    UpX += 0.5;
    add_point(UpX, y_n);
}

void ChannelCodeTh::buttom_jump(qreal range)
{
    qreal y_n = 0 + range, y_l = -1 + range;

    add_point(UpX, y_l);
    UpX += 0.5;
    add_point(UpX, y_l);
    add_point(UpX, y_n);
    UpX += 0.5;
    add_point(UpX, y_n);
}

void ChannelCodeTh::HDB3_v_jump(qreal range)
{
    if(HDB3_ch_flag){  //应该上翻，上一个是下翻，此处和前一个相同，所以下翻
        buttom_jump(range);
        HDB3_ch_flag = true;  //下一个应该上翻，继续交替
    }else{  //下翻
        top_jump(range);
        HDB3_ch_flag = false;   //下一个应该下翻，继续交替
    }
}

void ChannelCodeTh::HDB3_normal_jump(qreal range)
{
    if(HDB3_ch_flag){  //上翻
        top_jump(range);
        HDB3_ch_flag = false;
    }else{  //下翻
        buttom_jump(range);
        HDB3_ch_flag = true;
    }
}

void ChannelCodeTh::run()
{
    /*
     * 解析经过信源编码的01二进制流，通过Bool型模拟电平，传给编码器进行编码模拟
     * 要求能控制时速
     */

    QByteArray bstr = source_code.toLocal8Bit();
    const char *str = bstr;

    /*  编码控制  */
    for(*str; *str != '\0'; str++){
        if(*str == '1'){
            /*  发信号通知主界面更新code码元  */
            emit update_line_code(1);

            /*  发给编码器进行信道编码  */
            code_machine(true, false);
        }else{
            emit update_line_code(0);
            if(*(str + 1) == '\0'){
                /*  这是最后一个0，刷新出积累的0  */
                code_machine(false, true);
            }else{
                code_machine(false, false);
            }
        }
        /*  时速控制，十档延时  */
        msleep(20*(11-index));
    }

    /*  编码结束  */
    emit ch_code_finished();
    quit();
}


void ChannelCodeTh::code_machine(bool now, bool flush)
{
    /*
     * 输入一个电平，由此函数执行编码，产生高低电平，控制chart绘制模拟
     * last为上一次的电平，有用到的地方，都有是否是第一个码的判断
     */

    /*
     * 单极性非归零码
     * 双极性非归零码
     * 单极性归零码
     * AMI
     * HDB3
     * CMI
     */

    qreal range = 0;
    qreal h = 1 + range, n = 0 + range, l = -1 + range;
    QByteArray bstr = source_code.toLocal8Bit();
    const char *str = bstr;

    switch (way) {
    case 0:
        //时编码 单极性非归零码
        if(SNRZ_first){
            SNRZ_first = false;
            if(now){
                //高，初始加个上升沿
                add_point(UpX, h);
                UpX += 1;
                add_point(UpX, h);
            }else{
                //低，正常走
                UpX += 1;
                add_point(UpX, n);
            }
        }else{
            if(now){
                if(last){
                    //此高前高
                    UpX += 1;
                    add_point(UpX, h);
                }else{
                    //此高前低，加一个上升沿
                    add_point(UpX, h);
                    UpX += 1;
                    add_point(UpX, h);
                }
            }else{
                if(last){
                    //此低前高，加一个下降沿
                    add_point(UpX, n);
                    UpX += 1;
                    add_point(UpX, n);
                }else{
                    //此低前低
                    UpX += 1;
                    add_point(UpX, n);
                }
            }
        }
        break;

    case 1:
        //即时编码 双极性非归零码
        if(DNRZ_first){
            DNRZ_first = false;
            add_point(UpX, l);
            if(now){
                //高，加个上升沿
                add_point(UpX, h);
                UpX += 1;
                add_point(UpX, h);
            }else{
                //低，正常走
                UpX += 1;
                add_point(UpX, l);
            }
        }else{
            if(now){
                if(last){
                    //此高前高
                    UpX += 1;
                    add_point(UpX, h);
                }else{
                    //此高前低，加一个上升沿
                    add_point(UpX, h);
                    UpX += 1;
                    add_point(UpX, h);
                }
            }else{
                if(last){
                    //此低前高，加一个下降沿
                    add_point(UpX, l);
                    UpX += 1;
                    add_point(UpX, l);
                }else{
                    //此低前低
                    UpX += 1;
                    add_point(UpX, l);
                }
            }
        }
        break;

    case 2:
        //即时编码 单极性归零码
        if(now){
            //高电平
            top_jump(range);
        }else{
            //低电平
            normal_jump(range);
        }
        break;

    case 3:
        //即时编码 AMI(双极性归零码)
        if(now){
            //高电平
            if(AMI_ch_flag){  //上翻
                top_jump(range);
                AMI_ch_flag = false;
            }else{  //下翻
                buttom_jump(range);
                AMI_ch_flag = true;
            }
        }else{
            //低电平
            normal_jump(range);
        }
        break;

    case 4:
        /*
         * 半即时编码 HDB3
         * last_v_pos.level只有v_jump里面设置了，其它地方需要手动设置
         */
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
        break;

    case 5:
        //即时编码 CMI
        if(CMI_first){
            CMI_first = false;
            if(now){
                //加个上升沿
                add_point(UpX, h);
                /*  设第一个1为高，1是连续两个时钟周期为相同电平，要分两次加点，突出最小时钟周期，方便电平识别  */
                CMI_hi_jump(range, h);
                CMI_ch_flag = false;
            }else{
                CMI_zero_jump();
            }
        }else{
            if(now){
                //此1，分高平两种情况
                if(CMI_ch_flag){
                    //高
                    if(last){
                        /*  此高，前1，加上升沿，要分两次加点，突出最小时钟周期，方便电平识别  */
                        add_point(UpX, h);
                        CMI_hi_jump(range, h);
                        CMI_ch_flag = false;
                    }else{
                        //此高，前0，直接过
                        CMI_hi_jump(range, h);
                        CMI_ch_flag = false;
                    }
                }else{
                    //低
                    if(last){
                        //此低，前1，加下降沿
                        add_point(UpX, n);
                        CMI_hi_jump(range, n);
                        CMI_ch_flag = true;
                    }else{
                        //此低，前0，加下降沿
                        add_point(UpX, n);
                        CMI_hi_jump(range, n);
                        CMI_ch_flag = true;
                    }
                }
            }else{
                //此0，分前1前0两种情况
                if(last){
                    //前1，此0，分两种情况
                    if(CMI_ch_flag){
                        //上一个为平1，直接跳
                        CMI_zero_jump();
                    }else{
                        //上一个为高1，加下降沿
                        add_point(UpX, n);
                        CMI_zero_jump();
                    }
                }else{
                    //前0，此0，加下降沿
                    add_point(UpX, n);
                    CMI_zero_jump();
                }
            }
        }
        break;
    }

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

void ChannelCodeTh::add_point(qreal UpX, qreal Y)
{
    /*
     * 包含添加点、通过坐标识别电平高低
     */

    /*
     * 识别电平
     * 判断上一个Y坐标与这一个Y坐标关系，以裁决是产生了一个高电平还是产生了一个低电平
     */
    if(last_Y == Y){
        if(Y == 0)
            emit push_rec_ch('n');
        else if(Y == 1)
            emit push_rec_ch('h');
        else if(Y == -1)
            emit push_rec_ch('l');
    }

    /*  添加点  */
    points = series->pointsVector();
    points.append(QPointF(UpX, Y));
    series->replace(points);

    /*  记录上一次的y坐标，供识别电平用  */
    last_Y = Y;
}

void ChannelCodeTh::adjest_speed(int index)  //调速
{
    this->index = index;
}

