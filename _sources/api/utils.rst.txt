工具函数
========

主要是快速排序函数和字符串拼接函数。

使用 operator+ 合并多个字符串时，可能引起内存的反复移动，导致效率低下。

一种解决方法是，合并前先在 string 里预留空间（reserve），之后把每个字符串都附加到 string 之后（operator+=）。

代码实现参考了 `<https://stackoverflow.com/a/18899027>`_。

.. doxygenfile:: utils.h
   :project: 火车票管理系统