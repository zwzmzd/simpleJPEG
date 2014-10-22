simpleJPEG
==========

一个简单的bmp转jpg工具，使用C语言完成，可以辅助学习JPEG转换的原理。

** 程序暂时只支持24位BMP图片读取，请使用24位BMP图片作为输入 **

### 使用cmake生成编译工程(推荐)

在 Mac OSX Yosemite / Ubuntu 12.04 LTS / Visual Studio 2013 / Visual Studio 2008 下测试通过
```bash
$ cd simpleJPEG
$ mkdir build
$ cd build
$ cmake ..
```

### 直接在目录下使用make编译

在Ubuntu 12.04下测试通过

该方法在Windows下也能正确地编译运行，不过需要配置gcc和mingw环境。建议安装TDM-GCC套件，另外需要寻找一个可用的make程序。比较繁琐，推荐使用上面的cmake方法

```bash
$ cd simpleJPEG
$ make
$ ./main.exe testcase/in.bmp testcase/out.jpg
```

推荐一个解析JPEG格式的工具
[JPEGsnoop](http://www.impulseadventure.com/photo/jpeg-snoop.html)
