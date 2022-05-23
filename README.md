## 简介
mugix 是由 C++ 编写的web服务器
## 编译运行
```
// 编译
mkdir /src/build
cd /src/build
cmake ..
make
```
配置文件`mugix.cfg`
```
ip=0.0.0.0
port=10001
# html文件根目录
root=/home/website/blog/_site/
```

## 架构
--| src  
-----| app 应用  
-----| base 管理socket连接
-----| http 解析http请求，分发给对应app，最后响应http请求
-----| utils 工具
--| test 单元测试  
