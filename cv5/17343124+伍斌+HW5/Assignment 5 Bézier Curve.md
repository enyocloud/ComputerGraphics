# Assignment 5: Bézier Curve

姓名：伍斌

学号：17343124

[TOC]



---

## Task 1

根据 Bézier 曲线的定义来计算给定 t 值（ t∈[0, 1]）下的 Bézier 曲线上的点。

### 代码

```c++
Point2D BezierCurve::implementTask1(const std::vector<Point2D> &points, const double &t) const
{
	//Task1: implement Bezier curve generation algorithm accroding to the definition
	int n = points.size() - 1;
	int N = 1;
	for (int i = 1; i <= n; i++){
		N *= i;
	}
	int x = 0;
	int y = 0;
	double* T = (double*)malloc(sizeof(double) * points.size());
	for (int i = 0; i < points.size(); i++) {
		int a = 1;
		for (int j = 1; j <= i; j++) {
			a *= j;
		}
		int b = 1;
		for (int j = n - i; j > 0; j--) {
			b *= j;
		}
		T[i] = N * pow(t, i) * pow(1.0f - t, n - i) / (a * b);
		x += points[i].x * T[i];
		y += points[i].y * T[i];
	}
	free(T);
	return Point2D(x, y);
}
```

### 实现效果

![image-20210611222857130](C:\Users\42405\AppData\Roaming\Typora\typora-user-images\image-20210611222857130.png)

### 实现过程

根据 Bézier 曲线的定义：

![image-20210611224031724](C:\Users\42405\AppData\Roaming\Typora\typora-user-images\image-20210611224031724.png)

其中 Pi 为第 i 个控制顶点。

按照以上公式直接进行计算，获得 t 对应的曲线上的点所在的位置即可进行绘制。



## Task 2

实现更为简单、直观的de Casteljau算法来生成给定 t 值（ t∈[0, 1]）下的 Bézier 曲线上的点。

### 代码

```c++
Point2D BezierCurve::implementTask2(const std::vector<Point2D> &points, const double &t) const
{
	//Task2: implement de Casteljau algorithm for Bezier curve
	// Note: you should use Point2D::lerp().
	Point2D result(0, 0);
	if (points.size() == 1)
		result = *points.begin();
	std::vector<Point2D> old_points(points);
	std::vector<Point2D> new_points;
	while (old_points.size() != 1) {
		for (int i = 0; i < old_points.size() - 1; i++) {
			new_points.push_back(old_points[i] * (1 - t) + old_points[i + 1] * t);
		}
		old_points.clear();
		old_points = new_points;
		new_points.clear();
	}
	result = *old_points.begin();
	return result;
	//return Point2D(0, 0);
}
```

###  实现效果

![image-20210611223232187](C:\Users\42405\AppData\Roaming\Typora\typora-user-images\image-20210611223232187.png)

### 实现过程

对t∈[0, 1]，每次增加0.001，对每个t调用一次 de Casteljau 函数，通过循环实现
$$
B(t)=\sum_{i=0}^{n}\dbinom{n}{i}P_i(1-t)^{n-i}t^i
$$
一般参数公式，获得 t 对应的曲线上的点所在的位置即可进行绘制。





---

## Task3

**谈谈你对 Bézier 曲线的理解，Bézier 曲线的缺点是什么？**

​	Bezier曲线是以逼近为基础，通过多个控制点控制的平滑曲线，本质上是由调和函数根据控制点插
值生成。
​	Bezier曲线具有以下几个性质：端点性质，即第一个和最后一个控制点为曲线起点和终点；对称
性，保持特征多边形的顶点位置不变但顺序颠倒，所得的曲线形状不变；凸包性，曲线一定落在其控制
多边形的凸包之中；仿射不变性，曲线形状只与控制点相对位置有关；全局性，如果改变任一控制点位
置，整个曲线的形状都会受影响；其幂次受控制点数量影响，控制点越多幂次越高。
​	Bézier 曲线的缺点：一是没有局部控制能力，每个控制点都会影响整个曲线的形状；二是随着控制点的增加，曲线的次数也增加，因而计算量会持续增大。

