# LuaBinaryTable
Compress a Lua table into binary data file.

将Lua的table压缩成二进制数据文件

# 测试结果对比
Release模式下，解析bin/d_skill.lua文件1000次的对比结果

模式      |   文件大小(byte)   |   解析耗时(ms)
----------|------------------|-----------
LuaBinaryTable | 380341      |  3427
Lua       |     402701       |  3521

# 编译&运行测试程序
安装cmake

```shell
mkdir build
cd build
cmake -DCMAKE_INSTALL_PREFIX=".." -DCMAKE_BUILD_TYPE=Release ..
make
make install

cd ../bin
./run-test.sh
```
