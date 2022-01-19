## 简介
mugix 是由 C++ 编写的web服务器
## 编译运行
```
// 编译
mkdir build
cd build
cmake ..
make install

// 运行，默认监听50001
// -p   端口
// -d   网站根目录
// -m   epoll触发模式(listenfd+connfd)
// LT+LT LT+ET ET+LT ET+ET
// 0     1     2     3
// 
// -l   日志级别(OFF,FATAL,ERROR,WARN,INFO,DEBUG,ALL)

./build/mugix -p 50001 -d /home/neko/mugix/html/ -m 0 -l 4
```
## 架构
--| server http服务器   
-----| app 应用  

--| test 单元测试  

## 目标
实现高性能Web服务器，要求支持应用可拓展
## 要求
- [ ] 使用epoll(LT/ET)管理监听、连接套接字
- [ ] 使用连接池处理任务
- [ ] 使用状态机解析请求头