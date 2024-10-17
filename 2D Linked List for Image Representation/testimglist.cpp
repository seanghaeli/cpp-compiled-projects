// File:        testimglist.cpp
// Date:        2022-01-24 11:35
// Description: Contains basic tests for ImgList class
//              for CPSC 221 2021W2 PA1
//              You may add your own additional tests

#include "imglist.h"

#include <iostream>

using namespace std;
using namespace cs221util;

// test function definitions
void TestConstructorRender();
void TestCarveMode0();
void TestRenderMode2();
void TestDestructor();
void TestGetDimension();
void TestRenderModeBasic();

int main(void) {

  // call the various test functions
  // you may comment and uncomment these as needed
  // TestDestructor();
  // TestConstructorRender();
  TestCarveMode0();
  TestRenderMode2();
  // TestGetDimension();
  // TestRenderModeBasic();

  return 0;
}

void TestDestructor() {

  cout << "Entered TestDestructor function" << endl;
  PNG inimg;
  inimg.readFromFile("input-images/3x4.png");
  ImgList *list3x4 = new ImgList(inimg);
  delete list3x4;
  cout << "done." << endl;
}

void TestConstructorRender() {
  cout << "Entered TestConstructorRender function" << endl;

  PNG inimg;
  inimg.readFromFile("input-images/3x4.png");

  if (inimg.height() == 4 && inimg.width() == 3) {
    cout << "Input PNG 3x4.png successfully read." << endl;
    cout << "Creating ImgList object using input PNG... ";
    ImgList list3x4(inimg);
    cout << "done." << endl;

    cout << "Attempting to render unmodified list data to output PNG... ";
    PNG outputimg = list3x4.Render(false, 0);
    cout << "done." << endl;

    cout << "Writing output PNG to 3x4-0-0.png... ";
    outputimg.writeToFile("output-images/3x4-0-0.png");
    cout << "done." << endl;

    cout << "Exiting TestConstructorRender function" << endl;
  }
  else {
    cout << "Input PNG image not read. Exiting..." << endl;
  }
}

void TestCarveMode0() {
  cout << "Entered TestCarveMode0 function" << endl;

  PNG inimg;
  inimg.readFromFile("input-images/5x6.png");

  if (inimg.height() == 6 && inimg.width() == 5) {
    cout << "Input PNG 5x6.png successfully read." << endl;
    cout << "Creating ImgList object using input PNG... ";
    ImgList list5x6(inimg);
    cout << "done." << endl;

    cout << "Attempting to carve list using selection mode 0... ";
    list5x6.Carve(1, 0);
    cout << "done." << endl;

    cout << "Determining physical node dimensions of carved image... ";
    unsigned int postcarvex = list5x6.GetDimensionX();
    unsigned int postcarvey = list5x6.GetDimensionY();
    cout << "done." << endl;

    cout << "Expected horizontal dimension: 4" << endl;
    cout << "Your horizontal dimension: " << postcarvex << endl;

    cout << "Rendering carved image using rendermode 0... ";
    PNG outputimg1 = list5x6.Render(false, 0);
    cout << "done." << endl;

    cout << "Writing output PNG to 5x6-0-0.png... ";
    outputimg1.writeToFile("output-images/5x6-0-0.png");
    cout << "done." << endl;

    cout << "Rendering carved image using rendermode 1 and fillmode 0... ";
    PNG outputimg2 = list5x6.Render(true, 0);
    cout << "done." << endl;

    cout << "Writing output PNG to 5x6-1-0.png... ";
    outputimg2.writeToFile("output-images/5x6-1-0.png");
    cout << "done." << endl;

    cout << "Rendering carved image using rendermode 1 and fillmode 1... ";
    PNG outputimg3 = list5x6.Render(true, 1);
    cout << "done." << endl;

    PNG test;
    test.readFromFile("output-images/soln-5x6-1-1.png");

    for (int y = 0; y < test.height(); y++)
    {
      for (int x = 0; x < test.width(); x++)
      {
        HSLAPixel* testPixel = test.getPixel(x,y);
        cout << "Test pixel : {Hue: " << testPixel->h << ", Sat: " << testPixel->s << ", Lum: " << testPixel->l << ", Alpha: " << testPixel->a << "}" << endl;
        HSLAPixel* ourPixel = outputimg3.getPixel(x,y);
        cout << "Our pixel : {Hue: " << ourPixel->h << ", Sat: " << ourPixel->s << ", Lum: " << ourPixel->l << ", Alpha: " << ourPixel->a << "}" << endl;
        cout << "===" << endl;
      }
    }

    cout << "Writing output PNG to 5x6-1-1.png... ";
    outputimg3.writeToFile("output-images/5x6-1-1.png");
    cout << "done." << endl;

    cout << "Carving a second pixel from image... ";
    list5x6.Carve(1, 0);
    cout << "done." << endl;

    cout << "Determining physical node dimensions of carved image... ";
    postcarvex = list5x6.GetDimensionX();
    postcarvey = list5x6.GetDimensionY();
    cout << "done." << endl;

    cout << "Expected horizontal dimension: 3" << endl;
    cout << "Your horizontal dimension: " << postcarvex << endl;

    cout << "Rendering carved image using rendermode 0... ";
    PNG outputimg4 = list5x6.Render(false, 0);
    cout << "done." << endl;

    cout << "Writing output PNG to 5x6-0-0.png... ";
    outputimg4.writeToFile("output-images/5x6-0-0-c2.png");
    cout << "done." << endl;

    cout << "Rendering carved image using rendermode 1 and fillmode 2... ";
    PNG outputimg5 = list5x6.Render(true, 2);
    cout << "done." << endl;

    cout << "Writing output PNG to 5x6-1-2.png... ";
    outputimg5.writeToFile("output-images/5x6-1-2.png");
    cout << "done." << endl;

    cout << "Exiting TestCarveMode0 function" << endl;
  }
  else {
    cout << "Input PNG image not read. Exiting..." << endl;
  }
}

