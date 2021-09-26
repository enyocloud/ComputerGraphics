# Assignment 3: Lighting & Texturing

@[TOC](Assignment 3: Lighting & Texturing)

姓名：伍斌
学号：17343124

---

## Task1

实现纹理的最邻近采样算法。

**代码**

```c++
	glm::vec4 TRTexture2DSampler::textureSampling_nearest(const TRTexture2D &texture, glm::vec2 uv)
	{
		unsigned char r = 255, g = 255, b = 255, a = 255;

		//Task1: Implement nearest sampling algorithm for texture sampling
		// Note: You should use texture.readPixel() to read the pixel, and for instance, 
		//       use texture.readPixel(25,35,r,g,b,a) to read the pixel in (25, 35).
		//       But before that, you need to map uv from [0,1]*[0,1] to [0,width-1]*[0,height-1].
		{
			float x = uv.x;
			float y = uv.y;
			float x1 = x * (texture.getWidth() - 1);
			float y1 = y * (texture.getHeight() - 1);
			int x2 = (int)x1;
			int y2 = (int)y1;
			texture.readPixel(x2, y2, r, g, b, a);
		}

		constexpr float denom = 1.0f / 255.0f;
		return glm::vec4(r, g, b, a) * denom;
	}
```

**实验结果**

![image-20210516204123395](C:\Users\42405\AppData\Roaming\Typora\typora-user-images\image-20210516204123395.png)

**实现思路**

参数 texture 是要被采样的纹理，可以调用它的 readPixel 方法去读取指定位置的像素：

传入的位置应该是 
$$
[0,width-1] × [0,height-1]
$$
 uv 是二维纹理坐标，取值范围为
$$
[0,1] × [0,1]
$$
现在只需要将将纹理坐标 uv 从 [0,1] × [0,1] 映射到 [0,width-1] × [0,height-1] ，然后再将坐标进行四舍五入即可。



---

## Task2

在实现了Task 1的基础上，在片元着色器中实现 Phong 光照模型。

**代码**

```c++
		//Task2: Implement phong lighting algorithm
		// Note: The parameters you should use are described as follow:
		//        amb_color: the ambient color of the fragment
		//        dif_color: the diffuse color of the fragment
		//        spe_color: the specular color of the fragment
		//          fragPos: the fragment position in world space
		//           normal: the fragment normal in world space
		//          viewDir: viewing direction in world space
		//             m_kd: diffuse coefficient
		//         lightDir: lighting direction in world space
		// light.lightColor: the ambient, diffuse and specular color of light source
		//      m_shininess: specular hightlight exponent coefficient
		//   light.lightPos: the position of the light source
		//light.attenuation：the attenuation coefficients of the light source (x,y,z) -> (constant,linear,quadratic)
		//  you could use glm::pow(), glm::dot, glm::reflect(), glm::max(), glm::normalize(), glm::length() et al.
		{
			float distance = length(light.lightPos - fragPos);
			attenuation = 1.0 / (light.attenuation.x + light.attenuation.y * distance + light.attenuation.z * (distance * distance));

			// ambient
			ambient = light.lightColor * amb_color;

			//diffuse
			glm::vec3 norm = glm::vec3(glm::normalize(normal));
			float diff = glm::max(glm::dot(norm, lightDir), 0.0f);
			diffuse = diff * light.lightColor * dif_color;
		
			//specular
			viewDir = glm::normalize(m_viewer_pos - fragPos);
			glm::vec3 reflectDir = glm::reflect(-lightDir, norm);
			float spec = pow(glm::max(glm::dot(norm, reflectDir), 0.0f), m_shininess);
			specular = spec * light.lightColor * spe_color;
		}
```

**实现效果**

![image-20210516210106161](C:\Users\42405\AppData\Roaming\Typora\typora-user-images\image-20210516210106161.png)



![image-20210516205240965](C:\Users\42405\AppData\Roaming\Typora\typora-user-images\image-20210516205240965.png)

**实现思路**

参考点光源衰减公式

![image-20210516205353020](C:\Users\42405\AppData\Roaming\Typora\typora-user-images\image-20210516205353020.png)

在这里d代表了片段距光源的距离。接下来为了计算衰减值，我们定义3个（可配置的）项：
$$
常数项K_c、一次项K_l和二次项K_q
$$

