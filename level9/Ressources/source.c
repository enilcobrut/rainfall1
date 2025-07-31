
#include <cstring>
#include <cstdlib>

class N {
public:
    N(int type) : type(type) {}
    virtual void operator()(N *other) {}

    void setAnnotation(const char *str)
    {
        size_t len = strlen(str);
        memcpy(annotation, str, len);
    }

private:
    int   type;
    char  annotation[0x64];
};

int main(int argc, char **argv)
{
    if (argc < 2)
        _exit(1);

    N *a = new N(5);
    N *b = new N(6);
    a->setAnnotation(argv[1]);
    (*b)(a);
    return 0;
}



