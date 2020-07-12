/***************************************************************
    graph.c
***************************************************************/

#include <stdio.h>  // for sprintf()
#include <my_string.h>
#include "graph.h"


    Graph *headGr = NULL;

    char Graph_errorMessage[100];



    Graph *removed_graph = NULL;

    Graph *newGraph ()
    {
        Graph *graph;
        if(removed_graph==NULL)
        {
            graph = (Graph*)FreeMemory;
            FreeMemory += sizeof(Graph);
        }
        else
        {   graph = removed_graph;
            removed_graph = removed_graph->next;
        }
        return graph;
    }

    void freeGraph (Graph *graph)
    {
        graph->prev = NULL;
        if(removed_graph!=NULL)
            removed_graph->prev = graph;
        graph->next = removed_graph;
        removed_graph = graph;
    }




    bool editGraph(Graph *graph, char* _signature)
    {
        int arrayIndex[4];
        ParsedSignature *_myPS;
        Component *_origin_comp,  *_Zaxis_comp, *_rotation_comp;
        Component *_boundary_comp, *_accuracy_comp, *_transparency_comp;
        MainComponent *_graph_equation, *_graph_colouring;
        Expression *_equation, *_conditions, *_colouring;
        char str1[300], str2[300];
        char *strExpr;
        int i, len;


        _myPS = parseSignature(_signature);

        Expression_parsedSignature = _myPS;
        _origin_comp   = parseComponent (_myPS, "position", arrayIndex, 0);
        _Zaxis_comp    = parseComponent (_myPS, "direction", arrayIndex, 0);
        _rotation_comp = parseComponent (_myPS, "rotation_angle", arrayIndex, 0);
        _boundary_comp = parseComponent (_myPS, "boundary", arrayIndex, 0);
        _accuracy_comp = parseComponent (_myPS, "accuracy", arrayIndex, 0);
        _transparency_comp = _accuracy_comp; //parseComponent(_myPS, "transparency", arrayIndex, 0);

        _graph_equation  = findMainComponent (_myPS, "graph_equation");
        _graph_colouring = findMainComponent (_myPS, "graph_colouring");

        if(_origin_comp == NULL || _Zaxis_comp == NULL || _rotation_comp == NULL
        || _boundary_comp == NULL || _accuracy_comp == NULL || _transparency_comp == NULL
        || _graph_equation == NULL || _graph_colouring == NULL)
        { strcpy (Graph_errorMessage, ParsedSignature_errorMessage); return false; }


        strExpr = _graph_equation->strExpr;
        len = strlen(strExpr);

        /* extract str2 for graph extra conditions */
        for(i=0; i < len; i++)
            if(strExpr[i]==',')     // look for ','
                break;
        if(i < len)
        {
            strcpy2 (str2, strExpr, i+1, len);
            if(strlen(trim(str2))==0) strcpy(str2, "1");
            len = i;
        }
        else strcpy(str2, "1");

        /* extract str1 for graph main equation */
        for(i=0; i < len; i++)
            if(strExpr[i]=='=')     // look for '='
                break;
        if(i == len)
        {   strcpy (Graph_errorMessage,
            "The first graph equation has no '=' sign.");
            return false;
        }

        strcpy2 (str1, strExpr, 0, i);
        strcat  (str1, "-(");
        strcat2 (str1, strExpr, i+1, len);
        strcat  (str1, ")");

        _equation   = parseExpression (str1, NULL, 0);
        _conditions = parseExpression (str2, NULL, 0);
        _colouring  = parseExpression (_graph_colouring->strExpr, NULL, 0);


        if(_equation == NULL)
            sprintf(Graph_errorMessage,
            "Error in first graph equation: %s", Expression_errorMessage);
        if(_conditions == NULL)
            sprintf(Graph_errorMessage,
            "Error in graph_equation after first equation: %s", Expression_errorMessage);
        if(_colouring == NULL)
            sprintf(Graph_errorMessage,
            "Error in graph_colouring: %s", Expression_errorMessage);
        if(_equation == NULL || _conditions == NULL || _colouring == NULL)
            return false;


        graph->obj.myParsedSignature = _myPS;
        strcpy(graph->obj.signature, _signature);
        graph->obj.origin_comp = _origin_comp;
        graph->obj.Zaxis_comp = _Zaxis_comp;
        graph->obj.rotation_comp = _rotation_comp;
        graph->obj.boundary_comp = _boundary_comp;
        graph->obj.accuracy_comp = _accuracy_comp;
        graph->obj.transparency_comp = _transparency_comp;
        graph->equation = _equation;
        graph->extraConditions = _conditions;
        graph->colouringFunction = _colouring;
        return true;
    }


    bool addGraph (char* _signature)
    {
        Graph *head = newGraph();

        if(!editGraph(head, _signature))
        { printf("%s\n", Graph_errorMessage); return false; }

        if(headGr != NULL)
            headGr->prev = head;
        head->prev = NULL;
        head->next = headGr;
        headGr = head;
        return true;
    }



    void removeGraph(Graph *graph)
    {
        if(graph == headGr)
            headGr = graph->next;
        if(graph->prev != NULL)
            graph->prev->next = graph->next;
        if(graph->next != NULL)
            graph->next->prev = graph->prev;
    }



    void graph_processAll ()
    {
        Graph *gr;
        for(gr = headGr; gr != NULL; gr = gr->next)
            moveProcess((Object3D*)gr);
    }

    void graph_paintAll (Camera *cmr)
    {
        Graph *gr;
        objCmr = cmr;
        for(gr = headGr; gr != NULL; gr = gr->next)
            paint((Object3D*)gr);
    }



    bool shootPixelGetT (Graph *gr, double t1, double t2, int xp, int yp)
    {
        int i, colour;
        double s, t, low, high, prev, f_of_t;
        const double LIM = (double)0.01;
        double p[3];            // point p = (x, y, z) on the graph

        /* loop as from t1 till t2 */
        s = (t2-t1)/gr->obj.accuracy;
        p[0] = cameraOrigin[0] + t1 * pixel_direction[0];
        p[1] = cameraOrigin[1] + t1 * pixel_direction[1];
        p[2] = cameraOrigin[2] + t1 * pixel_direction[2];
        f_of_t = compute(gr->equation, p);
        prev = f_of_t;

        while(t1<=t2+s)     // +s because there may be a change of sign around t2
        {
            if(prev >= WORLD_LIMIT || f_of_t >= WORLD_LIMIT
            || prev <=-WORLD_LIMIT || f_of_t <=-WORLD_LIMIT)
            {
                prev = f_of_t;
                t1 += s;
                p[0] = cameraOrigin[0] + t1 * pixel_direction[0];
                p[1] = cameraOrigin[1] + t1 * pixel_direction[1];
                p[2] = cameraOrigin[2] + t1 * pixel_direction[2];
                f_of_t = compute(gr->equation, p);
                continue;
            }

            if(f_of_t < -LIM)
            {   if(prev > LIM)             // if f_of_t changes sign
                {
                    low = t1;
                    high = t1-s;
                    i=0;
                    while(true)
                    {
                        if(i++>10) break;
                        //if(low==high) return true;
                        t = (low+high) / (double)2.0;
                        p[0] = cameraOrigin[0] + t * pixel_direction[0];
                        p[1] = cameraOrigin[1] + t * pixel_direction[1];
                        p[2] = cameraOrigin[2] + t * pixel_direction[2];
                        f_of_t = compute(gr->equation, p);
                        if(f_of_t < -LIM) low = t;
                        else if(f_of_t > LIM) high = t;
                        else
                        {
                            if(compute(gr->extraConditions, p)==1)
                            {
                                colour = (int)compute(gr->colouringFunction, p);
                                putClosestPixel(&gr->obj, xp, yp, colour, t);
                                return true;
                            }
                            else break;
                        }
                    }
                }
            }
            else if(f_of_t > LIM)
            {   if(prev < -LIM)            // if f_of_t changes sign
                {
                    low = t1-s;
                    high = t1;
                    i=0;
                    while(true)
                    {
                        if(i++>10) break;
                        //if(low==high) return true;
                        t = (low+high) / (double)2.0;
                        p[0] = cameraOrigin[0] + t * pixel_direction[0];
                        p[1] = cameraOrigin[1] + t * pixel_direction[1];
                        p[2] = cameraOrigin[2] + t * pixel_direction[2];
                        f_of_t = compute(gr->equation, p);
                        if(f_of_t < -LIM) low = t;
                        else if(f_of_t > LIM) high = t;
                        else
                        {
                            if(compute(gr->extraConditions, p)==1)
                            {
                                colour = (int)compute(gr->colouringFunction, p);
                                putClosestPixel(&gr->obj, xp, yp, colour, t);
                                return true;
                            }
                            else break;
                        }
                    }
                }
            }
            else                // if -LIM <= f_of_t <= +LIM
            {
                if(compute(gr->extraConditions, p)==1)
                {
                    colour = (int)compute(gr->colouringFunction, p);
                    putClosestPixel(&gr->obj, xp, yp, colour, t1);
                    return true;
                }
            }
            prev = f_of_t;
            t1 += s;
            p[0] = cameraOrigin[0] + t1 * pixel_direction[0];
            p[1] = cameraOrigin[1] + t1 * pixel_direction[1];
            p[2] = cameraOrigin[2] + t1 * pixel_direction[2];
            f_of_t = compute(gr->equation, p);
        }
        return false;
    }


