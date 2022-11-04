# 3D-CAD-Model
这次作业参考了http://www.songho.ca/opengl/gl_tessellation.html<Br/>
该网站讲述了如何将一个多边形(不论凹凸)曲面细分成一个一个三角形并且渲染到屏幕。<Br/>
````c++
 for(auto i : get<2>(ret0)->faces){  
  
    glColor3f(1.0,0.5,1.0);
    gluTessBeginPolygon(tess, 0);       
    gluTessBeginContour(tess);
    auto point = i->outerLoop->leadingHalfEdge;
    do{
        gluTessVertex(tess, point->vertex->position.raw, point->vertex->position.raw);
        point = point->next;
    } while (i->outerLoop->leadingHalfEdge->vertex != point->vertex);
    gluTessEndContour(tess);
    
    for (auto j : i->innerLoops) {
        gluTessBeginContour(tess);
        auto lhe = j->leadingHalfEdge;
        auto point = j->leadingHalfEdge;
        do {
            gluTessVertex(tess, point->vertex->position.raw, point->vertex->position.raw);
            point = point->next;
        } while (point->vertex != lhe->vertex);
        gluTessEndContour(tess);
    }
    gluTessEndPolygon(tess);
}
````
<Br/>
这里定义了一个双孔的立方体<Br/>

````c++

    auto ret0 = MakeVertexFaceSolid(vec3f{ 0, 0, 0 });
    auto loop = get<2>(ret0)->faces.front()->outerLoop;
    auto ret1 = MakeEdgeVertex(std::get<0>(ret0), loop, vec3f{ 5, 0, 0 });
    auto ret2 = MakeEdgeVertex(std::get<1>(ret1), loop, vec3f{ 5, 3, 0 });
    auto ret3 = MakeEdgeVertex(std::get<1>(ret2), loop, vec3f{ 0, 3, 0 });
    auto ret4 = MakeEdgeFace(loop, std::get<1>(ret3), std::get<0>(ret0));
    
    loop = std::get<1>(ret4)->outerLoop;
    auto ret5 = MakeEdgeVertex(std::get<0>(ret0), loop, vec3f{ 1, 1, 0 });
    auto ret6 = KillEdgeMakeRing(std::get<0>(ret0), std::get<1>(ret5), loop);
    
    loop = std::get<0>(ret6);
    auto ret7 = MakeEdgeVertex(std::get<1>(ret5), loop, vec3f{ 2, 1, 0 });
    auto ret8 = MakeEdgeVertex(std::get<1>(ret7), loop, vec3f{ 2, 2, 0 });
    auto ret9 = MakeEdgeVertex(std::get<1>(ret8), loop, vec3f{ 1, 2, 0 });
    auto ret10 = MakeEdgeFace(loop, std::get<1>(ret9), std::get<1>(ret5));
    
    loop = std::get<1>(ret4)->outerLoop;
    auto ret11 = MakeEdgeVertex(std::get<1>(ret1), loop, vec3f{ 4, 1, 0 });
    auto ret12 = KillEdgeMakeRing(std::get<1>(ret1), std::get<1>(ret11), loop);
    
    loop = std::get<0>(ret12);
    auto ret13 = MakeEdgeVertex(std::get<1>(ret11), loop, vec3f{ 4, 2, 0 });
    auto ret14 = MakeEdgeVertex(std::get<1>(ret13), loop, vec3f{ 3, 2, 0 });
    auto ret15 = MakeEdgeVertex(std::get<1>(ret14), loop, vec3f{ 3, 1, 0 });
    auto ret16 = MakeEdgeFace(loop, std::get<1>(ret15), std::get<1>(ret11));
    
    Sweep(std::get<1>(ret4),vec3f{ 0, 0, 1 });
    KillFaceMakeRingHole(std::get<2>(ret0)->faces.front()->outerLoop, std::get<1>(ret10)->outerLoop);
    KillFaceMakeRingHole(std::get<2>(ret0)->faces.front()->outerLoop, std::get<1>(ret16)->outerLoop);
````

<Br/><Br/>
#使用方法<Br/>
点开exe文件后，按D即可切换渲染模式,鼠标拖动可以转变视角
![image](https://github.com/caixiao-0725/3D-CAD-Model/blob/main/image/1.png)
![image](https://github.com/caixiao-0725/3D-CAD-Model/blob/main/image/2.png)
