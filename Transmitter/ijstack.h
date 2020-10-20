#ifndef IJSTACK_H
#define IJSTACK_H
/*存放ij的栈。只存少量个元素，push时，满了就自动去掉最底元素。pop返回ij结构体，空时返回ij等于-1的结构体*/
#define ELEM_NUM 3
struct elem{
    int i;
    int j;
};

class ijStack
{
public:
    ijStack();
    void push(int i, int j);
    elem pop();

private:
    int top = -1;
    elem stack[ELEM_NUM];
};

#endif // IJSTACK_H
