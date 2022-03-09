# 网络通信服务器

---

## 版本说明

- v0.1：客户端将发送的消息通过服务端“转发”给所有客户端

## 待实现功能

- [x] 增加添加好友、群聊功能
- [x] 将好友、群聊列表添加进容器，查询的时候从容器中取
- [ ] 将添加的好友通过服务端同步给好友
- [x] 群聊给在线的好友发送数据

中文相关编码问题：

- [ ] 数据库表中字段有中文在VS中不能正常显示
- [ ] VS中文字符插入MySQL乱码
- [ ] 查询数据库中是否有某条语句有问题（因为中文编码的问题，有待解决）

消息格式定义：TODO

1# 被添加好友信息

2# 被删除好友信息



## 通信流程

1. 启动服务器
2. 启动客户端
3. 用户登录（没有账号就注册）
4. 查看联系人（包括聊天群）
5. 给联系人发送消息 TODO
6. 退出登录
7. 关闭客户端

## 使用方式

- 安装MySQL，在VS中配置MySQL