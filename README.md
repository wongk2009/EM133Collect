# EM133Collect
20190902:
1.完成树莓派modbus读写程序；

20190908:
1.完成EM133 modbus tcp数据读取程序，并将数据写入csv文件；
2.读取数据周期大约65ms，每读取10000条数据保存到一个csv文件中，csv文件保存在Data/文件夹中；
3.初步完成文件上传代码；

20190909:
1.新增获取已保存csv文件功能；

20190912:
1.修正TCP上传功能获取当前上前文件名函数；
2.新增多线程功能；
3.修正data数据日志文档windows文本格式写入功能；

20190915:
1.修正上传文件名错误bug；
2.更新远程服务器ip；

20190917:
1.完成modbus socket功能；
2.添加socket读写保存Log文件功能；
3.修正其他代码格式和功能；

20190923:
1.优化程序采集时间，有功功率、视在功率、和无功功率通过计算算出来；

20190927:
1.修改采集程序为读两次报文，有功功率、视在功率和无功功率通过电表数据读取得到；
2.修正部分bug；

20190930:
1.修正部分bug；
2.写入csv文件的数据加入各相的有功功率、无功功率、视在功率和功率因数；

20191008:
1.添加函数注释；
2.修改部分代码格式；

20191021:
1.修改tcp socket连接为短连接；
2.添加心跳机制；
