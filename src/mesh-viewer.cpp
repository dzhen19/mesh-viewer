//--------------------------------------------------
// Author:
// Date:
// Description: Loads PLY files in ASCII format
//--------------------------------------------------

#include <cmath>
#include <string>
#include <sstream>
#include <vector>
#include "agl/window.h"
#include "plymesh.h"
#include "osutils.h"

using namespace std;
using namespace glm;
using namespace agl;

glm::vec3
crossProduct(vec3 a, vec3 b)
{
   // (a2 * b3 – a3 * b2) * i + (a3 * b1 – a1 * b3) * j + (a1 * b2 – a2 * b1) * k,
   return vec3(
       a[1] * b[2] - a[2] * b[1],
       a[2] * b[0] - a[0] * b[2],
       a[0] * b[1] - a[1] * b[0]);
}

string getPathFromIndex(int idx)
{
   string filename = (GetFilenamesInDir("../models", "ply"))[idx];
   std::stringstream path;
   path << "../models/" << filename;
   return path.str();
}

class MeshViewer : public Window
{
public:
   int meshIdx = 0;
   float scaleFactor = 1;
   float translateX = 0;
   float translateY = 0;
   float translateZ = 0;

   float radius = 10;
   // in degrees [0, 360]
   float azimuth = 180;
   // in degrees [-90, 90]
   float elevation = 90;

   bool singleMeshBrowsing = false;
   bool canRotate = false;
   vector<string> shaders = {"normals", "phong-vertex", "phong-pixel"};
   int currentShader = 0;

   MeshViewer() : Window()
   {
      setup();
   }

   vec3 getEyePos()
   {
      return vec3(radius * sin(radians(azimuth)) * std::max(cos(radians(elevation)), float(0.01)),
                  radius * sin(radians(elevation)),
                  radius * cos(radians(azimuth)) * std::max(cos(radians(elevation)), float(0.01)));
   }

   vec3 getUpPos()
   {
      // z directional vector is just the vector going from 0,0,0 to eyePos, so eyePos
      vec3 zVec = getEyePos();
      // x directional vector is z vec crossed with arbitrary unit y
      vec3 xVec = crossProduct(zVec, vec3(0, 1, 0));
      vec3 yVec = crossProduct(zVec, xVec);
      return yVec;
   }

   // returns index in transformedMeshes vector
   int loadMesh(string path)
   {
      TransformedMesh transformedMesh;
      PLYMesh mesh;
      mesh.load(path);

      vec3 max = mesh.maxBounds();
      vec3 min = mesh.minBounds();

      float dx = max[0] - min[0];
      float dy = max[1] - min[1];
      float dz = max[2] - min[2];

      // calculate translate based on model origin
      translateX = -(max[0] - (dx / 2));
      translateY = -(max[1] - (dy / 2));
      translateZ = -(max[2] - (dz / 2));

      // calculate scale factor based on max bounds
      float maxD = std::max(dx, (std::max(dy, dz)));
      scaleFactor = 10 / maxD;

      vec3 scale = vec3(scaleFactor, scaleFactor, scaleFactor);
      vec3 translate = vec3(translateX, translateY, translateZ);
      vec3 rotate = vec3(0, 0, 0);

      transformedMesh = {scale, translate, rotate, mesh};
      transformedMeshes.push_back(transformedMesh);

      return transformedMeshes.size() - 1;
   }

   // inserts a copy of the transformedMesh into the meshes array
   int copyMesh(int id)
   {
      TransformedMesh original = transformedMeshes[id];
      TransformedMesh copy = {original.scale,
                              original.translate,
                              original.rotate,
                              original.mesh};
      transformedMeshes.push_back(copy);
      return transformedMeshes.size() - 1;
   }

   // translate mesh by second param
   void translateMesh(int id, vec3 translate)
   {
      transformedMeshes[id].translate += translate;
   }

   void scaleMesh(int id, float scale)
   {
      transformedMeshes[id].scale *= scale;
   }

   void rotateMesh(int id, vec3 rotate)
   {
      transformedMeshes[id].rotate += rotate;
   }

