#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "chw_adapt.h"
#include "srs_protocol_log.hpp"
#include "srs_app_st.hpp"
#include <st.h>
ISrsContext* _srs_context = NULL;

//0.在ST_TEST对象里启动一个协程
class ST_TEST : public ISrsCoroutineHandler{
public:
    ST_TEST(std::string flag) :_flag(flag)
    {
        _srs_context->set_id(_srs_context->generate_id());
        trd = new SrsSTCoroutine(flag, this, _srs_context->get_id());//2.new一个ST对象
    }
    srs_error_t startST(){
        srs_error_t err = srs_success;
        if ((err = trd->start()) != srs_success) {//3.start()创建协程
            return srs_error_wrap(err, "start timer");
        }
        printf("startST:%s\n",_flag.c_str());
        return err;
    }
public: virtual srs_error_t cycle() {//4.协程处理函数，回调cycle()
        srs_error_t err = srs_success;
        printf("cycle:%s\n",_flag.c_str());
        while(1)
        {
            srs_usleep(1000*1000);//协程睡眠，1秒
            //打印协程上下文ID
            printf("flag=%s，cid=%s\n",_flag.c_str(), _srs_context->get_id().c_str());
        }
        return err;
    }
private:
    SrsCoroutine* trd;
    std::string _flag;
};

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setbuf(stdout,NULL);

    _srs_context = new SrsThreadContext();
    srs_st_init();//1.初始化ST

    _srs_context->set_id(_srs_context->generate_id());
    printf("\nmain cid=%s\n", _srs_context->get_id().c_str());//打印主协程上下文ID
    ST_TEST *pST_TEST1 = new ST_TEST("hello");
    pST_TEST1->startST();
    srs_usleep(1000);
    ST_TEST *pST_TEST2 = new ST_TEST("world");
    pST_TEST2->startST();

    st_thread_exit(NULL);
    while(1){}
}

MainWindow::~MainWindow()
{
    delete ui;
}


