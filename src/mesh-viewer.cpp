//--------------------------------------------------
// Author:
// Date:
// Description: Loads PLY files in ASCII format
//--------------------------------------------------

#include <cmath>
#include <string>
#include <vector>
#include "agl/window.h"
#include "plymesh.h"
#include "osutils.h"

using namespace std;
using namespace glm;
using namespace agl;

class MeshViewer : public Window
{
public:
   MeshViewer() : Window()
   {
   }

   void setup()
   {
      mesh.load("../models/cube.ply");
   }

   void mouseMotion(int x, int y, int dx, int dy)
   {
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
   }

   void draw()
   {
      float aspect = ((float)width()) / height();
      renderer.perspective(glm::radians(60.0f), aspect, 0.1f, 50.0f);

      renderer.rotate(vec3(0, 0, 0));
      renderer.scale(vec3(1, 1, 1));
      renderer.translate(vec3(0, 0, 0));
      renderer.mesh(mesh);
      // renderer.cube(); // for debugging!
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
