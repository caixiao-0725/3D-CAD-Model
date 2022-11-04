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
