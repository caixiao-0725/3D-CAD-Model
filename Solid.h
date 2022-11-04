#pragma once
#include<list>
#include<tuple>

using namespace std;

template <typename T >
struct vec3
{
	union {
		struct { T x, y, z; };
		T raw[3];
	};
	inline vec3<T> operator +(vec3<T> &t){
		return vec3<T>{x + t.x, y + t.y, z + t.z};
	}
};

typedef vec3<GLdouble> vec3f;

//定义顶点结构

struct Vertex;
struct Edge;
struct HalfEdge;
struct Loop;
struct Face;
struct Solid;



struct Vertex
{
	vec3f position;
};

//定义完整边结构 包含两个半边的位置
struct Edge
{
	HalfEdge* halfEdge0 = nullptr;
	HalfEdge* halfEdge1 = nullptr;
};

//定义半边结构   其中包含一个顶点位置，他所在的完整边的信息以及他所在环的信息
struct HalfEdge
{

	Loop* parentLoop = nullptr;
	Edge* parentEdge = nullptr;
	Vertex* vertex = nullptr;

	HalfEdge* next = nullptr;
	HalfEdge* pre = nullptr;
	//找到所在完整边的另一条半边
	HalfEdge* mate() {
		return parentEdge->halfEdge0 == this ? parentEdge->halfEdge1 : parentEdge->halfEdge0;
	}
};

//定义环的结构 包含他所在的面和组成他的半边，通过这个半边能找到环里所有其他的边
struct Loop
{
	Face* parentFace = nullptr;
	HalfEdge* leadingHalfEdge = nullptr;
	//查找环里包含某一具体顶点的半边
	HalfEdge* find(Vertex* vertex) {
		auto temp_halfEdge = leadingHalfEdge;
		while (temp_halfEdge) {
			if (temp_halfEdge->vertex == vertex) return temp_halfEdge;
			temp_halfEdge = temp_halfEdge->next;
			if (temp_halfEdge == leadingHalfEdge) break;
		}
		return nullptr;
	}
};

//定义面的结构 包含他所在的体和面里存在的环，其中分为外环和内环，内环可以有很多个，但是外环只有一个
struct Face
{
	Solid* parentSolid = nullptr;
	Loop* outerLoop = nullptr;
	list<Loop*>	innerLoops;
};
//定义体的结构 包含体里所有的面、边、点的信息
struct Solid
{
	list<Face*>	faces;
	list<Edge*>	edges;
	list<Vertex*>	vertices;
};



//*******************************************
//*************各种make,kill操作*************
//*******************************************

tuple<Vertex*, Face*, Solid*> MakeVertexFaceSolid(const vec3f& coord)
{
	auto solid = new Solid;
	auto face = new Face;
	auto loop = new Loop;
	auto vertex = new Vertex;

	solid->faces.push_back(face);
	face->parentSolid = solid;

	face->outerLoop = loop;
	loop->parentFace = face;

	vertex->position = coord;
	solid->vertices.push_back(vertex);

	return make_tuple(vertex, face, solid);
}

//mev  给顶点v0分配一个在coord的顶点v1，并存到loop上
tuple<Edge*, Vertex*> MakeEdgeVertex(Vertex* vertex0, Loop* loop, vec3f coord)
{
	auto vertex1 = new Vertex;
	auto edge = new Edge;
	auto halfEdge0 = new HalfEdge;
	auto halfEdge1 = new HalfEdge;

	vertex1->position = coord;
	halfEdge0->vertex = vertex0;
	halfEdge1->vertex = vertex1;

	halfEdge0->next = halfEdge1;
	halfEdge1->pre = halfEdge0;

	halfEdge0->parentEdge = edge;
	halfEdge1->parentEdge = edge;
	halfEdge0->parentLoop = loop;
	halfEdge1->parentLoop = loop;

	edge->halfEdge0 = halfEdge0;
	edge->halfEdge1 = halfEdge1;

	if (loop->leadingHalfEdge == nullptr) {
		// if loop is empty, init with halfEdge0 and halfEdge1
		loop->leadingHalfEdge = halfEdge0;
		halfEdge0->pre = halfEdge1;
		halfEdge1->next = halfEdge0;
	}
	else {
		// loop is not empty, insert halfEdge0 and halfEdge1
		auto tmp = loop->find(vertex0);
		halfEdge0->pre = tmp->pre;
		halfEdge1->next = tmp;

		halfEdge0->pre->next = halfEdge0;
		halfEdge1->next->pre = halfEdge1;
	}

	auto solid = loop->parentFace->parentSolid;
	solid->edges.push_back(edge);
	solid->vertices.push_back(vertex1);

	return make_tuple(edge, vertex1);
}


