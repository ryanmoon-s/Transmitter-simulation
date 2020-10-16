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
    add_point(0, 0);
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
    if(HDB3_ch_flag){  //应该上翻，上一个是下翻，所以此处下翻
        buttom_jump(range);
        last_point.level = 0;
        HDB3_ch_flag = true;  //下一个应该上翻，继续交替
    }else{  //下翻
        top_jump(range);
        last_point.level = 1;
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
            code_machine(true);
        }else{
            emit update_line_code(0);
            code_machine(false);
        }
        /*  时速控制，五档延时30ms，1档延时150ms  */
        msleep(30*(6-index));
    }

    /*  编码结束  */
    emit ch_code_finished();
    quit();
}


void ChannelCodeTh::code_machine(bool now)
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
    count++;

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

    case 3:
        //即时编码 AMI
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
        //半即时编码 HDB3
        if(now){//高电平，直接译码
            //先打出积累了几个0的平电平
            for(int i = 0; i < HDB3_zero_num; i++){
                normal_jump(range);
            }
            //再打V
            HDB3_normal_jump(range);
            HDB3_zero_num = 0;
        }else{
            //低电平，积累0，满4个，译码
            HDB3_zero_num ++;
            if(HDB3_zero_num != 4)
                break;
            HDB3_zero_num = 0;
            //第一个v
            if(HDB3_is_first_v){
                last_point.index = count;
                //展示前3个0，固定为平，没有B
                for (int i = 0; i < 3; i++) {
                    normal_jump(range);
                }
                //检索前面是否有1，若有则和前面1的电平相同
                for(int i = count; i >=0; i--){
                    if(source_code.at(i) == '1'){
                        HDB3_v_jump(range);
                        HDB3_is_first_v = false;
                        break;
                    }
                }
                //若没有1，则它为上翻
                top_jump(range);
                HDB3_ch_flag = false;  //下一个应该下翻，继续交替
                HDB3_is_first_v = false;
                break;
            }
            //不是第一个v
            //查看与前一个v之间（不包含上一个v）有奇数还是偶数个1
            for (int i = count; i > last_point.index; i--) {
                if(source_code.at(i) == '1')
                    HDB3_one_num++;
            }
            last_point.index =count;
            //奇偶分开译
            if(HDB3_one_num % 2 == 0){
                //偶数个1
                //第一个零变为与前面一个1相反
                HDB3_normal_jump(range);
                //中间两个0
                for (int i = 0; i < 2; i++) {
                    normal_jump(range);
                }
                //最后一个零变为与前面一个零相同
                HDB3_v_jump(range);
            }else{
                //奇数个1
                //展示前3个0，固定为平，没有B
                for (int i = 0; i < 3; i++) {
                    normal_jump(range);
                }
                //最后一个v变为与前面一个1电平相同
                HDB3_v_jump(range);
            }
        }
        break;

    case 5:
        //即时编码 CMI
        if(CMI_first){
            CMI_first = false;
            if(now){
                //加个上升沿
                add_point(UpX, h);
                //第一个1高
                UpX += 1;
                add_point(UpX, h);
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
                        //此高，前1，加上升沿
                        add_point(UpX, h);
                        UpX += 1;
                        add_point(UpX, h);
                        CMI_ch_flag = false;
                    }else{
                        //此高，前0，直接过
                        UpX += 1;
                        add_point(UpX, h);
                        CMI_ch_flag = false;
                    }
                }else{
                    //低
                    if(last){
                        //此低，前1，加下降沿
                        add_point(UpX, n);
                        UpX += 1;
                        add_point(UpX, n);
                        CMI_ch_flag = true;
                    }else{
                        //此低，前0，加下降沿
                        add_point(UpX, n);
                        UpX += 1;
                        add_point(UpX, n);
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
                    //前0，此0，必加下降沿
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
         * 只受视觉影响，不影响效果
         */
        UpX = -1;
        QVector<QPointF> point;
        series->replace(point);

        /*  添加初始坐标原点  */
        add_point(0, 0);
    }
}

void ChannelCodeTh::add_point(qreal UpX, qreal Y)   //添加点
{
    points = series->pointsVector();
    points.append(QPointF(UpX, Y));
    series->replace(points);
}

void ChannelCodeTh::adjest_speed(int index)  //调速
{
    this->index = index;
}

