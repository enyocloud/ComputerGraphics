#include "TRShaderPipeline.h"

#include <algorithm>

namespace TinyRenderer
{
	//----------------------------------------------VertexData----------------------------------------------

	TRShaderPipeline::VertexData TRShaderPipeline::VertexData::lerp(
		const TRShaderPipeline::VertexData &v0,
		const TRShaderPipeline::VertexData &v1,
		float frac)
	{
		//Linear interpolation
		VertexData result;
		result.pos = (1.0f - frac) * v0.pos + frac * v1.pos;
		result.col = (1.0f - frac) * v0.col + frac * v1.col;
		result.nor = (1.0f - frac) * v0.nor + frac * v1.nor;
		result.tex = (1.0f - frac) * v0.tex + frac * v1.tex;
		result.cpos = (1.0f - frac) * v0.cpos + frac * v1.cpos;
		result.spos.x = (1.0f - frac) * v0.spos.x + frac * v1.spos.x;
		result.spos.y = (1.0f - frac) * v0.spos.y + frac * v1.spos.y;

		return result;
	}

	TRShaderPipeline::VertexData TRShaderPipeline::VertexData::barycentricLerp(
		const VertexData &v0, 
		const VertexData &v1, 
		const VertexData &v2,
		glm::vec3 w)
	{
		VertexData result;
		result.pos = w.x * v0.pos + w.y * v1.pos + w.z * v2.pos;
		result.col = w.x * v0.col + w.y * v1.col + w.z * v2.col;
		result.nor = w.x * v0.nor + w.y * v1.nor + w.z * v2.nor;
		result.tex = w.x * v0.tex + w.y * v1.tex + w.z * v2.tex;
		result.cpos = w.x * v0.cpos + w.y * v1.cpos + w.z * v2.cpos;
		result.spos.x = w.x * v0.spos.x + w.y * v1.spos.x + w.z * v2.spos.x;
		result.spos.y = w.x * v0.spos.y + w.y * v1.spos.y + w.z * v2.spos.y;

		return result;
	}

	void TRShaderPipeline::VertexData::prePerspCorrection(VertexData &v)
	{
		//Perspective correction: the world space properties should be multipy by 1/w before rasterization
		//https://zhuanlan.zhihu.com/p/144331875
		//We use pos.w to store 1/w
		v.pos.w = 1.0f / v.cpos.w;
		v.pos = glm::vec4(v.pos.x * v.pos.w, v.pos.y * v.pos.w, v.pos.z * v.pos.w, v.pos.w);
		v.tex = v.tex * v.pos.w;
		v.nor = v.nor * v.pos.w;
		v.col = v.col * v.pos.w;
	}

	void TRShaderPipeline::VertexData::aftPrespCorrection(VertexData &v)
	{
		//Perspective correction: the world space properties should be multipy by w after rasterization
		//https://zhuanlan.zhihu.com/p/144331875
		//We use pos.w to store 1/w
		float w = 1.0f / v.pos.w;
		v.pos = v.pos * w;
		v.tex = v.tex * w;
		v.nor = v.nor * w;
		v.col = v.col * w;
	}

	//----------------------------------------------TRShaderPipeline----------------------------------------------

	void TRShaderPipeline::rasterize_wire(
		const VertexData &v0,
		const VertexData &v1,
		const VertexData &v2,
		const unsigned int &screen_width,
		const unsigned int &screene_height,
		std::vector<VertexData> &rasterized_points)
	{
		//Draw each line step by step
		rasterize_wire_aux(v0, v1, screen_width, screene_height, rasterized_points);
		rasterize_wire_aux(v1, v2, screen_width, screene_height, rasterized_points);
		rasterize_wire_aux(v0, v2, screen_width, screene_height, rasterized_points);
	}

	static bool insideTriangle(float x, float y, const glm::vec3 v0, const glm::vec3 v1, const glm::vec3 v2)
	{
		// TODO : Implement this function to check if the point (x, y) is inside the triangle represented by _v[0], _v[1], _v[2]
		float x1 = v0.x;
		float x2 = v1.x;
		float x3 = v2.x;
		float y1 = v0.y;
		float y2 = v1.y;
		float y3 = v2.y;
		if ((y - y1)*(x2 - x1)-(y2 - y1)*(x - x1) < 0)//如果AB×AP<0，则不在三角形内，返回false
			return false;
		if ((x3 - x2)*(y - y2)-(y3 - y2)*(x - x2) < 0)//如果BC×BP<0，则不在三角形内，返回false
			return false;
		if ((x1 - x3)*(y - y3)-(y1 - y3)*(x - x3) < 0)//如果CA×CP<0，则不在三角形内，返回false
			return false;
		return true;
	}
	
