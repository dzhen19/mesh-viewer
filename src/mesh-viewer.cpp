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

   MeshViewer() : Window()
   {
      setup();
   }

   void setup()
   {
      mesh.load(getPathFromIndex(meshIdx));
   }

   void mouseMotion(int x, int y, int dx, int dy)
   {
      // std::cout << x << " " << y << " " << dx << " " << dy << std::endl;
      // std::cout << key << std::endl;
      // std::cout << key << std::endl;
      // std::cout << key << std::endl;
   }

   void mouseDown(int button, int mods)
   {
   }

   void mouseUp(int button, int mods)
   {
   }

   void scroll(float dx, float dy)
   {
   }

   void keyUp(int key, int mods)
   {
      int numFiles = getPathFromIndex(meshIdx).size();

      // next
      if (key == 78)
      {
         if (meshIdx == numFiles)
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
      PLYMesh newMesh;
      newMesh.load(getPathFromIndex(meshIdx));
      mesh = newMesh;
   }

   void draw()
   {

      float aspect = ((float)width()) / height();
      renderer.perspective(glm::radians(60.0f), aspect, 0.1f, 50.0f);
      renderer.lookAt(eyePos, lookPos, up);

      renderer.translate(vec3(0, 0, 0));
      renderer.rotate(vec3(0, 0, 0));
      renderer.scale(vec3(1, 1, 1));
      renderer.mesh(mesh);
   }

protected:
   PLYMesh mesh;
   vec3 eyePos = vec3(10, 0, 0);
   vec3 lookPos = vec3(0, 0, 0);
   vec3 up = vec3(0, 1, 0);
};

int main(int argc, char **argv)
{
   MeshViewer viewer;
   viewer.run();
   return 0;
}