tuple<Edge*, Face*> MakeEdgeFace(Loop* loop0, Vertex* vertex0, Vertex* vertex1)
{
	auto solid = loop0->parentFace->parentSolid;
	auto face = new Face;
	auto loop1 = new Loop;
	auto edge = new Edge;
	auto halfEdge0 = new HalfEdge;
	auto halfEdge1 = new HalfEdge;

	// set face
	solid->faces.push_back(face);
	face->parentSolid = loop0->parentFace->parentSolid;
	face->outerLoop = loop1;
	// set edge
	solid->edges.push_back(edge);
	edge->halfEdge0 = halfEdge0;
	edge->halfEdge1 = halfEdge1;
	// set loop1
	loop1->parentFace = face;

	// find tmp half edges
	auto tmphe0 = loop0->find(vertex0);
	auto tmphe1 = loop0->find(vertex1);

	// link two new half edges to loop
	halfEdge0->next = tmphe1;
	halfEdge0->pre = tmphe0->pre;
	halfEdge1->next = tmphe0;
	halfEdge1->pre = tmphe1->pre;

	tmphe0->pre->next = halfEdge0;
	tmphe1->pre->next = halfEdge1;
	tmphe0->pre = halfEdge1;
	tmphe1->pre = halfEdge0;

	// set two half edges
	halfEdge0->parentLoop = loop0;
	halfEdge0->parentEdge = edge;
	halfEdge1->parentLoop = loop1;
	halfEdge1->parentEdge = edge;
	halfEdge0->vertex = vertex0;
	halfEdge1->vertex = vertex1;
	// maintain two loop
	loop1->leadingHalfEdge = halfEdge1;
	loop0->leadingHalfEdge = halfEdge0;
	return make_tuple(edge, face);
}

std::tuple<Loop*> KillEdgeMakeRing(Vertex* vertex0, Vertex* vertex1, Loop* loop0)
{
	auto solid = loop0->parentFace->parentSolid;
	auto loop1 = new Loop;

	// find corresponding half edges
	HalfEdge* halfEdge0 = loop0->leadingHalfEdge;
	HalfEdge* halfEdge1 = halfEdge0->next;
	while (halfEdge0->vertex != vertex0 || halfEdge1->vertex != vertex1) {
		halfEdge0 = halfEdge1;
		halfEdge1 = halfEdge0->next;
	}

	// set the original loop
	loop0->leadingHalfEdge = halfEdge1->next;
	halfEdge0->pre->next = halfEdge1->next;
	halfEdge1->next->pre = halfEdge0->pre;
	// set new loop
	loop1->parentFace = loop0->parentFace;
	loop1->parentFace->innerLoops.push_back(loop1);
	// delete edge
	auto edge = halfEdge0->parentEdge;
	solid->edges.remove(edge);
	delete edge;
	// delete two halfedges
	delete halfEdge0;
	delete halfEdge1;
	// return the new loop
	return std::make_tuple(loop1);
}

void KillFaceMakeRingHole(Loop* outerLoop, Loop* innerLoop)
{
	auto solid = outerLoop->parentFace->parentSolid;
	outerLoop->parentFace->innerLoops.push_back(innerLoop);
	delete innerLoop->parentFace;
	solid->faces.remove(innerLoop->parentFace);
	innerLoop->parentFace = outerLoop->parentFace;
}

void Sweep(Face* face, vec3f dir)
{
	auto extrude = [](Loop* loop, vec3f dir)
	{
		auto endVertex = loop->leadingHalfEdge->vertex;
		auto he = loop->leadingHalfEdge;

		auto oriRet = MakeEdgeVertex(he->vertex, loop, he->vertex->position + dir);
		he = he->next;
		auto lastRet = oriRet;
		while (he->vertex != endVertex) {
			auto ret = MakeEdgeVertex(he->vertex, loop, he->vertex->position + dir);
			MakeEdgeFace(loop, std::get<1>(lastRet), std::get<1>(ret));
			lastRet = ret;
			he = he->next;
		}
		MakeEdgeFace(loop, std::get<1>(lastRet), std::get<1>(oriRet));
	};

	extrude(face->outerLoop, dir);
	for (auto loop : face->innerLoops)
		extrude(loop, dir);
}