   void setup()
   {
      if (singleMeshBrowsing)
      {
         loadMesh(getPathFromIndex(meshIdx));
      }
      else
      {
         // custom scene definition

         // shoeoncow
         int cow_id = loadMesh("../models/cow.ply");
         translateMesh(cow_id, vec3(0, 0, 0));
         // scaleMesh(cow_id);

         int shoe_id = loadMesh("../models/tennis_shoe.ply");
         scaleMesh(shoe_id, .2);
         translateMesh(shoe_id, vec3(8, -14, 5));

         int shoe_copy = copyMesh(shoe_id);
         translateMesh(shoe_copy, vec3(-25, 0, 1));

         // 101 cows
         // scaleMesh(cow_id, .3);
         // int copy_id;
         // for (int i = 0; i < 10; i++)
         // {
         //    for (int j = 0; j < 10; j++)
         //    {
         //       copy_id = copyMesh(cow_id);
         //       scaleMesh(copy_id, .3);
         //       translateMesh(copy_id, vec3(i * 5, 0, j * 3));
         //    }
         // }
      }

      // reset eye position
      radius = 10;
      azimuth = 180;
      elevation = 90;

      renderer.loadShader("phong-vertex", "../shaders/phong-vertex.vs", "../shaders/phong-vertex.fs");
      renderer.loadShader("phong-pixel", "../shaders/phong-pixel.vs", "../shaders/phong-pixel.fs");
      renderer.loadShader("normals", "../shaders/normals.vs", "../shaders/normals.fs");
      renderer.loadTexture("brick", "../textures/cow.png", 0);
   }

   // need to fix the math on this
   void mouseMotion(int x, int y, int dx, int dy)
   {
      if (!canRotate)
      {
         return;
      }

      float newAzimuth = azimuth + dx;
      azimuth = std::max(std::min(newAzimuth, float(360)), float(0));

      float newElevation = elevation + dy;
      elevation = std::max(std::min(newElevation, float(90)), float(-90));
   }

   void mouseDown(int button, int mods)
   {

      canRotate = true;
   }

   void mouseUp(int button, int mods)
   {
      canRotate = false;
   }

   void scroll(float dx, float dy)
   {
      float newRadius = radius += dy / 10;
      // radius should not be negative
      radius = std::max(newRadius, float(.01));
   }

   void keyUp(int key, int mods)
   {
      if (key == 83)
      {
         if (currentShader == shaders.size() - 1)
         {
            currentShader = 0;
         }
         else
         {
            // change shader
            currentShader++;
         }
      }

      // disable mesh browsing if we have a custom scene
      if (!singleMeshBrowsing)
      {
         return;
      }

      int numFiles = GetFilenamesInDir("../models", "ply").size();

      // next
      if (key == 78)
      {
         if (meshIdx == numFiles - 1)
         {
            meshIdx = 0;
         }
         else
         {
            meshIdx++;
         }
      }
      // prev
      else if (key == 80)
      {
         if (meshIdx == 0)
         {
            meshIdx = numFiles - 1;
         }
         else
         {
            meshIdx--;
         }
      }

      transformedMeshes.clear();
      setup();
   }

   void draw()
   {
      renderer.beginShader(shaders[currentShader]);
      renderer.texture("image", "brick");
      renderer.setUniform("Light.La", vec3(.1, .1, .2));
      renderer.setUniform("Light.Ld", vec3(.8, .8, 1.0));
      renderer.setUniform("Light.Ls", vec3(.8, .8, 1.0));
      renderer.setUniform("Light.Position", vec4(0.0, 10.0, 0, 1.0));

      renderer.setUniform("Material.f", 3.0f);
      renderer.setUniform("Material.Ka", vec3(.8));
      renderer.setUniform("Material.Kd", vec3(.8));
      renderer.setUniform("Material.Ks", vec3(1.0));

      eyePos = getEyePos();
      up = vec3(0, 1, 0);

      float aspect = ((float)width()) / height();
      renderer.perspective(glm::radians(60.0f), aspect, 0.1f, 50.0f);
      renderer.lookAt(eyePos, lookPos, up);

      int numMeshes = transformedMeshes.size();
      for (int i = 0; i < numMeshes; i++)
      {
         TransformedMesh m = transformedMeshes[i];
         renderer.push();
         renderer.scale(m.scale);
         renderer.translate(m.translate);
         renderer.rotate(m.rotate);
         renderer.mesh(m.mesh);
         renderer.pop();
      }
      renderer.endShader();
   }

protected:
   // PLYMesh mesh;
   vector<TransformedMesh> transformedMeshes;
   vec3 eyePos = getEyePos();
   vec3 lookPos = vec3(0, 0, 0);
   vec3 up = vec3(0, 1, 0);
};

int main(int argc, char **argv)
{
   MeshViewer viewer;
   viewer.run();
   return 0;
}
