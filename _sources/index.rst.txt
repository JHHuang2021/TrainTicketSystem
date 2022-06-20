.. 火车票管理系统 documentation master file, created by
   sphinx-quickstart on Thu Apr 28 08:51:05 2022.
   You can adapt this file completely to your liking, but it should at least
   contain the root `toctree` directive.

火车票管理系统开发文档
======================

简介
----

本项目为 SJTU CS1951 数据结构课程大作业，实现一个类似于 12306 的火车票订票系统。

后端为 C++ 实现，文件存储数据为 B+ 树


+-----------------------+---------------------------+
|         成员          |           分工            |
+=======================+===========================+
| 黄健浩 `JHHuang2021`_ |  B+ 树，前端（html）      |
+-----------------------+---------------------------+
| 林田川 `1024th`_      |  后端逻辑，前端（js, php）|
+-----------------------+---------------------------+

.. _JHHuang2021: https://github.com/JHHuang2021
.. _1024th: https://github.com/1024th

调用示意图
^^^^^^^^^^

.. image:: images/framework.svg

后端模块划分
------------

工具类
^^^^^^

.. toctree::
    :maxdepth: 2

    api/char
    api/vector
    api/optional_arg
    api/tuple
    api/datetime
    api/utils

主体逻辑
^^^^^^^^

.. toctree::
    :maxdepth: 2

    api/b_plus_tree
    api/command_parser
    api/user
    api/train

前端
----

:ref:`frontend`


索引
----

* :ref:`genindex`
* :ref:`search`
