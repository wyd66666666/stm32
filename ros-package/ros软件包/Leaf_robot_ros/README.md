# Leaf_robot_ros
ros机械臂代码上位机部分
使用smartarm作为机械臂载体
ros_moviet控制，成本较低。

适合希望低成本使用ros机械臂的机器人开发者。

我是河南大学计算机与信息工程学院16级网络工程系 吴雪铭，希望和大家多多交流，共同进步。

联系时，请实名化，否则一律拒绝添加好友。

微信 K15837821186 安集海下
qq 1362385699

This code is used for maniputor in ROS. And it's my graduation design.It will took a long time(1~20s) for load trajectory in arduino(or other MCU) before maniputor moves. Because Serial is slowly. I took 0.08s to load one point to arduino, it means that arduino coulde get 12.5 points in a second. But generally, movite would give me about 30 to 200 points in a trajectory, it would take a long time. If you use use Raspberry PI's GPIO to control steppers with controllers(eg. TB6600), and communicat Raspberry PI & PC by data cable, it should be better , because cable has faster communication speeds and more reliable communication quality. But will talk me more time and more money(circuit design, udp/tcp config .etc), and I have many other things to do. So just use it. I just bought a maniputor(about 160$) from other.And remap1.6(4$),TB6600 x 3(4$ * 3) ,A4988 x 3(1$ x 3), 24V&12V DC-power(12$), a cheaper arduino2560(10$) and others. In total, it cost me about 220$.



I'm happy to make some friends from other countries. If you want to build your maniputor in ROS.Please call me on Wechat(K15837821186), or HelloTalk(wuxueming). You'd better translate some words from Chinese. If you make friend with me, OK, I will be pleasure to give arduino code to you in free, (^_^)
