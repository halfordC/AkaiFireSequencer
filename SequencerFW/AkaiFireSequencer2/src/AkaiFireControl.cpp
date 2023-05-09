#include "AkaiFireControl.h"

AkaiFireControl::AkaiFireControl()
{

    
}

void AkaiFireControl::sysexBegin()
{
    sysexBuffer.clear();
    sysexBuffer.push_back(0x04); //sysex continue, required
    sysexBuffer.push_back(0xF0); //Sysex header
    sysexBuffer.push_back(0x47); //Akai manufacturer ID
    sysexBuffer.push_back(0x7F); //All Call Address
    sysexBuffer.push_back(0x43); //Fire Sub-ID
}

void AkaiFireControl::sendPadUpdate(uint16_t inPadChanged[4], uint8_t InPadLightIndex[4][48] )
{
  Serial.println("We have entered sendPadUpdate");
  sysexBegin();
  sysexBuffer.push_back(0x65); //Write Pads
  //we need to calculate the size of the changes.
  //then we need to push the changes based on the order that we get the changes in. 
  uint16_t totalChanges = 0;
  for(uint8_t i = 0; i<4; i++)
  {
    //not going to lie, I looked this up
    uint16_t destructableCount = inPadChanged[i];
    destructableCount = (destructableCount & countMask1) + ((destructableCount >> 1) & countMask1);
    destructableCount = (destructableCount & countMask2) + ((destructableCount >> 2) & countMask2);
    destructableCount = (destructableCount & countMask3) + ((destructableCount >> 4) & countMask3);
    destructableCount = (destructableCount & countMask4) + ((destructableCount >> 8) & countMask4);

    totalChanges += destructableCount;
  }
  
  totalChanges = totalChanges*4; //4 messages for each change. 

  sysexBuffer.push_back((totalChanges >> 7)&0x7F); //Message Length MSB
  sysexBuffer.push_back(totalChanges&0x7F); //Message Length LSB
  // now we need to do that again to find out what those changes are
  for(uint8_t row = 0; row<4; row++)
  {
    for(uint8_t col = 0; col<16; col++)
    {
      if(((inPadChanged[row]) >> col) & 0x01)
      {
        //we want to update that pad. 
        sysexBuffer.push_back(row*0x10 + col);
        sysexBuffer.push_back(InPadLightIndex[row][col*3] & 0x7F); //red
        sysexBuffer.push_back(InPadLightIndex[row][(col*3)+1] & 0x7F); //green
        sysexBuffer.push_back(InPadLightIndex[row][(col*3)+2] & 0x7F); //green
        inPadChanged[row] = inPadChanged[row] & (~(1<<col)); //this should set that bit to 0. 
      }

    }
  }
  sysexBuffer.push_back(0xF7);          //end of message
  for(uint16_t i = 0; i<sysexBuffer.size(); i++)
  {
    Serial.print(sysexBuffer[i], HEX);
    Serial.print(",");
  }
  Serial.println("");
}

void AkaiFireControl::writeOnePixel(uint16_t X, uint16_t Y, uint8_t C){
    unsigned RemapBit;
    
    if (X < 128 && Y < 64) 
    {
        // Unwind 128x64 arrangement into a 1024x8 arrangement of pixels.
        X += 128 * (Y/8);
        Y %= 8;
  
        // Remap by tiling 8x7 block of translated pixels.
        RemapBit = _aBitMutate[Y][X % 7];
        if (C > 0) {
            OLEDBitmap[4 + X/7*8 + RemapBit/7] |= 1u << (RemapBit % 7);
        } else {
            OLEDBitmap[4 + X/7*8 + RemapBit/7] &= ~(1u << (RemapBit % 7));
        }
    }

}

void AkaiFireControl::sendBitmap(uint8_t *pBitmap, uint16_t BitmapLen)
{
    sysexBegin();
  sysexBuffer.push_back(0x0E);              // WRITE OLED
  sysexBuffer.push_back(BitmapLen >> 7);    // Payload length high
  sysexBuffer.push_back(BitmapLen & 0x7F);  // Payload length low
  OLEDBitmap[0] = 0x00; //should these be here?
  OLEDBitmap[1] = 0x07; //this seems like something that shouldn't live with the image memory. 
  OLEDBitmap[2] = 0x00;
  OLEDBitmap[3] = 0x7F;
  
  while (BitmapLen > 0) {
    sysexBuffer.push_back(*pBitmap++);
    --BitmapLen;
  }
  sysexBuffer.push_back(0xF7);
}