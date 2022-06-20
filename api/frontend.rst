.. _frontend:

前端文档
========

工具栈
------

* JavaScript，jQuery
* Nginx，PHP
* PHP 与 C++ 使用 Socket 通信

.. image:: /images/frontend.svg

部署文档
--------

下载静态文件
^^^^^^^^^^^^


下载编译后端程序
^^^^^^^^^^^^^^^^

后端程序默认监听端口为 7777，如需使用其他端口请在 command_parser.cpp 内修改。


配置 nginx 与 php-fpm
^^^^^^^^^^^^^^^^^^^^^

以 Ubuntu 20.04 为例，安装 nginx 与 php-fpm::

  sudo apt -y install php7.4-fpm nginx

安装完成后，使用 ``systemctl status nginx`` 检查 nginx 是否正常运行，使用 ``systemctl status php7.4-fpm`` 检查 php7.4-fpm 是否正常运行。

配置 nginx（默认配置文件 ``/etc/nginx/sites-available/default``）::

  server {
    listen 80 default_server;
    listen [::]:80 default_server;

    root /var/www/html;

    server_name _;

    location / {
      # First attempt to serve request as file, then
      # as directory, then fall back to displaying a 404.
      try_files $uri $uri/ =404;
    }

    # pass PHP scripts to FastCGI server
    location ~ \.php$ {
      #include snippets/fastcgi-php.conf;
    
      # With php-fpm (or other unix sockets):
      fastcgi_pass unix:/var/run/php/php7.4-fpm.sock;
      # With php-cgi (or other tcp sockets):
      # fastcgi_pass 127.0.0.1:9000;
      include fastcgi_params;
      fastcgi_param SCRIPT_FILENAME $document_root$fastcgi_script_name;
      fastcgi_buffer_size 16k;
      fastcgi_buffers 4 16k;
      fastcgi_connect_timeout 1;
      fastcgi_send_timeout 1;
      fastcgi_read_timeout 1;
    }
  }

其中 ``/var/www/html`` 替换为存放静态页面的路径。根据 php-fpm 配置选择 unix sockets 或 tcp sockets。

运行 ``sudo nginx -s reload`` 重新加载配置。如果设置了防火墙，请放行 80 端口。此时可以从外网访问到前端页面。
