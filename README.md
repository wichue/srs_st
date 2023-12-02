C++音视频流媒体服务器SRS源码框架解读，协程库state-threads的使用.

SRS协程库ST的使用
C语言协程库state-threads（简称ST库）：https://sourceforge.net/projects/state-threads/。
SRS对state-threads库进行了2次开发 ：https://github.com/ossrs/state-threads。

1、ST库的编译
在下载的srs-5.0release.zip安装包里有ST源码，直接编译，在什么平台用就在什么平台编译，centos编译的库拿到ubuntu里用是不行的。

cd /srs-5.0release/trunk/3rdparty/st-srs
make linux-debug	#编译

在构建目录生成库文件libst.a，头文件st.h。

2、ST库的使用
SRS封装了协程类SrsSTCoroutine，通过C++类的继承和虚函数回调，实现了在回调函数执行协程处理函数（和linux线程库函数pthread_create用法类似）。

3、SrsContextId上下文ID
SRS上下文ID，由数字和英文字母串组成的随机8位字符串，用于唯一标记一个协程，SRS每个服务端、客户端都在一个单独协程工作，因此有了上下文ID就能找到对应的客户端服务端，日志也会打印出当前协程的上下文ID。
SRS上下文ID是写入ST协程库里的，当协程切换时可以自动获取当前协程上下文ID，使用很方便。

4、SrsAutoFree
这部分代码还包含了SrsAutoFree定义，可以在离开作用域时自动释放指针，也是很有用的一个模块。

.
