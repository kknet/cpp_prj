#!众安科技 虚拟实验室 量化投研平台

> ###作者：吴典（Andy.Woo） 联系方式 qq：84020702
>



# 《环境搭建》
> 1.win7、vs2013、qt5.5.1      下载地址： http://pan.baidu.com/s/1boUNd07

> 2.git clone https://github.com/timinGo/RA-sharedoc 
>在目录wudian/zhongan下面

>本工程的svn地址： https://218.81.139.87:4433/svn/zhongan    
> 只读账户：zhongan 密码：zhongan
> 注意："218.81.139.87"如果发生变化，应该按如下步骤操作svn：
> TortoiseSVN -> Relocate... -> 将ip替换为新的ip   -> 点击OK确认

> 如果SVN Update失败，则需要clean up（将Break Locks选项勾上！）后再Update就行了

> ubuntu:   admin@172.21.64.52   Admin@123        > 定时启停vnpy的存数据功能 ： crontab -l

#《一》项目

> 1.debug_client： okcoin的websocket c++项目，用于取ticker、depth、交易
目前用于向mysql存数据

> 2.NewTrader： ctp期货的量化框架
> 3.行情收集器: 存ctp期货的tick、分钟线

#《二》基础模块
> 1.common 
> 2.timer  定时器库 可用于切k线
> 3.network 



#《历史日志》
> 2017.6.13  新建工程
> 2017.9.4 从svn迁移到git  由于boost库太大，所以没有上传，如果需要可以单独发送或者自己编译
>
>