#include "Skin.h"

Skin::Skin(Skeleton *skel)
{
    Skel = skel;
}

bool Skin::Load(const char *filename)
{
    //Load position of vertexs
    t.Open(filename);
    t.FindToken("positions");
    int numPos = t.GetFloat();
    t.FindToken("{");

    for (int i = 0; i < numPos; i++)
    {
        Vertex *v = new Vertex();
        float x = t.GetFloat();
        float y = t.GetFloat();
        float z = t.GetFloat();
        
        v->position = glm::vec4(x, y, z, 1);
        allVertex.push_back(v);
        bindVertex.push_back(*v);
    }

    //Load Normals of vertex
    t.FindToken("normals");
    int numNorm = t.GetFloat();
    t.FindToken("{");

    for (int i = 0; i < numNorm; i++)
    {
        float x = t.GetFloat();
        float y = t.GetFloat();
        float z = t.GetFloat();

        glm::vec4 nm = glm::vec4(x, y, z, 0);
        allVertex[i]->normal = nm;
        bindVertex[i].normal = nm;
    }

    //load skinweights
    t.FindToken("skinweights");
    int numWeight = t.GetFloat();
    t.FindToken("{");

    for (int i = 0; i < numWeight; i++)
    {
        int numConnectedJoints = t.GetFloat();
        for (int j = 0; j < numConnectedJoints; j++)
        {
            int jntIdx = t.GetFloat();
            float weight = t.GetFloat();

            allVertex[i]->skinWeights.push_back(std::make_pair(Skel->allJoints[jntIdx], weight));
            bindVertex[i].skinWeights.push_back(std::make_pair(Skel->allJoints[jntIdx], weight));
        }
    }

    //load Triangle
    t.FindToken("triangles");
    int numTri = t.GetFloat();
    t.FindToken("{");

    for (int i = 0; i < numTri; i++)
    {
        Triangle *tri = new Triangle();
        tri->vertexs.push_back(allVertex[t.GetFloat()]);
        tri->vertexs.push_back(allVertex[t.GetFloat()]);
        tri->vertexs.push_back(allVertex[t.GetFloat()]);

        allTriangle.push_back(tri);
    }

    //Load bindings
    t.FindToken("bindings");
    int numBind = t.GetFloat();
    t.FindToken("{");

    for (int i = 0; i < numBind; i++)
    {
        t.FindToken("matrix");
        t.FindToken("{");

        float x0 = t.GetFloat();
        float y0 = t.GetFloat();
        float z0 = t.GetFloat();
        
        float x1 = t.GetFloat();
        float y1 = t.GetFloat();
        float z1 = t.GetFloat();

        float x2 = t.GetFloat();
        float y2 = t.GetFloat();
        float z2 = t.GetFloat();
        
        float x3 = t.GetFloat();
        float y3 = t.GetFloat();
        float z3 = t.GetFloat();

        glm::vec4 col1 = glm::vec4(x0, y0, z0, 0);
        glm::vec4 col2 = glm::vec4(x1, y1, z1, 0);
        glm::vec4 col3 = glm::vec4(x2, y2, z2, 0);
        glm::vec4 col4 = glm::vec4(x3, y3, z3, 1);

        glm::mat4 bind = glm::mat4(col1, col2, col3, col4);

        Skel->allJoints[i]->invBind = glm::inverse(bind);
    }

    return true;
}

void Skin::Update()
{
    for (int i = 0; i < bindVertex.size(); i++)
    {
        glm::vec4 blendPos = glm::vec4(0);
        glm::vec4 blendNorm = glm::vec4(0);

        for (auto weight : bindVertex[i].skinWeights)
        {
            Joint *jnt = weight.first;
            float w = weight.second;

            glm::mat4 M = jnt->toWorld * jnt->invBind;
            blendPos += w * (M * bindVertex[i].position);
            blendNorm += w * (M * bindVertex[i].normal);
        }

        allVertex[i]->position = blendPos;
        allVertex[i]->normal = glm::normalize(blendNorm);
    }
}

void Skin::Draw(const glm::mat4 &viewProjMtx, uint shader)
{
    for (auto i : allTriangle)
    {
        i->Draw(viewProjMtx, shader);
    }
}