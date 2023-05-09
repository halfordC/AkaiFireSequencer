#include <Arduino.h>
#include <vector>
#include <stdint.h>
#pragma once
using namespace std;

//Some of these will not be used.  
using namespace std;
#define DRUM_ROW_COLOR 0x003F00 //50% green
#define DRUM_SELECTED_COLOR 0x00FF00 //100% green
#define TRACK_ROW_COLOR 0x00003F //50% blue
#define TRACK_SELECTED_COLOR 0x0000FF //100% blue
#define PATTERN_ROW_COLOR 0x59003F //%50 purple
#define STEP_ROW_COLOR 0x3F3E1D // 50% yellow
#define STEP_SELECTED_COLOR 0xFCF874 //100% Yellow
#define SONG_ROW_COLOR 0x3F0000 // 50% Red
#define DRUM_IN_ROW_COLOR 0x3F2C08 //50% orange
#define WHITE_KEY 0x3F3F3F
#define BLACK_KEY 0x1F1F1F
#define TRACK_MUTED 0xFF0000 //100% Red
#define DRUM_MUTED 0xFF0000 //100% Red

//for this crazy fast bit counting algorithm
//maybe I shouldn't be this obsessed with efficiency?
//algo at https://www.youtube.com/watch?v=Hzuzo9NJrlc
#define countMask1 0b0101010101010101
#define countMask2 0b0011001100110011
#define countMask3 0b0000111100001111
#define countMask4 0b0000000011111111

class AkaiFireControl {


  public: 
  AkaiFireControl();
  void sysexBegin();
  void sendPadUpdate(uint16_t inPadChanged[4], uint8_t InPadLightIndex[4][48]);
  void writeOnePixel(uint16_t X, uint16_t Y, uint8_t C); //this probably doesn't need to be a 32 bit int, we only really need this to be a byte / bit.
  void sendBitmap(uint8_t *pBitmap, uint16_t BitmapLen);
  vector<uint8_t> sysexBuffer;

  private:
    
    uint32_t bitmapField[4][64] = {0}; 
    uint8_t OLEDBitmap[1175] = {0};
    uint8_t _aBitMutate[8][7] = {
    { 13, 19, 25, 31, 37, 43, 49 },
    { 0, 20, 26, 32, 38, 44, 50 },
    { 1, 7, 27, 33, 39, 45, 51 },
    { 2, 8, 14, 34, 40, 46, 52 },
    { 3, 9, 15, 21, 41, 47, 53 },
    { 4, 10, 16, 22, 28, 48, 54 },
    { 5, 11, 17, 23, 29, 35, 55 },
    { 6, 12, 18, 24, 30, 36, 42 }
    };



};