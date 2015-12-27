# LuaBinaryTable
Compress a Lua table into binary data file.

# NOTICE
The efficiency was not obvious, if you have any good idea, please contact me.
此工具可将Lua Table压缩成二进制格式，压缩后的文件大小要比`luac -s`预编译之后的大小要小很多。但是经过zip再次压缩之后，大小相差甚小。
原以为解析速度会比lua解释器快，但是实测之后发现反而要慢一点(o(╯□╰)o)。
所以，此工程实则无用。但是代码已经写完了，感兴趣的朋友可以提供一些优化方案。
