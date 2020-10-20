#include "ijstack.h"

ijStack::ijStack()
{

}

void ijStack::push(int i, int j)
{
    if(top == ELEM_NUM - 1){
        /*  栈满，移动元素  */
        for(int t = 0; t < ELEM_NUM - 1; t++){
            /*  ELEM_NUM - 1次，上面覆盖下面  */
            stack[t] = stack[t+1];
        }
        top--;
    }

    top++;
    stack[top].i = i;
    stack[top].j = j;
}

elem ijStack::pop()
{
    elem ret;
    if(top == -1){
        ret.i = -1;
        ret.j = -1;
    }else{
        ret.i = stack[top].i;
        ret.j = stack[top].j;
    }

    top--;

    return ret;
}