void TestRenderMode2() {
  cout << "Entered TestRenderMode2 function" << endl;

  PNG inimg;
  inimg.readFromFile("input-images/6x6grad.png");

  if (inimg.height() == 6 && inimg.width() == 6) {
    cout << "Input PNG 6x6.png successfully read." << endl;
    cout << "Creating ImgList object using input PNG... ";
    ImgList list6x6(inimg);
    cout << "done." << endl;

    cout << "Attempting to carve more pixels than allowable in mode 0... ";
    list6x6.Carve(12, 0);
    cout << "done." << endl;

    cout << "Determining physical node dimensions of carved image... ";
    unsigned int postcarvex = list6x6.GetDimensionX();
    unsigned int postcarvey = list6x6.GetDimensionY();
    cout << "done." << endl;

    cout << "Expected horizontal dimension: 2" << endl;
    cout << "Your horizontal dimension: " << postcarvex << endl;

    cout << "Rendering carved image using rendermode 0... ";
    PNG outputimg1 = list6x6.Render(false, 0);
    cout << "done." << endl;

    cout << "Writing output PNG to 6x6-0-0.png... ";
    outputimg1.writeToFile("output-images/6x6-0-0.png");
    cout << "done." << endl;

    cout << "Rendering carved image using rendermode 1 and fillmode 2... ";
    PNG outputimg2 = list6x6.Render(true, 2);
    cout << "done." << endl;

    PNG test;
    test.readFromFile("output-images/soln-6x6-1-2.png");

    for (int y = 0; y < test.height(); y++)
    {
      for (int x = 0; x < test.width(); x++)
      {
        HSLAPixel* testPixel = test.getPixel(x,y);
        cout << "Test pixel : {Hue: " << testPixel->h << ", Sat: " << testPixel->s << ", Lum: " << testPixel->l << ", Alpha: " << testPixel->a << "}" << endl;
        HSLAPixel* ourPixel = outputimg2.getPixel(x,y);
        cout << "Our pixel : {Hue: " << ourPixel->h << ", Sat: " << ourPixel->s << ", Lum: " << ourPixel->l << ", Alpha: " << ourPixel->a << "}" << endl;
        cout << "===" << endl;
      }
    }

    cout << "Writing output PNG to 6x6-1-2.png... ";
    outputimg2.writeToFile("output-images/6x6-1-2.png");
    cout << "done." << endl;

    cout << "Exiting TestRenderMode2 function" << endl;
  }
  else {
    cout << "Input PNG image not read. Exiting..." << endl;
  }
}

void TestRenderModeBasic() {
  cout << "Entered TestRenderModeBasic function" << endl;

  PNG inimg;
  inimg.readFromFile("input-images/6x6grad.png");

  if (inimg.height() == 6 && inimg.width() == 6) {
    cout << "Input PNG 6x6.png successfully read." << endl;
    cout << "Creating ImgList object using input PNG... ";
    ImgList list6x6(inimg);
    cout << "done." << endl;


    cout << "Rendering uncarved image using rendermode 0... ";
    PNG outputimg1 = list6x6.Render(false, 0);
    cout << "done." << endl;

    cout << "Writing output PNG to 6x6-0-0-identity.png... ";
    outputimg1.writeToFile("output-images/6x6-0-0-identity.png");
    cout << "done." << endl;
  }

  inimg.readFromFile("input-images/5x6.png");

  if (inimg.height() == 6 && inimg.width() == 5) {
    cout << "Input PNG 5x6.png successfully read." << endl;
    cout << "Creating ImgList object using input PNG... ";
    ImgList list5x6(inimg);
    cout << "done." << endl;


    cout << "Rendering uncarved image using rendermode 0... ";
    PNG outputimg1 = list5x6.Render(false, 0);
    cout << "done." << endl;

    cout << "Writing output PNG to 5x6-0-0-identity.png... ";
    outputimg1.writeToFile("output-images/5x6-0-0-identity.png");
    cout << "done." << endl;
  }
  else {
    cout << "Input PNG image not read. Exiting..." << endl;
  }
}
void TestGetDimension() {
  cout << "Entered TestGetDimensionXY function" << endl;

  PNG inimg;
  inimg.readFromFile("input-images/3x4.png");

  if (inimg.height() == 4 && inimg.width() == 3) {
    cout << "Input PNG 3x4.png successfully read." << endl;
    cout << "Creating ImgList object using input PNG... ";
    ImgList list3x4(inimg);
    cout << "done." << endl;

    cout << "Determining physical node dimensions of image... ";
    unsigned int xdim = list3x4.GetDimensionX();
    unsigned int ydim = list3x4.GetDimensionY();
    unsigned int fullXdim = list3x4.GetDimensionFullX();
    cout << "done." << endl;

    cout << "Expected horizontal dimension: 3" << endl;
    cout << "Your horizontal dimension: " << xdim << endl;

    cout << "Expected vertical dimension: 4" << endl;
    cout << "Your vertical dimension: " << ydim << endl;

    cout << "Expected full horizontal dimension: 3" << endl;
    cout << "Your full horizontal dimension: " << fullXdim << endl;
  }
}