	void TRShaderPipeline::rasterize_fill_edge_function(
		const VertexData& v0,
		const VertexData& v1,
		const VertexData& v2,
		const unsigned int& screen_width,
		const unsigned int& screene_height,
		std::vector<VertexData>& rasterized_points)
	{
		//Edge-function rasterization algorithm

		//Task4: Implement edge-function triangle rassterization algorithm
		// Note: You should use VertexData::barycentricLerp(v0, v1, v2, w) for interpolation, 
		//       interpolated points should be pushed back to rasterized_points.
		//       Interpolated points shold be discarded if they are outside the window. 
		//       v0.spos, v1.spos and v2.spos are the screen space vertices.
		
		//求出三角形包围盒
		VertexData v[3] = {v0, v1, v2};
		double minX, maxX, minY, maxY;
		minX = maxX = v0.spos.x;
		minY = maxY = v0.spos.y;
		for (int i = 0; i < 3; i++) {
			minX = std::min(minX, (double)v[i].spos.x);
			maxX = std::max(maxX, (double)v[i].spos.x);

			minY = std::min(minY, (double)v[i].spos.y);
			maxY = std::max(maxY, (double)v[i].spos.y);
		}
		//遍历包围盒中所有点
		for (int ix = minX; ix <= maxX; ix++) {
			for (int iy = minY; iy <= maxY; iy++) {
				//普通光栅化
				float x = ix + 1 / 2;
				float y = iy + 1 / 2;

				//判断点是否在三角形内
				glm::vec3 s1 = glm::vec3(v1.spos.x - v0.spos.x, v2.spos.x - v0.spos.x, v0.spos.x - x);
				glm::vec3 s2 = glm::vec3(v1.spos.y - v0.spos.y, v2.spos.y - v0.spos.y, v0.spos.y - y);
				glm::vec3 u = glm::cross(s1, s2);

				float x1 = v0.spos.x;
				float x2 = v1.spos.x;
				float x3 = v2.spos.x;
				float y1 = v0.spos.y;
				float y2 = v1.spos.y;
				float y3 = v2.spos.y;

				bool f1 = ((y - y1) * (x2 - x1) - (y2 - y1) * (x - x1) > 0);
				bool f2 = ((x3 - x2) * (y - y2) - (y3 - y2) * (x - x2) > 0);
				bool f3 = ((x1 - x3) * (y - y3) - (y1 - y3) * (x - x3) > 0);
				//三角形内：判断三个叉积的结果是否同号
				if (f1 == f2 && f2 == f3) {
					//插值计算,插值权重(1-u-v,u,v)即(1-(u.x+u.y)/u.z, u.x/u.z, u.y/u.z)
					glm::vec3 k;
					k.x = 1 - (u.x + u.y) / u.z;
					k.y = u.x / u.z;
					k.z = u.y / u.z;
					auto mid = VertexData::barycentricLerp(v0, v1, v2, k);
					mid.spos = glm::ivec2(ix, iy);//这是浮点数误差导致的孔洞
					//判断点是否在屏幕范围内
					if (mid.spos.x >= 0 && mid.spos.x < screen_width && mid.spos.y >= 0 && mid.spos.y < screene_height)
					{
						rasterized_points.push_back(mid);
					}
				}
			}
		}
		//For instance:
		rasterized_points.push_back(v0);
		rasterized_points.push_back(v1);
		rasterized_points.push_back(v2);
	}

	void TRShaderPipeline::rasterize_wire_aux(
		const VertexData &from,
		const VertexData &to,
		const unsigned int &screen_width,
		const unsigned int &screen_height,
		std::vector<VertexData> &rasterized_points)
	{

		//Task1: Implement Bresenham line rasterization
		// Note: You shold use VertexData::lerp(from, to, weight) for interpolation,
		//       interpolated points should be pushed back to rasterized_points.
		//       Interpolated points shold be discarded if they are outside the window. 
		
		//       from.spos and to.spos are the screen space vertices.
		int dx = to.spos.x - from.spos.x;
		int dy = to.spos.y - from.spos.y;
		int stepX = 1, stepY = 1;

		// judge the sign
		if (dx < 0)
		{
			stepX = -1;
			dx = -dx;
		}
		if (dy < 0)
		{
			stepY = -1;
			dy = -dy;
		}

		int d2x = 2 * dx, d2y = 2 * dy;
		int d2y_minus_d2x = d2y - d2x;
		int sx = from.spos.x;
		int sy = from.spos.y;


		if (dy <= dx)
		{
			int flag = d2y - dx;
			for (int i = 0; i <= dx; ++i)
			{
				auto mid = VertexData::lerp(from, to, static_cast<float>(i) / dx);
				mid.spos = glm::vec4(sx, sy, 0.0f, 1.0f);
				if (mid.spos.x >= 0 && mid.spos.x < screen_width && mid.spos.y >= 0 && mid.spos.y < screen_height)
				{
					rasterized_points.push_back(mid);
				}
				sx += stepX;
				if (flag <= 0)
				{
					flag += d2y;
				}
				else
				{
					sy += stepY;
					flag += d2y_minus_d2x;
				}
			}
		}
		// slope > 1.
		else
		{
			int flag = d2x - dy;
			for (int i = 0; i <= dy; ++i)
			{
				auto mid = VertexData::lerp(from, to, static_cast<float>(i) / dy);
				mid.spos = glm::vec4(sx, sy, 0.0f, 1.0f);
				if (mid.spos.x >= 0 && mid.spos.x < screen_width && mid.spos.y >= 0 && mid.spos.y < screen_height)
				{
					rasterized_points.push_back(mid);
				}
				sy += stepY;
				if (flag <= 0)
				{
					flag += d2x;
				}
				else
				{
					sx += stepX;
					flag -= d2y_minus_d2x;
				}
			}
		}
		//For instance:
		rasterized_points.push_back(from);
		rasterized_points.push_back(to);

	}

	void TRDefaultShaderPipeline::vertexShader(VertexData &vertex)
	{
		//Local space -> World space -> Camera space -> Project space
		vertex.pos = m_model_matrix * glm::vec4(vertex.pos.x, vertex.pos.y, vertex.pos.z, 1.0f);
		vertex.cpos = m_view_project_matrix * vertex.pos;
	}

	void TRDefaultShaderPipeline::fragmentShader(const VertexData &data, glm::vec4 &fragColor)
	{
		//Just return the color.
		fragColor = glm::vec4(data.tex, 0.0, 1.0f);
	}
}