- 常数项通常保持为1.0，它的主要作用是保证分母永远不会比1小，否则的话在某些距离上它反而会增加强度，这肯定不是我们想要的效果。
- 一次项会与距离值相乘，以线性的方式减少强度。
- 二次项会与距离的平方相乘，让光源以二次递减的方式减少强度。二次项在距离比较小的时候影响会比一次项小很多，但当距离值比较大的时候它就会比一次项更大了。

我们可以通过获取片段和光源之间的向量差，并获取结果向量的长度作为距离项。

```c++
float distance    = length(light.position - FragPos); 
float attenuation = 1.0 / (light.constant + light.linear * distance +      light.quadratic * (distance * distance));
```

接下来，我们将包含这个衰减值到光照计算中，将它分别乘以环境光、漫反射和镜面光颜色。

```c++
ambient  *= attenuation;
diffuse  *= attenuation;
specular *= attenuation;
```



---

## Task3

在实现了Task 3的基础上，对 Phong 的高光项进行改进，实现 Blinn-Phong 光照模型。

**代码**

```c++
		//Task2: Implement phong lighting algorithm
		// Note: The parameters you should use are described as follow:
		//        amb_color: the ambient color of the fragment
		//        dif_color: the diffuse color of the fragment
		//        spe_color: the specular color of the fragment
		//          fragPos: the fragment position in world space
		//           normal: the fragment normal in world space
		//          viewDir: viewing direction in world space
		//             m_kd: diffuse coefficient
		//         lightDir: lighting direction in world space
		// light.lightColor: the ambient, diffuse and specular color of light source
		//      m_shininess: specular hightlight exponent coefficient
		//   light.lightPos: the position of the light source
		//light.attenuation：the attenuation coefficients of the light source (x,y,z) -> (constant,linear,quadratic)
		//  you could use glm::pow(), glm::dot, glm::reflect(), glm::max(), glm::normalize(), glm::length() et al.
		{
			float distance = length(light.lightPos - fragPos);
			attenuation = 1.0 / (light.attenuation.x + light.attenuation.y * distance + light.attenuation.z * (distance * distance));

			// ambient
			ambient = light.lightColor * amb_color;

			//diffuse
			glm::vec3 norm = glm::vec3(glm::normalize(normal));
			float diff = glm::max(glm::dot(norm, lightDir), 0.0f);
			diffuse = diff * light.lightColor * dif_color;
		
			//specular
			viewDir = glm::normalize(m_viewer_pos - fragPos);
			//Task2 Phong
			//glm::vec3 reflectDir = glm::reflect(-lightDir, norm);
			//float spec = pow(glm::max(glm::dot(norm, reflectDir), 0.0f), m_shininess);
			//Task3 Blinn-Phong
			glm::vec3 halfwayDir = glm::normalize(lightDir + viewDir);
			float spec = pow(glm::max(glm::dot(viewDir, halfwayDir), 0.0f), m_shininess);
			specular = spec * light.lightColor * spe_color;
		}
```

**实现效果**

Task2 漫反射光的结果

![image-20210516210241716](C:\Users\42405\AppData\Roaming\Typora\typora-user-images\image-20210516210241716.png)

Task3 漫反射光的结果

![image-20210516210336135](C:\Users\42405\AppData\Roaming\Typora\typora-user-images\image-20210516210336135.png)

总体效果

![image-20210516210527911](C:\Users\42405\AppData\Roaming\Typora\typora-user-images\image-20210516210527911.png)

**实现思路**

Blinn-Phong 模型不再依赖于反射向量，而是采用了所谓的半程向量(Halfway Vector)，即光线与视线夹角一半方向上的一个单位向量。当半程向量与法线向量越接近时，镜面光分量就越大。当视线正好与（现在不需要的）反射向量对齐时，半程向量就会与法线完美契合。所以当观察者视线越接近于原本反射光线的方向时，镜面高光就会越强。

获取半程向量的方法很简单，只需要将光线的方向向量和观察向量加到一起，并将结果正规化(Normalize)就可以了：

![image-20210516211032235](C:\Users\42405\AppData\Roaming\Typora\typora-user-images\image-20210516211032235.png)

```c++
vec3 lightDir   = normalize(lightPos - FragPos);
vec3 viewDir    = normalize(viewPos - FragPos);
vec3 halfwayDir = normalize(lightDir + viewDir);
```

镜面光分量的实际计算只不过是对表面法线和半程向量进行一次约束点乘(Clamped Dot Product)，让点乘结果不为负，从而获取它们之间夹角的余弦值，之后对这个值取反光度次方：

```c++
float spec = pow(max(dot(normal, halfwayDir), 0.0), shininess);
vec3 specular = lightColor * spec;
```

